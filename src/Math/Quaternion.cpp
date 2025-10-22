#include "Quaternion.h"
#include "Matrix4.h"
#include <cmath>
#include <algorithm>

Quaternion Quaternion::operator+(const Quaternion& other) const {
    return Quaternion(x + other.x, y + other.y, z + other.z, w + other.w);
}

Quaternion Quaternion::operator-(const Quaternion& other) const {
    return Quaternion(x - other.x, y - other.y, z - other.z, w - other.w);
}

Quaternion Quaternion::operator*(const Quaternion& other) const {
    return Quaternion(
        w * other.x + x * other.w + y * other.z - z * other.y,
        w * other.y - x * other.z + y * other.w + z * other.x,
        w * other.z + x * other.y - y * other.x + z * other.w,
        w * other.w - x * other.x - y * other.y - z * other.z
    );
}

Quaternion Quaternion::operator*(float scalar) const {
    return Quaternion(x * scalar, y * scalar, z * scalar, w * scalar);
}

Quaternion& Quaternion::operator*=(const Quaternion& other) {
    *this = *this * other;
    return *this;
}

Vector3 Quaternion::operator*(const Vector3& v) const {
    // Rotate vector by quaternion: v' = q * (0, v) * q^{-1}
    Quaternion qv(v.x, v.y, v.z, 0.0f);
    Quaternion inv = inverse();
    Quaternion result = (*this) * qv * inv;
    return Vector3(result.x, result.y, result.z);
}

float Quaternion::length() const {
    return std::sqrt(x * x + y * y + z * z + w * w);
}

float Quaternion::lengthSquared() const {
    return x * x + y * y + z * z + w * w;
}

Quaternion Quaternion::normalized() const {
    float len = length();
    if (len > 0.0f) {
        return *this * (1.0f / len);
    }
    return identity();
}

void Quaternion::normalize() {
    float len = length();
    if (len > 0.0f) {
        *this = *this * (1.0f / len);
    }
}

Quaternion Quaternion::conjugate() const {
    return Quaternion(-x, -y, -z, w);
}

Quaternion Quaternion::inverse() const {
    float lenSq = lengthSquared();
    if (lenSq > 0.0f) {
        Quaternion conj = conjugate();
        return conj * (1.0f / lenSq);
    }
    return identity();
}

Matrix4 Quaternion::toMatrix() const {
    float xx = x * x;
    float yy = y * y;
    float zz = z * z;
    float xy = x * y;
    float xz = x * z;
    float yz = y * z;
    float wx = w * x;
    float wy = w * y;
    float wz = w * z;

    Matrix4 result;
    result(0, 0) = 1.0f - 2.0f * (yy + zz);
    result(0, 1) = 2.0f * (xy - wz);
    result(0, 2) = 2.0f * (xz + wy);
    result(0, 3) = 0.0f;

    result(1, 0) = 2.0f * (xy + wz);
    result(1, 1) = 1.0f - 2.0f * (xx + zz);
    result(1, 2) = 2.0f * (yz - wx);
    result(1, 3) = 0.0f;

    result(2, 0) = 2.0f * (xz - wy);
    result(2, 1) = 2.0f * (yz + wx);
    result(2, 2) = 1.0f - 2.0f * (xx + yy);
    result(2, 3) = 0.0f;

    result(3, 0) = 0.0f;
    result(3, 1) = 0.0f;
    result(3, 2) = 0.0f;
    result(3, 3) = 1.0f;

    return result;
}

Vector3 Quaternion::toEulerAngles() const {
    float roll, pitch, yaw;

    // Roll (x-axis rotation)
    float sinr_cosp = 2.0f * (w * x + y * z);
    float cosr_cosp = 1.0f - 2.0f * (x * x + y * y);
    roll = std::atan2(sinr_cosp, cosr_cosp);

    // Pitch (y-axis rotation)
    float sinp = 2.0f * (w * y - z * x);
    if (std::abs(sinp) >= 1.0f) {
        pitch = std::copysign(M_PI / 2.0f, sinp); // Use 90 degrees if out of range
    } else {
        pitch = std::asin(sinp);
    }

    // Yaw (z-axis rotation)
    float siny_cosp = 2.0f * (w * z + x * y);
    float cosy_cosp = 1.0f - 2.0f * (y * y + z * z);
    yaw = std::atan2(siny_cosp, cosy_cosp);

    return Vector3(roll, pitch, yaw);
}

Quaternion Quaternion::fromEulerAngles(const Vector3& euler) {
    float cr = std::cos(euler.x * 0.5f);
    float sr = std::sin(euler.x * 0.5f);
    float cp = std::cos(euler.y * 0.5f);
    float sp = std::sin(euler.y * 0.5f);
    float cy = std::cos(euler.z * 0.5f);
    float sy = std::sin(euler.z * 0.5f);

    return Quaternion(
        sr * cp * cy - cr * sp * sy,
        cr * sp * cy + sr * cp * sy,
        cr * cp * sy - sr * sp * cy,
        cr * cp * cy + sr * sp * sy
    );
}

Quaternion Quaternion::fromAxisAngle(const Vector3& axis, float angle) {
    float halfAngle = angle * 0.5f;
    float s = std::sin(halfAngle);
    Vector3 normalizedAxis = axis.normalized();
    
    return Quaternion(
        normalizedAxis.x * s,
        normalizedAxis.y * s,
        normalizedAxis.z * s,
        std::cos(halfAngle)
    );
}

Quaternion Quaternion::lookRotation(const Vector3& forward, const Vector3& up) {
    Vector3 f = forward.normalized();
    Vector3 u = up.normalized();
    Vector3 r = f.cross(u).normalized();
    u = r.cross(f);

    float trace = r.x + u.y + f.z;
    if (trace > 0.0f) {
        float s = std::sqrt(trace + 1.0f) * 2.0f;
        return Quaternion(
            (u.z - f.y) / s,
            (f.x - r.z) / s,
            (r.y - u.x) / s,
            0.25f * s
        );
    } else if (r.x > u.y && r.x > f.z) {
        float s = std::sqrt(1.0f + r.x - u.y - f.z) * 2.0f;
        return Quaternion(
            0.25f * s,
            (u.x + r.y) / s,
            (f.x + r.z) / s,
            (u.z - f.y) / s
        );
    } else if (u.y > f.z) {
        float s = std::sqrt(1.0f + u.y - r.x - f.z) * 2.0f;
        return Quaternion(
            (u.x + r.y) / s,
            0.25f * s,
            (f.y + u.z) / s,
            (f.x - r.z) / s
        );
    } else {
        float s = std::sqrt(1.0f + f.z - r.x - u.y) * 2.0f;
        return Quaternion(
            (f.x + r.z) / s,
            (f.y + u.z) / s,
            0.25f * s,
            (r.y - u.x) / s
        );
    }
}

Quaternion Quaternion::identity() {
    return Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
}

Quaternion Quaternion::slerp(const Quaternion& a, const Quaternion& b, float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    
    float dot = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    
    if (dot < 0.0f) {
        Quaternion negB = b * -1.0f;
        return slerp(a, negB, t);
    }
    
    if (dot > 0.9995f) {
        return (a + (b - a) * t).normalized();
    }
    
    float theta_0 = std::acos(std::abs(dot));
    float sin_theta_0 = std::sin(theta_0);
    float theta = theta_0 * t;
    float sin_theta = std::sin(theta);
    
    float s0 = std::cos(theta) - dot * sin_theta / sin_theta_0;
    float s1 = sin_theta / sin_theta_0;
    
    return (a * s0 + b * s1).normalized();
}