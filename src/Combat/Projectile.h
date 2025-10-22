#pragma once
#include "../Math/Vector3.h"
#include "../Math/Matrix4.h"
#include <vector>

// Forward declaration
class Player;

class Projectile {
public:
    enum class ProjectileType {
        Laser,
        Missile,
        Plasma,
        Energy
    };
    
private:
    // Core properties
    Vector3 position;
    Vector3 velocity;
    Vector3 direction;
    float speed;
    float damage;
    float maxRange;
    float distanceTraveled;
    float lifetime;
    float maxLifetime;
    bool active;
    
    // Visual properties
    ProjectileType type;
    Vector3 color;
    float size;
    float trailLength;
    std::vector<Vector3> trailPositions;
    
    // Owner
    Player* owner;
    int ownerId;
    
    // Collision
    float collisionRadius;
    bool hasHit;
    
    // Special effects
    bool isHoming;
    Player* homingTarget;
    float homingStrength;
    
    bool isPiercing;
    int pierceCount;
    int maxPierces;
    
    bool hasAreaDamage;
    float areaRadius;
    float areaDamage;
    
public:
    Projectile(const Vector3& startPos, const Vector3& velocity, float damage, float range, Player* owner);
    ~Projectile();
    
    // Update
    void update(float deltaTime);
    void updateMovement(float deltaTime);
    void updateTrail(float deltaTime);
    void updateHoming(float deltaTime);
    void checkCollision();
    
    // Actions
    void explode();
    void deactivate();
    void onHit(Player* target);
    
    // Getters
    Vector3 getPosition() const { return position; }
    Vector3 getVelocity() const { return velocity; }
    float getDamage() const { return damage; }
    float getSpeed() const { return speed; }
    bool isActive() const { return active; }
    Player* getOwner() const { return owner; }
    ProjectileType getType() const { return type; }
    Vector3 getColor() const { return color; }
    float getSize() const { return size; }
    const std::vector<Vector3>& getTrail() const { return trailPositions; }
    
    // Setters
    void setType(ProjectileType t) { type = t; }
    void setColor(const Vector3& c) { color = c; }
    void setHoming(Player* target, float strength);
    void setPiercing(int maxPierces);
    void setAreaDamage(float radius, float dmg);
    
    // Collision
    bool checkHitPlayer(Player* player) const;
    bool checkHitPoint(const Vector3& point, float radius) const;
    
    // Rendering
    Matrix4 getTransformMatrix() const;
};