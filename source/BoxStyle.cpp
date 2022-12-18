#include "BoxStyle.h"

#include <nlohmann/json.hpp>

namespace mcr {

BoxStyle::BoxStyle(const nlohmann::json& node)
  : top_color(node["TopColor"])
  , bottom_color(node["BottomColor"])
  , left_color(node["LeftColor"])
  , right_color(node["RightColor"])
  , front_color(node["FrontColor"])
  , back_color(node["BackColor"])
  , box_size_x(node["BoxSizeX"].get<double>())
  , box_size_y(node["BoxSizeY"].get<double>())
{
}

} // namespace mcr
