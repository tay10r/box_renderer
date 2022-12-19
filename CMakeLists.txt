cmake_minimum_required(VERSION 3.14.7)

project(box_renderer)

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

find_package(OpenMP REQUIRED COMPONENTS CXX)

add_executable(box_renderer
  main.cpp
  source/Scalar.h
  source/Bvh.h
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

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT box_renderer)

target_compile_features(box_renderer PRIVATE cxx_std_20)

target_link_libraries(box_renderer PRIVATE glm bvh nlohmann_json OpenMP::OpenMP_CXX)

target_include_directories(box_renderer PRIVATE ${stb_SOURCE_DIR})
