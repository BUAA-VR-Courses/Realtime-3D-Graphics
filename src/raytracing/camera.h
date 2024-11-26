#ifndef RAYTRACING_CAMERA_H
#define RAYTRACING_CAMERA_H

#include <cassert>

#include "Eigen/Dense"

#include "raytracing/hit.h"
#include "raytracing/ray.h"
#include "raytracing/record.h"
#include "utils/tools.h"

namespace RayTracing {

class Camera {
private:
    // Default camera values
    const int WIDTH = 800;
    const int HEIGHT = 800;
    const int SAMPLES_PER_PIXEL = 10000;
    const int MAX_DEPTH = 40;
    const double VFOV = 40.0;
    const vecf3 LOOKFROM = vecf3(478, 278, -600);
    const vecf3 LOOKAT = vecf3(278, 278, 0);
    const vecf3 VUP = vecf3(0.0, 1.0, 0.0);
    const vecf3 BACKGROUND = vecf3(0.0, 0.0, 0.0);
    const double DEFOCUS_ANGLE = 0.0;
    const double FOCUS_DIST = 10.0;

public:
    // Camera values
    int width;                  // Rendered image width in pixel count
    int height;                 // Rendered image height in pixel count
    int samples_per_pixel;      // Count of random samples for each pixel
    int max_depth;              // Maximum number of ray bounces into scene
    double vfov;                // Vertical view angle (field of view)
    vecf3 lookfrom;             // Point camera is looking from
    vecf3 lookat;               // Point camera is looking at
    vecf3 vup;                  // Camera-relative "up" direction
    vecf3 background;           // Scene background color
    double defocus_angle;       // Variation angle of rays through each pixel
    double focus_dist;          // Distance from camera lookfrom point to plane of perfect focus
    double pixel_samples_scale; // Scale factor for pixel samples

    Camera() {
        width = WIDTH;
        height = HEIGHT;
        samples_per_pixel = SAMPLES_PER_PIXEL;
        max_depth = MAX_DEPTH;
        vfov = VFOV;
        lookfrom = LOOKFROM;
        lookat = LOOKAT;
        vup = VUP;
        background = BACKGROUND;
        defocus_angle = DEFOCUS_ANGLE;
        focus_dist = FOCUS_DIST;
        
        aspect_ratio = (double)WIDTH / (double)HEIGHT;
        pixel_samples_scale = 1.0 / samples_per_pixel;
        center = lookfrom;

        // Determine viewport dimensions.
        auto theta = to_radian(vfov);
        auto h = std::tan(theta / 2);
        auto viewport_height = 2 * h * focus_dist;
        auto viewport_width = viewport_height * (double(width) / height);

        // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(vup.cross(w));
        v = w.cross(u);

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        vecf3 viewport_u = viewport_width * u;    // Vector across viewport horizontal edge
        vecf3 viewport_v = viewport_height * -v;  // Vector down viewport vertical edge

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / width;
        pixel_delta_v = viewport_v / height;

        // Calculate the location of the upper left pixel.
        auto viewport_upper_left = center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        // Calculate the camera defocus disk basis vectors.
        auto defocus_radius = focus_dist * std::tan(to_radian(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    Camera(int width, 
           int height, 
           int samples_per_pixel, 
           int max_depth, 
           double vfov, 
           vecf3 lookfrom, 
           vecf3 lookat, 
           vecf3 vup, 
           vecf3 background,          
           double defocus_angle, 
           double focus_dist)
        : width(width), 
          height(height), 
          samples_per_pixel(samples_per_pixel), 
          max_depth(max_depth),
          vfov(vfov), 
          lookfrom(lookfrom), 
          lookat(lookat), 
          vup(vup), 
          background(background),
          defocus_angle(defocus_angle), 
          focus_dist(focus_dist) {
        
        aspect_ratio = (double)WIDTH / (double)HEIGHT;
        pixel_samples_scale = 1.0 / samples_per_pixel;
        center = lookfrom;

        // Determine viewport dimensions.
        auto theta = to_radian(vfov);
        auto h = std::tan(theta / 2);
        auto viewport_height = 2 * h * focus_dist;
        auto viewport_width = viewport_height * (double(width) / height);

        // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(vup.cross(w));
        v = w.cross(u);

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        vecf3 viewport_u = viewport_width * u;    // Vector across viewport horizontal edge
        vecf3 viewport_v = viewport_height * -v;  // Vector down viewport vertical edge

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / width;
        pixel_delta_v = viewport_v / height;

        // Calculate the location of the upper left pixel.
        auto viewport_upper_left = center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        // Calculate the camera defocus disk basis vectors.
        auto defocus_radius = focus_dist * std::tan(to_radian(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

private:
    double aspect_ratio;        // Ratio of image width over height
    vecf3 center;               // Camera center
    vecf3 pixel00_loc;          // Location of pixel 0, 0
    vecf3 pixel_delta_u;        // Offset to pixel to the right
    vecf3 pixel_delta_v;        // Offset to pixel below
    vecf3 u, v, w;              // Camera frame basis vectors
    vecf3 defocus_disk_u;       // Defocus disk horizontal radius
    vecf3 defocus_disk_v;       // Defocus disk vertical radius

public:
    Ray get_ray(int i, int j) const {
        // Construct a camera ray originating from the defocus disk and directed at a randomly
        // sampled point around the pixel location i, j.

        auto offset = sample_square();
        auto pixel_sample = pixel00_loc
            + ((i + offset.x()) * pixel_delta_u)
            + ((j + offset.y()) * pixel_delta_v);

        auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
        auto ray_direction = pixel_sample - ray_origin;
        auto ray_time = random_double();

        return Ray(ray_origin, ray_direction, ray_time);
    }

    vecf3 sample_square() const {
        // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
        return vecf3(random_double() - 0.5, random_double() - 0.5, 0);
    }

    vecf3 sample_disk(double radius) const {
        // Returns a random point in the unit (radius 0.5) disk centered at the origin.
        return radius * random_in_unit_disk();
    }

    vecf3 defocus_disk_sample() const {
        // Returns a random point in the camera defocus disk.
        auto p = random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

    vecf3 ray_color(const Ray& r, int depth, const Hittable& world) const;
};
}

#endif // !RAYTRACING_CAMERA_H
