#include "Box.h"

#include <nlohmann/json.hpp>

#include "Utility.h"

namespace mcr {

Box::Box(const nlohmann::json& node)
  : position(get_xyz(node["Position"]))
  , style_index(node["StyleIndex"].get<std::size_t>())
{
}

} // namespace mcr
