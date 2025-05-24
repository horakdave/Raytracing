#include "scene.h"
#include <algorithm>
#include <cmath>

Color Scene::trace_ray(const Ray& ray, int depth) const {
    if (depth > 5) {  // maximum recursion depth
        return Color(0, 0, 0);
    }

    float closest_t = std::numeric_limits<float>::infinity();
    const Sphere* closest_sphere = nullptr;

    // closest intersection
    for (const auto& sphere : spheres) {
        auto t = sphere.intersect(ray);
        if (t && *t < closest_t) {
            closest_t = *t;
            closest_sphere = &sphere;
        }
    }

    if (!closest_sphere) {
        return Color(0, 0, 0);  // Background color
    }

    // intersection point and normal
    Vec3 hit_point = ray.at(closest_t);
    Vec3 normal = (hit_point - closest_sphere->center).normalize();

    // Calculate lighting
    float intensity = ambient_light;


    for (const auto& light : lights) {
        Vec3 light_dir = light - hit_point;
        float light_dist = light_dir.length();
        if (light_dist < 1e-6) {
            continue;
        }
        light_dir = light_dir * (1.0f / light_dist);  // Normalize manually
        
        float diffuse = std::max(0.0f, normal.dot(light_dir));
        intensity += diffuse;

        // Add specular lighting
        if (closest_sphere->specular > 0) {
            Vec3 view_dir = ray.origin - hit_point;
            float view_dist = view_dir.length();
            if (view_dist < 1e-6) {
                continue;
            }
            view_dir = view_dir * (1.0f / view_dist);  // Normalize manually
            
            // Calculate reflection direction
            Vec3 reflect_dir = normal * (2 * normal.dot(light_dir)) - light_dir;
            float reflect_dist = reflect_dir.length();
            if (reflect_dist < 1e-6) {
                continue;
            }
            reflect_dir = reflect_dir * (1.0f / reflect_dist);  // Normalize manually
            
            float specular = std::pow(std::max(0.0f, view_dir.dot(reflect_dir)), 32);
            intensity += specular * closest_sphere->specular;
        }
    }

    // Apply lighting to color
    Color color(
        std::min(255, static_cast<int>(closest_sphere->color.r * intensity)),
        std::min(255, static_cast<int>(closest_sphere->color.g * intensity)),
        std::min(255, static_cast<int>(closest_sphere->color.b * intensity))
    );

    // Handle reflections
    if (closest_sphere->specular > 0) {
        // Calculate reflection direction
        Vec3 reflect_dir = ray.direction - normal * (2 * normal.dot(ray.direction));
        float reflect_dist = reflect_dir.length();
        if (reflect_dist > 1e-6) {  // Only valid reflections
            reflect_dir = reflect_dir * (1.0f / reflect_dist);  // Normalize manually
            Vec3 offset_point = hit_point + normal * 1e-4f;
            Ray reflect_ray(offset_point, reflect_dir);
            Color reflect_color = trace_ray(reflect_ray, depth + 1);
            
            color.r = static_cast<unsigned char>(
                color.r * (1 - closest_sphere->specular) + 
                reflect_color.r * closest_sphere->specular
            );
            color.g = static_cast<unsigned char>(
                color.g * (1 - closest_sphere->specular) + 
                reflect_color.g * closest_sphere->specular
            );
            color.b = static_cast<unsigned char>(
                color.b * (1 - closest_sphere->specular) + 
                reflect_color.b * closest_sphere->specular
            );
        }
    }

    return color;
} 