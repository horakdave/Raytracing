#pragma once
#include "sphere.h"
#include "ray.h"
#include <vector>
#include <memory>

class Scene {
public:
    Scene() : ambient_light(0.2f) {}

    void add_sphere(const Sphere& sphere) {
        spheres.push_back(sphere);
    }

    void add_light(const Vec3& light) {
        lights.push_back(light);
    }

    Color trace_ray(const Ray& ray, int depth = 0) const;

private:
    std::vector<Sphere> spheres;
    std::vector<Vec3> lights;
    float ambient_light;
}; 