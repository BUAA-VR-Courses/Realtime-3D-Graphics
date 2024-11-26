#ifndef UTILS_TOOLS_H
#define UTILS_TOOLS_H

#include <iostream>
#include <vector>
#include <cmath>

#include "Eigen/Dense"

using veci2 = Eigen::Vector2i;
using veci3 = Eigen::Vector3i;
using veci4 = Eigen::Vector4i;
using vecf2 = Eigen::Vector2f;
using vecf3 = Eigen::Vector3f;
using vecf4 = Eigen::Vector4f;
using matf3 = Eigen::Matrix3f;
using matf4 = Eigen::Matrix4f;

// Constants
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

#define to_radian(degree) (degree * pi / 180.0)
#define to_degree(radian) (radian * 180.0 / pi)

inline double random_double() {
    // Returns a random real in [0,1).
    return std::rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max - min) * random_double();
}

inline int random_int(int min, int max) {
    // Returns a random integer in [min,max].
    return int(random_double(min, max + 1));
}

inline vecf3 random_vecf3() {
    // Returns a random vector3.
    return vecf3(random_double(), random_double(), random_double());
}

inline vecf3 random_vecf3(double min, double max) {
    // Returns a random vector3 in [min,max).
    return vecf3(random_double(min, max), random_double(min, max), random_double(min, max));
}

inline vecf3 unit_vector(const vecf3& v) {
    return v / v.norm();
}

inline vecf3 random_in_unit_disk() {
    while (true) {
        auto p = vecf3(random_double(-1, 1), random_double(-1, 1), 0);
        if (p.squaredNorm() < 1) {
            return p;
        }
    }
}

inline vecf3 random_unit_vector() {
    while (true) {
        auto p = random_vecf3(-1, 1);
        auto lensq = p.squaredNorm();
        if (1e-160 < lensq && lensq <= 1.0) {
            return p / sqrt(lensq);
        }
    }
}

inline vecf3 random_on_hemisphere(const vecf3& normal) {
    vecf3 on_unit_sphere = random_unit_vector();
    if (on_unit_sphere.dot(normal) > 0.0) { // In the same hemisphere as the normal
        return on_unit_sphere;
    } else {
        return -on_unit_sphere;
    }  
}

inline vecf3 reflect(const vecf3& v, const vecf3& n) {
    return v - 2 * v.dot(n) * n;
}

inline vecf3 refract(const vecf3& uv, const vecf3& n, double etai_over_etat) {
    auto cos_theta = std::fmin(-uv.dot(n), 1.0);
    vecf3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
    vecf3 r_out_parallel = -std::sqrt(std::fabs(1.0 - r_out_perp.squaredNorm())) * n;
    return r_out_perp + r_out_parallel;
}

inline double linear_to_gamma(double linear_component) {
    if (linear_component > 0)
        return std::sqrt(linear_component);

    return 0;
}

struct Interval {
public:
    double min, max;
    Interval() : min(+infinity), max(-infinity) {} // Default interval is empty

    Interval(double min, double max) : min(min), max(max) {}

    Interval(const Interval& a, const Interval& b) {
        // Create the interval tightly enclosing the two input intervals.
        min = a.min <= b.min ? a.min : b.min;
        max = a.max >= b.max ? a.max : b.max;
    }

    double size() const {
        return max - min;
    }

    bool contains(double x) const {
        return min <= x && x <= max;
    }

    bool surrounds(double x) const {
        return min < x && x < max;
    }

    double clamp(double x) const {
        if (x < min) return min;
        if (x > max) return max;
        return x;
    }

    Interval expand(double delta) const {
        auto padding = delta/2;
        return Interval(min - padding, max + padding);
    }

    static const Interval empty, universe;
};

Interval operator+(const Interval& ival, double displacement);
Interval operator+(double displacement, const Interval& ival);

#endif // UTILS_TOOLS_H
