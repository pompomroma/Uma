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
    , cameraYaw(0.0f)
    , cameraPitch(-20.0f)
    , targetYaw(0.0f)
    , targetPitch(-20.0f)
    , rotationSmoothSpeed(8.0f) {
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
    , cameraYaw(0.0f)
    , cameraPitch(-20.0f)
    , targetYaw(0.0f)
    , targetPitch(-20.0f)
    , rotationSmoothSpeed(8.0f) {
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

void Camera::updateThirdPerson(const Vector3& targetPosition, const Vector3& targetForward, float deltaTime) {
    if (mode != CameraMode::ThirdPerson) return;
    
    // Update smooth rotation
    updateSmoothRotation(deltaTime);
    
    // Calculate camera position based on current rotation
    float yawRad = cameraYaw * M_PI / 180.0f;
    float pitchRad = cameraPitch * M_PI / 180.0f;
    
    // Calculate offset from target using spherical coordinates
    float cosYaw = std::cos(yawRad);
    float sinYaw = std::sin(yawRad);
    float cosPitch = std::cos(pitchRad);
    float sinPitch = std::sin(pitchRad);
    
    // Camera offset in world space (Roblox-style orbit camera)
    Vector3 offset(
        sinYaw * cosPitch * followDistance,
        sinPitch * followDistance + followHeight,
        cosYaw * cosPitch * followDistance
    );
    
    Vector3 desiredPosition = targetPosition + offset;
    
    // Smooth camera position
    Vector3 positionDiff = desiredPosition - position;
    float smoothFactor = 1.0f - std::exp(-smoothSpeed * deltaTime);
    position += positionDiff * smoothFactor;
    
    // Always look at the target with a slight offset for better visibility
    target = targetPosition + Vector3(0, 2.0f, 0);  // Look slightly above the player
    updateVectors();
}

void Camera::updateSmoothRotation(float deltaTime) {
    // Smooth rotation interpolation
    float smoothFactor = 1.0f - std::exp(-rotationSmoothSpeed * deltaTime);
    
    // Interpolate yaw (handle wrapping around 360 degrees)
    float yawDiff = targetYaw - cameraYaw;
    if (yawDiff > 180.0f) yawDiff -= 360.0f;
    if (yawDiff < -180.0f) yawDiff += 360.0f;
    cameraYaw += yawDiff * smoothFactor;
    
    // Keep yaw in [0, 360) range
    while (cameraYaw < 0.0f) cameraYaw += 360.0f;
    while (cameraYaw >= 360.0f) cameraYaw -= 360.0f;
    
    // Interpolate pitch
    cameraPitch += (targetPitch - cameraPitch) * smoothFactor;
}

void Camera::handleMouseInput(float deltaX, float deltaY) {
    if (!isMouseLookActive) return;
    
    float sensitivity = mouseSensitivity * 0.1f;
    rotate(deltaX * sensitivity, -deltaY * sensitivity);
}

void Camera::handleTouchDrag(float deltaX, float deltaY) {
    // Touch drag for camera rotation (Roblox-style)
    float sensitivity = mouseSensitivity * 2.0f;  // Higher sensitivity for touch
    
    targetYaw += deltaX * sensitivity;
    targetPitch -= deltaY * sensitivity;
    
    // Clamp pitch to prevent camera flipping
    targetPitch = std::clamp(targetPitch, -80.0f, 80.0f);
    
    // Keep yaw in reasonable range
    while (targetYaw < 0.0f) targetYaw += 360.0f;
    while (targetYaw >= 360.0f) targetYaw -= 360.0f;
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
        // Update smooth rotation even when not explicitly updating third person
        updateSmoothRotation(deltaTime);
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
    cameraYaw = 0.0f;
    cameraPitch = -20.0f;
    targetYaw = 0.0f;
    targetPitch = -20.0f;
}

void Camera::setCameraRotation(float newYaw, float newPitch) {
    targetYaw = newYaw;
    targetPitch = std::clamp(newPitch, -80.0f, 80.0f);
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