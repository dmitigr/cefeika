// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or url.hpp

#include "dmitigr/url/query_string.hpp"
#include "dmitigr/url/implementation_header.hpp"

#include <dmitigr/util/debug.hpp>
#include <dmitigr/util/string.hpp>

#include <algorithm>
#include <limits>
#include <locale>
#include <stdexcept>
#include <utility>
#include <vector>

namespace dmitigr::url::detail {

/**
 * @brief The Query_string::Parameter implementation.
 */
class iQuery_string_parameter final : public Query_string_parameter {
public:
  /**
   * @brief The constructor.
   */
  explicit iQuery_string_parameter(std::string name, std::optional<std::string> value = {})
    : name_{std::move(name)}
    , value_{std::move(value)}
  {
    DMITIGR_ASSERT(is_invariant_ok());
  }

  const std::string& name() const override
  {
    return name_;
  }

  void set_name(std::string name) override
  {
    name_ = std::move(name);
  }

  const std::optional<std::string>& value() const override
  {
    return value_;
  }

  void set_value(std::optional<std::string> value) override
  {
    value_ = std::move(value);
  }

private:
  friend iQuery_string;

  std::string name_;
  std::optional<std::string> value_;

  iQuery_string_parameter() = default; // constructs the invalid object!

  bool is_invariant_ok() const
  {
    return !name_.empty();
  }
};

/**
 * @brief The Query_string implementation.
 */
class iQuery_string final : public Query_string {
public:
  /**
   * @brief The alias of iQuery_string_parameter.
   */
  using iParameter = iQuery_string_parameter;

