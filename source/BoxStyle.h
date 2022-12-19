#pragma once

#include <nlohmann/json_fwd.hpp>

#include <filesystem>

#include "Texture.h"
#include "Scalar.h"

namespace mcr {

struct BoxStyle final
{
  Texture top_color;

  Texture bottom_color;

  Texture left_color;

  Texture right_color;

  Texture front_color;

  Texture back_color;

  bvh::v2::Vec<Scalar, 3> box_size{ 1, 1, 1 };

  Scalar light_scale{ 0 };

  explicit BoxStyle(const nlohmann::json& node, const std::filesystem::path& directory_path);
};

} // namespace mcr
