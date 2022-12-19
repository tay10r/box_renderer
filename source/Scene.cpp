#include "Scene.h"

#include <nlohmann/json.hpp>

#include <bvh/v2/default_builder.h>

#include <fstream>

namespace mcr {

namespace {

class ErrorHandler final
{
public:
  ErrorHandler(void* caller, void (*callback)(void*, const char* err))
    : caller_(caller)
      , callback_(callback)
  {
  }

private:
  void* caller_;

  void (*callback_)(void*, const char* error);
};

class SceneLoader final
{
public:
  explicit SceneLoader(const ErrorHandler error_handler)
    : error_handler_(error_handler)
  {
  }

  SceneData get_scene_data() { return std::move(data_); }

  bool load(const std::filesystem::path& path)
  {
    std::ifstream file;

    file.exceptions(std::ios::failbit);

    file.open(path);

    const std::filesystem::path directory_path = path.parent_path();

    const auto root = nlohmann::json::parse(file);

    for (const auto& node : root["BoxStyles"])
      data_.box_styles.emplace_back(BoxStyle(node, directory_path));

    for (const auto& node : root["Cameras"])
      data_.cameras.emplace_back(Camera(node));

    for (const auto& node : root["Boxes"])
      data_.boxes.emplace_back(Box(node));

    return true;
  }

  ErrorHandler error_handler_;

  SceneData data_;
};

} // namespace

bool
Scene::load(const std::filesystem::path& path,
            void* caller,
            void (*error_callback)(void*, const char* err))
{
  try {

    const ErrorHandler error_handler(caller, error_callback);

    SceneLoader loader(error_handler);

    if (!loader.load(path))
      return false;

    scene_data_ = loader.get_scene_data();

  } catch (const std::runtime_error& err) {

    error_callback(caller, err.what());
  }

  return true;
}

std::unique_ptr<Renderer>
Scene::start_rendering() const
{
  return Renderer::create(scene_data_);
}

} // namespace mcr
