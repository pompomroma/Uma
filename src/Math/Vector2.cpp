#include "Vector2.h"
#include <algorithm>

Vector2 Vector2::operator+(const Vector2& other) const {
    return Vector2(x + other.x, y + other.y);
}

Vector2 Vector2::operator-(const Vector2& other) const {
    return Vector2(x - other.x, y - other.y);
}

Vector2 Vector2::operator*(float scalar) const {
    return Vector2(x * scalar, y * scalar);
}

Vector2 Vector2::operator/(float scalar) const {
    if (scalar != 0.0f) {
        return Vector2(x / scalar, y / scalar);
    }
    return Vector2(0.0f, 0.0f);
}

Vector2& Vector2::operator+=(const Vector2& other) {
    x += other.x;
    y += other.y;
    return *this;
}

Vector2& Vector2::operator-=(const Vector2& other) {
    x -= other.x;
    y -= other.y;
    return *this;
}

Vector2& Vector2::operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    return *this;
}

Vector2& Vector2::operator/=(float scalar) {
    if (scalar != 0.0f) {
        x /= scalar;
        y /= scalar;
    }
    return *this;
}

float Vector2::dot(const Vector2& other) const {
    return x * other.x + y * other.y;
}

float Vector2::length() const {
    return std::sqrt(x * x + y * y);
}

float Vector2::lengthSquared() const {
    return x * x + y * y;
}

Vector2 Vector2::normalized() const {
    float len = length();
    if (len > 0.0f) {
        return Vector2(x / len, y / len);
    }
    return Vector2(0.0f, 0.0f);
}

void Vector2::normalize() {
    float len = length();
    if (len > 0.0f) {
        x /= len;
        y /= len;
    }
}

Vector2 Vector2::zero() {
    return Vector2(0.0f, 0.0f);
}

Vector2 Vector2::up() {
    return Vector2(0.0f, 1.0f);
}

Vector2 Vector2::right() {
    return Vector2(1.0f, 0.0f);
}

Vector2 Vector2::lerp(const Vector2& a, const Vector2& b, float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    return a + (b - a) * t;
}