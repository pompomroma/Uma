#pragma once
#include <cmath>

class Vector2 {
public:
    float x, y;
    
    // Constructors
    Vector2() : x(0), y(0) {}
    Vector2(float x, float y) : x(x), y(y) {}
    Vector2(float value) : x(value), y(value) {}
    Vector2(const Vector2& other) : x(other.x), y(other.y) {}
    
    // Static vectors
    static Vector2 zero() { return Vector2(0, 0); }
    static Vector2 one() { return Vector2(1, 1); }
    static Vector2 up() { return Vector2(0, 1); }
    static Vector2 down() { return Vector2(0, -1); }
    static Vector2 left() { return Vector2(-1, 0); }
    static Vector2 right() { return Vector2(1, 0); }
    
    // Operators
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
    
    bool operator==(const Vector2& other) const {
        return x == other.x && y == other.y;
    }
    
    bool operator!=(const Vector2& other) const {
        return !(*this == other);
    }
    
    // Methods
    float magnitude() const {
        return std::sqrt(x * x + y * y);
    }
    
    float magnitudeSquared() const {
        return x * x + y * y;
    }
    
    Vector2 normalized() const {
        float mag = magnitude();
        if (mag > 0) {
            return *this / mag;
        }
        return Vector2::zero();
    }
    
    void normalize() {
        float mag = magnitude();
        if (mag > 0) {
            x /= mag;
            y /= mag;
        }
    }
    
    float dot(const Vector2& other) const {
        return x * other.x + y * other.y;
    }
    
    float distance(const Vector2& other) const {
        return (*this - other).magnitude();
    }
    
    float distanceSquared(const Vector2& other) const {
        return (*this - other).magnitudeSquared();
    }
    
    float angle() const {
        return std::atan2(y, x);
    }
    
    static Vector2 lerp(const Vector2& a, const Vector2& b, float t) {
        return a + (b - a) * t;
    }
    
    static float angle(const Vector2& a, const Vector2& b) {
        return std::acos(a.normalized().dot(b.normalized()));
    }
};