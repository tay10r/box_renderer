#pragma once

#include <nlohmann/json_fwd.hpp>

#include "Texture.h"

namespace mcr {

struct BoxStyle final
{
  Texture top_color;

  Texture bottom_color;

  Texture left_color;

  Texture right_color;

  Texture front_color;

  Texture back_color;

  double box_size_x{ 1 };

  double box_size_y{ 1 };

  explicit BoxStyle(const nlohmann::json& node);
};

} // namespace mcr
