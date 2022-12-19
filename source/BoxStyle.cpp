#include "BoxStyle.h"

#include <nlohmann/json.hpp>

namespace mcr {

BoxStyle::BoxStyle(const nlohmann::json& node, const std::filesystem::path& directory_path)
  : top_color(node["TopColor"], directory_path)
    , bottom_color(node["BottomColor"], directory_path)
    , left_color(node["LeftColor"], directory_path)
    , right_color(node["RightColor"], directory_path)
    , front_color(node["FrontColor"], directory_path)
    , back_color(node["BackColor"], directory_path)
    , box_size(get_xyz(node["BoxSize"]))
    , light_scale(node["LightScale"].get<Scalar>())
{
}

} // namespace mcr
