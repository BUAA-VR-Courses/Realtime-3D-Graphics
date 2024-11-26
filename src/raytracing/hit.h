#ifndef RAYTRACING_HIT_H
#define RAYTRACING_HIT_H

#include <algorithm>
#include <memory>
#include <vector>

#include "raytracing/aabb.h"
#include "raytracing/material.h"
#include "raytracing/record.h"
#include "utils/tools.h"

namespace RayTracing {

class Hittable {
public:
    virtual ~Hittable() = default;

    virtual bool hit(const Ray& r, Interval ray_t, Record& rec) const = 0;

    virtual AABB bounding_box() const = 0;
};


class HittableList : public Hittable {
public:
    std::vector<std::shared_ptr<Hittable>> objects;

    HittableList() {}
    HittableList(std::shared_ptr<Hittable> object) { add(object); }

    void clear() { objects.clear(); }

    void add(std::shared_ptr<Hittable> object) {
        objects.push_back(object);
        bbox = AABB(bbox, object->bounding_box());
    }

    bool hit(const Ray& r, Interval ray_t, Record& rec) const override;

    AABB bounding_box() const override { return bbox; }

private:
    AABB bbox;
};


class Sphere : public Hittable {
public:
    // Stationary Sphere
    Sphere(const vecf3& static_center, double radius, std::shared_ptr<Material> mat)
      : center(static_center, vecf3(0,0,0)), radius(std::fmax(0,radius)), mat(mat) {
        auto rvec = vecf3(radius, radius, radius);
        bbox = AABB(static_center - rvec, static_center + rvec);
    }

    // Moving Sphere
    Sphere(const vecf3& center1, const vecf3& center2, double radius, std::shared_ptr<Material> mat)
      : center(center1, center2 - center1), radius(std::fmax(0,radius)), mat(mat) {
        auto rvec = vecf3(radius, radius, radius);
        AABB box1(center.at(0) - rvec, center.at(0) + rvec);
        AABB box2(center.at(1) - rvec, center.at(1) + rvec);
        bbox = AABB(box1, box2);
    }

    bool hit(const Ray& r, Interval ray_t, Record& rec) const override;

    AABB bounding_box() const override { return bbox; }

private:
    Ray center;
    double radius;
    std::shared_ptr<Material> mat;
    AABB bbox;

    static void get_sphere_uv(const vecf3& p, double& u, double& v) {
        // p: a given point on the sphere of radius one, centered at the origin.
        // u: returned value [0,1] of angle around the Y axis from X=-1.
        // v: returned value [0,1] of angle from Y=-1 to Y=+1.
        //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
        //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
        //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

        auto theta = std::acos(-p.y());
        auto phi = std::atan2(-p.z(), p.x()) + pi;

        u = phi / (2*pi);
        v = theta / pi;
    }
};


class Quad : public Hittable {
public:
    Quad(const vecf3& Q, const vecf3& u, const vecf3& v, std::shared_ptr<Material> mat)
      : Q(Q), u(u), v(v), mat(mat) {
        auto n = u.cross(v);
        normal = unit_vector(n);
        D = normal.dot(Q);
        w = n / n.dot(n);

        set_bounding_box();
    }

    void set_bounding_box() {
        // Compute the bounding box of all four vertices.
        auto bbox_diagonal1 = AABB(Q, Q + u + v);
        auto bbox_diagonal2 = AABB(Q + u, Q + v);
        bbox = AABB(bbox_diagonal1, bbox_diagonal2);
    }

    bool is_interior(double a, double b, Record& rec) const {
        static const Interval unit_interval = Interval(0, 1);
        // Given the hit point in plane coordinates, return false if it is outside the
        // primitive, otherwise set the hit record UV coordinates and return true.

        if (!unit_interval.contains(a) || !unit_interval.contains(b)) {
            return false;
        }

        rec.u = a;
        rec.v = b;
        return true;
    }

    bool hit(const Ray& r, Interval ray_t, Record& rec) const override;

    AABB bounding_box() const override { return bbox; }

private:
    vecf3 Q;
    vecf3 u, v;
    vecf3 w;
    std::shared_ptr<Material> mat;
    AABB bbox;
    vecf3 normal;
    double D;
};

// Returns the 3D box (six sides) that contains the two opposite vertices a & b.
std::shared_ptr<HittableList> box(const vecf3& a, const vecf3& b, std::shared_ptr<Material> mat);


class ConstantMedium : public Hittable {
public:
    ConstantMedium(std::shared_ptr<Hittable> boundary, double density, std::shared_ptr<Texture> tex)
      : boundary(boundary), neg_inv_density(-1 / density),
        phase_function(std::make_shared<Isotropic>(tex)) {}

