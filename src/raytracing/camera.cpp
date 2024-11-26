#include "raytracing/camera.h"
#include "raytracing/hit.h"
#include "raytracing/material.h"
#include "raytracing/record.h"

namespace RayTracing {

vecf3 Camera::ray_color(const Ray& r, int depth, const Hittable& world) const {
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return vecf3(0, 0, 0);

    Record rec;

    // TODO: 4.1 Get the color from scattered or emitted rays
    return vecf3(0, 0, 0);
}

}