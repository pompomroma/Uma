#include "Camera.h"
#include <cmath>
#include <algorithm>

Camera::Camera() 
    : position(0.0f, 5.0f, 10.0f)
    , target(0.0f, 0.0f, 0.0f)
    , up(0.0f, 1.0f, 0.0f)
    , forward(0.0f, 0.0f, -1.0f)
    , right(1.0f, 0.0f, 0.0f)
    , fov(45.0f)
    , aspectRatio(16.0f / 9.0f)
    , nearPlane(0.1f)
    , farPlane(1000.0f)
    , mode(CameraMode::ThirdPerson)
    , followTarget(0.0f, 0.0f, 0.0f)
    , followDistance(15.0f)
    , followHeight(8.0f)
    , followAngle(0.0f)
    , mouseSensitivity(2.0f)
    , velocity(0.0f, 0.0f, 0.0f)
    , smoothSpeed(5.0f)
    , yaw(0.0f)
    , pitch(0.0f)
    , isMouseLookActive(false) {
    updateVectors();
}

Camera::Camera(const Vector3& position, const Vector3& target, const Vector3& up)
    : position(position)
    , target(target)
    , up(up)
    , fov(45.0f)
    , aspectRatio(16.0f / 9.0f)
    , nearPlane(0.1f)
    , farPlane(1000.0f)
    , mode(CameraMode::ThirdPerson)
    , followTarget(0.0f, 0.0f, 0.0f)
    , followDistance(15.0f)
    , followHeight(8.0f)
    , followAngle(0.0f)
    , mouseSensitivity(2.0f)
    , velocity(0.0f, 0.0f, 0.0f)
    , smoothSpeed(5.0f)
    , yaw(0.0f)
    , pitch(0.0f)
    , isMouseLookActive(false) {
    updateVectors();
}

void Camera::setPosition(const Vector3& pos) {
    position = pos;
    updateVectors();
}

void Camera::setTarget(const Vector3& tgt) {
    target = tgt;
    updateVectors();
}

void Camera::setUp(const Vector3& upVec) {
    up = upVec.normalized();
    updateVectors();
}

void Camera::setFov(float fovDegrees) {
    fov = std::clamp(fovDegrees, 1.0f, 120.0f);
}

void Camera::setAspectRatio(float aspect) {
    aspectRatio = aspect;
}

void Camera::setNearPlane(float near) {
    nearPlane = near;
}

void Camera::setFarPlane(float far) {
    farPlane = far;
}

void Camera::setMode(CameraMode newMode) {
    mode = newMode;
    if (mode == CameraMode::ThirdPerson) {
        resetThirdPerson();
    }
}

void Camera::setFollowTarget(const Vector3& target) {
    followTarget = target;
}

void Camera::setFollowDistance(float distance) {
    followDistance = std::max(1.0f, distance);
}

void Camera::setFollowHeight(float height) {
    followHeight = height;
}

void Camera::setFollowAngle(float angle) {
    followAngle = angle;
}

void Camera::setMouseSensitivity(float sensitivity) {
    mouseSensitivity = std::max(0.1f, sensitivity);
}

void Camera::setLockedMode(bool locked) {
    isLocked = locked;
    if (locked) {
        mode = CameraMode::ThirdPersonLocked;
    }
}

void Camera::setLockedOffset(const Vector3& offset) {
    lockedOffset = offset;
}

void Camera::setLockedDistance(float distance) {
    lockedDistance = std::max(1.0f, distance);
}

void Camera::setLockedHeight(float height) {
    lockedHeight = height;
}

void Camera::setLockedAngles(float yaw, float pitch) {
    lockedYaw = yaw;
    lockedPitch = std::clamp(pitch, -89.0f, 89.0f);
}

void Camera::move(const Vector3& offset) {
    position += offset;
    target += offset;
}

void Camera::moveForward(float distance) {
    Vector3 offset = forward * distance;
    position += offset;
    target += offset;
}

void Camera::moveRight(float distance) {
    Vector3 offset = right * distance;
    position += offset;
    target += offset;
}

void Camera::moveUp(float distance) {
    Vector3 offset = up * distance;
    position += offset;
    target += offset;
}

void Camera::rotate(float yawDelta, float pitchDelta) {
    yaw += yawDelta;
    pitch += pitchDelta;
    
    // Clamp pitch to prevent camera flipping
    pitch = std::clamp(pitch, -89.0f, 89.0f);
    
    updateVectors();
}

void Camera::lookAt(const Vector3& targetPos) {
    target = targetPos;
    updateVectors();
}

void Camera::updateThirdPerson(const Vector3& targetPosition, const Vector3& targetForward) {
    if (mode != CameraMode::ThirdPerson) return;
    
    // Calculate desired camera position
    Vector3 targetRight = targetForward.cross(Vector3::up()).normalized();
    Vector3 targetUp = targetRight.cross(targetForward).normalized();
    
    // Apply yaw rotation around the target
    float yawRad = yaw * M_PI / 180.0f;
    float pitchRad = pitch * M_PI / 180.0f;
    
    Vector3 offset = targetRight * std::sin(yawRad) * followDistance +
                    targetUp * followHeight +
                    targetForward * std::cos(yawRad) * followDistance;
    
    Vector3 desiredPosition = targetPosition + offset;
    
    // Apply pitch to look down/up
    Vector3 pitchOffset = targetUp * std::sin(pitchRad) * followDistance;
    desiredPosition += pitchOffset;
    
    // Smooth camera movement
    Vector3 direction = desiredPosition - position;
    float distance = direction.length();
    
    if (distance > 0.01f) {
        velocity = direction.normalized() * std::min(distance * smoothSpeed, distance);
        position += velocity;
    }
    
    // Always look at the target
    target = targetPosition;
    updateVectors();
}

