#pragma once
#include "../Math/Vector3.h"

class Player;

class Projectile {
public:
    enum class ProjectileType {
        Laser,
        Rocket,
        EnergyBall,
        Ultimate
    };

private:
    Vector3 position;
    Vector3 velocity;
    Vector3 direction;
    float speed;
    float damage;
    float lifetime;
    float maxLifetime;
    float radius;
    bool alive;
    
    Player* owner;
    ProjectileType type;
    
    // Visual properties
    Vector3 color;
    float trailIntensity;
    bool hasTrail;

public:
    Projectile(const Vector3& startPos, const Vector3& dir, float dmg, float spd, Player* playerOwner);
    ~Projectile();
    
    void update(float deltaTime);
    void checkCollisions();
    void onHit(Player* target);
    void destroy();
    
    // Getters
    Vector3 getPosition() const { return position; }
    Vector3 getVelocity() const { return velocity; }
    Vector3 getDirection() const { return direction; }
    float getDamage() const { return damage; }
    float getRadius() const { return radius; }
    bool isDead() const { return !alive; }
    Player* getOwner() const { return owner; }
    ProjectileType getType() const { return type; }
    Vector3 getColor() const { return color; }
    float getTrailIntensity() const { return trailIntensity; }
    bool shouldRenderTrail() const { return hasTrail; }
    
    // Setters
    void setType(ProjectileType projType);
    void setColor(const Vector3& col) { color = col; }
    void setHasTrail(bool trail) { hasTrail = trail; }
};
