#include "Projectile.h"
#include "Player.h"
#include <cmath>

Projectile::Projectile(const Vector3& startPos, const Vector3& dir, float dmg, float spd, Player* playerOwner)
    : position(startPos)
    , direction(dir.normalized())
    , damage(dmg)
    , speed(spd)
    , lifetime(0.0f)
    , maxLifetime(5.0f)
    , radius(0.2f)
    , alive(true)
    , owner(playerOwner)
    , type(ProjectileType::Laser)
    , color(0.0f, 1.0f, 1.0f)
    , trailIntensity(1.0f)
    , hasTrail(true) {
    
    velocity = direction * speed;
}

Projectile::~Projectile() {
}

void Projectile::update(float deltaTime) {
    if (!alive) return;
    
    // Update position
    position += velocity * deltaTime;
    
    // Update lifetime
    lifetime += deltaTime;
    if (lifetime >= maxLifetime) {
        destroy();
        return;
    }
    
    // Check collisions
    checkCollisions();
    
    // Update trail intensity based on lifetime
    trailIntensity = 1.0f - (lifetime / maxLifetime);
}

void Projectile::checkCollisions() {
    // TODO: Implement collision detection with players, environment, etc.
    // For now, just check if projectile is below ground
    if (position.y < 0.0f) {
        destroy();
    }
}

void Projectile::onHit(Player* target) {
    if (!alive || !target) return;
    
    // Apply damage to target
    target->takeDamage(damage, owner);
    
    // Destroy projectile
    destroy();
}

void Projectile::destroy() {
    alive = false;
}

void Projectile::setType(ProjectileType projType) {
    type = projType;
    
    // Set properties based on type
    switch (type) {
        case ProjectileType::Laser:
            color = Vector3(0.0f, 1.0f, 1.0f);
            radius = 0.2f;
            hasTrail = true;
            break;
            
        case ProjectileType::Rocket:
            color = Vector3(1.0f, 0.5f, 0.0f);
            radius = 0.3f;
            hasTrail = true;
            speed *= 0.8f;
            damage *= 1.5f;
            break;
            
        case ProjectileType::EnergyBall:
            color = Vector3(0.5f, 0.0f, 1.0f);
            radius = 0.4f;
            hasTrail = false;
            break;
            
        case ProjectileType::Ultimate:
            color = Vector3(1.0f, 0.0f, 1.0f);
            radius = 0.5f;
            hasTrail = true;
            speed *= 1.2f;
            damage *= 2.0f;
            break;
    }
    
    velocity = direction * speed;
}
