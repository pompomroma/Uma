#include "Matrix4.h"
#include <cmath>
#include <cstring>

Matrix4::Matrix4() {
    std::memset(m, 0, sizeof(m));
    m[0] = m[5] = m[10] = m[15] = 1.0f;
}

Matrix4::Matrix4(const Matrix4& other) {
    std::memcpy(m, other.m, sizeof(m));
}

Matrix4 Matrix4::operator*(const Matrix4& other) const {
    Matrix4 result;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result(i, j) = 0.0f;
            for (int k = 0; k < 4; k++) {
                result(i, j) += (*this)(i, k) * other(k, j);
            }
        }
    }
    return result;
}

Vector3 Matrix4::operator*(const Vector3& v) const {
    float w = (*this)(3, 0) * v.x + (*this)(3, 1) * v.y + (*this)(3, 2) * v.z + (*this)(3, 3);
    if (w != 0.0f) {
        w = 1.0f / w;
    }
    return Vector3(
        ((*this)(0, 0) * v.x + (*this)(0, 1) * v.y + (*this)(0, 2) * v.z + (*this)(0, 3)) * w,
        ((*this)(1, 0) * v.x + (*this)(1, 1) * v.y + (*this)(1, 2) * v.z + (*this)(1, 3)) * w,
        ((*this)(2, 0) * v.x + (*this)(2, 1) * v.y + (*this)(2, 2) * v.z + (*this)(2, 3)) * w
    );
}

Matrix4& Matrix4::operator*=(const Matrix4& other) {
    *this = *this * other;
    return *this;
}

float& Matrix4::operator()(int row, int col) {
    return m[row * 4 + col];
}

const float& Matrix4::operator()(int row, int col) const {
    return m[row * 4 + col];
}

Matrix4 Matrix4::identity() {
    return Matrix4();
}

Matrix4 Matrix4::translation(const Vector3& translation) {
    Matrix4 result = identity();
    result(0, 3) = translation.x;
    result(1, 3) = translation.y;
    result(2, 3) = translation.z;
    return result;
}

Matrix4 Matrix4::rotation(const Vector3& axis, float angle) {
    Vector3 normalizedAxis = axis.normalized();
    float c = std::cos(angle);
    float s = std::sin(angle);
    float t = 1.0f - c;

    Matrix4 result;
    result(0, 0) = t * normalizedAxis.x * normalizedAxis.x + c;
    result(0, 1) = t * normalizedAxis.x * normalizedAxis.y + s * normalizedAxis.z;
    result(0, 2) = t * normalizedAxis.x * normalizedAxis.z - s * normalizedAxis.y;
    result(0, 3) = 0.0f;

    result(1, 0) = t * normalizedAxis.x * normalizedAxis.y - s * normalizedAxis.z;
    result(1, 1) = t * normalizedAxis.y * normalizedAxis.y + c;
    result(1, 2) = t * normalizedAxis.y * normalizedAxis.z + s * normalizedAxis.x;
    result(1, 3) = 0.0f;

    result(2, 0) = t * normalizedAxis.x * normalizedAxis.z + s * normalizedAxis.y;
    result(2, 1) = t * normalizedAxis.y * normalizedAxis.z - s * normalizedAxis.x;
    result(2, 2) = t * normalizedAxis.z * normalizedAxis.z + c;
    result(2, 3) = 0.0f;

    result(3, 0) = 0.0f;
    result(3, 1) = 0.0f;
    result(3, 2) = 0.0f;
    result(3, 3) = 1.0f;

    return result;
}

Matrix4 Matrix4::scale(const Vector3& scale) {
    Matrix4 result = identity();
    result(0, 0) = scale.x;
    result(1, 1) = scale.y;
    result(2, 2) = scale.z;
    return result;
}

Matrix4 Matrix4::lookAt(const Vector3& eye, const Vector3& center, const Vector3& up) {
    Vector3 f = (center - eye).normalized();
    Vector3 s = f.cross(up).normalized();
    Vector3 u = s.cross(f);

    Matrix4 result;
    result(0, 0) = s.x;
    result(0, 1) = s.y;
    result(0, 2) = s.z;
    result(0, 3) = -s.dot(eye);

    result(1, 0) = u.x;
    result(1, 1) = u.y;
    result(1, 2) = u.z;
    result(1, 3) = -u.dot(eye);

    result(2, 0) = -f.x;
    result(2, 1) = -f.y;
    result(2, 2) = -f.z;
    result(2, 3) = f.dot(eye);

    result(3, 0) = 0.0f;
    result(3, 1) = 0.0f;
    result(3, 2) = 0.0f;
    result(3, 3) = 1.0f;

    return result;
}

Matrix4 Matrix4::perspective(float fov, float aspect, float near, float far) {
    float tanHalfFov = std::tan(fov * 0.5f);
    
    Matrix4 result;
    result(0, 0) = 1.0f / (aspect * tanHalfFov);
    result(0, 1) = 0.0f;
    result(0, 2) = 0.0f;
    result(0, 3) = 0.0f;

    result(1, 0) = 0.0f;
    result(1, 1) = 1.0f / tanHalfFov;
    result(1, 2) = 0.0f;
    result(1, 3) = 0.0f;

    result(2, 0) = 0.0f;
    result(2, 1) = 0.0f;
    result(2, 2) = -(far + near) / (far - near);
    result(2, 3) = -(2.0f * far * near) / (far - near);

    result(3, 0) = 0.0f;
    result(3, 1) = 0.0f;
    result(3, 2) = -1.0f;
    result(3, 3) = 0.0f;

    return result;
}

Matrix4 Matrix4::orthographic(float left, float right, float bottom, float top, float near, float far) {
    Matrix4 result = identity();
    result(0, 0) = 2.0f / (right - left);
    result(1, 1) = 2.0f / (top - bottom);
    result(2, 2) = -2.0f / (far - near);
    result(0, 3) = -(right + left) / (right - left);
    result(1, 3) = -(top + bottom) / (top - bottom);
    result(2, 3) = -(far + near) / (far - near);
    return result;
}

Matrix4 Matrix4::transpose() const {
    Matrix4 result;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result(i, j) = (*this)(j, i);
        }
    }
    return result;
}

Matrix4 Matrix4::inverse() const {
    // Simplified inverse calculation - for production use a proper matrix inverse
    Matrix4 result = *this;
    // This is a placeholder - implement proper matrix inverse
    return result;
}

Vector3 Matrix4::getTranslation() const {
    return Vector3((*this)(0, 3), (*this)(1, 3), (*this)(2, 3));
}

Vector3 Matrix4::getScale() const {
    return Vector3(
        Vector3((*this)(0, 0), (*this)(1, 0), (*this)(2, 0)).length(),
        Vector3((*this)(0, 1), (*this)(1, 1), (*this)(2, 1)).length(),
        Vector3((*this)(0, 2), (*this)(1, 2), (*this)(2, 2)).length()
    );
}