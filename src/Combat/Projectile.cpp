#include "Projectile.h"
#include "../Physics/Car.h"
#include <cmath>
#include <algorithm>

int Projectile::nextProjectileId = 1;

Projectile::Projectile(ProjectileType type, Car* owner, const Vector3& startPos, const Vector3& direction)
    : type(type)
    , state(ProjectileState::Active)
    , owner(owner)
    , projectileId(nextProjectileId++)
    , position(startPos)
    , velocity(Vector3::zero())
    , direction(direction.normalized())
    , rotation(Quaternion::identity())
    , speed(50.0f)
    , acceleration(0.0f)
    , damage(25.0f)
    , range(100.0f)
    , explosionRadius(0.0f)
    , piercing(false)
    , maxPierceTargets(1)
    , currentPierceCount(0)
    , homingTarget(nullptr)
    , homingStrength(0.0f)
    , homingRange(50.0f)
    , lifetime(0.0f)
    , maxLifetime(5.0f)
    , explosionDuration(0.5f)
    , currentExplosionTime(0.0f)
    , color(1.0f, 0.0f, 0.0f)
    , size(0.2f)
    , glowIntensity(1.0f)
    , hasTrail(true)
    , trailLength(2.0f)
    , collisionRadius(0.3f)
    , hasCollided(false)
    , collisionPoint(Vector3::zero())
    , collisionNormal(Vector3::up()) {
    
    initializeProjectile();
    setupProjectileProperties();
}

Projectile::~Projectile() {
    if (onDestroyed) {
        onDestroyed(this);
    }
}

void Projectile::initializeProjectile() {
    // Set initial velocity based on direction and speed
    velocity = direction * speed;
    
    // Set rotation to face the direction of travel
    if (direction.length() > 0.001f) {
        Vector3 forward = Vector3::forward();
        Vector3 axis = forward.cross(direction);
        if (axis.length() > 0.001f) {
            float angle = std::acos(std::clamp(forward.dot(direction), -1.0f, 1.0f));
            rotation = Quaternion::fromAxisAngle(axis.normalized(), angle);
        }
    }
}

void Projectile::setupProjectileProperties() {
    switch (type) {
        case ProjectileType::Laser:
            speed = 80.0f;
            damage = 25.0f;
            range = 150.0f;
            maxLifetime = 2.0f;
            size = 0.1f;
            color = Vector3(1.0f, 0.0f, 0.0f); // Red
            collisionRadius = 0.2f;
            hasTrail = true;
            trailLength = 3.0f;
            break;
            
        case ProjectileType::Plasma:
            speed = 40.0f;
            damage = 40.0f;
            range = 100.0f;
            maxLifetime = 3.0f;
            size = 0.3f;
            color = Vector3(0.0f, 1.0f, 1.0f); // Cyan
            collisionRadius = 0.4f;
            explosionRadius = 2.0f;
            hasTrail = true;
            trailLength = 2.0f;
            break;
            
        case ProjectileType::Missile:
            speed = 30.0f;
            acceleration = 20.0f;
            damage = 60.0f;
            range = 200.0f;
            maxLifetime = 8.0f;
            size = 0.4f;
            color = Vector3(1.0f, 0.5f, 0.0f); // Orange
            collisionRadius = 0.5f;
            explosionRadius = 5.0f;
            homingStrength = 2.0f;
            hasTrail = true;
            trailLength = 4.0f;
            break;
            
        case ProjectileType::EnergyBall:
            speed = 25.0f;
            damage = 35.0f;
            range = 80.0f;
            maxLifetime = 4.0f;
            size = 0.6f;
            color = Vector3(0.5f, 0.0f, 1.0f); // Purple
            collisionRadius = 0.8f;
            explosionRadius = 8.0f;
            hasTrail = false;
            glowIntensity = 2.0f;
            break;
            
        case ProjectileType::Fist:
            speed = 15.0f;
            damage = 50.0f;
            range = 10.0f;
            maxLifetime = 0.8f;
            size = 1.0f;
            color = Vector3(1.0f, 1.0f, 0.0f); // Yellow
            collisionRadius = 1.2f;
            hasTrail = false;
            glowIntensity = 1.5f;
            break;
    }
    
    // Update velocity with new speed
    velocity = direction * speed;
}

void Projectile::update(float deltaTime) {
    if (state == ProjectileState::Destroyed) return;
    
    if (state == ProjectileState::Active) {
        updateMovement(deltaTime);
        updateHoming(deltaTime);
        updateLifetime(deltaTime);
        
        // Check for collisions
        if (checkCollisionWithWorld()) {
            return; // Projectile was destroyed
        }
    } else if (state == ProjectileState::Exploding) {
        updateExplosion(deltaTime);
    }
}

void Projectile::updateMovement(float deltaTime) {
    // Apply acceleration (for missiles)
    if (acceleration > 0.0f) {
        speed += acceleration * deltaTime;
        velocity = direction * speed;
    }
    
    // Update position
    position += velocity * deltaTime;
    
    // Update rotation to face movement direction
    if (velocity.length() > 0.001f) {
        Vector3 forward = Vector3::forward();
        Vector3 moveDir = velocity.normalized();
        Vector3 axis = forward.cross(moveDir);
        if (axis.length() > 0.001f) {
            float angle = std::acos(std::clamp(forward.dot(moveDir), -1.0f, 1.0f));
            rotation = Quaternion::fromAxisAngle(axis.normalized(), angle);
        }
    }
}

