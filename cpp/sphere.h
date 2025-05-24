#pragma once
#include "ray.h"
#include <optional>
#include <cmath>

struct Color {
    unsigned char r, g, b;
    Color(unsigned char r, unsigned char g, unsigned char b) 
        : r(r), g(g), b(b) {}
};

class Sphere {
public:
    Vec3 center;
    float radius;
    Color color;
    float specular;

    Sphere(const Vec3& center, float radius, const Color& color, float specular = 0.0f)
        : center(center), radius(radius), color(color), specular(specular) {}

    std::optional<float> intersect(const Ray& ray) const {
        Vec3 oc = ray.origin - center;
        float a = ray.direction.dot(ray.direction);
        float b = 2.0f * oc.dot(ray.direction);
        float c = oc.dot(oc) - radius * radius;
        float discriminant = b * b - 4 * a * c;

        if (discriminant < 0) {
            return std::nullopt;
        }

        float t = (-b - std::sqrt(discriminant)) / (2.0f * a);
        if (t < 0) {
            t = (-b + std::sqrt(discriminant)) / (2.0f * a);
            if (t < 0) {
                return std::nullopt;
            }
        }
        return t;
    }
}; 