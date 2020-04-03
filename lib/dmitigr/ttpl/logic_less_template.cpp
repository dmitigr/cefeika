// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or ttpl.hpp

#include "dmitigr/ttpl/logic_less_template.hpp"
#include <dmitigr/util/debug.hpp>

#include <algorithm>
#include <locale>
#include <list>
#include <vector>

namespace dmitigr::ttpl::detail {

/**
 * @brief Implementation of Logic_less_template_parameter.
 */
class iLogic_less_template_parameter final : public Logic_less_template_parameter {
public:
  /**
   * @brief The constructor.
   */
  explicit iLogic_less_template_parameter(std::string name, std::optional<std::string> value = {})
    : name_{name}
    , value_{value}
  {}

  const std::string& name() const override
  {
    return name_;
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
  std::string name_;
  std::optional<std::string> value_;
};

/**
 * @brief The Logic_less_template implementation.
 */
class iLogic_less_template final : public Logic_less_template {
public:
  /**
   * @brief The alias of iLogic_less_template_parameter.
   */
  using iParameter = iLogic_less_template_parameter;

  /**
   * @brief See Logic_less_template::make().
   */
  explicit iLogic_less_template(const std::string_view input)
  {
    if (input.empty()) {
      DMITIGR_ASSERT(is_invariant_ok());
      return;
    }

    enum { text, lbrace1, lbrace2, parameter, space_after_parameter, rbrace1, rbrace2 } state = text;

    static const auto is_valid_parameter_name_character = [](const char c)
    {
      static const std::locale l{"C"};
      return std::isalnum(c, l) || (c == '_') || (c == '-');
    };

    std::string extracted_text;
    std::string extracted_parameter;

    const auto store_extracted_text = [&]()
    {
      fragments_.emplace_back(Fragment::text, std::move(extracted_text));
      extracted_text = {};
    };

    const auto store_extracted_parameter = [&]()
    {
      if (!extracted_parameter.empty()) {
        // Since equally named parameters will share the same value,
        // we must check the presentence of the parameter in parameters_.
        if (const auto index = parameter_index(extracted_parameter); !index)
          parameters_.emplace_back(extracted_parameter, std::nullopt);

        fragments_.emplace_back(Fragment::parameter, std::move(extracted_parameter));

        extracted_parameter = {};
      }
    };

    for (const char c : input) {
      switch (state) {
      case text:
        if (c == '{') {
          state = lbrace1;
          continue; // skip {
        }
        break;

      case lbrace1:
        if (c == '{') {
          state = lbrace2;
          continue; // skip {
        } else {
          state = text;
          extracted_text += '{'; // restore skipped {
        }
        break;

      case lbrace2:
        if (c == ' ') {
          state = parameter;
          continue; // skip space
        } else if (c == '{') {
          state = lbrace2;
          // { will be restored at the end of loop
        } else {
          state = text;
          extracted_text += "{{"; // restore skipped {{
        }
        break;

      case parameter:
        if (c == ' ') {
          state = space_after_parameter;
          continue; // skip space
        } else if (is_valid_parameter_name_character(c)) {
          extracted_parameter += c;
          continue; // c is already stored
        } else {
          state = text;
          extracted_text.append("{{ ").append(extracted_parameter); // = "extracted_text {{ extracted_parameter"
          extracted_parameter.clear();
        }
        break;

      case space_after_parameter:
        if (c == '}') {
          state = rbrace1;
          continue; // skip }
        } else {
          state = text;
          extracted_text.append("{{ ").append(extracted_parameter).append(" "); // = "extracted_text {{ extracted_parameter "
          extracted_parameter.clear();
        }
        break;

      case rbrace1:
        if (c == '}') {
          state = rbrace2;
          continue; // skip }
        } else {
          state = text;
          extracted_text.append("{{ ").append(extracted_parameter).append(" }"); // = "extracted_text {{ extracted_parameter }"
          extracted_parameter.clear();
        }
        break;

      case rbrace2:
        state = text;
        store_extracted_text();
        store_extracted_parameter();
        break;
      }

      extracted_text += c;
    }

    if (state == rbrace2) {
      store_extracted_text();
      store_extracted_parameter();
    } else if (!extracted_parameter.empty())
      extracted_text.append(extracted_parameter);

    if (!extracted_text.empty())
      store_extracted_text();

    DMITIGR_ASSERT(is_invariant_ok());
  }

  std::unique_ptr<Logic_less_template> to_logic_less_template() const override
  {
    return std::make_unique<iLogic_less_template>(*this);
  }

  std::size_t parameter_count() const override
  {
    return parameters_.size();
  }

  std::optional<std::size_t> parameter_index(const std::string_view name) const override
  {
    const auto b = cbegin(parameters_);
    const auto e = cend(parameters_);
    const auto i = std::find_if(b, e, [&](const auto& p) { return p.name() == name; });
    return (i != e) ? std::make_optional(i - b) : std::nullopt;
  }

