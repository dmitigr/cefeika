// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or mulf.hpp

#ifndef DMITIGR_MULF_FORM_DATA_HPP
#define DMITIGR_MULF_FORM_DATA_HPP

#include "dmitigr/mulf/types_fwd.hpp"
#include <dmitigr/base/debug.hpp>
#include <dmitigr/str/str.hpp>

#include <algorithm>
#include <cstddef>
#include <locale>
#include <optional>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

namespace dmitigr::mulf {

/**
 * @brief An entry of multipart/form-data.
 */
class Form_data_entry final {
public:
  /**
   * @brief The constructor.
   */
  explicit Form_data_entry(std::string name)
    : name_{std::move(name)}
  {
    DMITIGR_ASSERT(is_invariant_ok());
  }

  /**
   * @returns The name of the entry.
   *
   * @see set_name().
   */
  const std::string& name() const noexcept
  {
    return name_;
  }

  /**
   * @brief Sets the name of the entry.
   *
   * @par Requires
   * `!name.empty()`.
   *
   * @see name().
   */
  void set_name(std::string name)
  {
    DMITIGR_REQUIRE(!name.empty(), std::invalid_argument);

    name_ = std::move(name);

    DMITIGR_ASSERT(is_invariant_ok());
  }

  /**
   * @returns The filename of the entry.
   *
   * @see set_filename().
   */
  const std::optional<std::string>& filename() const
  {
    return filename_;
  }

  /**
   * @brief Sets the filename of the entry.
   *
   * @par Requires
   * `(!filename || !filename->empty())`.
   *
   * @see filename().
   */
  void set_filename(std::optional<std::string> filename)
  {
    DMITIGR_REQUIRE(!filename || !filename->empty(), std::invalid_argument);

    filename_ = std::move(filename);

    DMITIGR_ASSERT(is_invariant_ok());
  }

  /**
   * @returns The content type of the entry.
   *
   * @see set_content_type().
   */
  const std::optional<std::string>& content_type() const
  {
    return content_type_;
  }

  /**
   * @brief Sets the content type of the entry.
   *
   * @par Requires
   * `(!content_type || !content_type->empty())`.
   *
   * @see content_type().
   */
  void set_content_type(std::optional<std::string> content_type)
  {
    DMITIGR_REQUIRE(!content_type || !content_type->empty(), std::invalid_argument);

    content_type_ = std::move(content_type);

    DMITIGR_ASSERT(is_invariant_ok());
  }

  /**
   * @returns The charset of the entry.
   *
   * @see set_charset().
   */
  const std::optional<std::string>& charset() const
  {
    return charset_;
  }

  /**
   * @brief Sets the charset of the entry.
   *
   * @par Requires
   * `(!charset || !charset->empty())`.
   *
   * @see charset().
   */
  void set_charset(std::optional<std::string> charset)
  {
    DMITIGR_REQUIRE(!charset || !charset->empty(), std::invalid_argument);

    charset_ = std::move(charset);

    DMITIGR_ASSERT(is_invariant_ok());
  }

  /**
   * @returns The content of the entry.
   *
   * @see set_content().
   */
  std::optional<std::string_view> content() const
  {
    static const auto visitor = [](auto& result)
    {
      return std::make_optional(static_cast<std::string_view>(result));
    };
    return content_ ? std::visit(visitor, *content_) : std::nullopt;
  }

  /**
   * @brief Sets the content of the entry.
   *
   * @par Requires
   * `(!view || !view->empty())`.
   *
   * @see content().
   */
  void set_content(std::optional<std::string_view> view)
  {
    DMITIGR_REQUIRE(!view || !view->empty(), std::invalid_argument);

    content_ = std::move(view);

    DMITIGR_ASSERT(is_invariant_ok());
  }

  /**
   * @overload
   *
   * @par Requires
   * `(!content || !content->empty())`.
   *
   * @see content().
   */
  void set_content(std::optional<std::string> content)
  {
    DMITIGR_REQUIRE(!content || !content->empty(), std::invalid_argument);

    content_ = std::move(content);

    DMITIGR_ASSERT(is_invariant_ok());
  }

private:
  friend Form_data;

