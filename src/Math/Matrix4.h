#pragma once
#include "Vector3.h"

class Matrix4 {
public:
    float m[16];

    Matrix4();
    Matrix4(const Matrix4& other);
    
    // Matrix operations
    Matrix4 operator*(const Matrix4& other) const;
    Vector3 operator*(const Vector3& v) const;
    Matrix4& operator*=(const Matrix4& other);

    // Access elements
    float& operator()(int row, int col);
    const float& operator()(int row, int col) const;

    // Static factory methods
    static Matrix4 identity();
    static Matrix4 translation(const Vector3& translation);
    static Matrix4 rotation(const Vector3& axis, float angle);
    static Matrix4 scale(const Vector3& scale);
    static Matrix4 lookAt(const Vector3& eye, const Vector3& center, const Vector3& up);
    static Matrix4 perspective(float fov, float aspect, float near, float far);
    static Matrix4 orthographic(float left, float right, float bottom, float top, float near, float far);

    // Utility functions
    Matrix4 transpose() const;
    Matrix4 inverse() const;
    Vector3 getTranslation() const;
    Vector3 getScale() const;
};