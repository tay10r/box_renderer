#pragma once

#include <filesystem>

#include <bvh/v2/vec.h>

#include <glm/glm.hpp>

#include "Box.h"
#include "BoxStyle.h"
#include "Camera.h"
#include "Renderer.h"

namespace mcr {

struct SceneData final
{
  std::vector<Camera> cameras;

  std::vector<BoxStyle> box_styles;

  std::vector<Box> diffuse_boxes;

  std::vector<Box> emissive_boxes;
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
