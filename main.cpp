#include "source/Scene.h"

#include <filesystem>
#include <iostream>

#include <cstdlib>

using namespace mcr;

int
main(int argc, char** argv)
{
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <scene.json>" << std::endl;
    return EXIT_FAILURE;
  }

  const std::filesystem::path scene_path = argv[1];

  Scene scene;

  auto error_handler = [](void*, const char* error) {
    std::cerr << error << std::endl;
  };

  if (!scene.load(scene_path, nullptr, error_handler))
    return EXIT_FAILURE;

  auto renderer = scene.start_rendering();

  while (!renderer->done()) {

    renderer->iterate();
  }

  return EXIT_SUCCESS;
}
