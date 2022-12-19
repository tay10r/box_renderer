#include "Camera.h"

#include "Utility.h"
#include "Scalar.h"

#include <nlohmann/json.hpp>

namespace mcr {

Camera::Camera(const nlohmann::json& node)
  : field_of_view(node["FieldOfView"].get<Scalar>())
    , position(get_xyz(node["Position"]))
    , target(get_xyz(node["Target"]))
    , resolution_x(node["ResolutionX"].get<int>())
    , resolution_y(node["ResolutionY"].get<int>())
    , samples_per_pixel(node["SamplesPerPixel"].get<int>())
    , output_path(node["OutputPath"])
{
}

} // namespace mcr
