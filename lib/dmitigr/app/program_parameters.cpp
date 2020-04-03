// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or app.hpp

#include "dmitigr/app/program_parameters.hpp"
#include "dmitigr/util/debug.hpp"

namespace dmitigr::app::detail {

/**
 * @brief An implementation of Program_parameters.
 */
class iProgram_parameters final : public Program_parameters {
public:
  iProgram_parameters(const int argc, const char* const* argv)
  {
    DMITIGR_REQUIRE(argc > 0 && argv && argv[0], std::invalid_argument);

    static const auto opt = [](const std::string_view arg)
      -> std::optional<std::pair<std::string, std::optional<std::string>>>
      {
        if (auto pos = arg.find("--"); pos == 0) {
          if (arg.size() == 2) {
            return std::make_pair(std::string{}, std::nullopt);
          } else if (pos = arg.find('=', 2); pos != std::string::npos) {
            auto name = arg.substr(2, pos - 2);
            auto value = arg.substr(pos + 1);
            return std::make_pair(std::string{name}, std::string{value});
          } else
            return std::make_pair(std::string{arg.substr(2)}, std::nullopt);
        } else
          return std::nullopt;
      };

    executable_path_.assign(argv[0]);

    if (argc == 1)
      return;

    int argi = 1;

    // Extracting a command name
    if (auto cmd = opt(argv[argi]); !cmd) {
      command_name_ = std::string{argv[argi]};
      ++argi;
    }

    // Collecting options
    for (; argi < argc; ++argi) {
      if (auto o = opt(argv[argi])) {
        if (o->first.empty()) {
          ++argi;
          break;
        } else
          options_[std::move(o->first)] = std::move(o->second);
      } else
        break;
    }

    // Collecting arguments
    for (; argi < argc; ++argi)
      arguments_.emplace_back(argv[argi]);

    DMITIGR_ASSERT(is_invariant_ok());
  }

  const std::filesystem::path& executable_path() const override
  {
    return executable_path_;
  }

  const std::optional<std::string>& command_name() const override
  {
    return command_name_;
  }

  const Option_map& options() const override
  {
    return options_;
  }

  const Argument_vector& arguments() const override
  {
    return arguments_;
  }

  bool has_option(const std::string& name) override
  {
    DMITIGR_REQUIRE(!name.empty(), std::invalid_argument);
    return options_.find(name) != cend(options_);
  }

private:
  std::filesystem::path executable_path_;
  std::optional<std::string> command_name_;
  Option_map options_;
  Argument_vector arguments_;

  bool is_invariant_ok() const
  {
    return !executable_path_.empty();
  }
};

} // namespace dmitigr::app::detail

namespace dmitigr::app {

DMITIGR_APP_INLINE std::unique_ptr<Program_parameters> Program_parameters::make(const int argc, const char* const* argv)
{
  return std::make_unique<detail::iProgram_parameters>(argc, argv);
}

} // namespace dmitigr::app
