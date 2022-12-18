#include "Utility.h"

#include <nlohmann/json.hpp>

namespace mcr {

bvh::v2::Vec<double, 3>
get_xyz(const nlohmann::json& node)
{
  return { node["X"].get<double>(),
           node["Y"].get<double>(),
           node["Z"].get<double>() };
}

} // namespace mcr