  std::string name_;
  std::optional<std::string> filename_;
  std::optional<std::string> content_type_;
  std::optional<std::string> charset_;
  std::optional<std::variant<std::string, std::string_view>> content_;

  Form_data_entry() = default; // constructs invalid object

  bool is_invariant_ok() const
  {
    const bool name_ok = !name_.empty();
    const bool filename_ok = !filename_ || !filename_->empty();
    const bool content_type_ok = !content_type_ || !content_type_->empty();
    const bool charset_ok = !charset_ || !charset_->empty();
    const bool content_ok = !content_ || !content()->empty();
    return name_ok && filename_ok && content_type_ok && charset_ok && content_ok;
  }
};

/**
 * @brief A parsed multipart/form-data.
 *
 * @remarks Since several entries can be named equally, `offset` can be
 * specified as the starting lookup index in the corresponding methods.
 *
 */
class Form_data final {
public:
  /** The alias of Form_data_entry. */
  using Entry = Form_data_entry;

  /**
   * @brief Constructs the object by parsing the multipart/form-data.
   *
   * @param data - the unparsed multipart/form-data;
   * @param boundary - the boundary of multipart/form-data.
   *
   * @remarks The `data` will be used as a storage area to avoid
   * copying the content of the entries (which are can be huge).
   */
  Form_data(std::string data, const std::string& boundary)
    : data_{std::move(data)}
  {
    if (!is_boundary_valid(boundary))
      throw std::runtime_error{"dmitigr::mulf: invalid boundary"};

    const auto delimiter{"\r\n--" + boundary};

    auto pos = data_.find(delimiter);
    if (pos != std::string::npos) {
      pos += delimiter.size();
      pos = skip_transport_padding(data_, pos);
      pos = skip_mandatory_crlf(data_, pos);
    } else
      throw std::runtime_error{"dmitigr::mulf: no boundary"};

    while (true) {
      const auto next_delimiter_pos = data_.find(delimiter, pos);
      if (next_delimiter_pos == std::string::npos)
        throw std::runtime_error{"dmitigr::mulf: unclosed boundary"};

      DMITIGR_ASSERT(pos < data_.size());

      Entry entry;
      entries_.emplace_back(std::move(entry));
      pos = set_headers(entries_.back(), data_, pos);
      DMITIGR_ASSERT(pos <= next_delimiter_pos);
      if (pos < next_delimiter_pos)
        set_content(entries_.back(), data_, pos, next_delimiter_pos);

      pos = next_delimiter_pos + delimiter.size();
      if (pos + 1 < data_.size()) {
        if (data_[pos] == '-') {
          if (data_[pos + 1] == '-')
            break; // Close-delimiter found. Don't care about transport padding and epilogue.
          else
            throw std::runtime_error{"dmitigr::mulf: invalid close-delimiter"};
        } else {
          pos = skip_transport_padding(data_, pos);
          pos = skip_mandatory_crlf(data_, pos);
        }
      } else
        throw std::runtime_error{"dmitigr::mulf: no close-delimiter"};
    }

    DMITIGR_ASSERT(is_invariant_ok());
  }

  /**
   * @returns The number of entries.
   */
  std::size_t entry_count() const
  {
    return entries_.size();
  }

  /**
   * @returns The entry index if `has_entry(name, offset)`, or
   * `std::nullopt` otherwise.
   */
  std::optional<std::size_t> entry_index(const std::string_view name, const std::size_t offset = 0) const
  {
    if (offset < entry_count()) {
      const auto b = cbegin(entries_);
      const auto e = cend(entries_);
      const auto i = std::find_if(b + offset, e, [&](const auto& entry) { return entry.name() == name; });
      return (i != e) ? std::make_optional(i - b) : std::nullopt;
    } else
      return std::nullopt;
  }

