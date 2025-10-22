#pragma once
#include <cmath>

class Vector2 {
public:
    float x;
    float y;

    Vector2() : x(0.0f), y(0.0f) {}
    Vector2(float x, float y) : x(x), y(y) {}
    Vector2(const Vector2& other) : x(other.x), y(other.y) {}

    // Basic operations
    Vector2 operator+(const Vector2& other) const { return Vector2(x + other.x, y + other.y); }
    Vector2 operator-(const Vector2& other) const { return Vector2(x - other.x, y - other.y); }
    Vector2 operator*(float scalar) const { return Vector2(x * scalar, y * scalar); }
    Vector2 operator/(float scalar) const { return Vector2(x / scalar, y / scalar); }
    Vector2& operator+=(const Vector2& other) { x += other.x; y += other.y; return *this; }
    Vector2& operator-=(const Vector2& other) { x -= other.x; y -= other.y; return *this; }
    Vector2& operator*=(float scalar) { x *= scalar; y *= scalar; return *this; }
    Vector2& operator/=(float scalar) { x /= scalar; y /= scalar; return *this; }

    // Vector operations
    float dot(const Vector2& other) const { return x * other.x + y * other.y; }
    float length() const { return std::sqrt(x * x + y * y); }
    float lengthSquared() const { return x * x + y * y; }
    Vector2 normalized() const {
        float len = length();
        if (len == 0.0f) return Vector2(0.0f, 0.0f);
        return Vector2(x / len, y / len);
    }
    void normalize() {
        float len = length();
        if (len == 0.0f) return;
        x /= len;
        y /= len;
    }

    // Utility
    static Vector2 zero() { return Vector2(0.0f, 0.0f); }
    static Vector2 up() { return Vector2(0.0f, 1.0f); }
    static Vector2 right() { return Vector2(1.0f, 0.0f); }
};