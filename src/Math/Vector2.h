#pragma once
#include <cmath>

class Vector2 {
public:
    float x, y;

    Vector2() : x(0.0f), y(0.0f) {}
    Vector2(float x, float y) : x(x), y(y) {}
    Vector2(const Vector2& other) : x(other.x), y(other.y) {}

    // Basic operations
    Vector2 operator+(const Vector2& other) const;
    Vector2 operator-(const Vector2& other) const;
    Vector2 operator*(float scalar) const;
    Vector2 operator/(float scalar) const;
    Vector2& operator+=(const Vector2& other);
    Vector2& operator-=(const Vector2& other);
    Vector2& operator*=(float scalar);
    Vector2& operator/=(float scalar);

    // Vector operations
    float dot(const Vector2& other) const;
    float length() const;
    float lengthSquared() const;
    Vector2 normalized() const;
    void normalize();

    // Utility functions
    static Vector2 zero();
    static Vector2 up();
    static Vector2 right();
    static Vector2 lerp(const Vector2& a, const Vector2& b, float t);
};