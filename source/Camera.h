#pragma once

#include <bvh/v2/vec.h>

#include <nlohmann/json_fwd.hpp>

#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include "Scalar.h"

namespace mcr {

struct RayGenerator final
{
  bvh::v2::Vec<Scalar, 3> dir;

  bvh::v2::Vec<Scalar, 3> dx;

  bvh::v2::Vec<Scalar, 3> dy;

  [[nodiscard]] bvh::v2::Vec<Scalar, 3> generate(const Scalar x, const Scalar y) const
  {
    return normalize(dir + (dx * x) + (dy * y));
  }
};

struct Camera final
{
  Scalar field_of_view{ glm::radians(static_cast<Scalar>(65)) };

  bvh::v2::Vec<Scalar, 3> position{ 0, 0, 0 };

  bvh::v2::Vec<Scalar, 3> target{ 0, 0, 1 };

  int resolution_x = 256;

  int resolution_y = 192;

  int samples_per_pixel = 16;

  std::string output_path;

  explicit Camera(const nlohmann::json& node);

  [[nodiscard]] RayGenerator get_ray_generator() const
  {
    const auto dir = normalize(target - position);

    const auto up = bvh::v2::Vec<Scalar, 3>(0, 1, 0);

    const auto right = normalize(cross(up, dir));

    const auto down = normalize(cross(right, dir));

    return { dir, right, down };
  }

private:
  static bvh::v2::Vec<Scalar, 3> to_bvh_vec(const glm::vec4& in)
  {
    return { in[0], in[1], in[2] };
  }

  static glm::vec3 to_glm(const bvh::v2::Vec<Scalar, 3> in)
  {
    return { in[0], in[1], in[2] };
  }
};

} // namespace mcr
