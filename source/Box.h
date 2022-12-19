#pragma once

#include <bvh/v2/vec.h>

#include <nlohmann/json.hpp>

#include "Scalar.h"

namespace mcr {

struct Box final
{
  bvh::v2::Vec<Scalar, 3> position;

  size_t style_index{ 0 };

  explicit Box(const nlohmann::json& node);
};

} // namespace mcr
