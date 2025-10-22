#include "Player.h"
#include <algorithm>

void Player::update(float deltaTime) {
    updateMovement(deltaTime);
    updateAnimation(deltaTime);
    updateEffects(deltaTime);
    checkGroundCollision();
    
    // Update timers
    if (dashTimer > 0) {
        dashTimer -= deltaTime;
        if (dashTimer <= 0 && state == PlayerState::Dashing) {
            state = PlayerState::Running;
        }
    }
    
    if (dashCooldownTimer > 0) {
        dashCooldownTimer -= deltaTime;
    }
    
    if (jumpTimer > 0) {
        jumpTimer -= deltaTime;
    }
}

void Player::handleInput(const Vec3& direction, bool dash, bool jump) {
    inputDirection = direction;
    
    // Handle dash input
    if (dash && !dashInput && dashCooldownTimer <= 0 && isGrounded) {
        startDash();
    }
    dashInput = dash;
    
    // Handle jump input
    if (jump && !jumpInput && isGrounded) {
        startJump();
    }
    jumpInput = jump;
}

void Player::startDash() {
    if (inputDirection.length() > 0.1f) {
        state = PlayerState::Dashing;
        dashTimer = dashDuration;
        dashCooldownTimer = dashCooldown;
        
        // Set dash velocity
        Vec3 dashDir = inputDirection.normalize();
        velocity = Vec3(dashDir.x * dashSpeed, velocity.y, dashDir.z * dashSpeed);
        
        // Enable dash trail effect
        showDashTrail = true;
        dashTrailIntensity = 1.0f;
    }
}

void Player::startJump() {
    state = PlayerState::Jumping;
    velocity.y = jumpForce;
    jumpTimer = 0.2f; // Brief jump state
    isGrounded = false;
}

void Player::updateMovement(float deltaTime) {
    Vec3 targetVelocity = velocity;
    
    switch (state) {
        case PlayerState::Idle:
            if (inputDirection.length() > 0.1f) {
                state = PlayerState::Running;
            }
            targetVelocity.x = 0;
            targetVelocity.z = 0;
            break;
            
        case PlayerState::Running:
            if (inputDirection.length() < 0.1f) {
                state = PlayerState::Idle;
                targetVelocity.x = 0;
                targetVelocity.z = 0;
            } else {
                Vec3 moveDir = inputDirection.normalize();
                targetVelocity.x = moveDir.x * runSpeed;
                targetVelocity.z = moveDir.z * runSpeed;
                
                // Rotate player to face movement direction
                transform.lookAt(transform.position + moveDir);
            }
            break;
            
        case PlayerState::Dashing:
            // Dash velocity is set in startDash(), just maintain it
            if (dashTimer <= 0) {
                state = inputDirection.length() > 0.1f ? PlayerState::Running : PlayerState::Idle;
            }
            break;
            
        case PlayerState::Jumping:
        case PlayerState::Falling:
            // Maintain horizontal movement during jump/fall
            if (inputDirection.length() > 0.1f) {
                Vec3 moveDir = inputDirection.normalize();
                targetVelocity.x = moveDir.x * runSpeed * 0.7f; // Reduced air control
                targetVelocity.z = moveDir.z * runSpeed * 0.7f;
            }
            
            if (jumpTimer <= 0 && velocity.y < 0) {
                state = PlayerState::Falling;
            }
            break;
    }
    
    // Apply gravity
    if (!isGrounded) {
        targetVelocity.y += gravity * deltaTime;
    }
    
    // Smooth velocity changes (except for dashing)
    if (state != PlayerState::Dashing) {
        float lerpFactor = isGrounded ? 10.0f : 5.0f; // Faster response on ground
        velocity.x = velocity.x + (targetVelocity.x - velocity.x) * lerpFactor * deltaTime;
        velocity.z = velocity.z + (targetVelocity.z - velocity.z) * lerpFactor * deltaTime;
    }
    velocity.y = targetVelocity.y;
    
    // Apply velocity to position
    transform.position = transform.position + velocity * deltaTime;
}

void Player::updateAnimation(float deltaTime) {
    float speed = getCurrentSpeed();
    
    switch (state) {
        case PlayerState::Running:
            animationTime += runAnimationSpeed * (speed / runSpeed) * deltaTime;
            break;
        case PlayerState::Dashing:
            animationTime += dashAnimationSpeed * deltaTime;
            break;
        default:
            animationTime += 0.5f * deltaTime; // Slow idle animation
            break;
    }
    
    // Keep animation time in reasonable range
    if (animationTime > 1000.0f) {
        animationTime = 0;
    }
}

void Player::updateEffects(float deltaTime) {
    // Update dash trail effect
    if (showDashTrail) {
        dashTrailIntensity -= deltaTime * 3.0f; // Fade out over time
        if (dashTrailIntensity <= 0) {
            showDashTrail = false;
            dashTrailIntensity = 0;
        }
    }
}

void Player::checkGroundCollision() {
    // Simple ground check - in a real game, you'd use proper collision detection
    if (transform.position.y <= 0) {
        transform.position.y = 0;
        velocity.y = 0;
        isGrounded = true;
        
        if (state == PlayerState::Falling) {
            state = inputDirection.length() > 0.1f ? PlayerState::Running : PlayerState::Idle;
        }
    } else if (transform.position.y > groundCheckDistance) {
        isGrounded = false;
    }
}

float Player::getCurrentSpeed() const {
    return std::sqrt(velocity.x * velocity.x + velocity.z * velocity.z);
}