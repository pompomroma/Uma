#pragma once
#include "Transform.h"
#include "Math.h"

enum class PlayerState {
    Idle,
    Running,
    Dashing,
    Jumping,
    Falling
};

class Player {
public:
    Transform transform;
    Vec3 velocity;
    PlayerState state;
    
    // Movement parameters
    float runSpeed;
    float dashSpeed;
    float dashDuration;
    float dashCooldown;
    float jumpForce;
    float gravity;
    
    // State timers
    float dashTimer;
    float dashCooldownTimer;
    float jumpTimer;
    
    // Animation parameters
    float runAnimationSpeed;
    float dashAnimationSpeed;
    float animationTime;
    
    // Ground detection
    bool isGrounded;
    float groundCheckDistance;
    
    // Input
    Vec3 inputDirection;
    bool dashInput;
    bool jumpInput;
    
    // Visual effects
    bool showDashTrail;
    float dashTrailIntensity;
    Vec3 dashTrailColor;
    
    Player() :
        state(PlayerState::Idle),
        velocity(0, 0, 0),
        runSpeed(10.0f),
        dashSpeed(25.0f),
        dashDuration(0.3f),
        dashCooldown(1.0f),
        jumpForce(15.0f),
        gravity(-30.0f),
        dashTimer(0),
        dashCooldownTimer(0),
        jumpTimer(0),
        runAnimationSpeed(1.0f),
        dashAnimationSpeed(2.0f),
        animationTime(0),
        isGrounded(true),
        groundCheckDistance(1.1f),
        inputDirection(0, 0, 0),
        dashInput(false),
        jumpInput(false),
        showDashTrail(false),
        dashTrailIntensity(0),
        dashTrailColor(1, 0.5f, 0) {}
    
    void update(float deltaTime);
    void handleInput(const Vec3& direction, bool dash, bool jump);
    void startDash();
    void startJump();
    void checkGroundCollision();
    
    // Get current speed for camera shake and effects
    float getCurrentSpeed() const;
    bool isDashing() const { return state == PlayerState::Dashing; }
    bool isRunning() const { return state == PlayerState::Running; }
    
private:
    void updateMovement(float deltaTime);
    void updateAnimation(float deltaTime);
    void updateEffects(float deltaTime);
};