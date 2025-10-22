#include "Math.h"
#include <cstring>

Mat4 Mat4::perspective(float fov, float aspect, float near, float far) {
    Mat4 result;
    memset(result.m, 0, sizeof(result.m));
    
    float tanHalfFov = std::tan(fov * 0.5f);
    
    result.m[0] = 1.0f / (aspect * tanHalfFov);
    result.m[5] = 1.0f / tanHalfFov;
    result.m[10] = -(far + near) / (far - near);
    result.m[11] = -1.0f;
    result.m[14] = -(2.0f * far * near) / (far - near);
    
    return result;
}

Mat4 Mat4::lookAt(const Vec3& eye, const Vec3& target, const Vec3& up) {
    Vec3 forward = (target - eye).normalize();
    Vec3 right = forward.cross(up).normalize();
    Vec3 newUp = right.cross(forward);
    
    Mat4 result;
    result.m[0] = right.x;
    result.m[1] = newUp.x;
    result.m[2] = -forward.x;
    result.m[3] = 0;
    
    result.m[4] = right.y;
    result.m[5] = newUp.y;
    result.m[6] = -forward.y;
    result.m[7] = 0;
    
    result.m[8] = right.z;
    result.m[9] = newUp.z;
    result.m[10] = -forward.z;
    result.m[11] = 0;
    
    result.m[12] = -right.dot(eye);
    result.m[13] = -newUp.dot(eye);
    result.m[14] = forward.dot(eye);
    result.m[15] = 1;
    
    return result;
}

Mat4 Mat4::translate(const Vec3& translation) {
    Mat4 result;
    result.m[12] = translation.x;
    result.m[13] = translation.y;
    result.m[14] = translation.z;
    return result;
}

Mat4 Mat4::rotate(float angle, const Vec3& axis) {
    Mat4 result;
    
    float c = std::cos(angle);
    float s = std::sin(angle);
    float omc = 1.0f - c;
    
    Vec3 a = axis.normalize();
    
    result.m[0] = c + a.x * a.x * omc;
    result.m[1] = a.y * a.x * omc + a.z * s;
    result.m[2] = a.z * a.x * omc - a.y * s;
    result.m[3] = 0;
    
    result.m[4] = a.x * a.y * omc - a.z * s;
    result.m[5] = c + a.y * a.y * omc;
    result.m[6] = a.z * a.y * omc + a.x * s;
    result.m[7] = 0;
    
    result.m[8] = a.x * a.z * omc + a.y * s;
    result.m[9] = a.y * a.z * omc - a.x * s;
    result.m[10] = c + a.z * a.z * omc;
    result.m[11] = 0;
    
    result.m[12] = 0;
    result.m[13] = 0;
    result.m[14] = 0;
    result.m[15] = 1;
    
    return result;
}

Mat4 Mat4::scale(const Vec3& scale) {
    Mat4 result;
    result.m[0] = scale.x;
    result.m[5] = scale.y;
    result.m[10] = scale.z;
    return result;
}

Mat4 Mat4::operator*(const Mat4& other) const {
    Mat4 result;
    
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result.m[i * 4 + j] = 0;
            for (int k = 0; k < 4; k++) {
                result.m[i * 4 + j] += m[i * 4 + k] * other.m[k * 4 + j];
            }
        }
    }
    
    return result;
}

Vec3 Mat4::transformPoint(const Vec3& point) const {
    float x = m[0] * point.x + m[4] * point.y + m[8] * point.z + m[12];
    float y = m[1] * point.x + m[5] * point.y + m[9] * point.z + m[13];
    float z = m[2] * point.x + m[6] * point.y + m[10] * point.z + m[14];
    float w = m[3] * point.x + m[7] * point.y + m[11] * point.z + m[15];
    
    if (w != 0) {
        return Vec3(x / w, y / w, z / w);
    }
    return Vec3(x, y, z);
}

Quaternion Quaternion::fromAxisAngle(const Vec3& axis, float angle) {
    float halfAngle = angle * 0.5f;
    float s = std::sin(halfAngle);
    Vec3 a = axis.normalize();
    
    return Quaternion(a.x * s, a.y * s, a.z * s, std::cos(halfAngle));
}

Mat4 Quaternion::toMatrix() const {
    Mat4 result;
    
    float xx = x * x;
    float yy = y * y;
    float zz = z * z;
    float xy = x * y;
    float xz = x * z;
    float yz = y * z;
    float wx = w * x;
    float wy = w * y;
    float wz = w * z;
    
    result.m[0] = 1 - 2 * (yy + zz);
    result.m[1] = 2 * (xy + wz);
    result.m[2] = 2 * (xz - wy);
    result.m[3] = 0;
    
    result.m[4] = 2 * (xy - wz);
    result.m[5] = 1 - 2 * (xx + zz);
    result.m[6] = 2 * (yz + wx);
    result.m[7] = 0;
    
    result.m[8] = 2 * (xz + wy);
    result.m[9] = 2 * (yz - wx);
    result.m[10] = 1 - 2 * (xx + yy);
    result.m[11] = 0;
    
    result.m[12] = 0;
    result.m[13] = 0;
    result.m[14] = 0;
    result.m[15] = 1;
    
    return result;
}

Quaternion Quaternion::operator*(const Quaternion& other) const {
    return Quaternion(
        w * other.x + x * other.w + y * other.z - z * other.y,
        w * other.y - x * other.z + y * other.w + z * other.x,
        w * other.z + x * other.y - y * other.x + z * other.w,
        w * other.w - x * other.x - y * other.y - z * other.z
    );
}

Quaternion Quaternion::slerp(const Quaternion& other, float t) const {
    float dot = x * other.x + y * other.y + z * other.z + w * other.w;
    
    if (dot < 0.0f) {
        dot = -dot;
    }
    
    if (dot > 0.9995f) {
        // Linear interpolation for very close quaternions
        Quaternion result(
            x + t * (other.x - x),
            y + t * (other.y - y),
            z + t * (other.z - z),
            w + t * (other.w - w)
        );
        
        // Normalize
        float len = std::sqrt(result.x * result.x + result.y * result.y + 
                             result.z * result.z + result.w * result.w);
        if (len > 0) {
            result.x /= len;
            result.y /= len;
            result.z /= len;
            result.w /= len;
        }
        
        return result;
    }
    
    float theta = std::acos(dot);
    float sinTheta = std::sin(theta);
    float factor1 = std::sin((1 - t) * theta) / sinTheta;
    float factor2 = std::sin(t * theta) / sinTheta;
    
    return Quaternion(
        factor1 * x + factor2 * other.x,
        factor1 * y + factor2 * other.y,
        factor1 * z + factor2 * other.z,
        factor1 * w + factor2 * other.w
    );
}