  /**
   * @brief See Query_string::make().
   */
  explicit iQuery_string(const std::string_view input)
  {
    if (input.empty()) {
      DMITIGR_ASSERT(is_invariant_ok());
      return;
    }

    enum { param, param_hex, value, value_hex } state = param;
    constexpr char sep{'&'};
    constexpr char eq{'='};
    std::string hex;

    static const auto is_hexademical_character = [](char c)
    {
      static const char allowed[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
      static const std::locale l{"C"};
      c = std::tolower(c, l);
      return std::any_of(std::cbegin(allowed), std::cend(allowed), [c](const char ch) { return ch == c; });
    };

    const auto append_invalid_parameter = [&]()
    {
      parameters_.emplace_back(iParameter{});
    };

    DMITIGR_ASSERT(!input.empty());
    append_invalid_parameter();
    std::string* extracted = &parameters_.back().name_; // extracting the name first
    for (const auto c : input) {
      switch (state) {
      case param:
        if (c == eq) {
          if (extracted->empty())
            throw std::runtime_error{"dmitigr::url: parameter name is empty"};

          parameters_.back().value_ = std::string{}; // the value is empty but not null now
          extracted = &parameters_.back().value_.value(); // extracting the value now
          state = value;
          continue; // skip eq
        }
        [[fallthrough]];

      case value /* or param */:
        if (c == sep) {
          append_invalid_parameter();
          extracted = &parameters_.back().name_; // extracting the name now
          state = param;
          continue; // skip sep
        } else if (is_simple_character(c) || c == '~') {
          *extracted += c; // store as is
        } else if (c == '+') {
          *extracted += ' ';
        } else if (c == '%') {
          DMITIGR_ASSERT(state == param || state == value);
          state = (state == param) ? param_hex : value_hex;
          continue; // skip %
        } else
          throw std::runtime_error{"dmitigr::url: unallowed character"};
        break;

      case param_hex:
        [[fallthrough]];

      case value_hex /* or param_hex */:
        if (is_hexademical_character(c)) {
          DMITIGR_ASSERT(hex.size() < 2);
          hex += c;
          if (hex.size() == 2) {
            // Note: hex == "20" - space, hex == "2B" - +.
            std::size_t pos{};
            const int code = std::stoi(hex, &pos, 16);
            DMITIGR_ASSERT(pos == hex.size());
            DMITIGR_ASSERT(code <= std::numeric_limits<unsigned char>::max());
            *extracted += char(code);
            hex.clear();
            DMITIGR_ASSERT(state == param_hex || state == value_hex);
            state = (state == param_hex) ? param : value;
          }
        } else
          throw std::runtime_error{"dmitigr::url: invalid code octet of percent-encoded query string"};
        break;
      }
    }

    if (parameters_.back().name().empty())
      throw std::runtime_error{"dmitigr::url: parameter name is empty"};

    DMITIGR_ASSERT(is_invariant_ok());
  }

  std::size_t parameter_count() const override
  {
    return parameters_.size();
  }

  std::optional<std::size_t> parameter_index(std::string_view name, const std::size_t offset) const override
  {
    DMITIGR_REQUIRE(offset < parameter_count(), std::out_of_range);

    const auto b = cbegin(parameters_);
    const auto e = cend(parameters_);
    const auto i = std::find_if(b + offset, e, [&](const auto& p) { return p.name() == name; });
    return i != e ? std::make_optional<std::size_t>(i - b) : std::nullopt;
  }

  const iParameter* parameter(const std::size_t index) const override
  {
    DMITIGR_REQUIRE(index < parameter_count(), std::out_of_range);

    return &parameters_[index];
  }

  iParameter* parameter(const std::size_t index) override
  {
    return const_cast<iParameter*>(static_cast<const iQuery_string*>(this)->parameter(index));
  }

  const iParameter* parameter(const std::string_view name, const std::size_t offset) const override
  {
    const auto index = parameter_index(name, offset);
    DMITIGR_REQUIRE(index, std::out_of_range);

    return &parameters_[*index];
  }

  iParameter* parameter(const std::string_view name, const std::size_t offset) override
  {
    return const_cast<iParameter*>(static_cast<const iQuery_string*>(this)->parameter(name, offset));
  }

  bool has_parameter(const std::string_view name, const std::size_t offset) const override
  {
    return bool(parameter_index(name, offset));
  }

  bool has_parameters() const override
  {
    return !parameters_.empty();
  }

  void append_parameter(std::string name, std::optional<std::string> value) override
  {
    parameters_.emplace_back(std::move(name), std::move(value));

    DMITIGR_ASSERT(is_invariant_ok());
  }

  void remove_parameter(const std::size_t index) override
  {
    DMITIGR_REQUIRE(index < parameter_count(), std::out_of_range);

    parameters_.erase(cbegin(parameters_) + index);

    DMITIGR_ASSERT(is_invariant_ok());
  }

  void remove_parameter(const std::string_view name, const std::size_t offset) override
  {
    const auto index = parameter_index(name, offset);
    DMITIGR_REQUIRE(index, std::out_of_range);

    parameters_.erase(cbegin(parameters_) + *index);

    DMITIGR_ASSERT(is_invariant_ok());
  }

  std::string to_string() const override
  {
    std::string result;

    static const auto encoded_string = [](std::string_view str)
    {
      std::string result;
      for (const auto c : str) {
        // Note: tilde ('~') is permitted in query string by
        // RFC3986, but must be percent-encoded in HTML forms.
        if (is_simple_character(c) || c == '~')
          result += c;
        else if (c == ' ')
          result += "%20";
        else if (c == '+')
          result += "%2B";
        else
          result.append("%").append(string::to_string(static_cast<unsigned char>(c), static_cast<unsigned char>(16)));
      }
      return result;
    };

    for (const auto& p : parameters_) {
      result += encoded_string(p.name());

      if (const auto& value = p.value(); value) {
        result += '=';
        result += encoded_string(*value);
      }

      result += '&';
    }
    if (!result.empty())
      result.pop_back();

    return result;
  }

private:
  std::vector<iParameter> parameters_;

  bool is_invariant_ok() const
  {
    const bool parameters_ok = [&]()
    {
      return std::all_of(cbegin(parameters_), cend(parameters_), [](const auto& p) {
        return p.is_invariant_ok();
      });
    }();

    return parameters_ok;
  }

  /**
   * @returns `true` if the specified character `c` is a "simple" character
   * according to https://url.spec.whatwg.org/#urlencoded-serializing, or
   * `false` otherwise.
   */
  static bool is_simple_character(const char c)
  {
    static const char allowed[] = { '*', '-', '.', '_' };
    static const std::locale l{"C"};
    return std::isalnum(c, l) || std::any_of(std::cbegin(allowed), std::cend(allowed), [c](const char ch) { return ch == c; });
  };
};

} // namespace dmitigr::url::detail

namespace dmitigr::url {

DMITIGR_URL_INLINE std::unique_ptr<Query_string> Query_string::make(const std::string_view input)
{
  return std::make_unique<detail::iQuery_string>(input);
}

} // namespace dmitigr::url

#include "dmitigr/url/implementation_footer.hpp"
