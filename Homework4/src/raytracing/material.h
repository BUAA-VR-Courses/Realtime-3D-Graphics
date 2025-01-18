#ifndef RAYTRACING_MATERIAL_H
#define RAYTRACING_MATERIAL_H

#include <memory>

#include "raytracing/ray.h"
#include "raytracing/record.h"
#include "raytracing/texture.h"
#include "utils/tools.h"

namespace RayTracing {

class Material {
public:
    virtual ~Material() = default;

    virtual vecf3 emitted(double u, double v, const vecf3& p) const {
        return vecf3(0,0,0);
    }

    virtual bool scatter(const Ray& r_in, const Record& rec, vecf3& attenuation, Ray& scattered) const {
        return false;
    }
};


class Lambertian : public Material {
public:
    Lambertian(const vecf3& albedo) : tex(std::make_shared<SolidColor>(albedo)) {}
    Lambertian(std::shared_ptr<Texture> tex) : tex(tex) {}

    bool scatter(const Ray& r_in, const Record& rec, vecf3& attenuation, Ray& scattered) const override {
        vecf3 scatter_direction = rec.normal + random_unit_vector();

        // Catch degenerate scatter direction
        if (scatter_direction.isZero(1e-8)) {
            scatter_direction = rec.normal;
        }

        scattered = Ray(rec.p, scatter_direction, r_in.time);
        attenuation = tex->value(rec.u, rec.v, rec.p);
        return true;
    }

private:
    std::shared_ptr<Texture> tex;
};


class Metal : public Material {
public:
    Metal(const vecf3& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

    bool scatter(const Ray& r_in, const Record& rec, vecf3& attenuation, Ray& scattered) const override {
        vecf3 reflected = reflect(r_in.direction, rec.normal);
        reflected = unit_vector(reflected) + (fuzz * random_unit_vector());
        scattered = Ray(rec.p, reflected, r_in.time);
        attenuation = albedo;
        return (scattered.direction.dot(rec.normal) > 0);
    }

private:
    vecf3 albedo;
    double fuzz;
};


class Dielectric : public Material {
public:
    Dielectric(double refraction_index) : refraction_index(refraction_index) {}

    bool scatter(const Ray& r_in, const Record& rec, vecf3& attenuation, Ray& scattered) const override {
        attenuation = vecf3(1.0, 1.0, 1.0);
        double ri = rec.front_face ? (1.0/refraction_index) : refraction_index;

        vecf3 unit_direction = unit_vector(r_in.direction);
        double cos_theta = std::fmin(-unit_direction.dot(rec.normal), 1.0);
        double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);

        bool cannot_refract = ri * sin_theta > 1.0;
        vecf3 direction;

        if (cannot_refract || reflectance(cos_theta, ri) > random_double())
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, ri);

        scattered = Ray(rec.p, direction, r_in.time);
        return true;
    }

private:
    // Refractive index in vacuum or air, or the ratio of the material's refractive index over
    // the refractive index of the enclosing media
    double refraction_index;

    static double reflectance(double cosine, double refraction_index) {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1 - refraction_index) / (1 + refraction_index);
        r0 = r0 * r0;
        return r0 + (1 - r0) * std::pow((1 - cosine), 5);
    }
};


class DiffuseLight : public Material {
public:
    DiffuseLight(const vecf3& emit) : tex(std::make_shared<SolidColor>(emit)) {}

    vecf3 emitted(double u, double v, const vecf3& p) const override {
        return tex->value(u, v, p);
    }

private:
    std::shared_ptr<Texture> tex;
};


class Isotropic : public Material {
public:
    Isotropic(const vecf3& albedo) : tex(std::make_shared<SolidColor>(albedo)) {}
    Isotropic(std::shared_ptr<Texture> tex) : tex(tex) {}

    bool scatter(const Ray& r_in, const Record& rec, vecf3& attenuation, Ray& scattered) const override {
        scattered = Ray(rec.p, random_unit_vector(), r_in.time);
        attenuation = tex->value(rec.u, rec.v, rec.p);
        return true;
    }

private:
    std::shared_ptr<Texture> tex;
};

}

#endif // RAYTRACING_MATERIAL_H