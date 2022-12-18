#pragma once

#include <nlohmann/json_fwd.hpp>

#include <bvh/v2/vec.h>

namespace mcr {

template<typename T>
T
min(T a, T b)
{
  return a < b ? a : b;
}
template<typename T>
T
max(T a, T b)
{
  return a > b ? a : b;
}

template<typename T>
T
clamp(T x, T min_value, T max_value)
{
  return max(min(x, max_value), min_value);
}

bvh::v2::Vec<double, 3>
get_xyz(const nlohmann::json& node);

} // namespace mcr
