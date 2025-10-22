#include "Vector3.h"
#include <algorithm>

Vector3 Vector3::operator+(const Vector3& other) const {
    return Vector3(x + other.x, y + other.y, z + other.z);
}

Vector3 Vector3::operator-(const Vector3& other) const {
    return Vector3(x - other.x, y - other.y, z - other.z);
}

Vector3 Vector3::operator*(float scalar) const {
    return Vector3(x * scalar, y * scalar, z * scalar);
}

Vector3 Vector3::operator/(float scalar) const {
    return Vector3(x / scalar, y / scalar, z / scalar);
}

Vector3& Vector3::operator+=(const Vector3& other) {
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
}

Vector3& Vector3::operator-=(const Vector3& other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
}

Vector3& Vector3::operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

Vector3& Vector3::operator/=(float scalar) {
    x /= scalar;
    y /= scalar;
    z /= scalar;
    return *this;
}

float Vector3::dot(const Vector3& other) const {
    return x * other.x + y * other.y + z * other.z;
}

Vector3 Vector3::cross(const Vector3& other) const {
    return Vector3(
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x
    );
}

float Vector3::length() const {
    return std::sqrt(x * x + y * y + z * z);
}

float Vector3::lengthSquared() const {
    return x * x + y * y + z * z;
}

Vector3 Vector3::normalized() const {
    float len = length();
    if (len > 0.0f) {
        return *this / len;
    }
    return Vector3::zero();
}

void Vector3::normalize() {
    float len = length();
    if (len > 0.0f) {
        *this /= len;
    }
}

Vector3 Vector3::zero() {
    return Vector3(0.0f, 0.0f, 0.0f);
}

Vector3 Vector3::up() {
    return Vector3(0.0f, 1.0f, 0.0f);
}

Vector3 Vector3::forward() {
    return Vector3(0.0f, 0.0f, -1.0f);
}

Vector3 Vector3::right() {
    return Vector3(1.0f, 0.0f, 0.0f);
}

Vector3 Vector3::lerp(const Vector3& a, const Vector3& b, float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    return a + (b - a) * t;
}