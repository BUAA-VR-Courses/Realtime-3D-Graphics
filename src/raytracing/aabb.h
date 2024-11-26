#ifndef RAYTRACING_AABB_H
#define RAYTRACING_AABB_H

#include "raytracing/ray.h"
#include "utils/tools.h"

namespace RayTracing {

class AABB {
public:
    Interval x, y, z;

    AABB() {} // The default AABB is empty, since intervals are empty by default.

    AABB(const Interval& x, const Interval& y, const Interval& z) : x(x), y(y), z(z) {
        pad_to_minimums();
    }

    AABB(const vecf3& a, const vecf3& b) {
        // Treat the two points a and b as extrema for the bounding box, so we don't require a
        // particular minimum/maximum coordinate order.
        x = (a[0] <= b[0]) ? Interval(a[0], b[0]) : Interval(b[0], a[0]);
        y = (a[1] <= b[1]) ? Interval(a[1], b[1]) : Interval(b[1], a[1]);
        z = (a[2] <= b[2]) ? Interval(a[2], b[2]) : Interval(b[2], a[2]);

        pad_to_minimums();
    }

    AABB(const AABB& box0, const AABB& box1) {
        x = Interval(box0.x, box1.x);
        y = Interval(box0.y, box1.y);
        z = Interval(box0.z, box1.z);
    }

    const Interval& axis_interval(int n) const {
        switch (n) {
        case 0: return x;
        case 1: return y;
        case 2: return z;
        default:
            return x; // This should never happen.
        }
    }

    bool hit(const Ray& r, Interval ray_t) const {
        const vecf3& ray_orig = r.origin;
        const vecf3& ray_dir  = r.direction;

        for (int axis = 0; axis < 3; axis++) {
            const Interval& ax = axis_interval(axis);
            const double adinv = 1.0 / ray_dir[axis];

            auto t0 = (ax.min - ray_orig[axis]) * adinv;
            auto t1 = (ax.max - ray_orig[axis]) * adinv;

            if (t0 < t1) {
                if (t0 > ray_t.min) ray_t.min = t0;
                if (t1 < ray_t.max) ray_t.max = t1;
            } else {
                if (t1 > ray_t.min) ray_t.min = t1;
                if (t0 < ray_t.max) ray_t.max = t0;
            }

            if (ray_t.max <= ray_t.min)
                return false;
        }
        return true;
    }

    int longest_axis() const {
        // Returns the index of the longest axis of the bounding box.
        if (x.size() > y.size()) {
            return x.size() > z.size() ? 0 : 2;
        } else{
            return y.size() > z.size() ? 1 : 2;
        }
    }

    static const AABB empty, universe;

private:

    void pad_to_minimums() {
        // Adjust the AABB so that no side is narrower than some delta, padding if necessary.
        static constexpr double delta = 0.0001;
        if (x.size() < delta) x = x.expand(delta);
        if (y.size() < delta) y = y.expand(delta);
        if (z.size() < delta) z = z.expand(delta);
    }
};

AABB operator+(const AABB& bbox, const vecf3& offset);
AABB operator+(const vecf3& offset, const AABB& bbox);

}


#endif // RAYTRACING_AABB_H