// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt

#include "../../testo.hpp"
#include "../../rajson.hpp"

namespace rajson = dmitigr::rajson;
namespace testo = dmitigr::testo;

int main(const int, const char* const argv[])
try {
  using rajson::to;
  rapidjson::Document doc{rapidjson::kObjectType};
  auto& alloc = doc.GetAllocator();

  {
    std::vector<int> vi{1,2,3};
    auto val = to<rapidjson::Value>(vi, alloc);
    auto vi_copy = to<std::vector<int>>(val);
    ASSERT(vi == vi_copy);
  }

  {
    std::vector<std::optional<int>> vi{1,std::nullopt,3};
    auto val = to<rapidjson::Value>(vi, alloc);
    auto vi_copy = to<std::vector<std::optional<int>>>(val);
    ASSERT(vi == vi_copy);
  }

  {
    std::vector<float> vf{1.0,2.0,3.0};
    auto val = to<rapidjson::Value>(vf, alloc);
    auto vf_copy = to<std::vector<float>>(val);
    ASSERT(vf == vf_copy);
  }

  {
    std::vector<std::optional<float>> vf{1.0,std::nullopt,3.0};
    auto val = to<rapidjson::Value>(vf, alloc);
    auto vf_copy = to<std::vector<std::optional<float>>>(val);
    ASSERT(vf == vf_copy);
  }
} catch (const std::exception& e) {
  testo::report_failure(argv[0], e);
} catch (...) {
  testo::report_failure(argv[0]);
}
