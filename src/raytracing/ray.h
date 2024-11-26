#ifndef RAYTRACING_RAY_H
#define RAYTRACING_RAY_H

#include "Eigen/Dense"

#include "utils/tools.h"

namespace RayTracing {

class Ray {
public:
    vecf3 origin;
    vecf3 direction;
    double time;

    Ray() {}
    
    Ray(const vecf3& origin, const vecf3& direction, double time = 0.0)
        : origin(origin), direction(direction), time(time) {}
    
    vecf3 at(double t) const {
        return origin + t * direction;
    }
};
}

#endif // !RAYTRACING_RAY_H
