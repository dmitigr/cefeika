// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or http.hpp

#ifndef DMITIGR_HTTP_CONNECTION_HPP
#define DMITIGR_HTTP_CONNECTION_HPP

#include "dmitigr/http/basics.hpp"
#include "dmitigr/http/types_fwd.hpp"
#include <dmitigr/net/descriptor.hpp>

//#include <iostream>

#include <algorithm>
#include <array>
#include <cstdint>
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
   * `(!is_closed() && is_start_sent() && !is_body_sent())`.
   */
  void send_header(const std::string_view name, const std::string_view value)
  {
    DMITIGR_REQUIRE(!is_closed() && is_start_sent() && !is_body_sent(), std::logic_error);
    std::string whole{name};
    whole.append(": ").append(value).append("\r\n");
    send__(whole, "dmitigr::http: unable to send header");
    DMITIGR_ASSERT(is_invariant_ok());
  }

  /**
   * @par Requires
   * `(!is_closed() && is_start_sent() && (is_server() || !is_end_sent()))`.
   */
  void send_body(const std::string_view data)
  {
    DMITIGR_REQUIRE(!is_closed() && is_start_sent() && (is_server() || !is_end_sent()), std::logic_error);
    if (!is_body_sent_)
      send__("\r\n", "dmitigr::http: unable to send body");
    send__(data, "dmitigr::http: unable to send body");
    is_body_sent_ = true;
    DMITIGR_ASSERT(is_invariant_ok());
  }

  /// @returns `true` if body (or its part) sent, or `false` otherwise.
  bool is_body_sent() const
  {
    return is_body_sent_;
  }

  /// Shutdowns the send part of socket.
  void send_end()
  {
    auto s = static_cast<net::Socket_native>(io_->native_handle());
    ::shutdown(s, net::sd_send);
    is_end_sent_ = true;
    DMITIGR_ASSERT(is_invariant_ok());
  }

  /// @returns `true` if the send part of socket is shutted down, or `false` otherwise.
  bool is_end_sent() const
  {
    return is_end_sent_;
  }

  /**
   * @par Requires
   * `((is_server() && !is_head_received()) || (!is_server() && is_end_sent()))`
   */
  void receive_head()
  {
    DMITIGR_REQUIRE((is_server() && !is_head_received()) || (!is_server() && is_end_sent()), std::logic_error);

    // Parsing start line.

    head_size_ = recv__(head_.data(), head_.size());
    head_body_offset_ = head_size_;
    if (head_size_ < min_head_size)
      return;

    unsigned hpos = 0;

    // method
    for (; hpos < 7 && head_[hpos] != ' '; hpos++);

    if (const auto m = to_method({head_.data(), hpos}))
      method_size_ = hpos;
    else
      return;

    // path
    DMITIGR_ASSERT(head_[hpos] == ' ');
    for (hpos++; hpos < head_size_ && head_[hpos] != ' '; hpos++);

    if (hpos < head_size_ && head_[hpos] == ' ')
      path_size_ = hpos - 1 - method_size_;
    else
      return;

    // http version
    DMITIGR_ASSERT(head_[hpos] == ' ');
    for (hpos++; hpos < head_size_ && head_[hpos] != '\r'; hpos++);

    if (hpos + 1 < head_size_ && head_[hpos] == '\r' && head_[hpos + 1] == '\n') {
      version_size_ = hpos - 1 - path_size_ - 1 - method_size_;
      hpos += 2; // skip CRLF
    } else
      return;

    // Parsing headers.

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

    enum { name = 1, before_value, value, cr, crlfcr, crlfcrlf } state = name;
    unsigned name_offset = hpos;
    unsigned name_size{};
    unsigned hws_size{};
    unsigned value_size{};

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
            return; // bad input

        case before_value:
          if (is_hws_character(c)) {
            hws_size++;
            continue; // skip HWS
          } else if (c == '\r' || c == '\n')
            return; // headers without values are not allowed
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
            return; // bad input

        case cr:
          if (c == '\n') {
            name_offset = hpos + 1;
            state = name;
            continue; // skip LF
          } else
            return; // expected CRLF not found

        case crlfcr:
          if (c == '\n') {
            state = crlfcrlf; // done
            continue; // skip LF
          } else
            return; // expected CRLFCRLF not found

        case crlfcrlf:;
        }

        if (name_size > 64 || value_size > 128)
          return; // name/value too long
      }

      if (state != crlfcrlf)
        return;
    }

    if (hpos < head_size_)
      head_body_offset_ = hpos;
    else
      is_body_received_ = true;

    is_head_received_ = true;

    DMITIGR_ASSERT(is_invariant_ok());
  }

  /// @returns `true` if start line and headers are received, or `false` otherwise.
  bool is_head_received() const
  {
    return is_head_received_;
  }

  /// @returns The method extracted from start line.
  std::string_view method() const
  {
    const char* const offset = head_.data();
    return {offset, method_size_};
  }

  /// @returns The path extracted from start line.
  std::string_view path() const
  {
    const char* const offset = head_.data() + method_size_ + 1;
    return {offset, path_size_};
  }

  /// @returns The HTTP version extracted from start line.
  std::string_view version() const
  {
    const char* const offset = head_.data() + method_size_ + 1 + path_size_ + 1;
    return {offset, version_size_};
  }

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

  /**
   * @returns The value of "Content-Length" HTTP header if presents, or
   * `std::nullopt` otherwise.
   */
  std::optional<std::intmax_t> content_length() const
  {
    std::optional<std::intmax_t> result{};
    const std::string cl{header("content-length")};
    return !cl.empty() ? std::stoll(cl) : std::make_optional<std::intmax_t>();
  }

  /**
   * @returns The size of body received.
   *
   * @par Requires
   * `is_head_received()`.
   */
  unsigned receive_body(char* buf, unsigned size)
  {
    DMITIGR_REQUIRE(is_head_received(), std::logic_error);
    if (is_body_received_)
      return 0;

    const unsigned head_body_size = head_size_ - head_body_offset_;
    if (head_body_size) {
      if (size < head_body_size) {
        std::memcpy(buf, head_.data() + head_body_offset_, size);
        head_body_offset_ += size;
        return size;
      } else {
        std::memcpy(buf, head_.data() + head_body_offset_, head_body_size);
        head_body_offset_ += head_body_size;
        if (head_size_ < head_.size()) {
          is_body_received_ = true;
          return head_body_size;
        } else {
          size -= head_body_size;
          buf += head_body_size;
          DMITIGR_ASSERT(head_body_offset_ == head_size_);
        }
      }
    }

    const auto result = recv__(buf, size);
    if (result < size)
      is_body_received_ = true;
    DMITIGR_ASSERT(is_invariant_ok());
    return head_body_size + result;
  }

  /// Convenient method to receive an entire body to string.
  std::string receive_body_to_string()
  {
    if (const auto cl = content_length()) {
      std::string result(*cl, 0);
      receive_body(result.data(), result.size());
      return result;
    } else
      return std::string{};
  }

  /// @returns `true` if body received, or `false` otherwise.
  bool is_body_received() const
  {
    return is_body_received_;
  }

  /// Dismisses the body.
  void dismiss_body()
  {
    constexpr unsigned bufsize = 65536;
    char trashcan[bufsize];
    while (const auto n = receive_body(trashcan, bufsize)) {
      if (n < bufsize)
        break;
    }
    DMITIGR_ASSERT(is_body_received());
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

protected:
  explicit Connection(std::unique_ptr<net::Descriptor>&& io)
    : io_{std::move(io)}
  {
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
    DMITIGR_REQUIRE(!is_closed() && !is_start_sent() && !is_body_sent(), std::logic_error);
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

  std::array<char, max_head_size> head_;
  bool is_start_sent_{};
  bool is_body_sent_{};
  bool is_end_sent_{};
  bool is_head_received_{};
  bool is_body_received_{};
  unsigned method_size_{};
  unsigned path_size_{};
  unsigned version_size_{};
  unsigned head_size_{};
  unsigned head_body_offset_{};
  std::unique_ptr<net::Descriptor> io_;
  Header_map headers_;

  virtual bool is_invariant_ok() const
  {
    const bool body_sent_ok = !is_body_sent_ || is_start_sent_;
    const bool end_sent_ok = !is_end_sent_ || (is_body_sent_ && is_start_sent_);
    const bool body_received_ok = !is_body_received_ || is_head_received_;
    const bool start_line_ok = !is_head_received_ || (method_size_ > 0 && path_size_ > 0 && version_size_ > 0);
    const bool head_ok = (head_body_offset_ <= head_size_) && (head_size_ <= head_.size());
    const bool io_ok = static_cast<bool>(io_);
    return body_sent_ok && end_sent_ok && body_received_ok && start_line_ok && head_ok && io_ok;
  }
};

} // namespace dmitigr::http

#endif  // DMITIGR_HTTP_CONNECTION_HPP
