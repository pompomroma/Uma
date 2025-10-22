#include "Car.h"
#include <cmath>
#include <algorithm>

Car::Car() 
    : position(0.0f, 0.0f, 0.0f)
    , velocity(0.0f, 0.0f, 0.0f)
    , angularVelocity(0.0f, 0.0f, 0.0f)
    , rotation(Quaternion::identity())
    , mass(1200.0f)
    , centerOfMass(0.0f, -0.5f, 0.0f)
    , inertia(1.0f, 1.0f, 1.0f)
    , throttleInput(0.0f)
    , brakeInput(0.0f)
    , steerInput(0.0f)
    , handbrakeInput(false)
    , combatMode(false)
    , aimDirection(0.0f, 0.0f, 1.0f)
    , health(100.0f)
    , maxHealth(100.0f)
    , isInvulnerable(false)
    , invulnerabilityTimer(0.0f)
    , maxSteerAngle(30.0f)
    , maxSpeed(50.0f)
    , acceleration(20.0f)
    , brakeForce(30.0f)
    , friction(0.8f)
    , airResistance(0.3f)
    , downforce(0.1f)
    , boostPower(50.0f)
    , boostCapacity(100.0f)
    , currentBoost(boostCapacity)
    , isBoosting(false)
    , boostCooldown(0.0f)
    , boostRechargeRate(20.0f)
    , lastPosition(0.0f, 0.0f, 0.0f)
    , velocityDirection(0.0f, 0.0f, 0.0f)
    , speedEffectIntensity(0.0f)
    , isGrounded(false)
    , groundHeight(0.0f)
    , groundNormal(0.0f, 1.0f, 0.0f) {
    
    // Initialize wheels
    wheels[0].position = Vector3(-1.0f, -0.5f, 1.5f);  // Front left
    wheels[1].position = Vector3(1.0f, -0.5f, 1.5f);   // Front right
    wheels[2].position = Vector3(-1.0f, -0.5f, -1.5f); // Rear left
    wheels[3].position = Vector3(1.0f, -0.5f, -1.5f);  // Rear right
    
    for (int i = 0; i < 4; i++) {
        wheels[i].velocity = Vector3::zero();
        wheels[i].rotation = 0.0f;
        wheels[i].angularVelocity = 0.0f;
        wheels[i].radius = 0.3f;
        wheels[i].width = 0.2f;
        wheels[i].isGrounded = false;
        wheels[i].suspensionLength = 0.5f;
        wheels[i].suspensionStiffness = 20.0f;
        wheels[i].damping = 2.0f;
        wheels[i].normal = Vector3::up();
    }
    
    // Initialize engine
    engine.rpm = 0.0f;
    engine.maxRpm = 6000.0f;
    engine.torque = 0.0f;
    engine.maxTorque = 300.0f;
    engine.throttle = 0.0f;
    engine.gear = 1.0f;
    engine.maxGear = 6.0f;
    engine.gearRatio = 2.5f;
    engine.finalDrive = 3.5f;
    engine.isRunning = true;
}

Car::Car(const Vector3& startPosition) : Car() {
    position = startPosition;
}

Vector3 Car::getForward() const {
    return rotation * Vector3::forward();
}

Vector3 Car::getRight() const {
    return rotation * Vector3::right();
}

Vector3 Car::getUp() const {
    return rotation * Vector3::up();
}

float Car::getSpeed() const {
    return velocity.length();
}

float Car::getSpeedKmh() const {
    return getSpeed() * 3.6f; // Convert m/s to km/h
}

void Car::setPosition(const Vector3& pos) {
    position = pos;
}

void Car::setVelocity(const Vector3& vel) {
    velocity = vel;
}

void Car::setRotation(const Quaternion& rot) {
    rotation = rot;
}

void Car::setMass(float carMass) {
    mass = std::max(100.0f, carMass);
}

void Car::setMaxSpeed(float speed) {
    maxSpeed = std::max(1.0f, speed);
}

void Car::setAcceleration(float accel) {
    acceleration = std::max(0.1f, accel);
}

void Car::setBrakeForce(float force) {
    brakeForce = std::max(0.1f, force);
}

void Car::setFriction(float fric) {
    friction = std::clamp(fric, 0.0f, 1.0f);
}

void Car::setThrottle(float throttle) {
    throttleInput = std::clamp(throttle, -1.0f, 1.0f);
}

void Car::setBrake(float brake) {
    brakeInput = std::clamp(brake, 0.0f, 1.0f);
}

void Car::setSteer(float steer) {
    steerInput = std::clamp(steer, -1.0f, 1.0f);
}

void Car::setBoost(bool boost) {
    if (boost && currentBoost > 0.0f && boostCooldown <= 0.0f) {
        activateBoost();
    } else if (!boost) {
        deactivateBoost();
    }
}

void Car::setHandbrake(bool handbrake) {
    handbrakeInput = handbrake;
}

