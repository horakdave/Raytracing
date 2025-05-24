#pragma once
#include <cmath>

class Vec3 {
public:
    float x, y, z;

    Vec3() : x(0), y(0), z(0) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    Vec3 operator+(const Vec3& other) const {
        return Vec3(x + other.x, y + other.y, z + other.z);
    }

    Vec3 operator-(const Vec3& other) const {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }

    Vec3 operator*(float scalar) const {
        return Vec3(x * scalar, y * scalar, z * scalar);
    }

    float dot(const Vec3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    float length() const {
        return std::sqrt(dot(*this));
    }

    Vec3 normalize() const {
        float len = length();
        if (len < 1e-6) {
            return Vec3(0, 0, 0);
        }
        return *this * (1.0f / len);
    }
}; 