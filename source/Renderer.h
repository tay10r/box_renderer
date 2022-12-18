#pragma once

#include <memory>
#include <vector>

namespace mcr {

struct SceneData;

class Renderer
{
public:
  static std::unique_ptr<Renderer> create(const SceneData&);

  virtual ~Renderer() = default;

  /// @brief Renders one sample per pixel on all incomplete images.
  virtual void iterate() = 0;

  /// @returns True when all images have completed rendering.
  virtual bool done() const = 0;

  /// @returns The progress (between 0 and 1) of each camera rendering.
  virtual std::vector<float> progress() const = 0;
};

} // namespace mcr
