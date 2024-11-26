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
    vecf3 current_center = center.at(r.time);
    vecf3 oc = current_center - r.origin;
    auto a = r.direction.squaredNorm();
    auto h = r.direction.dot(oc);
    auto c = oc.squaredNorm() - radius * radius;

    auto discriminant = h*h - a*c;
    if (discriminant < 0)
        return false;

    auto sqrtd = std::sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    auto root = (h - sqrtd) / a;
    if (!ray_t.surrounds(root)) {
        root = (h + sqrtd) / a;
        if (!ray_t.surrounds(root))
            return false;
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    vecf3 outward_normal = (rec.p - current_center) / radius;
    rec.set_face_normal(r, outward_normal);
    get_sphere_uv(outward_normal, rec.u, rec.v);
    rec.mat = mat;

    return true;
}


bool Quad::hit(const Ray& r, Interval ray_t, Record& rec) const {
    // TODO: 4.2 Quad & ray intersection
    return false;
}

// Returns the 3D box (six sides) that contains the two opposite vertices a & b.
std::shared_ptr<HittableList> box(const vecf3& a, const vecf3& b, std::shared_ptr<Material> mat) {
    auto sides = std::make_shared<HittableList>();

    // Construct the two opposite vertices with the minimum and maximum coordinates.
    auto min = vecf3(std::fmin(a.x(),b.x()), std::fmin(a.y(),b.y()), std::fmin(a.z(),b.z()));
    auto max = vecf3(std::fmax(a.x(),b.x()), std::fmax(a.y(),b.y()), std::fmax(a.z(),b.z()));

    auto dx = vecf3(max.x() - min.x(), 0, 0);
    auto dy = vecf3(0, max.y() - min.y(), 0);
    auto dz = vecf3(0, 0, max.z() - min.z());

    sides->add(std::make_shared<Quad>(vecf3(min.x(), min.y(), max.z()),  dx,  dy, mat)); // front
    sides->add(std::make_shared<Quad>(vecf3(max.x(), min.y(), max.z()), -dz,  dy, mat)); // right
    sides->add(std::make_shared<Quad>(vecf3(max.x(), min.y(), min.z()), -dx,  dy, mat)); // back
    sides->add(std::make_shared<Quad>(vecf3(min.x(), min.y(), min.z()),  dz,  dy, mat)); // left
    sides->add(std::make_shared<Quad>(vecf3(min.x(), max.y(), max.z()),  dx, -dz, mat)); // top
    sides->add(std::make_shared<Quad>(vecf3(min.x(), min.y(), min.z()),  dx,  dz, mat)); // bottom

    return sides;
}


bool ConstantMedium::hit(const Ray& r, Interval ray_t, Record& rec) const {
    // TODO: 4.2 Constant Medium & ray intersection
    return false;
}


bool Translate::hit(const Ray& r, Interval ray_t, Record& rec) const {
    // TODO: 4.2 Translate & ray intersection
    return false;
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
    // TODO: 4.3 Using BVH to accelerate ray tracing
    return false;
}

}