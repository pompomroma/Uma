#pragma once
#include "../Math/Vector3.h"
#include "../Math/Quaternion.h"
#include "../Math/Matrix4.h"
#include <functional>

class Car;

class Projectile {
public:
    enum class ProjectileType {
        Laser,          // Fast, straight-line projectile
        Plasma,         // Slower but more powerful
        Missile,        // Homing projectile
        EnergyBall,     // Area of effect projectile
        Fist            // Melee attack projectile (short range, high damage)
    };

    enum class ProjectileState {
        Active,         // Flying through the air
        Exploding,      // In explosion animation
        Destroyed       // Ready for cleanup
    };

private:
    // Identity and type
    ProjectileType type;
    ProjectileState state;
    Car* owner;             // Who fired this projectile
    int projectileId;       // Unique identifier
    
    // Physics
    Vector3 position;
    Vector3 velocity;
    Vector3 direction;
    Quaternion rotation;
    float speed;
    float acceleration;     // For missiles that accelerate
    
    // Combat properties
    float damage;
    float range;
    float explosionRadius;  // For area of effect projectiles
    bool piercing;          // Can hit multiple targets
    int maxPierceTargets;
    int currentPierceCount;
    
    // Homing properties (for missiles)
    Car* homingTarget;
    float homingStrength;   // How aggressively it homes
    float homingRange;      // Max range for target acquisition
    
    // Lifetime and effects
    float lifetime;         // How long the projectile lives
    float maxLifetime;
    float explosionDuration;
    float currentExplosionTime;
    
    // Visual properties
    Vector3 color;
    float size;
    float glowIntensity;
    bool hasTrail;
    float trailLength;
    
    // Collision
    float collisionRadius;
    bool hasCollided;
    Vector3 collisionPoint;
    Vector3 collisionNormal;
    
    // Callbacks
    std::function<void(Projectile*, Car*)> onHitTarget;
    std::function<void(Projectile*, const Vector3&)> onHitGround;
    std::function<void(Projectile*)> onDestroyed;

public:
    Projectile(ProjectileType type, Car* owner, const Vector3& startPos, const Vector3& direction);
    ~Projectile();
    
    // Update
    void update(float deltaTime);
    void updateMovement(float deltaTime);
    void updateHoming(float deltaTime);
    void updateLifetime(float deltaTime);
    void updateExplosion(float deltaTime);
    
    // Collision
    bool checkCollisionWithCar(Car* car);
    bool checkCollisionWithGround();
    bool checkCollisionWithWorld();
    void handleCollision(Car* target);
    void handleGroundCollision(const Vector3& point, const Vector3& normal);
    void explode();
    
    // Getters
    ProjectileType getType() const { return type; }
    ProjectileState getState() const { return state; }
    Car* getOwner() const { return owner; }
    int getId() const { return projectileId; }
    Vector3 getPosition() const { return position; }
    Vector3 getVelocity() const { return velocity; }
    Vector3 getDirection() const { return direction; }
    Quaternion getRotation() const { return rotation; }
    float getSpeed() const { return speed; }
    float getDamage() const { return damage; }
    float getRange() const { return range; }
    float getExplosionRadius() const { return explosionRadius; }
    float getLifetime() const { return lifetime; }
    float getMaxLifetime() const { return maxLifetime; }
    Vector3 getColor() const { return color; }
    float getSize() const { return size; }
    float getGlowIntensity() const { return glowIntensity; }
    bool getHasTrail() const { return hasTrail; }
    float getTrailLength() const { return trailLength; }
    float getCollisionRadius() const { return collisionRadius; }
    bool isDestroyed() const { return state == ProjectileState::Destroyed; }
    bool isActive() const { return state == ProjectileState::Active; }
    bool isExploding() const { return state == ProjectileState::Exploding; }
    
    // Setters
    void setPosition(const Vector3& pos) { position = pos; }
    void setVelocity(const Vector3& vel) { velocity = vel; }
    void setDirection(const Vector3& dir) { direction = dir.normalized(); }
    void setSpeed(float spd) { speed = spd; }
    void setDamage(float dmg) { damage = dmg; }
    void setRange(float rng) { range = rng; }
    void setExplosionRadius(float radius) { explosionRadius = radius; }
    void setHomingTarget(Car* target) { homingTarget = target; }
    void setHomingStrength(float strength) { homingStrength = strength; }
    void setColor(const Vector3& col) { color = col; }
    void setSize(float sz) { size = sz; }
    void setGlowIntensity(float intensity) { glowIntensity = intensity; }
    void setPiercing(bool pierce, int maxTargets = 3) { piercing = pierce; maxPierceTargets = maxTargets; }
    
    // Utility
    Matrix4 getTransformMatrix() const;
    float getDistanceTraveled() const;
    float getDistanceToTarget(const Vector3& target) const;
    bool isInRange(const Vector3& target) const;
    void destroy();
    
    // Callbacks
    void setHitTargetCallback(std::function<void(Projectile*, Car*)> callback) { onHitTarget = callback; }
    void setHitGroundCallback(std::function<void(Projectile*, const Vector3&)> callback) { onHitGround = callback; }
    void setDestroyedCallback(std::function<void(Projectile*)> callback) { onDestroyed = callback; }
    
    // Factory methods for different projectile types
    static Projectile* createLaser(Car* owner, const Vector3& startPos, const Vector3& direction, float damage = 25.0f);
    static Projectile* createPlasma(Car* owner, const Vector3& startPos, const Vector3& direction, float damage = 40.0f);
    static Projectile* createMissile(Car* owner, const Vector3& startPos, const Vector3& direction, Car* target = nullptr, float damage = 60.0f);
    static Projectile* createEnergyBall(Car* owner, const Vector3& startPos, const Vector3& direction, float damage = 35.0f);
    static Projectile* createFist(Car* owner, const Vector3& startPos, const Vector3& direction, float damage = 50.0f);
    
private:
    void initializeProjectile();
    void setupProjectileProperties();
    Car* findNearestTarget();
    Vector3 calculateHomingDirection();
    static int nextProjectileId;
};