  /**
   * @returns The entry index.
   *
   * @par Requires
   * `has_entry(name, offset)`.
   */
  std::size_t entry_index_throw(const std::string_view name, const std::size_t offset = 0) const
  {
    const auto result = entry_index(name, offset);
    DMITIGR_REQUIRE(result, std::out_of_range,
      "the instance of dmitigr::mulf::Form_data has no entry \"" + std::string{name} + "\"");
    return *result;
  }

  /**
   * @returns The entry.
   *
   * @par Requires
   * `(index < entry_count())`.
   */
  const Entry& entry(const std::size_t index) const
  {
    DMITIGR_REQUIRE(index < entry_count(), std::out_of_range,
      "invalid form data entry index (" + std::to_string(index) + ")"
      " of the dmitigr::mulf::Form_data instance");
    return entries_[index];
  }

  /**
   * @overload
   *
   * @returns `entry(entry_index_throw(name, offset))`.
   */
  const Entry* entry(const std::string_view name, const std::size_t offset = 0) const
  {
    const auto index = entry_index_throw(name, offset);
    return &entries_[index];
  }

  /**
   * @returns `true` if this instance has the entry with the specified `name`,
   * or `false` otherwise.
   */
  bool has_entry(const std::string_view name, const std::size_t offset = 0) const
  {
    return static_cast<bool>(entry_index(name, offset));
  }

  /**
   * @returns `(entry_count() > 0)`
   */
  bool has_entries() const
  {
    return !entries_.empty();
  }

private:
  std::string data_;
  std::vector<Entry> entries_;

  constexpr bool is_invariant_ok()
  {
    return true;
  }

  /**
   * @returns `true` if the `boundary` contains only allowed characters
   * according to https://tools.ietf.org/html/rfc2046#section-5.1.1, or
   * `false` otherwise.
   */
  static bool is_boundary_valid(const std::string& boundary)
  {
    static const auto is_valid_boundary_character = [](const char c)
    {
      static const char allowed[] = { '\'', '(', ')', '+', '_', ',', '-', '.', '/', ':', '=', '?', ' ' };
      static const std::locale l{"C"};
      return std::isalnum(c, l) || std::any_of(std::cbegin(allowed), std::cend(allowed), [c](const char ch) { return ch == c; });
    };

    return !boundary.empty() && boundary.size() <= 70 &&
      std::all_of(cbegin(boundary), cend(boundary), is_valid_boundary_character);
  }

  /**
   * @returns The position of a character immediately following the transport
   * padding, described at https://tools.ietf.org/html/rfc2046#section-5.1.1,
   * or `pos` if the `data` doesn't starts with such a transport padding.
   */
  static std::string::size_type skip_transport_padding(const std::string& data, std::string::size_type pos)
  {
    DMITIGR_ASSERT(pos < data.size());

    bool is_crlf_reached{};
    if (const char c = data[pos]; c == ' ' || c == '\t' || c == '\r' || c == '\n') {
      for (++pos; pos < data.size(); ++pos) {
        const char ch = data[pos];
        switch (ch) {
        case ' ':
        case '\t':
          is_crlf_reached = false;
          continue;
        case '\r':
          if (is_crlf_reached)
            return pos - 2;
          continue;
        case '\n':
          if (is_crlf_reached)
            return pos - 2;
          is_crlf_reached = (data[pos - 1] == '\r');
          continue;
        default:
          goto end;
        }
      }
    }
  end:
    return is_crlf_reached ? pos - 2 : pos;
  }

  /**
   * @returns The position of a character immediately following the `CRLF` sequence.
   *
   * @throws `std::runtime_error` if the `data` doesn't starts with `CRLF`.
   */
  static std::string::size_type skip_mandatory_crlf(const std::string& data, const std::string::size_type pos)
  {
    DMITIGR_ASSERT(pos < data.size());

    if (pos + 1 < data.size() && data[pos] == '\r' && data[pos + 1] == '\n')
      return pos + 2;
    else
      throw std::runtime_error{"dmitigr::mulf: expected CRLF not found"};
  }

