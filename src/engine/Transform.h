#pragma once
#include "Math.h"

class Transform {
public:
    Vec3 position;
    Quaternion rotation;
    Vec3 scale;
    
    Transform() : position(0, 0, 0), rotation(), scale(1, 1, 1) {}
    
    Mat4 getMatrix() const {
        Mat4 t = Mat4::translate(position);
        Mat4 r = rotation.toMatrix();
        Mat4 s = Mat4::scale(scale);
        return t * r * s;
    }
    
    Vec3 forward() const {
        return rotation.toMatrix().transformPoint(Vec3(0, 0, -1)).normalize();
    }
    
    Vec3 right() const {
        return rotation.toMatrix().transformPoint(Vec3(1, 0, 0)).normalize();
    }
    
    Vec3 up() const {
        return rotation.toMatrix().transformPoint(Vec3(0, 1, 0)).normalize();
    }
    
    void lookAt(const Vec3& target, const Vec3& up = Vec3(0, 1, 0)) {
        Vec3 forward = (target - position).normalize();
        Vec3 right = forward.cross(up).normalize();
        Vec3 newUp = right.cross(forward);
        
        // Convert to quaternion (simplified)
        Mat4 rotMatrix;
        rotMatrix.m[0] = right.x;   rotMatrix.m[4] = right.y;   rotMatrix.m[8] = right.z;
        rotMatrix.m[1] = newUp.x;   rotMatrix.m[5] = newUp.y;   rotMatrix.m[9] = newUp.z;
        rotMatrix.m[2] = -forward.x; rotMatrix.m[6] = -forward.y; rotMatrix.m[10] = -forward.z;
        
        // Convert rotation matrix to quaternion
        float trace = rotMatrix.m[0] + rotMatrix.m[5] + rotMatrix.m[10];
        if (trace > 0) {
            float s = std::sqrt(trace + 1.0f) * 2;
            rotation.w = 0.25f * s;
            rotation.x = (rotMatrix.m[9] - rotMatrix.m[6]) / s;
            rotation.y = (rotMatrix.m[2] - rotMatrix.m[8]) / s;
            rotation.z = (rotMatrix.m[4] - rotMatrix.m[1]) / s;
        } else if (rotMatrix.m[0] > rotMatrix.m[5] && rotMatrix.m[0] > rotMatrix.m[10]) {
            float s = std::sqrt(1.0f + rotMatrix.m[0] - rotMatrix.m[5] - rotMatrix.m[10]) * 2;
            rotation.w = (rotMatrix.m[9] - rotMatrix.m[6]) / s;
            rotation.x = 0.25f * s;
            rotation.y = (rotMatrix.m[1] + rotMatrix.m[4]) / s;
            rotation.z = (rotMatrix.m[2] + rotMatrix.m[8]) / s;
        } else if (rotMatrix.m[5] > rotMatrix.m[10]) {
            float s = std::sqrt(1.0f + rotMatrix.m[5] - rotMatrix.m[0] - rotMatrix.m[10]) * 2;
            rotation.w = (rotMatrix.m[2] - rotMatrix.m[8]) / s;
            rotation.x = (rotMatrix.m[1] + rotMatrix.m[4]) / s;
            rotation.y = 0.25f * s;
            rotation.z = (rotMatrix.m[6] + rotMatrix.m[9]) / s;
        } else {
            float s = std::sqrt(1.0f + rotMatrix.m[10] - rotMatrix.m[0] - rotMatrix.m[5]) * 2;
            rotation.w = (rotMatrix.m[4] - rotMatrix.m[1]) / s;
            rotation.x = (rotMatrix.m[2] + rotMatrix.m[8]) / s;
            rotation.y = (rotMatrix.m[6] + rotMatrix.m[9]) / s;
            rotation.z = 0.25f * s;
        }
    }
};