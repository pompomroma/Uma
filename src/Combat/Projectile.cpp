#include "Projectile.h"
#include "Player.h"
#include <algorithm>
#include <cmath>

Projectile::Projectile(const Vector3& startPos, const Vector3& vel, float dmg, float range, Player* own)
    : position(startPos)
    , velocity(vel)
    , damage(dmg)
    , maxRange(range)
    , distanceTraveled(0.0f)
    , lifetime(0.0f)
    , maxLifetime(10.0f)  // Max 10 seconds
    , active(true)
    , type(ProjectileType::Laser)
    , color(Vector3(1.0f, 0.2f, 0.2f))  // Red by default
    , size(0.2f)
    , trailLength(5.0f)
    , owner(own)
    , ownerId(own ? own->getPlayerId() : -1)
    , collisionRadius(0.3f)
    , hasHit(false)
    , isHoming(false)
    , homingTarget(nullptr)
    , homingStrength(0.0f)
    , isPiercing(false)
    , pierceCount(0)
    , maxPierces(1)
    , hasAreaDamage(false)
    , areaRadius(0.0f)
    , areaDamage(0.0f) {
    
    direction = velocity.normalized();
    speed = velocity.magnitude();
}

Projectile::~Projectile() {
}

void Projectile::update(float deltaTime) {
    if (!active) return;
    
    updateHoming(deltaTime);
    updateMovement(deltaTime);
    updateTrail(deltaTime);
    
    lifetime += deltaTime;
    
    // Check if projectile should be deactivated
    if (distanceTraveled >= maxRange || lifetime >= maxLifetime) {
        if (hasAreaDamage) {
            explode();
        } else {
            deactivate();
        }
    }
}

void Projectile::updateMovement(float deltaTime) {
    Vector3 movement = velocity * deltaTime;
    position = position + movement;
    distanceTraveled += movement.magnitude();
}

void Projectile::updateTrail(float deltaTime) {
    // Add current position to trail
    trailPositions.insert(trailPositions.begin(), position);
    
    // Limit trail length
    if (trailPositions.size() > 20) {
        trailPositions.pop_back();
    }
}

void Projectile::updateHoming(float deltaTime) {
    if (!isHoming || !homingTarget || !homingTarget->isAlive()) return;
    
    // Calculate direction to target
    Vector3 toTarget = homingTarget->getPosition() - position;
    toTarget = toTarget.normalized();
    
    // Lerp velocity towards target
    Vector3 targetVelocity = toTarget * speed;
    velocity = velocity + (targetVelocity - velocity) * homingStrength * deltaTime;
    velocity = velocity.normalized() * speed;
    direction = velocity.normalized();
}

void Projectile::checkCollision() {
    // This would be called by the game's collision system
    // to check against all players
}

void Projectile::explode() {
    if (hasAreaDamage) {
        // Game logic will handle area damage to nearby players
    }
    
    deactivate();
}

void Projectile::deactivate() {
    active = false;
}

void Projectile::onHit(Player* target) {
    if (!active || !target || target == owner) return;
    
    target->takeDamage(damage, owner);
    
    if (isPiercing && pierceCount < maxPierces) {
        pierceCount++;
        // Continue flying
    } else {
        if (hasAreaDamage) {
            explode();
        } else {
            deactivate();
        }
    }
}

void Projectile::setHoming(Player* target, float strength) {
    isHoming = true;
    homingTarget = target;
    homingStrength = std::clamp(strength, 0.0f, 1.0f);
}

void Projectile::setPiercing(int pierces) {
    isPiercing = true;
    maxPierces = pierces;
}

void Projectile::setAreaDamage(float radius, float dmg) {
    hasAreaDamage = true;
    areaRadius = radius;
    areaDamage = dmg;
}

bool Projectile::checkHitPlayer(Player* player) const {
    if (!player || player == owner || !active) return false;
    
    return checkHitPoint(player->getPosition(), 1.0f);  // Assume player radius of 1.0
}

bool Projectile::checkHitPoint(const Vector3& point, float radius) const {
    float distance = (point - position).magnitude();
    return distance <= (collisionRadius + radius);
}

Matrix4 Projectile::getTransformMatrix() const {
    return Matrix4::translation(position) * Matrix4::scale(Vector3(size, size, size));
}