#pragma once
#include <cmath>

class Vector2 {
public:
    float x, y;
    
    // Constructors
    Vector2() : x(0.0f), y(0.0f) {}
    Vector2(float x, float y) : x(x), y(y) {}
    Vector2(const Vector2& other) : x(other.x), y(other.y) {}
    
    // Assignment operator
    Vector2& operator=(const Vector2& other) {
        if (this != &other) {
            x = other.x;
            y = other.y;
        }
        return *this;
    }
    
    // Arithmetic operators
    Vector2 operator+(const Vector2& other) const {
        return Vector2(x + other.x, y + other.y);
    }
    
    Vector2 operator-(const Vector2& other) const {
        return Vector2(x - other.x, y - other.y);
    }
    
    Vector2 operator*(float scalar) const {
        return Vector2(x * scalar, y * scalar);
    }
    
    Vector2 operator/(float scalar) const {
        return Vector2(x / scalar, y / scalar);
    }
    
    Vector2& operator+=(const Vector2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
    
    Vector2& operator-=(const Vector2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }
    
    Vector2& operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }
    
    Vector2& operator/=(float scalar) {
        x /= scalar;
        y /= scalar;
        return *this;
    }
    
    // Comparison operators
    bool operator==(const Vector2& other) const {
        return std::abs(x - other.x) < 1e-6f && std::abs(y - other.y) < 1e-6f;
    }
    
    bool operator!=(const Vector2& other) const {
        return !(*this == other);
    }
    
    // Vector operations
    float length() const {
        return std::sqrt(x * x + y * y);
    }
    
    float lengthSquared() const {
        return x * x + y * y;
    }
    
    Vector2 normalized() const {
        float len = length();
        if (len > 1e-6f) {
            return Vector2(x / len, y / len);
        }
        return Vector2(0.0f, 0.0f);
    }
    
    void normalize() {
        float len = length();
        if (len > 1e-6f) {
            x /= len;
            y /= len;
        }
    }
    
    float dot(const Vector2& other) const {
        return x * other.x + y * other.y;
    }
    
    float cross(const Vector2& other) const {
        return x * other.y - y * other.x;
    }
    
    Vector2 perpendicular() const {
        return Vector2(-y, x);
    }
    
    float angle() const {
        return std::atan2(y, x);
    }
    
    float angleTo(const Vector2& other) const {
        return std::atan2(cross(other), dot(other));
    }
    
    Vector2 rotated(float angle) const {
        float cos_a = std::cos(angle);
        float sin_a = std::sin(angle);
        return Vector2(x * cos_a - y * sin_a, x * sin_a + y * cos_a);
    }
    
    Vector2 lerp(const Vector2& other, float t) const {
        return Vector2(x + (other.x - x) * t, y + (other.y - y) * t);
    }
    
    // Static methods
    static Vector2 zero() { return Vector2(0.0f, 0.0f); }
    static Vector2 one() { return Vector2(1.0f, 1.0f); }
    static Vector2 up() { return Vector2(0.0f, 1.0f); }
    static Vector2 down() { return Vector2(0.0f, -1.0f); }
    static Vector2 left() { return Vector2(-1.0f, 0.0f); }
    static Vector2 right() { return Vector2(1.0f, 0.0f); }
    
    static float distance(const Vector2& a, const Vector2& b) {
        return (a - b).length();
    }
    
    static float distanceSquared(const Vector2& a, const Vector2& b) {
        return (a - b).lengthSquared();
    }
    
    static Vector2 lerp(const Vector2& a, const Vector2& b, float t) {
        return a.lerp(b, t);
    }
    
    static Vector2 min(const Vector2& a, const Vector2& b) {
        return Vector2(std::min(a.x, b.x), std::min(a.y, b.y));
    }
    
    static Vector2 max(const Vector2& a, const Vector2& b) {
        return Vector2(std::max(a.x, b.x), std::max(a.y, b.y));
    }
};

// Global operators
inline Vector2 operator*(float scalar, const Vector2& vec) {
    return vec * scalar;
}