void Car::update(float deltaTime) {
    updatePhysics(deltaTime);
    updateWheels(deltaTime);
    updateEngine(deltaTime);
    updateBoost(deltaTime);
    updateVisualEffects(deltaTime);
    updateCombat(deltaTime);
    
    // Update boost cooldown
    if (boostCooldown > 0.0f) {
        boostCooldown -= deltaTime;
    }
}

void Car::updatePhysics(float deltaTime) {
    checkGroundCollision();
    applyGroundForces();
    applyAirResistance();
    applyDownforce();
    
    // Update position and rotation
    position += velocity * deltaTime;
    
    // Update rotation based on angular velocity
    if (angularVelocity.length() > 0.001f) {
        Quaternion angularQuat = Quaternion::fromAxisAngle(angularVelocity.normalized(), angularVelocity.length() * deltaTime);
        rotation = rotation * angularQuat;
        rotation.normalize();
    }
    
    // Apply damping
    velocity *= 0.99f;
    angularVelocity *= 0.95f;
}

void Car::updateWheels(float deltaTime) {
    for (int i = 0; i < 4; i++) {
        Wheel& wheel = wheels[i];
        
        // Update wheel position relative to car
        Vector3 worldWheelPos = position + rotation * wheel.position;
        
        // Simple ground collision
        wheel.isGrounded = worldWheelPos.y <= 0.0f;
        
        if (wheel.isGrounded) {
            // Apply suspension forces
            float suspensionForce = wheel.suspensionStiffness * (wheel.suspensionLength - worldWheelPos.y);
            float dampingForce = wheel.damping * wheel.velocity.y;
            
            Vector3 suspensionVector = Vector3::up() * (suspensionForce - dampingForce);
            velocity += suspensionVector * deltaTime / mass;
            
            // Update wheel rotation based on forward velocity
            float forwardSpeed = velocity.dot(getForward());
            wheel.angularVelocity = forwardSpeed / wheel.radius;
            wheel.rotation += wheel.angularVelocity * deltaTime;
            
            // Apply friction
            Vector3 frictionForce = -velocity * friction;
            velocity += frictionForce * deltaTime;
            
            // Apply handbrake
            if (handbrakeInput) {
                Vector3 handbrakeForce = -velocity * 0.8f; // Strong braking
                velocity += handbrakeForce * deltaTime;
            }
        }
    }
}

void Car::updateEngine(float deltaTime) {
    if (!engine.isRunning) return;
    
    // Update throttle
    engine.throttle = throttleInput;
    
    // Calculate target RPM based on throttle
    float targetRpm = engine.throttle * engine.maxRpm;
    
    // Smooth RPM change
    float rpmChange = (targetRpm - engine.rpm) * 5.0f * deltaTime;
    engine.rpm += rpmChange;
    engine.rpm = std::max(0.0f, engine.rpm);
    
    // Calculate torque
    float torqueCurve = 1.0f - (engine.rpm / engine.maxRpm) * 0.3f;
    engine.torque = engine.throttle * engine.maxTorque * torqueCurve;
    
    // Apply engine force
    if (engine.torque > 0.0f && isGrounded) {
        Vector3 engineForce = getForward() * engine.torque * engine.gearRatio * engine.finalDrive;
        velocity += engineForce * deltaTime / mass;
    }
}

void Car::updateBoost(float deltaTime) {
    if (isBoosting) {
        if (currentBoost > 0.0f) {
            // Apply boost force
            Vector3 boostForce = getForward() * boostPower;
            velocity += boostForce * deltaTime / mass;
            
            // Consume boost
            currentBoost -= 30.0f * deltaTime; // Consume 30 units per second
            currentBoost = std::max(0.0f, currentBoost);
            
            if (currentBoost <= 0.0f) {
                deactivateBoost();
            }
        }
    } else {
        rechargeBoost(deltaTime);
    }
}

void Car::updateVisualEffects(float deltaTime) {
    // Calculate speed effect intensity
    float speed = getSpeed();
    speedEffectIntensity = std::min(speed / maxSpeed, 1.0f);
    
    // Update velocity direction for motion blur effects
    if (speed > 0.1f) {
        velocityDirection = velocity.normalized();
    }
    
    // Store last position for trail effects
    lastPosition = position;
}

void Car::checkGroundCollision() {
    isGrounded = false;
    groundHeight = 0.0f;
    groundNormal = Vector3::up();
    
    // Simple ground plane collision
    if (position.y <= 0.0f) {
        isGrounded = true;
        groundHeight = 0.0f;
        position.y = 0.0f;
        
        // Stop downward velocity
        if (velocity.y < 0.0f) {
            velocity.y = 0.0f;
        }
    }
}

void Car::applyGroundForces() {
    if (!isGrounded) return;
    
    // Apply gravity
    velocity.y -= 9.81f * 0.016f; // Assuming 60 FPS
}

void Car::applyAirResistance() {
    float speed = getSpeed();
    if (speed > 0.1f) {
        Vector3 airResistanceForce = -velocity.normalized() * speed * speed * airResistance;
        velocity += airResistanceForce * 0.016f / mass;
    }
}

