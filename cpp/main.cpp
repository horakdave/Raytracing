#include "scene.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

int main() {
    const int width = 800;
    const int height = 600;
    sf::RenderWindow window(sf::VideoMode(width, height), "Raytracer");
    sf::Image image;
    sf::Texture texture;
    sf::Sprite sprite;
    
    image.create(width, height);
    texture.create(width, height);
    sprite.setTexture(texture);

    Scene scene;

    // Add spheres
    scene.add_sphere(Sphere(Vec3(0, 0, -5), 1.0f, Color(255, 0, 0), 0.5f));    // Red sphere
    scene.add_sphere(Sphere(Vec3(-2, 0, -5), 1.0f, Color(0, 255, 0), 0.3f));   // Green sphere
    scene.add_sphere(Sphere(Vec3(2, 0, -5), 1.0f, Color(0, 0, 255), 0.7f));    // Blue sphere
    scene.add_sphere(Sphere(Vec3(0, -5001, 0), 5000.0f, Color(200, 200, 200), 0.0f));  // Floor

    // Add lights
    scene.add_light(Vec3(5, 5, -5));
    scene.add_light(Vec3(-5, 5, -5));

    // Camera setup
    Vec3 camera_pos(0, 0, 0);
    float camera_rotation = 0.0f;
    float fov = M_PI / 2;
    float camera_speed = 0.1f;
    float rotation_speed = 0.1f;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                    case sf::Keyboard::W:
                        camera_pos.y += camera_speed;
                        std::cout << "Camera moved up to y=" << camera_pos.y << std::endl;
                        break;
                    case sf::Keyboard::S:
                        camera_pos.y -= camera_speed;
                        std::cout << "Camera moved down to y=" << camera_pos.y << std::endl;
                        break;
                    case sf::Keyboard::A:
                        camera_pos.x -= camera_speed;
                        std::cout << "Camera moved left to x=" << camera_pos.x << std::endl;
                        break;
                    case sf::Keyboard::D:
                        camera_pos.x += camera_speed;
                        std::cout << "Camera moved right to x=" << camera_pos.x << std::endl;
                        break;
                    case sf::Keyboard::Q:
                        camera_rotation += rotation_speed;
                        std::cout << "Camera rotated left to " << camera_rotation * 180.0f / M_PI << "°" << std::endl;
                        break;
                    case sf::Keyboard::E:
                        camera_rotation -= rotation_speed;
                        std::cout << "Camera rotated right to " << camera_rotation * 180.0f / M_PI << "°" << std::endl;
                        break;
                    default:
                        break;
                }
            }
        }

        // Render the scene
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                // Convert pixel coordinates to normalized device coordinates
                float ndc_x = (2.0f * ((x + 0.5f) / width) - 1.0f) * std::tan(fov / 2.0f) * (width / float(height));
                float ndc_y = (1.0f - 2.0f * ((y + 0.5f) / height)) * std::tan(fov / 2.0f);

                // Create ray from camera through pixel with rotation
                Vec3 ray_dir(ndc_x, ndc_y, -1.0f);
                // Apply rotation around Y axis
                float cos_rot = std::cos(camera_rotation);
                float sin_rot = std::sin(camera_rotation);
                float rotated_x = ray_dir.x * cos_rot + ray_dir.z * sin_rot;
                float rotated_z = -ray_dir.x * sin_rot + ray_dir.z * cos_rot;
                ray_dir = Vec3(rotated_x, ray_dir.y, rotated_z).normalize();
                Ray ray(camera_pos, ray_dir);

                // Trace ray and get color
                Color color = scene.trace_ray(ray);
                image.setPixel(x, y, sf::Color(color.r, color.g, color.b));
            }
        }

        texture.update(image);
        window.clear();
        window.draw(sprite);
        window.display();
    }

    return 0;
} 