void Projectile::updateHoming(float deltaTime) {
    if (type != ProjectileType::Missile || homingStrength <= 0.0f) return;
    
    // Find target if we don't have one
    if (!homingTarget) {
        homingTarget = findNearestTarget();
    }
    
    // Home towards target
    if (homingTarget) {
        Vector3 targetPos = homingTarget->getPosition();
        float distanceToTarget = (targetPos - position).length();
        
        // Stop homing if target is too far or destroyed
        if (distanceToTarget > homingRange) {
            homingTarget = nullptr;
            return;
        }
        
        // Calculate homing direction
        Vector3 homingDir = calculateHomingDirection();
        
        // Blend current direction with homing direction
        float homingFactor = std::min(1.0f, homingStrength * deltaTime);
        direction = Vector3::lerp(direction, homingDir, homingFactor).normalized();
        velocity = direction * speed;
    }
}

void Projectile::updateLifetime(float deltaTime) {
    lifetime += deltaTime;
    
    // Check if projectile has exceeded its lifetime or range
    float distanceTraveled = getDistanceTraveled();
    if (lifetime >= maxLifetime || distanceTraveled >= range) {
        destroy();
    }
}

void Projectile::updateExplosion(float deltaTime) {
    currentExplosionTime += deltaTime;
    
    if (currentExplosionTime >= explosionDuration) {
        state = ProjectileState::Destroyed;
    }
}

bool Projectile::checkCollisionWithCar(Car* car) {
    if (!car || car == owner) return false;
    
    Vector3 carPos = car->getPosition();
    float distance = (position - carPos).length();
    
    // Simple sphere collision
    float carRadius = 2.0f; // Approximate car collision radius
    return distance <= (collisionRadius + carRadius);
}

bool Projectile::checkCollisionWithGround() {
    // Simple ground plane collision
    if (position.y <= 0.0f) {
        collisionPoint = Vector3(position.x, 0.0f, position.z);
        collisionNormal = Vector3::up();
        return true;
    }
    return false;
}

bool Projectile::checkCollisionWithWorld() {
    // Check ground collision
    if (checkCollisionWithGround()) {
        handleGroundCollision(collisionPoint, collisionNormal);
        return true;
    }
    
    return false;
}

void Projectile::handleCollision(Car* target) {
    if (hasCollided && !piercing) return;
    
    hasCollided = true;
    currentPierceCount++;
    
    // Trigger hit callback
    if (onHitTarget) {
        onHitTarget(this, target);
    }
    
    // Check if we should destroy the projectile
    if (!piercing || currentPierceCount >= maxPierceTargets) {
        if (explosionRadius > 0.0f) {
            explode();
        } else {
            destroy();
        }
    }
}

void Projectile::handleGroundCollision(const Vector3& point, const Vector3& normal) {
    collisionPoint = point;
    collisionNormal = normal;
    
    // Trigger ground hit callback
    if (onHitGround) {
        onHitGround(this, point);
    }
    
    if (explosionRadius > 0.0f) {
        explode();
    } else {
        destroy();
    }
}

void Projectile::explode() {
    state = ProjectileState::Exploding;
    currentExplosionTime = 0.0f;
    velocity = Vector3::zero();
    
    // Explosion effects would be handled by the combat system
}

Matrix4 Projectile::getTransformMatrix() const {
    Matrix4 translation = Matrix4::translation(position);
    Matrix4 rotationMatrix = rotation.toMatrix();
    Matrix4 scale = Matrix4::scale(Vector3(size, size, size));
    return translation * rotationMatrix * scale;
}

float Projectile::getDistanceTraveled() const {
    return velocity.length() * lifetime;
}

float Projectile::getDistanceToTarget(const Vector3& target) const {
    return (target - position).length();
}

bool Projectile::isInRange(const Vector3& target) const {
    return getDistanceToTarget(target) <= range;
}

void Projectile::destroy() {
    state = ProjectileState::Destroyed;
}

Car* Projectile::findNearestTarget() {
    // This would need to be implemented with access to the game's car list
    // For now, return nullptr
    return nullptr;
}

Vector3 Projectile::calculateHomingDirection() {
    if (!homingTarget) return direction;
    
    Vector3 targetPos = homingTarget->getPosition();
    Vector3 toTarget = (targetPos - position).normalized();
    
    // Predict target movement for better homing
    Vector3 targetVel = homingTarget->getVelocity();
    float timeToTarget = getDistanceToTarget(targetPos) / speed;
    Vector3 predictedPos = targetPos + targetVel * timeToTarget;
    
    return (predictedPos - position).normalized();
}

// Factory methods
Projectile* Projectile::createLaser(Car* owner, const Vector3& startPos, const Vector3& direction, float damage) {
    Projectile* projectile = new Projectile(ProjectileType::Laser, owner, startPos, direction);
    projectile->setDamage(damage);
    return projectile;
}

Projectile* Projectile::createPlasma(Car* owner, const Vector3& startPos, const Vector3& direction, float damage) {
    Projectile* projectile = new Projectile(ProjectileType::Plasma, owner, startPos, direction);
    projectile->setDamage(damage);
    return projectile;
}

Projectile* Projectile::createMissile(Car* owner, const Vector3& startPos, const Vector3& direction, Car* target, float damage) {
    Projectile* projectile = new Projectile(ProjectileType::Missile, owner, startPos, direction);
    projectile->setDamage(damage);
    projectile->setHomingTarget(target);
    return projectile;
}

Projectile* Projectile::createEnergyBall(Car* owner, const Vector3& startPos, const Vector3& direction, float damage) {
    Projectile* projectile = new Projectile(ProjectileType::EnergyBall, owner, startPos, direction);
    projectile->setDamage(damage);
    return projectile;
}

Projectile* Projectile::createFist(Car* owner, const Vector3& startPos, const Vector3& direction, float damage) {
    Projectile* projectile = new Projectile(ProjectileType::Fist, owner, startPos, direction);
    projectile->setDamage(damage);
    return projectile;
}