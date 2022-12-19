#pragma once

#include <bvh/v2/vec.h>

#include <nlohmann/json_fwd.hpp>

#include <filesystem>
#include <vector>

#include "Utility.h"
#include "Scalar.h"

namespace mcr {

class Texture final
{
public:
  using Color = bvh::v2::Vec<Scalar, 4>;

  explicit Texture(const nlohmann::json& node, const std::filesystem::path& directory_path);

  Texture(Texture&& other) = default;

  Texture(int w, int h, std::vector<Color> color);

  [[nodiscard]] auto at(const Scalar u, const Scalar v) const
  {
    const int x = clamp(static_cast<int>(u * width_), 0, width_ - 1);
    const int y = clamp(static_cast<int>(v * height_), 0, height_ - 1);
    return color_[(y * width_) + x];
  }

  [[nodiscard]] auto pixel_count() const { return width_ * height_; }

  [[nodiscard]] auto width() const { return width_; }

  [[nodiscard]] auto height() const { return height_; }

  [[nodiscard]] bool save_png(const char* path) const;

private:
  std::vector<Color> color_;

  const int width_;

  const int height_;
};

} // namespace mcr