  std::size_t parameter_index_throw(const std::string_view name) const override
  {
    const auto result = parameter_index(name);
    DMITIGR_REQUIRE(result, std::out_of_range,
      "the instance of dmitigr::ttpl::Logic_less_template has no parameter \"" + std::string{name} + "\"");
    return *result;
  }

  const iParameter* parameter(const std::size_t index) const override
  {
    DMITIGR_REQUIRE(index < parameter_count(), std::out_of_range,
      "invalid parameter index (" + std::to_string(index) + ")"
      " of the dmitigr::ttpl::Logic_less_template instance");
    return &parameters_[index];
  }

  iParameter* parameter(const std::size_t index) override
  {
    return const_cast<iParameter*>(static_cast<const iLogic_less_template*>(this)->parameter(index));
  }

  const iParameter* parameter(const std::string_view name) const override
  {
    const auto index = parameter_index_throw(name);
    return &parameters_[index];
  }

  iParameter* parameter(const std::string_view name) override
  {
    return const_cast<iParameter*>(static_cast<const iLogic_less_template*>(this)->parameter(name));
  }

  bool has_parameter(const std::string_view name) const override
  {
    return static_cast<bool>(parameter_index(name));
  }

  bool has_parameters() const override
  {
    return !parameters_.empty();
  }

  bool has_unset_parameters() const override
  {
    return std::any_of(cbegin(parameters_), cend(parameters_), [](const auto& p) {
      return !p.value();
    });
  }

  void replace_parameter(const std::string_view name, const Logic_less_template* const replacement) override
  {
    DMITIGR_REQUIRE(has_parameter(name), std::out_of_range);
    DMITIGR_REQUIRE(replacement && replacement != this, std::invalid_argument);
    const auto* const ireplacement = dynamic_cast<const iLogic_less_template*>(replacement);
    DMITIGR_ASSERT_ALWAYS(ireplacement);

    auto old_fragments = fragments_;
    auto old_parameters = parameters_;
    try {
      // Borrowing fragments.
      for (auto fi = begin(fragments_); fi != end(fragments_);) {
        if (fi->first == Fragment::parameter && fi->second == name) {
          // Firstly, we'll insert the `replacement` just before `fi`.
          fragments_.insert(fi, cbegin(ireplacement->fragments_), cend(ireplacement->fragments_));
          // Secondly, we'll erase the parameter pointed by `fi` and got the next iterator.
          fi = fragments_.erase(fi);
        } else
          ++fi;
      }

      // Removing parameters from the original which are in replacement.
      for (const auto& p : ireplacement->parameters_) {
        if (const auto i = parameter_index(p.name()))
          parameters_.erase(begin(parameters_) + *i);
      }

      // Borrowing parameters from the replacement.
      const auto idx = parameter_index(name);
      DMITIGR_ASSERT(idx);
      const auto itr = parameters_.erase(begin(parameters_) + *idx);
      parameters_.insert(itr, begin(ireplacement->parameters_), end(ireplacement->parameters_));
    } catch (...) {
      parameters_.swap(old_parameters);
      fragments_.swap(old_fragments);
      throw;
    }

    DMITIGR_ASSERT(is_invariant_ok());
  }

  void replace_parameter(const std::string_view name, const std::string_view replacement) override
  {
    iLogic_less_template r(replacement);
    replace_parameter(name, &r); // includes invariant check
  }

  std::string to_string() const override
  {
    std::string result;
    for (const auto fragment : fragments_) {
      switch (fragment.first) {
      case Fragment::text:
        result.append(fragment.second);
        break;
      case Fragment::parameter:
        result.append("{{ ").append(fragment.second).append(" }}");
        break;
      }
    }
    return result;
  }

  std::string to_output() const override
  {
    std::string result;
    for (const auto fragment : fragments_) {
      const auto& name = fragment.second;
      switch (fragment.first) {
      case Fragment::text:
        result.append(name);
        break;
      case Fragment::parameter:
        const auto& value = parameter(name)->value();
        DMITIGR_REQUIRE(value, std::logic_error,
          "the parameter \"" + name + "\""
          " of the dmitigr::ttpl::Logic_less_template instance is unset");
        result.append(value.value());
        break;
      }
    }
    return result;
  }

private:
  enum class Fragment { text, parameter };

  std::list<std::pair<Fragment, std::string>> fragments_;
  std::vector<iParameter> parameters_;

  bool is_invariant_ok() const
  {
    const bool cond1 = parameters_.empty() || !fragments_.empty();
    return cond1;
  }
};

} // namespace dmitigr::ttpl::detail

namespace dmitigr::ttpl {

DMITIGR_TTPL_INLINE std::unique_ptr<Logic_less_template> Logic_less_template::make(const std::string_view input)
{
  return std::make_unique<detail::iLogic_less_template>(input);
}

} // namespace dmitigr::ttpl
