#include "Camera.h"
#include <cstdlib>
#include <ctime>

void Camera::followTarget(const Vec3& target, const Vec3& targetForward, float deltaTime) {
    // Calculate desired position behind the target
    Vec3 desiredPosition = target - targetForward * offset.z + Vec3(0, offset.y, 0);
    
    // Smooth movement using velocity
    Vec3 displacement = desiredPosition - transform.position;
    velocity = velocity + displacement * (followSpeed * deltaTime);
    velocity = velocity * (1.0f - smoothTime * deltaTime); // Damping
    
    transform.position = transform.position + velocity * deltaTime;
    
    // Look at the target with some offset ahead
    Vec3 lookTarget = target + targetForward * 5.0f;
    transform.lookAt(lookTarget);
    
    targetPosition = target;
}

void Camera::addShake(float intensity, float duration) {
    shakeIntensity = intensity;
    shakeDuration = duration;
    shakeTimer = 0;
}

void Camera::update(float deltaTime) {
    // Update camera shake
    if (shakeTimer < shakeDuration) {
        shakeTimer += deltaTime;
        float shakeAmount = shakeIntensity * (1.0f - shakeTimer / shakeDuration);
        
        // Generate random shake offset
        shakeOffset = Vec3(
            ((float)rand() / RAND_MAX - 0.5f) * 2.0f * shakeAmount,
            ((float)rand() / RAND_MAX - 0.5f) * 2.0f * shakeAmount,
            ((float)rand() / RAND_MAX - 0.5f) * 2.0f * shakeAmount
        );
    } else {
        shakeOffset = Vec3(0, 0, 0);
    }
}