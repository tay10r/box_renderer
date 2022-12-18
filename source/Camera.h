#pragma once

#include <bvh/v2/vec.h>

#include <nlohmann/json_fwd.hpp>

#include <glm/glm.hpp>

namespace mcr {

struct Camera final
{
  double field_of_view{ glm::radians(65.0) };

  bvh::v2::Vec<double, 3> position{ 0, 0, 0 };

  bvh::v2::Vec<double, 3> target{ 0, 0, 1 };

  int resolution_x = 256;

  int resolution_y = 192;

  int samples_per_pixel = 16;

  std::string output_path;

  explicit Camera(const nlohmann::json& node);
};

} // namespace mcr
