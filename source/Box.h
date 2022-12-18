#pragma once

#include <bvh/v2/vec.h>

#include <nlohmann/json.hpp>

namespace mcr {

struct Box final
{
  bvh::v2::Vec<double, 3> position;

  size_t style_index{ 0 };

  explicit Box(const nlohmann::json& node);
};

} // namespace mcr
