#ifndef RAYTRACING_RECORD_H
#define RAYTRACING_RECORD_H

#include <memory>

#include "raytracing/ray.h"
#include "utils/tools.h"

namespace RayTracing {

class Material;

class Record {
public:
    vecf3 p;            // Hit point
    vecf3 normal;       // Normal vector
    std::shared_ptr<Material> mat;

    double t;           // Ray's t parameter
    double u, v;        // Texture's u,v
    bool front_face;

    void set_face_normal(const Ray& r, const vecf3& outward_normal) {
        // Sets the hit record normal vector.
        // NOTE: the parameter `outward_normal` is assumed to have unit length.
        front_face = r.direction.dot(outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

}


#endif // RAYTRACING_RECORD_H