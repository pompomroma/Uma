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
    , isMouseLookActive(false)
    , isCameraLocked(false)
    , cameraOffset(0.0f, 8.0f, -15.0f)
    , horizontalAngle(0.0f)
    , verticalAngle(-20.0f)
    , minVerticalAngle(-80.0f)
    , maxVerticalAngle(80.0f)
    , cameraLerpSpeed(8.0f) {
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
    , isMouseLookActive(false)
    , isCameraLocked(false)
    , cameraOffset(0.0f, 8.0f, -15.0f)
    , horizontalAngle(0.0f)
    , verticalAngle(-20.0f)
    , minVerticalAngle(-80.0f)
    , maxVerticalAngle(80.0f)
    , cameraLerpSpeed(8.0f) {
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

void Camera::handleMouseInput(float deltaX, float deltaY) {
    if (!isMouseLookActive) return;
    
    float sensitivity = mouseSensitivity * 0.1f;
    rotate(deltaX * sensitivity, -deltaY * sensitivity);
}

void Camera::handleScrollInput(float scrollDelta) {
    if (mode == CameraMode::ThirdPerson) {
        followDistance = std::max(1.0f, followDistance - scrollDelta * 2.0f);
    } else {
        moveForward(scrollDelta * 2.0f);
    }
}

void Camera::update(float deltaTime) {
    if (mode == CameraMode::ThirdPerson) {
        // Third person camera updates are handled by updateThirdPerson
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

// Mobile camera control implementations
void Camera::setCameraLocked(bool locked) {
    isCameraLocked = locked;
    if (locked) {
        // Initialize camera angles based on current camera state
        Vector3 direction = (target - position).normalized();
        horizontalAngle = std::atan2(direction.x, direction.z) * 180.0f / M_PI;
        verticalAngle = std::asin(-direction.y) * 180.0f / M_PI;
        verticalAngle = std::clamp(verticalAngle, minVerticalAngle, maxVerticalAngle);
    }
}

void Camera::handleScreenDrag(float deltaX, float deltaY, float screenWidth, float screenHeight) {
    if (!isCameraLocked) return;
    
    // Convert screen delta to angle delta
    float sensitivity = mouseSensitivity * 0.5f; // Adjust for touch sensitivity
    
    // Horizontal rotation (yaw)
    horizontalAngle += deltaX * sensitivity;
    
    // Vertical rotation (pitch) - inverted for natural touch feel
    verticalAngle -= deltaY * sensitivity;
    verticalAngle = std::clamp(verticalAngle, minVerticalAngle, maxVerticalAngle);
}

void Camera::updateLockedThirdPerson(const Vector3& targetPosition, float deltaTime) {
    if (!isCameraLocked) {
        // Fall back to regular third person if not locked
        updateThirdPerson(targetPosition, Vector3::forward());
        return;
    }
    
    // Calculate camera position based on angles
    float hRadians = horizontalAngle * M_PI / 180.0f;
    float vRadians = verticalAngle * M_PI / 180.0f;
    
    // Calculate offset from target
    Vector3 offset;
    offset.x = std::sin(hRadians) * std::cos(vRadians) * followDistance;
    offset.y = std::sin(vRadians) * followDistance + followHeight;
    offset.z = std::cos(hRadians) * std::cos(vRadians) * followDistance;
    
    Vector3 desiredPosition = targetPosition + offset;
    
    // Smooth camera movement
    Vector3 direction = desiredPosition - position;
    float distance = direction.length();
    
    if (distance > 0.01f) {
        float lerpFactor = std::min(1.0f, cameraLerpSpeed * deltaTime);
        position = position + direction * lerpFactor;
    }
    
    // Always look at the target
    target = targetPosition + Vector3(0, 1.0f, 0); // Look slightly above target center
    updateVectors();
}

Vector3 Camera::getCameraForwardHorizontal() const {
    // Get forward direction projected onto horizontal plane
    Vector3 horizontalForward = forward;
    horizontalForward.y = 0;
    return horizontalForward.normalized();
}

void Camera::setVerticalAngleLimits(float minAngle, float maxAngle) {
    minVerticalAngle = minAngle;
    maxVerticalAngle = maxAngle;
}