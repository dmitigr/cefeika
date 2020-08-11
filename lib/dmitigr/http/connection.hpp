// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or http.hpp

#ifndef DMITIGR_HTTP_CONNECTION_HPP
#define DMITIGR_HTTP_CONNECTION_HPP

#include "dmitigr/http/basics.hpp"
#include "dmitigr/http/types_fwd.hpp"
#include <dmitigr/net/descriptor.hpp>
#include <dmitigr/str/transformators.hpp>

//#include <iostream>

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <locale>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace dmitigr::http {

/// Denotes the minimum head (start line + headers) size.
constexpr unsigned min_head_size = 3 + 1 + 1 + 1 + 8 + 2; // GET / HTTP/1.1

/// Denotes the maximum head (start line + headers) size.
constexpr unsigned max_head_size = 8192;

/// Denotes the maximum header name size.
constexpr unsigned max_header_name_size = 64;

/// Denotes the maximum header value size.
constexpr unsigned max_header_value_size = 128;

static_assert(min_head_size <= max_head_size);

/// A HTTP connection.
class Connection {
public:
  /**
   * @brief A view to raw (unparsed) header.
   *
   * @remarks The objects of this type becomes invalid when the
   * corresponding connection destroyed.
   */
  class Raw_header_view final {
  public:
    /// The constructor.
    Raw_header_view(const unsigned name_offset, const unsigned name_size,
      const unsigned value_offset, const unsigned value_size,
      const std::array<char, max_head_size>& head)
      : name_offset_{name_offset}
      , name_size_{name_size}
      , value_offset_{value_offset}
      , value_size_{value_size}
      , head_{head}
    {}

    /// @returns The header name.
    std::string_view name() const
    {
      return std::string_view{head_.data() + name_offset_, name_size_};
    }

    /// @returns The header value.
    std::string_view value() const
    {
      return std::string_view{head_.data() + value_offset_, value_size_};
    }

  private:
    unsigned name_offset_{};
    unsigned name_size_{};
    unsigned value_offset_{};
    unsigned value_size_{};
    const std::array<char, max_head_size>& head_;
  };

  /// @brief The destructor.
  virtual ~Connection() = default;

  /// @returns `true` if this is a server connection, or `false` otherwise.
  virtual bool is_server() const = 0;

  /// @returns `true` if start line sent, or `false` otherwise.
  bool is_start_sent() const
  {
    return is_start_sent_;
  }

  /**
   * @par Requires
   * `(!is_closed() && is_start_sent() && !is_headers_sent())`.
   */
  void send_header(const std::string_view name, const std::string_view value, const bool is_last = false)
  {
    DMITIGR_REQUIRE(!is_closed() && is_start_sent() && !is_headers_sent(), std::logic_error);
    std::string whole;
    whole.reserve(name.size() + 2 + value.size() + 2 + (is_last ? 2 : 0));
    if (name.size() == 14) { // for Content-Length only
      whole = name;
      static const std::locale l{"C"};
      str::lowercase(whole, l);
      if (whole == "content-length") {
        whole = value;
        errno = 0;
        const auto cl = std::strtoll(whole.data(), nullptr, 10);
        if (errno || cl < 0)
          throw std::runtime_error{"bad value of Content-Length header"};
        else
          unsent_content_length_ = cl;
      }
      whole.clear();
    }
    whole.append(name).append(": ").append(value).append("\r\n");
    if (is_last)
      whole.append("\r\n");
    send__(whole, "dmitigr::http: unable to send header");
    is_headers_sent_ = is_last;
    DMITIGR_ASSERT(is_invariant_ok());
  }

  /// Alternative of `send_header(name, value, true)`.
  void send_last_header(const std::string_view name, const std::string_view value)
  {
    send_header(name, value, true);
  }

  /// @returns `true` if headers are sent, or `false` otherwise.
  bool is_headers_sent() const
  {
    return is_headers_sent_;
  }

  /**
   * @returns The amount of bytes wasn't send yet. This value can be set initially by
   * sending "Content-Length" header to the remote side.
   */
  std::uintmax_t unsent_content_length() const
  {
    return unsent_content_length_;
  }

  /**
   * @par Requires
   * `(!is_closed() && is_headers_sent() && data.size() <= unsent_content_length())`.
   */
  void send_content(const std::string_view data)
  {
    DMITIGR_REQUIRE(!is_closed() && is_headers_sent() &&
      data.size() <= static_cast<std::uintmax_t>(unsent_content_length()), std::logic_error);
    send__(data, "dmitigr::http: unable to send content");
    unsent_content_length_ -= data.size();
    DMITIGR_ASSERT(is_invariant_ok());
  }

  /**
   * @par Requires
   * `(!is_closed() && !is_head_received() && ((is_server() && !is_start_sent()) || (!is_server() && is_headers_sent() && !unsent_content_length())))`
   */
  void receive_head()
  {
    DMITIGR_REQUIRE(!is_closed() && !is_head_received() &&
      ((is_server() && !is_start_sent()) ||
       (!is_server() && is_headers_sent() && !unsent_content_length())), std::logic_error);

    unsigned hpos{};

    const auto recv_head = [this, &hpos]() -> unsigned
    {
      if (hpos < head_size_)
        return head_size_ - hpos;
      else if (const unsigned n = recv__(head_.data() + head_size_, head_.size() - head_size_)) {
        head_size_ += n;
        head_content_offset_ = head_size_;
        return n;
      } else
        return 0;
    };

    const auto parse_start_line = [this, &hpos, recv_head]() -> int
    {
      if (!recv_head() || head_size_ < min_head_size)
        return -1;

      if (is_server()) {
        // method
        for (; hpos < 7 && hpos < head_size_ && head_[hpos] != ' '; hpos++);

        if (hpos >= head_size_)
          return 1;
        else if (const auto m = to_method({head_.data(), hpos}))
          method_size_ = hpos;
        else
          return -1;

        // path
        DMITIGR_ASSERT(head_[hpos] == ' ');
        for (hpos++; hpos < head_size_ && head_[hpos] != ' '; hpos++);

        if (hpos >= head_size_)
          return 1;
        else if (head_[hpos] == ' ')
          path_size_ = hpos - 1 - method_size_;
        else
          return -1;

        // http version
        DMITIGR_ASSERT(head_[hpos] == ' ');
        for (hpos++; hpos < head_size_ && head_[hpos] != '\r'; hpos++);

        if (hpos + 1 >= head_size_)
          return 1;
        else if (head_[hpos] == '\r' && head_[hpos + 1] == '\n') {
          version_size_ = hpos - 1 - path_size_ - 1 - method_size_;
          hpos += 2; // skip CRLF
        } else
          return -1;
      } else {
        // http version
        for (; hpos < 8 && hpos < head_size_ && head_[hpos] != ' '; hpos++);

        if (hpos >= head_size_)
          return 1;
        else if (const std::string_view ver{head_.data(), hpos}; ver == "HTTP/1.0" || ver == "HTTP/1.1")
          version_size_ = ver.size();
        else
          return -1;

        // http status code
        DMITIGR_ASSERT(head_[hpos] == ' ');
        for (hpos++; hpos < head_size_ && head_[hpos] != ' '; hpos++);

        if (hpos >= head_size_)
          return 1;
        else if (head_[hpos] == ' ')
          code_size_ = hpos - 1 - version_size_;
        else
          return -1;

        // http status phrase
        DMITIGR_ASSERT(head_[hpos] == ' ');
        for (hpos++; hpos < head_size_ && head_[hpos] != '\r'; hpos++);

        if (hpos + 1 >= head_size_)
          return 1;
        else if (head_[hpos] == '\r' && head_[hpos + 1] == '\n') {
          phrase_size_ = hpos - 1 - code_size_ - 1 - version_size_;
          hpos += 2; // skip CRLF
        } else
          return -1;
      }

      return 0;
    };

    unsigned headers_received{};
    unsigned name_offset{};
    unsigned name_size{};
    unsigned hws_size{};
    unsigned value_size{};
    enum { name = 1, before_value, value, cr, crlfcr, crlfcrlf } state{name};
    const auto parse_headers = [this, &hpos, &headers_received, &name_offset,
      &name_size, &hws_size, &value_size, &state, recv_head]() -> int
    {
      static const auto is_hws_character = [](const char c)
      {
        return c == ' ' || c == '\t';
      };
      static const auto is_valid_name_character = [](const char c)
      {
        // According to https://tools.ietf.org/html/rfc7230#section-3.2.6
        static const char allowed[] = { '!', '#', '$', '%', '&', '\'', '*', '+', '-', '.', '^', '_', '`', '|', '~' };
        static const std::locale l{"C"};
        return std::isalnum(c, l) || std::any_of(std::cbegin(allowed), std::cend(allowed),
          [c](const char ch) { return ch == c; });
      };
      static const auto is_valid_value_character = [](const char c)
      {
        // According to https://tools.ietf.org/html/rfc7230
        static const std::locale l{"C"};
        return std::isprint(c, l);
      };

      if (const auto n = recv_head())
        headers_received += n;
      else if (!headers_received)
        return 0; // no headers sent

      //std::cerr << "headers received = " << headers_received << std::endl;

      if (hpos < head_size_) {
        headers_.pairs().reserve(16);
        static const std::locale l{"C"};
        for (; hpos < head_size_ && state != crlfcrlf; hpos++) {
          const char c = head_[hpos];
          //std::cerr << "char = " << c << " state = " << (int)state << std::endl;
          switch (state) {
          case name:
            if (c == ':') {
              state = before_value;
              continue; // skip :
            } else if (is_valid_name_character(c)) {
              head_[hpos] = std::tolower(c, l);
              name_size++;
              break; // ok
            } else if (c == '\r') {
              state = crlfcr;
              continue;
            } else
              return -1; // bad input

          case before_value:
            if (is_hws_character(c)) {
              hws_size++;
              continue; // skip HWS
            } else if (c == '\r' || c == '\n')
              return -1; // headers without values are not allowed
            else {
              state = value;
            }
            [[fallthrough]];

          case value:
            if (c == '\r') {
              const unsigned value_offset = name_offset + name_size + 1 + hws_size;
              headers_.add(name_offset, name_size, value_offset, value_size, head_);
              name_size = value_size = hws_size = 0;
              state = cr;
              continue; // skip CR
            } else if (is_valid_value_character(c)) {
              value_size++;
              break; // ok
            } else
              return -1; // bad input

          case cr:
            if (c == '\n') {
              name_offset = hpos + 1;
              state = name;
              continue; // skip LF
            } else
              return -1; // expected CRLF not found

          case crlfcr:
            if (c == '\n') {
              state = crlfcrlf; // done
              continue; // skip LF
            } else
              return -1; // expected CRLFCRLF not found

          case crlfcrlf:;
          }

          if (name_size > max_header_name_size || value_size > max_header_value_size)
            return -1; // name/value too long
        }

        if (state == crlfcrlf)
          return 0;
        else if (hpos >= head_size_)
          return 1;
      }

      return -1;
    };

    //std::cerr << "Parsing start line" << std::endl;
    while (const int r = parse_start_line())
      if (r < 0) return;

    //std::cerr << "Parsing headers" << std::endl;
    name_offset = hpos; // both captured by parse_headers()
    while (const int r = parse_headers())
      if (r < 0) return;

    is_head_received_ = true;

    if (hpos < head_size_)
      head_content_offset_ = hpos;

    unreceived_content_length_ = content_length();

    DMITIGR_ASSERT(is_invariant_ok());
  }

  /// @returns `true` if start line and headers are received, or `false` otherwise.
  bool is_head_received() const
  {
    return is_head_received_;
  }

  /// @returns The HTTP version extracted from start line.
  virtual std::string_view version() const = 0;

  /// @returns The value of given HTTP header.
  std::string_view header(const std::string_view name) const
  {
    if (const auto index = headers_.index(name))
      return headers_.pairs()[*index].value();
    else
      return {};
  }

  /// @returns The vector of HTTP headers.
  const std::vector<Raw_header_view>& headers() const
  {
    return headers_.pairs();
  }

  /// @returns The value of "Content-Length" HTTP header if presents, or `0` otherwise.
  std::intmax_t content_length() const
  {
    const std::string cl{header("content-length")};
    return !cl.empty() ? std::stoll(cl) : 0;
  }

  /**
   * @returns The size of content received.
   *
   * @par Requires
   * `is_head_received() && unreceived_content_length() > 0`.
   */
  unsigned receive_content(char* buf, unsigned size)
  {
    DMITIGR_REQUIRE(is_head_received() && unreceived_content_length() > 0, std::logic_error);

    const unsigned head_content_size = head_size_ - head_content_offset_;
    if (head_content_size) {
      if (size < head_content_size) {
        std::memcpy(buf, head_.data() + head_content_offset_, size);
        head_content_offset_ += size;
        return size;
      } else {
        std::memcpy(buf, head_.data() + head_content_offset_, head_content_size);
        head_content_offset_ += head_content_size;
        if (head_size_ < head_.size()) {
          unreceived_content_length_ -= head_content_size;
          return head_content_size;
        } else {
          size -= head_content_size;
          buf += head_content_size;
          DMITIGR_ASSERT(head_content_offset_ == head_size_);
        }
      }
    }

    const auto result = recv__(buf, std::min(static_cast<std::intmax_t>(size), unreceived_content_length_));
    unreceived_content_length_ -= result;
    DMITIGR_ASSERT(is_invariant_ok());
    return head_content_size + result;
  }

  /// Convenient method to receive an entire (unreceived) content to string.
  std::string receive_content_to_string()
  {
    if (unreceived_content_length_) {
      std::string result(unreceived_content_length_, 0);
      receive_content(result.data(), result.size());
      return result;
    } else
      return std::string{};
  }

  /// @returns The amount of bytes of content which are not yet received.
  std::uintmax_t unreceived_content_length() const
  {
    return unreceived_content_length_;
  }

  /// Dismisses the content.
  void dismiss_content()
  {
    constexpr unsigned bufsize = 65536;
    char trashcan[bufsize];
    while (const auto n = receive_content(trashcan, bufsize)) {
      if (n < bufsize)
        break;
    }
    DMITIGR_ASSERT(!unreceived_content_length());
    DMITIGR_ASSERT(is_invariant_ok());
  }

  /// Closes the connection.
  void close()
  {
    io_->close();
    io_.reset();
    DMITIGR_ASSERT(is_invariant_ok());
  }

  /// @returns `true` if connection is closed, or `false` otherwise.
  bool is_closed() const
  {
    return !static_cast<bool>(io_);
  }

  /**
   * @returns The underlying IO descriptor. (Use with care.)
   *
   * @par Requires
   * `!is_closed()`.
   */
  auto native_handle() const
  {
    DMITIGR_REQUIRE(!is_closed(), std::logic_error);
    return io_->native_handle();
  }

protected:
  Connection() = default;

  explicit Connection(std::unique_ptr<net::Descriptor>&& io)
    : io_{std::move(io)}
  {
    DMITIGR_ASSERT(is_invariant_ok());
  }

  void init(std::unique_ptr<net::Descriptor>&& io)
  {
    io_ = std::move(io);
    DMITIGR_ASSERT(is_invariant_ok());
  }

  Connection(const Connection&) = delete;
  Connection& operator=(const Connection&) = delete;
  Connection(Connection&&) = delete;
  Connection& operator=(Connection&&) = delete;

  void send__(const std::string_view data, const char* const errmsg)
  {
    DMITIGR_ASSERT(!is_closed());
    DMITIGR_ASSERT(errmsg);
    const auto data_size = static_cast<std::streamsize>(data.size());
    const auto n = io_->write(data.data(), data_size);
    if (n != data_size)
      throw std::runtime_error{errmsg};
  }

  std::streamsize recv__(char* const buf, const std::streamsize size)
  {
    DMITIGR_ASSERT(!is_closed());
    return io_->read(buf, size);
  }

  void send_start__(const std::string_view line)
  {
    DMITIGR_REQUIRE(!is_closed() && !is_start_sent(), std::logic_error);
    send__(line, "dmitigr::http: unable to send start line");
    is_start_sent_ = true;
  }

private:
  /// A container of name-value pairs to store variable-length values.
  class Header_map final {
  public:
    /// @returns The pair index by the given `name`.
    std::optional<std::size_t> index(const std::string_view name) const
    {
      const auto b = cbegin(pairs_);
      const auto e = cend(pairs_);
      const auto i = std::find_if(b, e, [&](const auto& p) { return p.name() == name; });
      return i != e ? std::make_optional<std::size_t>(i - b) : std::nullopt;
    }

    /// @returns The vector of name/value pairs.
    const std::vector<Raw_header_view>& pairs() const
    {
      return pairs_;
    }

    /// @overload
    std::vector<Raw_header_view>& pairs()
    {
      return pairs_;
    }

    /**
     * @brief Adds the name-value pair.
     *
     * @param name_offset Offset of name in head
     * @param name_size Name size in head
     * @param value_offset Offset of value in head
     * @param value_size Value size in head
     * @param head A reference to array with names and values
     */
    void add(const unsigned name_offset, const unsigned name_size,
      const unsigned value_offset, const unsigned value_size,
      const std::array<char, max_head_size>& head)
    {
      pairs_.emplace_back(name_offset, name_size, value_offset, value_size, head);
    }

  private:
    std::vector<Raw_header_view> pairs_;
  };

protected:
  std::array<char, max_head_size> head_;
  bool is_headers_sent_{};
private:
  bool is_start_sent_{};
  bool is_head_received_{};
protected:
  unsigned method_size_{};
  unsigned path_size_{};
  unsigned version_size_{};
  unsigned code_size_{};
  unsigned phrase_size_{};
private:
  unsigned head_size_{};
  unsigned head_content_offset_{};
  std::intmax_t unsent_content_length_{};
  std::intmax_t unreceived_content_length_{};
  std::unique_ptr<net::Descriptor> io_;
  Header_map headers_;

  virtual bool is_invariant_ok() const
  {
    const bool start_line_ok = !is_head_received_ ||
      (is_server() && method_size_ > 0 && path_size_ > 0 && version_size_ > 0) ||
      (!is_server() && version_size_ > 0 && code_size_ > 0 && phrase_size_ > 0);
    const bool head_ok = (head_content_offset_ <= head_size_) && (head_size_ <= head_.size());
    const bool content_lengths_ok = (unsent_content_length_ >= 0) && (unreceived_content_length_ >= 0);
    const bool io_ok = static_cast<bool>(io_);

    return start_line_ok && head_ok && content_lengths_ok && io_ok;
  }
};

} // namespace dmitigr::http

#endif  // DMITIGR_HTTP_CONNECTION_HPP
