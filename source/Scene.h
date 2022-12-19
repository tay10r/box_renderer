#pragma once

#include <filesystem>

#include <glm/glm.hpp>

#include "Box.h"
#include "BoxStyle.h"
#include "Camera.h"
#include "Renderer.h"
#include "Bvh.h"

namespace mcr {

struct Material final
{
  const Texture* texture{ nullptr };

  bool is_light{ false };
};

struct SceneData final
{
  std::vector<Camera> cameras;

  std::vector<BoxStyle> box_styles;

  Vec3 background{ 0, 0, 0 };

  std::vector<Box> boxes;
};

class Scene final
{
public:
  bool load(const std::filesystem::path& path,
            void* caller,
            void (*error_callback)(void*, const char* err));

  std::unique_ptr<Renderer> start_rendering() const;

private:
  SceneData scene_data_;
};

} // namespace mcr
