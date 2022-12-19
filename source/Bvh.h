#pragma once

#include <bvh/v2/bvh.h>
#include <bvh/v2/tri.h>

#include "Scalar.h"

namespace mcr {

using BvhNode = bvh::v2::Node<Scalar, 3>;

using Bvh = bvh::v2::Bvh<BvhNode>;

using BoundingBox = bvh::v2::BBox<Scalar, 3>;

using Vec2 = bvh::v2::Vec<Scalar, 2>;

using Vec3 = bvh::v2::Vec<Scalar, 3>;

using Ray = bvh::v2::Ray<Scalar, 3>;

using Tri = bvh::v2::Tri<Scalar, 3>;

using PrecomputedTri = bvh::v2::PrecomputedTri<Scalar>;

} // namespace mcr