void Camera::updateLockedThirdPerson(const Vector3& targetPosition, const Vector3& targetForward) {
    if (mode != CameraMode::ThirdPersonLocked) return;
    
    // Calculate camera position based on locked angles and target's orientation
    Vector3 targetRight = targetForward.cross(Vector3::up()).normalized();
    Vector3 targetUp = targetRight.cross(targetForward).normalized();
    
    // Apply locked yaw and pitch
    float yawRad = lockedYaw * M_PI / 180.0f;
    float pitchRad = lockedPitch * M_PI / 180.0f;
    
    // Calculate offset from target
    Vector3 offset = targetRight * std::sin(yawRad) * lockedDistance +
                    targetUp * lockedHeight +
                    targetForward * std::cos(yawRad) * lockedDistance;
    
    // Apply pitch offset
    Vector3 pitchOffset = targetUp * std::sin(pitchRad) * lockedDistance;
    offset += pitchOffset;
    
    // Set camera position
    Vector3 desiredPosition = targetPosition + offset + lockedOffset;
    
    // Smooth camera movement
    Vector3 direction = desiredPosition - position;
    float distance = direction.length();
    
    if (distance > 0.01f) {
        velocity = direction.normalized() * std::min(distance * smoothSpeed, distance);
        position += velocity;
    }
    
    // Always look at the target
    target = targetPosition;
    updateVectors();
}

void Camera::handleMouseInput(float deltaX, float deltaY) {
    if (!isMouseLookActive) return;
    
    float sensitivity = mouseSensitivity * 0.1f;
    rotate(deltaX * sensitivity, -deltaY * sensitivity);
}

void Camera::handleTouchCameraInput(float deltaX, float deltaY) {
    if (mode != CameraMode::ThirdPersonLocked) return;
    
    float sensitivity = mouseSensitivity * 0.2f;
    
    // Update locked angles based on touch input
    lockedYaw += deltaX * sensitivity;
    lockedPitch += -deltaY * sensitivity;  // Invert Y for touch
    
    // Clamp pitch to prevent camera flipping
    lockedPitch = std::clamp(lockedPitch, -89.0f, 89.0f);
    
    // Normalize yaw
    while (lockedYaw > 360.0f) lockedYaw -= 360.0f;
    while (lockedYaw < 0.0f) lockedYaw += 360.0f;
}

void Camera::handleScrollInput(float scrollDelta) {
    if (mode == CameraMode::ThirdPerson) {
        followDistance = std::max(1.0f, followDistance - scrollDelta * 2.0f);
    } else {
        moveForward(scrollDelta * 2.0f);
    }
}

void Camera::update(float deltaTime) {
    if (mode == CameraMode::ThirdPerson || mode == CameraMode::ThirdPersonLocked) {
        // Third person camera updates are handled by updateThirdPerson or updateLockedThirdPerson
        return;
    }
    
    // Smooth camera movement for other modes
    if (velocity.length() > 0.01f) {
        position += velocity * deltaTime;
        velocity *= 0.9f; // Damping
    }
}

Matrix4 Camera::getViewMatrix() const {
    return Matrix4::lookAt(position, target, up);
}

Matrix4 Camera::getProjectionMatrix() const {
    return Matrix4::perspective(fov * M_PI / 180.0f, aspectRatio, nearPlane, farPlane);
}

Matrix4 Camera::getViewProjectionMatrix() const {
    return getProjectionMatrix() * getViewMatrix();
}

void Camera::reset() {
    position = Vector3(0.0f, 5.0f, 10.0f);
    target = Vector3(0.0f, 0.0f, 0.0f);
    up = Vector3(0.0f, 1.0f, 0.0f);
    yaw = 0.0f;
    pitch = 0.0f;
    velocity = Vector3::zero();
    updateVectors();
}

void Camera::resetThirdPerson() {
    followDistance = 15.0f;
    followHeight = 8.0f;
    followAngle = 0.0f;
    yaw = 0.0f;
    pitch = -20.0f; // Look down slightly
}

Vector3 Camera::screenToWorld(const Vector3& screenPos, float screenWidth, float screenHeight) const {
    // Convert screen coordinates to normalized device coordinates
    float x = (2.0f * screenPos.x) / screenWidth - 1.0f;
    float y = 1.0f - (2.0f * screenPos.y) / screenHeight;
    float z = screenPos.z;
    
    // Convert to clip coordinates
    Vector3 clipCoords(x, y, z);
    
    // Convert to eye coordinates
    Matrix4 projInverse = getProjectionMatrix().inverse();
    Vector3 eyeCoords = projInverse * clipCoords;
    
    // Convert to world coordinates
    Matrix4 viewInverse = getViewMatrix().inverse();
    return viewInverse * eyeCoords;
}

Vector3 Camera::worldToScreen(const Vector3& worldPos, float screenWidth, float screenHeight) const {
    // Convert world position to clip coordinates
    Vector3 clipCoords = getViewProjectionMatrix() * worldPos;
    
    // Convert to normalized device coordinates
    if (clipCoords.z != 0.0f) {
        clipCoords.x /= clipCoords.z;
        clipCoords.y /= clipCoords.z;
    }
    
    // Convert to screen coordinates
    float x = (clipCoords.x + 1.0f) * 0.5f * screenWidth;
    float y = (1.0f - clipCoords.y) * 0.5f * screenHeight;
    
    return Vector3(x, y, clipCoords.z);
}

void Camera::updateVectors() {
    forward = (target - position).normalized();
    right = forward.cross(up).normalized();
    up = right.cross(forward).normalized();
}