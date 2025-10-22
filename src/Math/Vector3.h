#pragma once
#include <cmath>

class Vector3 {
public:
    float x, y, z;

    Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
    Vector3(const Vector3& other) : x(other.x), y(other.y), z(other.z) {}

    // Basic operations
    Vector3 operator+(const Vector3& other) const;
    Vector3 operator-(const Vector3& other) const;
    Vector3 operator-() const;
    Vector3 operator*(float scalar) const;
    Vector3 operator/(float scalar) const;
    Vector3& operator+=(const Vector3& other);
    Vector3& operator-=(const Vector3& other);
    Vector3& operator*=(float scalar);
    Vector3& operator/=(float scalar);

    // Vector operations
    float dot(const Vector3& other) const;
    Vector3 cross(const Vector3& other) const;
    float length() const;
    float lengthSquared() const;
    Vector3 normalized() const;
    void normalize();

    // Utility functions
    static Vector3 zero();
    static Vector3 up();
    static Vector3 forward();
    static Vector3 right();
    static Vector3 lerp(const Vector3& a, const Vector3& b, float t);
};