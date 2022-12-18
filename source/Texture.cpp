#include "Texture.h"

#include <nlohmann/json.hpp>

#include <stdexcept>

#include <stb_image.h>
#include <stb_image_write.h>

namespace mcr {

namespace {

unsigned char
get_nibble(char c)
{
  if ((c >= '0') && (c <= '9'))
    return c - '0';
  else if ((c >= 'a') && (c <= 'f'))
    return 10 + (c - 'a');
  else if ((c >= 'A') && (c <= 'F'))
    return 10 + (c - 'A');

  throw std::runtime_error("invalid character in hex color code");
}

double
get_color(char hi, char lo)
{
  const unsigned char color = (get_nibble(hi) << 8) | get_nibble(lo);

  return color / 255.0;
}

Texture
from_hex_color_code(const std::string& str)
{
  if ((str.size() != 7) && (str.size() != 9))
    throw std::runtime_error("incorrectly sized hex color code");

  const double r = get_color(str[1], str[2]);
  const double g = get_color(str[3], str[4]);
  const double b = get_color(str[5], str[6]);
  const double a = (str.size() == 9) ? get_color(str[7], str[8]) : 1.0;

  int w = 1;
  int h = 1;
  std::vector<Texture::Color> colors;
  colors.resize(1);
  colors[0] = Texture::Color(r, g, b, a);
  return Texture(w, h, std::move(colors));
}

Texture
from_image_path(const std::string& str)
{
  int w = 1;
  int h = 1;
  std::vector<Texture::Color> colors;
  colors.resize(1);
  colors[0] = Texture::Color(1, 0, 0, 1);
  return Texture(w, h, std::move(colors));
}

Texture
from_json_node(const nlohmann::json& node)
{
  const auto str = node.get<std::string>();

  if (str.empty()) {
    throw std::runtime_error("Texture string is empty.");
  }

  if (str[0] == '#')
    return from_hex_color_code(str);

  return from_image_path(str);
}

} // namespace

Texture::Texture(const nlohmann::json& node)
  : Texture(from_json_node(node))
{
}

Texture::Texture(int w, int h, std::vector<Color> color)
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

    const auto r = clamp(static_cast<int>(color_[i][0] * 255.0), 0, 255);
    const auto g = clamp(static_cast<int>(color_[i][1] * 255.0), 0, 255);
    const auto b = clamp(static_cast<int>(color_[i][2] * 255.0), 0, 255);
    const auto a = clamp(static_cast<int>(color_[i][3] * 255.0), 0, 255);

    color[(i * 4) + 0] = static_cast<unsigned char>(r);
    color[(i * 4) + 1] = static_cast<unsigned char>(g);
    color[(i * 4) + 2] = static_cast<unsigned char>(b);
    color[(i * 4) + 3] = static_cast<unsigned char>(a);
  }

  return stbi_write_png(path, width_, height_, 4, color.data(), width_ * 4);
}

} // namespace mcr
