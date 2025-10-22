#pragma once
#include "Vector3.h"

class Quaternion {
public:
    float x, y, z, w;

    Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
    Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    Quaternion(const Quaternion& other) : x(other.x), y(other.y), z(other.z), w(other.w) {}

    // Basic operations
    Quaternion operator+(const Quaternion& other) const;
    Quaternion operator-(const Quaternion& other) const;
    Quaternion operator*(const Quaternion& other) const;
    Quaternion operator*(float scalar) const;
    Quaternion& operator*=(const Quaternion& other);

    // Utility functions
    float length() const;
    float lengthSquared() const;
    Quaternion normalized() const;
    void normalize();
    Quaternion conjugate() const;
    Quaternion inverse() const;

    // Conversion functions
    Matrix4 toMatrix() const;
    Vector3 toEulerAngles() const;
    static Quaternion fromEulerAngles(const Vector3& euler);
    static Quaternion fromAxisAngle(const Vector3& axis, float angle);
    static Quaternion lookRotation(const Vector3& forward, const Vector3& up = Vector3::up());

    // Static constants
    static Quaternion identity();
    static Quaternion slerp(const Quaternion& a, const Quaternion& b, float t);
};