    ConstantMedium(std::shared_ptr<Hittable> boundary, double density, const vecf3& albedo)
      : boundary(boundary), neg_inv_density(-1 / density),
        phase_function(std::make_shared<Isotropic>(albedo)) {}

    bool hit(const Ray& r, Interval ray_t, Record& rec) const override;

    AABB bounding_box() const override { return boundary->bounding_box(); }

private:
    std::shared_ptr<Hittable> boundary;
    double neg_inv_density;
    std::shared_ptr<Material> phase_function;
};


class Translate : public Hittable {
public:
    Translate(std::shared_ptr<Hittable> object, const vecf3& offset)
      : object(object), offset(offset)
    {
        bbox = object->bounding_box() + offset;
    }

    bool hit(const Ray& r, Interval ray_t, Record& rec) const override;

    AABB bounding_box() const override { return bbox; }

private:
    std::shared_ptr<Hittable> object;
    vecf3 offset;
    AABB bbox;
};


class RotateY : public Hittable {
public:
    RotateY(std::shared_ptr<Hittable> object, double angle) : object(object) {
        auto radians = to_radian(angle);
        sin_theta = std::sin(radians);
        cos_theta = std::cos(radians);
        bbox = object->bounding_box();

        vecf3 min( infinity,  infinity,  infinity);
        vecf3 max(-infinity, -infinity, -infinity);

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    auto x = i*bbox.x.max + (1-i)*bbox.x.min;
                    auto y = j*bbox.y.max + (1-j)*bbox.y.min;
                    auto z = k*bbox.z.max + (1-k)*bbox.z.min;

                    auto newx =  cos_theta*x + sin_theta*z;
                    auto newz = -sin_theta*x + cos_theta*z;

                    vecf3 tester(newx, y, newz);

                    for (int c = 0; c < 3; c++) {
                        min[c] = std::fmin(min[c], tester[c]);
                        max[c] = std::fmax(max[c], tester[c]);
                    }
                }
            }
        }

        bbox = AABB(min, max);
    }

    bool hit(const Ray& r, Interval ray_t, Record& rec) const override;

    AABB bounding_box() const override { return bbox; }

private:
    std::shared_ptr<Hittable> object;
    double sin_theta;
    double cos_theta;
    AABB bbox;
};


class BVHNode : public Hittable {
public:
    BVHNode(HittableList list) : BVHNode(list.objects, 0, list.objects.size()) {
        // There's a C++ subtlety here. This constructor (without span indices) creates an
        // implicit copy of the hittable list, which we will modify. The lifetime of the copied
        // list only extends until this constructor exits. That's OK, because we only need to
        // persist the resulting bounding volume hierarchy.
    }

    BVHNode(std::vector<std::shared_ptr<Hittable>>& objects, size_t start, size_t end) {
        // Build the bounding box of the span of source objects.
        bbox = AABB::empty;
        for (size_t object_index=start; object_index < end; object_index++)
            bbox = AABB(bbox, objects[object_index]->bounding_box());

        int axis = bbox.longest_axis();

        auto comparator = (axis == 0) ? box_x_compare
                        : (axis == 1) ? box_y_compare
                                      : box_z_compare;

        size_t object_span = end - start;

        if (object_span == 1) {
            left = right = objects[start];
        } else if (object_span == 2) {
            left = objects[start];
            right = objects[start+1];
        } else {
            std::sort(std::begin(objects) + start, std::begin(objects) + end, comparator);

            auto mid = start + object_span/2;
            left = std::make_shared<BVHNode>(objects, start, mid);
            right = std::make_shared<BVHNode>(objects, mid, end);
        }
    }

    bool hit(const Ray& r, Interval ray_t, Record& rec) const override;

    AABB bounding_box() const override { return bbox; }

private:
    std::shared_ptr<Hittable> left;
    std::shared_ptr<Hittable> right;
    AABB bbox;

    static bool box_compare(
        const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b, int axis_index
    ) {
        auto a_axis_interval = a->bounding_box().axis_interval(axis_index);
        auto b_axis_interval = b->bounding_box().axis_interval(axis_index);
        return a_axis_interval.min < b_axis_interval.min;
    }

    static bool box_x_compare (const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
        return box_compare(a, b, 0);
    }

    static bool box_y_compare (const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
        return box_compare(a, b, 1);
    }

    static bool box_z_compare (const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
        return box_compare(a, b, 2);
    }
};

}

#endif // RAYTRACING_HIT_H