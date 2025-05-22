# https://www.edmundoptics.com/knowledge-center/application-notes/optics/geometrical-optics-101-paraxial-ray-tracing-calculations/?srsltid=AfmBOorFJE1nMmqNXSdI4kj-daEtLxGTXdOFenDmnNTKRLl_rNrmq0f9
# https://funprogramming.org/140-Recursive-graphics.html#:~:text=Using%20recursion%20in%20computer%20programs,the%20function%20can%20call%20itself.

import pygame
import math
from dataclasses import dataclass
from typing import List, Tuple, Optional

@dataclass
class Vec3:
    x: float
    y: float
    z: float

    def __add__(self, other):
        return Vec3(self.x + other.x, self.y + other.y, self.z + other.z)

    def __sub__(self, other):
        return Vec3(self.x - other.x, self.y - other.y, self.z - other.z)

    def __mul__(self, scalar):
        return Vec3(self.x * scalar, self.y * scalar, self.z * scalar)

    def dot(self, other):
        return self.x * other.x + self.y * other.y + self.z * other.z

    def length(self):
        return math.sqrt(self.dot(self))

    def normalize(self):
        length = self.length()
        if length < 1e-6:  # Avoid division by zero
            return Vec3(0, 0, 0)
        return self * (1.0 / length)

@dataclass
class Ray:
    origin: Vec3
    direction: Vec3

    def at(self, t: float) -> Vec3:
        return self.origin + self.direction * t

@dataclass
class Sphere:
    center: Vec3
    radius: float
    color: Tuple[int, int, int]
    specular: float = 0.0  # 0 to 1, how reflective the sphere is

    def intersect(self, ray: Ray) -> Optional[float]:
        oc = ray.origin - self.center
        a = ray.direction.dot(ray.direction)
        b = 2.0 * oc.dot(ray.direction)
        c = oc.dot(oc) - self.radius * self.radius
        discriminant = b * b - 4 * a * c

        if discriminant < 0:
            return None

        t = (-b - math.sqrt(discriminant)) / (2.0 * a)
        if t < 0:
            t = (-b + math.sqrt(discriminant)) / (2.0 * a)
            if t < 0:
                return None
        return t

class Scene:
    def __init__(self):
        self.spheres: List[Sphere] = []
        self.lights: List[Vec3] = []
        self.ambient_light = 0.2

    def add_sphere(self, sphere: Sphere):
        self.spheres.append(sphere)

    def add_light(self, light: Vec3):
        self.lights.append(light)

    def trace_ray(self, ray: Ray, depth: int = 0) -> Tuple[int, int, int]:
        if depth > 5:  # Maximum recursion depth
            return (0, 0, 0)

        closest_t = float('inf')
        closest_sphere = None

        # Find closest intersection
        for sphere in self.spheres:
            t = sphere.intersect(ray)
            if t is not None and t < closest_t:
                closest_t = t
                closest_sphere = sphere

        if closest_sphere is None:
            return (0, 0, 0)  # Background color

        # Calculate intersection point and normal
        hit_point = ray.at(closest_t)
        normal = (hit_point - closest_sphere.center).normalize()

        # Calculate lighting
        color = list(closest_sphere.color)
        intensity = self.ambient_light

        # Add diffuse lighting
        for light in self.lights:
            light_dir = (light - hit_point)
            light_dist = light_dir.length()
            if light_dist < 1e-6:  # Skip if light is too close
                continue
            light_dir = light_dir * (1.0 / light_dist)  # Normalize manually
            
            diffuse = max(0, normal.dot(light_dir))
            intensity += diffuse

            # Add specular lighting
            if closest_sphere.specular > 0:
                view_dir = (ray.origin - hit_point)
                view_dist = view_dir.length()
                if view_dist < 1e-6:  # Skip if view direction is too small
                    continue
                view_dir = view_dir * (1.0 / view_dist)  # Normalize manually
                
                # Calculate reflection direction
                reflect_dir = normal * (2 * normal.dot(light_dir)) - light_dir
                reflect_dist = reflect_dir.length()
                if reflect_dist < 1e-6:  # Skip if reflection direction is too small
                    continue
                reflect_dir = reflect_dir * (1.0 / reflect_dist)  # Normalize manually
                
                specular = max(0, view_dir.dot(reflect_dir)) ** 32
                intensity += specular * closest_sphere.specular

        # Apply lighting to color
        color = [min(255, int(c * intensity)) for c in color]

        # Handle reflections
        if closest_sphere.specular > 0:
            # Calculate reflection direction
            reflect_dir = ray.direction - normal * (2 * normal.dot(ray.direction))
            reflect_dist = reflect_dir.length()
            if reflect_dist > 1e-6:  # Only reflect if direction is valid
                reflect_dir = reflect_dir * (1.0 / reflect_dist)  # Normalize manually
                # Add a small offset to the reflection ray origin to prevent self-intersection
                offset_point = hit_point + normal * 1e-4
                reflect_ray = Ray(offset_point, reflect_dir)
                reflect_color = self.trace_ray(reflect_ray, depth + 1)
                color = [int(c * (1 - closest_sphere.specular) + rc * closest_sphere.specular)
                        for c, rc in zip(color, reflect_color)]

        return tuple(color)

def main():
    pygame.init()
    width, height = 800, 600
    screen = pygame.display.set_mode((width, height))
    pygame.display.set_caption("Raytracer")

    scene = Scene()

    # Add some spheres
    scene.add_sphere(Sphere(Vec3(0, 0, -5), 1.0, (255, 0, 0), 0.5))  # Red sphere
    scene.add_sphere(Sphere(Vec3(-2, 0, -5), 1.0, (0, 255, 0), 0.3))  # Green sphere
    scene.add_sphere(Sphere(Vec3(2, 0, -5), 1.0, (0, 0, 255), 0.7))   # Blue sphere
    scene.add_sphere(Sphere(Vec3(0, -5001, 0), 5000, (200, 200, 200), 0.0))  # Floor

    # Add lights
    scene.add_light(Vec3(5, 5, -5))
    scene.add_light(Vec3(-5, 5, -5))

    # Camera setup
    camera_pos = Vec3(0, 0, 0)
    fov = math.pi / 2
    camera_speed = 0.1

    clock = pygame.time.Clock()
    running = True
    while running:
        # Handle events
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_w:
                    camera_pos.y += camera_speed
                    print(f"Camera moved up to y={camera_pos.y}")
                elif event.key == pygame.K_s:
                    camera_pos.y -= camera_speed
                    print(f"Camera moved down to y={camera_pos.y}")
                elif event.key == pygame.K_a:
                    camera_pos.x -= camera_speed
                    print(f"Camera moved left to x={camera_pos.x}")
                elif event.key == pygame.K_d:
                    camera_pos.x += camera_speed
                    print(f"Camera moved right to x={camera_pos.x}")

        # Clear the screen
        screen.fill((0, 0, 0))

        # Render the scene
        for y in range(height):
            for x in range(width):
                # Convert pixel coordinates to normalized device coordinates
                ndc_x = (2 * ((x + 0.5) / width) - 1) * math.tan(fov / 2) * (width / height)
                ndc_y = (1 - 2 * ((y + 0.5) / height)) * math.tan(fov / 2)

                # Create ray from camera through pixel
                ray_dir = Vec3(ndc_x, ndc_y, -1).normalize()
                ray = Ray(camera_pos, ray_dir)

                # Trace ray and get color
                color = scene.trace_ray(ray)
                screen.set_at((x, y), color)

        pygame.display.flip()
        clock.tick(60)  # Limit to 60 FPS

    pygame.quit()

if __name__ == "__main__":
    main() 