  /**
   * @brief Parses headers in `data` and stores them in `entry`.
   *
   * According to https://tools.ietf.org/html/rfc7578#section-4.5 the example
   * of valid data to parse is:
   *
   *   content-disposition: form-data; name="field1"
   *   content-type: text/plain;charset=UTF-8
   *
   * @returns The position of a character immediately following the `CRLFCRLF`,
   * sequence.
   *
   * @remarks According to https://tools.ietf.org/html/rfc7578#section-4.7
   * the `content-transfer-encoding` header field is deprecated and this
   * implementation doesn't parse it.
   */
  static std::string::size_type set_headers(Entry& entry,
    const std::string& data, std::string::size_type pos)
  {

    DMITIGR_ASSERT(pos < data.size());

    enum { name = 1,
           before_parameter_name,
           parameter_name,
           before_parameter_value,
           parameter_value,
           parameter_quoted_value,
           parameter_quoted_value_bslash,
           parameter_quoted_value_quote,
           cr, crlf, crlfcr, crlfcrlf } state = name;

    enum { content_disposition = 1, content_type } type = static_cast<decltype(type)>(0);
    enum { name_parameter = 1, filename_parameter, charset_parameter } param = static_cast<decltype(param)>(0);

    std::string extracted;
    bool form_data_extracted{};

    static const auto is_hws_character = [](const char c)
    {
      return c == ' ' || c == '\t';
    };
    static const auto is_valid_name_character = [](const char c)
    {
      static const std::locale l{"C"};
      return std::isalnum(c, l) || c == '-';
    };
    static const auto is_valid_parameter_name_character = [](const char c)
    {
      static const std::locale l{"C"};
      return std::isalnum(c, l) || c == '-' || c == '/';
    };
    static const auto is_valid_parameter_value_character = [](const char c)
    {
      // According to https://tools.ietf.org/html/rfc7230#section-3.2.6
      static const char allowed[] = { '!', '#', '$', '%', '&', '\'', '*', '+', '-', '.', '^', '_', '`', '|', '~' };
      static const std::locale l{"C"};
      return std::isalnum(c, l) || std::any_of(std::cbegin(allowed), std::cend(allowed), [c](const char ch) { return ch == c; });
    };
    static const auto is_valid_parameter_quoted_value_character = [](const char c)
    {
      return is_valid_parameter_value_character(c) || is_hws_character(c);
    };

    const auto process_parameter_name = [&](std::string&& extracted) {
      switch (type) {
      case content_disposition:
        if (extracted == "name")
          param = name_parameter;
        else if (extracted == "filename")
          param = filename_parameter;
        else if (extracted == "form-data")
          if (!form_data_extracted)
            form_data_extracted = true;
          else
            throw std::runtime_error{"dmitigr::mulf: invalid content-disposition"};
        else
          throw std::runtime_error{"dmitigr::mulf: invalid content-disposition"};
        break;
      case content_type:
        if (extracted == "charset")
          param = charset_parameter;
        else if (!entry.content_type_)
          entry.content_type_ = std::move(extracted);
        else
          throw std::runtime_error{"dmitigr::mulf: invalid content-type"};
        break;
      };
    };

    const auto process_parameter_value = [&](std::string&& extracted) {
      switch (type) {
      case content_disposition:
        if (param == name_parameter)
          entry.name_ = std::move(extracted);
        else if (param == filename_parameter)
          entry.filename_ = std::move(extracted);
        else
          throw std::runtime_error{"dmitigr::mulf: invalid content-disposition"};
        break;
      case content_type:
        if (param == charset_parameter)
          entry.charset_ = std::move(extracted);
        else
          throw std::runtime_error{"dmitigr::mulf: invalid content-type"};
        break;
      };
    };

    const auto data_size = data.size();
    for (; pos < data_size && state != crlfcrlf; ++pos) {
      const char c = data[pos];
      switch (state) {
      case name:
        if (c == ':') {
          str::lowercase(extracted);
          if (extracted == "content-disposition")
            type = content_disposition;
          else if (extracted == "content-type")
            type = content_type;
          else
            throw std::runtime_error{"dmitigr::mulf: unallowable or empty header name"};
          extracted.clear();
          state = before_parameter_name;
          continue; // skip :
        } else if (!is_valid_name_character(c))
          throw std::runtime_error{"dmitigr::mulf: invalid header name"};
        break;

      case before_parameter_name:
        if (!is_hws_character(c)) {
          if (is_valid_parameter_name_character(c))
            state = parameter_name;
          else
            throw std::runtime_error{"dmitigr::mulf: invalid header value"};
        } else
          continue; // skip HWS character
        break;

      case parameter_name:
        if (c == ';' || c == '=' || c == '\r') {
          str::lowercase(extracted);
          process_parameter_name(std::move(extracted));
          extracted = {};
          state = (c == ';') ? before_parameter_name : (c == '=') ? before_parameter_value : cr;
          continue; // skip ; or = or CR
        } else if (!is_valid_parameter_name_character(c))
          throw std::runtime_error{"dmitigr::mulf: invalid character in the header value"};
        break;

      case before_parameter_value:
        if (!is_hws_character(c)) {
          if (c == '"') {
            state = parameter_quoted_value;
            continue; // skip "
          } else if (is_valid_parameter_value_character(c))
            state = parameter_value;
          else
            throw std::runtime_error{"dmitigr::mulf: invalid header value"};
        } else
          continue; // skip HWS
        break;

      case parameter_value:
        if (is_hws_character(c) || c == '\r') {
          process_parameter_value(std::move(extracted));
          extracted = {};
          state = is_hws_character(c) ? before_parameter_name : cr;
          continue; // skip HWS or CR
        } else if (!is_valid_parameter_value_character(c))
          throw std::runtime_error{"dmitigr::mulf: invalid header value"};
        break;

      case parameter_quoted_value:
        if (c == '"') {
          state = parameter_quoted_value_quote;
          continue; // skip "
        } else if (c == '\\') {
          state = parameter_quoted_value_bslash;
          continue; // skip back-slash
        } else if (!is_valid_parameter_quoted_value_character(c))
          throw std::runtime_error{"dmitigr::mulf: invalid header value"};
        break;

      case parameter_quoted_value_quote:
        if (is_hws_character(c) || is_valid_parameter_name_character(c) || c == '\r') {
          process_parameter_value(std::move(extracted));
          extracted = {};
          state = (c != '\r') ? before_parameter_name : cr;
          if (is_hws_character(c) || c == '\r')
            continue; // skip HWS or CR
        } else
          throw std::runtime_error{"dmitigr::mulf: invalid header value"};
        break;

      case parameter_quoted_value_bslash:
        if (c == '"')
          state = parameter_quoted_value;
        else
          throw std::runtime_error{"dmitigr::mulf: invalid header value"};
        break;

      case cr:
        if (c == '\n') {
          state = crlf;
          continue; // skip LF
        }
        throw std::runtime_error{"dmitigr::mulf: expected CRLF not found"};

      case crlf:
        if (c == '\r') {
          state = crlfcr;
          continue; // skip CR
        } else
          state = name;
        break;

      case crlfcr:
        if (c == '\n') {
          state = crlfcrlf; // done
          continue; // skip LF
        }
        throw std::runtime_error{"dmitigr::mulf: expected CRLFCRLF not found"};

      case crlfcrlf:
        DMITIGR_ASSERT_ALWAYS(!true);
      }

      extracted += c;
    }

    if (entry.name().empty() || !form_data_extracted || state != crlfcrlf)
      throw std::runtime_error{"dmitigr::mulf: invalid MIME-part-headers"};

    DMITIGR_ASSERT(entry.is_invariant_ok());

    return pos;
  }

  /**
   * @brief Creates the view to the `data` by the specified positions and sets
   * it as the content of the `entry`.
   */
  static void set_content(Entry& entry, const std::string& data,
    const std::string::size_type beg, const std::string::size_type end)
  {
    DMITIGR_ASSERT(beg < end && end < data.size());

    entry.content_ = std::string_view{data.data() + beg, end - beg};

    DMITIGR_ASSERT(entry.is_invariant_ok());
  }
};

} // namespace dmitigr::mulf

#endif // DMITIGR_MULF_FORM_DATA_HPP
