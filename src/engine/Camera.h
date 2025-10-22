#pragma once
#include "Math.h"
#include "Transform.h"

class Camera {
public:
    Transform transform;
    float fov;
    float aspectRatio;
    float nearPlane;
    float farPlane;
    
    // Third person camera settings
    Vec3 targetPosition;
    Vec3 offset;
    float followSpeed;
    float rotationSpeed;
    float smoothTime;
    
    // Camera shake for racing effects
    float shakeIntensity;
    float shakeDuration;
    float shakeTimer;
    Vec3 shakeOffset;
    
    Camera() : 
        fov(60.0f * M_PI / 180.0f),
        aspectRatio(16.0f / 9.0f),
        nearPlane(0.1f),
        farPlane(1000.0f),
        offset(0, 5, 10),
        followSpeed(5.0f),
        rotationSpeed(2.0f),
        smoothTime(0.3f),
        shakeIntensity(0),
        shakeDuration(0),
        shakeTimer(0) {}
    
    Mat4 getViewMatrix() const {
        Vec3 pos = transform.position + shakeOffset;
        Vec3 target = pos + transform.forward();
        return Mat4::lookAt(pos, target, Vec3(0, 1, 0));
    }
    
    Mat4 getProjectionMatrix() const {
        return Mat4::perspective(fov, aspectRatio, nearPlane, farPlane);
    }
    
    void followTarget(const Vec3& target, const Vec3& targetForward, float deltaTime);
    void addShake(float intensity, float duration);
    void update(float deltaTime);
    
private:
    Vec3 velocity;
    Vec3 lastTargetPosition;
};