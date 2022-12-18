cmake_minimum_required(VERSION 3.14.7)

project(minecraft_style_renderer)

include(FetchContent)

FetchContent_Declare(glm URL "https://github.com/g-truc/glm/archive/refs/tags/0.9.9.8.zip")
FetchContent_MakeAvailable(glm)

set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_Declare(bvh URL "https://github.com/madmann91/bvh/archive/refs/heads/master.zip")
FetchContent_MakeAvailable(bvh)

FetchContent_Declare(nlohmann_json URL "https://github.com/nlohmann/json/archive/refs/tags/v3.11.2.zip")
FetchContent_MakeAvailable(nlohmann_json)

FetchContent_Declare(stb URL "https://github.com/nothings/stb/archive/refs/heads/master.zip")
FetchContent_MakeAvailable(stb)
FetchContent_GetProperties(stb)

add_executable(mc_render
  main.cpp
  source/Box.h
  source/Box.cpp
  source/BoxStyle.h
  source/BoxStyle.cpp
  source/Camera.h
  source/Camera.cpp
  source/Renderer.h
  source/Renderer.cpp
  source/Scene.h
  source/Scene.cpp
  source/Texture.h
  source/Texture.cpp
  source/Utility.h
  source/Utility.cpp
  source/stb.c)

target_link_libraries(mc_render PRIVATE glm bvh nlohmann_json)

target_include_directories(mc_render PRIVATE ${stb_SOURCE_DIR})
