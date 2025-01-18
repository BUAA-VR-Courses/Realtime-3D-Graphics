#include "raytracing/hit.h"

namespace RayTracing {


bool HittableList::hit(const Ray& r, Interval ray_t, Record& rec) const {
    Record temp_rec;
    bool hit_anything = false;
    auto closest_so_far = ray_t.max;

    for (const auto& object : objects) {
        if (object->hit(r, Interval(ray_t.min, closest_so_far), temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }

    return hit_anything;
}


bool Sphere::hit(const Ray& r, Interval ray_t, Record& rec) const {
    // TODO: 4.3 Sphere & ray intersection
    return false;
}


bool Quad::hit(const Ray& r, Interval ray_t, Record& rec) const {
    // TODO: 4.3 Quad & ray intersection
    return false;
}

// Returns the 3D box (six sides) that contains the two opposite vertices a & b.
std::shared_ptr<HittableList> box(const vecf3& a, const vecf3& b, std::shared_ptr<Material> mat) {
    auto sides = std::make_shared<HittableList>();

    // TODO: Use the Quad class to create the six sides of the box.
    // `a` and `b` are the two opposite vertices of the box.

    return sides;
}


bool ConstantMedium::hit(const Ray& r, Interval ray_t, Record& rec) const {
    // TODO: 4.3 Constant Medium & ray intersection
    return false;
}


bool Translate::hit(const Ray& r, Interval ray_t, Record& rec) const {
    // Move the ray backwards by the offset
    Ray offset_r(r.origin - offset, r.direction, r.time);

    // Determine whether an intersection exists along the offset ray (and if so, where)
    if (!object->hit(offset_r, ray_t, rec)) {
        return false;
    }

    // Move the intersection point forwards by the offset
    rec.p += offset;

    return true;
}


bool RotateY::hit(const Ray& r, Interval ray_t, Record& rec) const {
    // Transform the ray from world space to object space.
    auto origin = vecf3(
        (cos_theta * r.origin.x()) - (sin_theta * r.origin.z()),
        r.origin.y(),
        (sin_theta * r.origin.x()) + (cos_theta * r.origin.z())
    );

    auto direction = vecf3(
        (cos_theta * r.direction.x()) - (sin_theta * r.direction.z()),
        r.direction.y(),
        (sin_theta * r.direction.x()) + (cos_theta * r.direction.z())
    );

    Ray rotated_r(origin, direction, r.time);

    // Determine whether an intersection exists in object space (and if so, where).
    if (!object->hit(rotated_r, ray_t, rec))
        return false;

    // Transform the intersection from object space back to world space.
    rec.p = vecf3(
        (cos_theta * rec.p.x()) + (sin_theta * rec.p.z()),
        rec.p.y(),
        (-sin_theta * rec.p.x()) + (cos_theta * rec.p.z())
    );

    rec.normal = vecf3(
        (cos_theta * rec.normal.x()) + (sin_theta * rec.normal.z()),
        rec.normal.y(),
        (-sin_theta * rec.normal.x()) + (cos_theta * rec.normal.z())
    );

    return true;
}


bool BVHNode::hit(const Ray& r, Interval ray_t, Record& rec) const {
    // TODO: 4.4 Using BVH to accelerate ray tracing
    return false;
}

}