#pragma once

#include <bvh/v2/vec.h>

#include <nlohmann/json_fwd.hpp>

#include <vector>

#include "Utility.h"

namespace mcr {

class Texture final
{
public:
  using Color = bvh::v2::Vec<double, 4>;

  Texture(const nlohmann::json& node);

  Texture(Texture&& other) = default;

  Texture(int w, int h, std::vector<Color> color);

  Color at(double u, double v) const
  {
    const int x = clamp(static_cast<int>(u * width_), 0, width_ - 1);
    const int y = clamp(static_cast<int>(v * height_), 0, height_ - 1);
    return color_[(y * width_) + x];
  }

  int pixel_count() const { return width_ * height_; }

  int width() const { return width_; }

  int height() const { return height_; }

  bool save_png(const char* path) const;

private:
  std::vector<Color> color_;

  const int width_;

  const int height_;
};

} // namespace mcr
