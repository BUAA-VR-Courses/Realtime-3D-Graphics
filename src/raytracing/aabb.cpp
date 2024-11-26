#include "raytracing/aabb.h"

namespace RayTracing {

const AABB AABB::empty    = AABB(Interval::empty,    Interval::empty,    Interval::empty);
const AABB AABB::universe = AABB(Interval::universe, Interval::universe, Interval::universe);


AABB operator+(const AABB& bbox, const vecf3& offset) {
    return AABB(bbox.x + offset.x(), bbox.y + offset.y(), bbox.z + offset.z());
}

AABB operator+(const vecf3& offset, const AABB& bbox) {
    return bbox + offset;
}

}