void Car::applyDownforce() {
    if (isGrounded) {
        float speed = getSpeed();
        Vector3 downforceVector = Vector3::up() * speed * speed * downforce;
        velocity += downforceVector * 0.016f / mass;
    }
}

void Car::activateBoost() {
    if (currentBoost > 0.0f && boostCooldown <= 0.0f) {
        isBoosting = true;
        boostCooldown = 0.5f; // 0.5 second cooldown
    }
}

void Car::deactivateBoost() {
    isBoosting = false;
}

void Car::rechargeBoost(float deltaTime) {
    if (currentBoost < boostCapacity) {
        currentBoost += boostRechargeRate * deltaTime;
        currentBoost = std::min(boostCapacity, currentBoost);
    }
}

Matrix4 Car::getTransformMatrix() const {
    Matrix4 translation = Matrix4::translation(position);
    Matrix4 rotationMatrix = rotation.toMatrix();
    return translation * rotationMatrix;
}

Vector3 Car::getWheelPosition(int wheelIndex) const {
    if (wheelIndex >= 0 && wheelIndex < 4) {
        return position + rotation * wheels[wheelIndex].position;
    }
    return position;
}

void Car::reset() {
    position = Vector3::zero();
    velocity = Vector3::zero();
    angularVelocity = Vector3::zero();
    rotation = Quaternion::identity();
    throttleInput = 0.0f;
    brakeInput = 0.0f;
    steerInput = 0.0f;
    handbrakeInput = false;
    currentBoost = boostCapacity;
    isBoosting = false;
    boostCooldown = 0.0f;
    engine.rpm = 0.0f;
    engine.throttle = 0.0f;
    
    // Reset combat state
    health = maxHealth;
    combatMode = false;
    aimDirection = Vector3::forward();
    isInvulnerable = false;
    invulnerabilityTimer = 0.0f;
    
    for (int i = 0; i < 4; i++) {
        wheels[i].velocity = Vector3::zero();
        wheels[i].angularVelocity = 0.0f;
        wheels[i].isGrounded = false;
    }
}

void Car::resetToPosition(const Vector3& pos) {
    reset();
    position = pos;
}

void Car::updateCombat(float deltaTime) {
    // Update invulnerability timer
    if (isInvulnerable && invulnerabilityTimer > 0.0f) {
        invulnerabilityTimer -= deltaTime;
        if (invulnerabilityTimer <= 0.0f) {
            isInvulnerable = false;
        }
    }
    
    // Update aim direction in combat mode
    if (combatMode) {
        // In combat mode, aim direction could be updated based on input
        // For now, it defaults to forward direction
        aimDirection = getForward();
    }
}

void Car::setCombatMode(bool enabled) {
    combatMode = enabled;
    if (enabled) {
        aimDirection = getForward();
    }
}

void Car::setAimDirection(const Vector3& direction) {
    if (direction.length() > 0.001f) {
        aimDirection = direction.normalized();
    }
}

void Car::setHealth(float hp) {
    health = std::clamp(hp, 0.0f, maxHealth);
}

void Car::setMaxHealth(float maxHp) {
    maxHealth = std::max(1.0f, maxHp);
    if (health > maxHealth) {
        health = maxHealth;
    }
}

void Car::setInvulnerable(bool invulnerable, float duration) {
    isInvulnerable = invulnerable;
    if (invulnerable && duration > 0.0f) {
        invulnerabilityTimer = duration;
    }
}

void Car::takeDamage(float damage) {
    if (isInvulnerable || health <= 0.0f) return;
    
    health -= damage;
    health = std::max(0.0f, health);
    
    // Brief invulnerability after taking damage
    setInvulnerable(true, 0.5f);
}

void Car::heal(float amount) {
    if (health <= 0.0f) return; // Can't heal if dead
    
    health += amount;
    health = std::min(health, maxHealth);
}

void Car::kill() {
    health = 0.0f;
    velocity = Vector3::zero();
    angularVelocity = Vector3::zero();
}

void Car::respawn() {
    health = maxHealth;
    velocity = Vector3::zero();
    angularVelocity = Vector3::zero();
    throttleInput = 0.0f;
    brakeInput = 0.0f;
    steerInput = 0.0f;
    handbrakeInput = false;
    currentBoost = boostCapacity;
    isBoosting = false;
    boostCooldown = 0.0f;
    isInvulnerable = true;
    invulnerabilityTimer = 3.0f; // 3 seconds of spawn protection
    
    for (int i = 0; i < 4; i++) {
        wheels[i].velocity = Vector3::zero();
        wheels[i].angularVelocity = 0.0f;
        wheels[i].isGrounded = false;
    }
}

void Car::respawn(const Vector3& position) {
    respawn();
    setPosition(position);
}

Vector3 Car::getProjectileSpawnPosition() const {
    // Spawn projectiles slightly in front and above the car
    Vector3 forward = getForward();
    Vector3 right = getRight();
    Vector3 up = getUp();
    
    return position + forward * 2.5f + up * 1.2f + right * 0.3f;
}

void Car::debugDraw() const {
    // This would be implemented with a debug rendering system
    // For now, it's a placeholder
}