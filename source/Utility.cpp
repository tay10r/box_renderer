#include "Utility.h"

#include <nlohmann/json.hpp>

namespace mcr {

bvh::v2::Vec<Scalar, 3>
get_xyz(const nlohmann::json& node)
{
  return { node["X"].get<Scalar>(),
           node["Y"].get<Scalar>(),
           node["Z"].get<Scalar>() };
}

} // namespace mcr
