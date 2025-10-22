#pragma once
#include "../Math/Vector3.h"
#include "../Math/Matrix4.h"
#include "../Math/Quaternion.h"

class Car {
public:
    struct Wheel {
        Vector3 position;
        Vector3 velocity;
        float rotation;
        float angularVelocity;
        float radius;
        float width;
        bool isGrounded;
        float suspensionLength;
        float suspensionStiffness;
        float damping;
        Vector3 normal; // Ground normal
    };

    struct Engine {
        float rpm;
        float maxRpm;
        float torque;
        float maxTorque;
        float throttle;
        float gear;
        float maxGear;
        float gearRatio;
        float finalDrive;
        bool isRunning;
    };

private:
    // Physical properties
    Vector3 position;
    Vector3 velocity;
    Vector3 angularVelocity;
    Quaternion rotation;
    
    float mass;
    Vector3 centerOfMass;
    Vector3 inertia;
    
    // Wheels
    Wheel wheels[4]; // Front left, front right, rear left, rear right
    
    // Engine
    Engine engine;
    
    // Input
    float throttleInput;
    float brakeInput;
    float steerInput;
    
    // Physics constants
    float maxSteerAngle;
    float maxSpeed;
    float acceleration;
    float brakeForce;
    float friction;
    float airResistance;
    float downforce;
    
    // Dash/Boost system
    float boostPower;
    float boostCapacity;
    float currentBoost;
    bool isBoosting;
    float boostCooldown;
    float boostRechargeRate;
    
    // Visual effects
    Vector3 lastPosition;
    Vector3 velocityDirection;
    float speedEffectIntensity;
    
    // State
    bool isGrounded;
    float groundHeight;
    Vector3 groundNormal;
    
    // Combat state
    bool handbrakeInput;
    bool combatMode;
    Vector3 aimDirection;
    float health;
    float maxHealth;
    bool isInvulnerable;
    float invulnerabilityTimer;

public:
    Car();
    Car(const Vector3& startPosition);
    
    // Getters
    Vector3 getPosition() const { return position; }
    Vector3 getVelocity() const { return velocity; }
    Vector3 getAngularVelocity() const { return angularVelocity; }
    Quaternion getRotation() const { return rotation; }
    Vector3 getForward() const;
    Vector3 getRight() const;
    Vector3 getUp() const;
    float getSpeed() const;
    float getSpeedKmh() const;
    bool getIsGrounded() const { return isGrounded; }
    float getCurrentBoost() const { return currentBoost; }
    bool getIsBoosting() const { return isBoosting; }
    float getSpeedEffectIntensity() const { return speedEffectIntensity; }
    
    // Combat getters
    bool getCombatMode() const { return combatMode; }
    Vector3 getAimDirection() const { return aimDirection; }
    float getHealth() const { return health; }
    float getMaxHealth() const { return maxHealth; }
    float getHealthPercentage() const { return maxHealth > 0.0f ? health / maxHealth : 0.0f; }
    bool getIsInvulnerable() const { return isInvulnerable; }
    bool getHandbrakeInput() const { return handbrakeInput; }
    
    // Setters
    void setPosition(const Vector3& pos);
    void setVelocity(const Vector3& vel);
    void setRotation(const Quaternion& rot);
    void setMass(float carMass);
    void setMaxSpeed(float speed);
    void setAcceleration(float accel);
    void setBrakeForce(float force);
    void setFriction(float fric);
    
    // Combat setters
    void setCombatMode(bool enabled);
    void setAimDirection(const Vector3& direction);
    void setHealth(float hp);
    void setMaxHealth(float maxHp);
    void setInvulnerable(bool invulnerable, float duration = 0.0f);
    
    // Input handling
    void setThrottle(float throttle);
    void setBrake(float brake);
    void setSteer(float steer);
    void setBoost(bool boost);
    void setHandbrake(bool handbrake);
    
    // Physics update
    void update(float deltaTime);
    void updatePhysics(float deltaTime);
    void updateWheels(float deltaTime);
    void updateEngine(float deltaTime);
    void updateBoost(float deltaTime);
    void updateVisualEffects(float deltaTime);
    void updateCombat(float deltaTime);
    
    // Collision and ground detection
    void checkGroundCollision();
    void applyGroundForces();
    void applyAirResistance();
    void applyDownforce();
    
    // Boost system
    void activateBoost();
    void deactivateBoost();
    void rechargeBoost(float deltaTime);
    
    // Combat functions
    void takeDamage(float damage);
    void heal(float amount);
    bool isDead() const { return health <= 0.0f; }
    void kill();
    void respawn();
    void respawn(const Vector3& position);
    
    // Utility functions
    Matrix4 getTransformMatrix() const;
    Vector3 getWheelPosition(int wheelIndex) const;
    Vector3 getProjectileSpawnPosition() const;
    void reset();
    void resetToPosition(const Vector3& pos);
    
    // Debug
    void debugDraw() const;
};