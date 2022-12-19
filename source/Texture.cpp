#include "Texture.h"

#include <nlohmann/json.hpp>

#include <stdexcept>

#include <stb_image.h>
#include <stb_image_write.h>

namespace mcr {

namespace {

unsigned char
get_nibble(const char c)
{
  if ((c >= '0') && (c <= '9'))
    return c - '0';
  else if ((c >= 'a') && (c <= 'f'))
    return 10 + (c - 'a');
  else if ((c >= 'A') && (c <= 'F'))
    return 10 + (c - 'A');

  throw std::runtime_error("invalid character in hex color code");
}

Scalar
get_color(const char hi, const char lo)
{
  const unsigned char color = (get_nibble(hi) << 4) | static_cast<int>(get_nibble(lo));

  return static_cast<int>(color) / static_cast<Scalar>(255);
}

Texture
from_hex_color_code(const std::string& str)
{
  if ((str.size() != 7) && (str.size() != 9))
    throw std::runtime_error("incorrectly sized hex color code");

  const auto r = get_color(str[1], str[2]);
  const auto g = get_color(str[3], str[4]);
  const auto b = get_color(str[5], str[6]);
  const auto a = (str.size() == 9) ? get_color(str[7], str[8]) : static_cast<Scalar>(1);

  constexpr auto w = 1;
  constexpr auto h = 1;
  std::vector<Texture::Color> colors;
  colors.resize(1);
  colors[0] = Texture::Color(r, g, b, a);

  return { w, h, std::move(colors) };
}

Texture
from_image_path(const std::string& str, const std::filesystem::path& directory_path)
{
  const auto full_path = directory_path / std::filesystem::path(str);

  const auto full_path_str = full_path.string();

  int w = 0;
  int h = 0;

  auto* pixels = stbi_load(full_path_str.c_str(), &w, &h, nullptr, 4);

  if (pixels == nullptr) {
    constexpr auto w = 1;
    constexpr auto h = 1;
    std::vector<Texture::Color> colors;
    colors.resize(1);
    colors[0] = Texture::Color(1, 0, 0, 1);
    return { w, h, std::move(colors) };
  }

  std::vector<Texture::Color> colors(w * h);

  for (int i = 0; i < (w * h); i++) {
    colors[i][0] = static_cast<Scalar>(static_cast<int>(pixels[(i * 4) + 0])) / 255;
    colors[i][1] = static_cast<Scalar>(static_cast<int>(pixels[(i * 4) + 1])) / 255;
    colors[i][2] = static_cast<Scalar>(static_cast<int>(pixels[(i * 4) + 2])) / 255;
    colors[i][3] = static_cast<Scalar>(static_cast<int>(pixels[(i * 4) + 3])) / 255;
  }

  stbi_image_free(pixels);

  return { w, h, std::move(colors) };
}

Texture
from_json_node(const nlohmann::json& node, const std::filesystem::path& directory_path)
{
  const auto str = node.get<std::string>();

  if (str.empty()) {
    throw std::runtime_error("Texture string is empty.");
  }

  if (str[0] == '#')
    return from_hex_color_code(str);

  return from_image_path(str, directory_path);
}

} // namespace

Texture::Texture(const nlohmann::json& node, const std::filesystem::path& directory_path)
  : Texture(from_json_node(node, directory_path))
{
}

Texture::Texture(const int w, const int h, std::vector<Color> color)
  : color_(std::move(color))
    , width_(w)
    , height_(h)
{
}

bool
Texture::save_png(const char* path) const
{
  std::vector<unsigned char> color(width_ * height_ * 4);

  for (int i = 0; i < (width_ * height_); i++) {

    const auto r = clamp(static_cast<int>(color_[i][0] * 255), 0, 255);
    const auto g = clamp(static_cast<int>(color_[i][1] * 255), 0, 255);
    const auto b = clamp(static_cast<int>(color_[i][2] * 255), 0, 255);
    const auto a = clamp(static_cast<int>(color_[i][3] * 255), 0, 255);

    color[(i * 4) + 0] = static_cast<unsigned char>(r);
    color[(i * 4) + 1] = static_cast<unsigned char>(g);
    color[(i * 4) + 2] = static_cast<unsigned char>(b);
    color[(i * 4) + 3] = static_cast<unsigned char>(a);
  }

  return stbi_write_png(path, width_, height_, 4, color.data(), width_ * 4);
}

} // namespace mcr
