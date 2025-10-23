#pragma once
#include "../Math/Vector3.h"
#include "../Math/Matrix4.h"
#include "../Math/Quaternion.h"

class Camera {
public:
    enum class CameraMode {
        ThirdPerson,
        ThirdPersonLocked,
        FirstPerson,
        Free
    };

private:
    Vector3 position;
    Vector3 target;
    Vector3 up;
    Vector3 forward;
    Vector3 right;
    
    float fov;
    float aspectRatio;
    float nearPlane;
    float farPlane;
    
    CameraMode mode;
    
    // Third person specific
    Vector3 followTarget;
    float followDistance;
    float followHeight;
    float followAngle;
    float mouseSensitivity;
    
    // Locked third person specific
    bool isLocked;
    Vector3 lockedOffset;  // Fixed offset from target
    float lockedDistance;
    float lockedHeight;
    float lockedYaw;       // Fixed yaw angle
    float lockedPitch;     // Fixed pitch angle
    
    // Camera smoothing
    Vector3 velocity;
    float smoothSpeed;
    
    // Input state
    float yaw;
    float pitch;
    bool isMouseLookActive;

public:
    Camera();
    Camera(const Vector3& position, const Vector3& target, const Vector3& up);
    
    // Getters
    Vector3 getPosition() const { return position; }
    Vector3 getTarget() const { return target; }
    Vector3 getUp() const { return up; }
    Vector3 getForward() const { return forward; }
    Vector3 getRight() const { return right; }
    float getFov() const { return fov; }
    float getAspectRatio() const { return aspectRatio; }
    float getNearPlane() const { return nearPlane; }
    float getFarPlane() const { return farPlane; }
    CameraMode getMode() const { return mode; }
    
    // Setters
    void setPosition(const Vector3& pos);
    void setTarget(const Vector3& tgt);
    void setUp(const Vector3& upVec);
    void setFov(float fovDegrees);
    void setAspectRatio(float aspect);
    void setNearPlane(float near);
    void setFarPlane(float far);
    void setMode(CameraMode newMode);
    
    // Third person camera controls
    void setFollowTarget(const Vector3& target);
    void setFollowDistance(float distance);
    void setFollowHeight(float height);
    void setFollowAngle(float angle);
    void setMouseSensitivity(float sensitivity);
    
    // Locked third person camera controls
    void setLockedMode(bool locked);
    void setLockedOffset(const Vector3& offset);
    void setLockedDistance(float distance);
    void setLockedHeight(float height);
    void setLockedAngles(float yaw, float pitch);
    bool isLockedMode() const { return isLocked; }
    
    // Camera movement
    void move(const Vector3& offset);
    void moveForward(float distance);
    void moveRight(float distance);
    void moveUp(float distance);
    void rotate(float yawDelta, float pitchDelta);
    void lookAt(const Vector3& target);
    
    // Third person specific
    void updateThirdPerson(const Vector3& targetPosition, const Vector3& targetForward);
    void updateLockedThirdPerson(const Vector3& targetPosition, const Vector3& targetForward);
    void handleMouseInput(float deltaX, float deltaY);
    void handleScrollInput(float scrollDelta);
    void handleTouchCameraInput(float deltaX, float deltaY);
    
    // Update camera
    void update(float deltaTime);
    
    // Matrix generation
    Matrix4 getViewMatrix() const;
    Matrix4 getProjectionMatrix() const;
    Matrix4 getViewProjectionMatrix() const;
    
    // Utility functions
    void reset();
    void resetThirdPerson();
    Vector3 screenToWorld(const Vector3& screenPos, float screenWidth, float screenHeight) const;
    Vector3 worldToScreen(const Vector3& worldPos, float screenWidth, float screenHeight) const;
    
private:
    void updateVectors();
};