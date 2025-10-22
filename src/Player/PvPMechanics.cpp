#include "PvPMechanics.h"
#include "../Physics/Car.h"
#include <algorithm>
#include <cmath>

PvPMechanics::PvPMechanics() 
    : combatEnabled(false)
    , globalCooldown(0.0f)
    , lastGlobalUpdate(0.0f) {
}

PvPMechanics::~PvPMechanics() {
    shutdown();
}

void PvPMechanics::initialize() {
    combatEnabled = true;
    activeAttacks.clear();
    activeProjectiles.clear();
    players.clear();
    laserTrails.clear();
    shieldEffects.clear();
    teleportEffects.clear();
}

void PvPMechanics::shutdown() {
    clearAllAttacks();
    clearAllProjectiles();
    clearPlayers();
    laserTrails.clear();
    shieldEffects.clear();
    teleportEffects.clear();
}

void PvPMechanics::addPlayer(Car* player) {
    if (player && std::find(players.begin(), players.end(), player) == players.end()) {
        players.push_back(player);
    }
}

void PvPMechanics::removePlayer(Car* player) {
    auto it = std::find(players.begin(), players.end(), player);
    if (it != players.end()) {
        players.erase(it);
    }
}

void PvPMechanics::clearPlayers() {
    players.clear();
}

void PvPMechanics::enableCombat(bool enable) {
    combatEnabled = enable;
    if (!enable) {
        clearAllAttacks();
        clearAllProjectiles();
    }
}

bool PvPMechanics::performLaserAttack(Car* attacker, const Vector3& direction) {
    if (!combatEnabled || !attacker) return false;
    
    // Create laser attack
    Attack attack = createLaserAttack(attacker, direction);
    activeAttacks.push_back(attack);
    
    // Create laser projectile
    Projectile projectile = createLaserProjectile(attacker, direction);
    activeProjectiles.push_back(projectile);
    
    // Add visual effect
    Vector3 start = attacker->getPosition() + attacker->getForward() * 2.0f;
    Vector3 end = start + direction * LASER_SPEED * LASER_LIFETIME;
    addLaserTrail(start, end);
    
    return true;
}

bool PvPMechanics::performFistAttack(Car* attacker, Car* target) {
    if (!combatEnabled || !attacker || !target) return false;
    
    // Check if target is in range
    if (!isInRange(attacker->getPosition(), target->getPosition(), FIST_RANGE)) {
        return false;
    }
    
    // Create fist attack
    Attack attack = createFistAttack(attacker, target);
    activeAttacks.push_back(attack);
    
    // Resolve attack immediately
    resolveAttack(attack);
    
    return true;
}

bool PvPMechanics::activateShield(Car* player) {
    if (!combatEnabled || !player) return false;
    
    // Create shield attack
    Attack attack = createShieldAttack(player);
    activeAttacks.push_back(attack);
    
    // Add visual effect
    addShieldEffect(player);
    
    return true;
}

bool PvPMechanics::performTeleport(Car* player, const Vector3& targetPosition) {
    if (!combatEnabled || !player) return false;
    
    // Check if target position is in range
    if (!isInRange(player->getPosition(), targetPosition, TELEPORT_RANGE)) {
        return false;
    }
    
    // Check line of sight
    if (!hasLineOfSight(player->getPosition(), targetPosition)) {
        return false;
    }
    
    // Create teleport attack
    Attack attack = createTeleportAttack(player, targetPosition);
    activeAttacks.push_back(attack);
    
    // Move player
    player->setPosition(targetPosition);
    
    // Add visual effect
    addTeleportEffect(targetPosition);
    
    return true;
}

void PvPMechanics::updateProjectiles(float deltaTime) {
    for (auto& projectile : activeProjectiles) {
        if (!projectile.isActive) continue;
        
        // Update position
        projectile.position += projectile.velocity * deltaTime;
        projectile.lifetime -= deltaTime;
        
        // Check if expired
        if (projectile.lifetime <= 0.0f) {
            projectile.isActive = false;
            continue;
        }
        
        // Check collisions
        checkProjectileCollisions();
    }
}

void PvPMechanics::renderProjectiles() {
    // This would be implemented with the rendering system
    // For now, it's a placeholder for visual effects
}

void PvPMechanics::checkProjectileCollisions() {
    for (auto& projectile : activeProjectiles) {
        if (!projectile.isActive) continue;
        
        for (auto* player : players) {
            if (player == projectile.owner) continue; // Don't hit self
            
            if (checkProjectileCarCollision(projectile, player)) {
                // Apply damage
                applyDamage(player, projectile.damage, projectile.owner);
                
                // Deactivate projectile
                projectile.isActive = false;
                break;
            }
        }
    }
}

void PvPMechanics::resolveAttack(const Attack& attack) {
    switch (attack.type) {
        case Attack::LASER:
            // Laser attacks are handled by projectiles
            break;
            
        case Attack::FIST:
            if (attack.target) {
                applyDamage(attack.target, attack.damage, attack.attacker);
                
                // Apply knockback
                Vector3 knockbackDir = (attack.target->getPosition() - attack.attacker->getPosition()).normalized();
                applyKnockback(attack.target, knockbackDir, 10.0f);
            }
            break;
            
        case Attack::SHIELD:
            // Shield is handled by the player's stats system
            break;
            
        case Attack::TELEPORT:
            // Teleport is handled immediately
            break;
    }
}

void PvPMechanics::applyDamage(Car* target, float damage, Car* attacker) {
    if (!target) return;
    
    // Get player stats (this would need to be integrated with the Car class)
    // For now, we'll assume the Car has a PlayerStats component
    
    // Apply damage based on target's defense
    float actualDamage = damage; // This would be modified by target's defense
    
    // Apply damage to target
    // target->getPlayerStats()->takeDamage(actualDamage);
    
    // Add visual effects for damage
    // This would trigger damage particles, screen shake, etc.
}

void PvPMechanics::applyKnockback(Car* target, const Vector3& direction, float force) {
    if (!target) return;
    
    // Apply knockback force to target's velocity
    Vector3 knockbackForce = direction * force;
    Vector3 currentVelocity = target->getVelocity();
    target->setVelocity(currentVelocity + knockbackForce);
}

Car* PvPMechanics::findNearestEnemy(Car* player, float maxRange) {
    if (!player) return nullptr;
    
    Car* nearest = nullptr;
    float nearestDistance = maxRange;
    
    for (auto* other : players) {
        if (other == player) continue;
        
        float distance = (other->getPosition() - player->getPosition()).length();
        if (distance < nearestDistance) {
            nearest = other;
            nearestDistance = distance;
        }
    }
    
    return nearest;
}

Car* PvPMechanics::findTargetInDirection(Car* player, const Vector3& direction, float maxRange) {
    if (!player) return nullptr;
    
    Car* bestTarget = nullptr;
    float bestAngle = 180.0f; // Start with worst possible angle
    
    for (auto* other : players) {
        if (other == player) continue;
        
        Vector3 toTarget = (other->getPosition() - player->getPosition()).normalized();
        float distance = (other->getPosition() - player->getPosition()).length();
        
        if (distance > maxRange) continue;
        
        float angle = std::acos(direction.dot(toTarget)) * 180.0f / M_PI;
        if (angle < bestAngle && angle < 30.0f) { // Within 30 degrees
            bestTarget = other;
            bestAngle = angle;
        }
    }
    
    return bestTarget;
}

bool PvPMechanics::isInRange(const Vector3& from, const Vector3& to, float range) {
    return (to - from).length() <= range;
}

bool PvPMechanics::hasLineOfSight(const Vector3& from, const Vector3& to) {
    // Simple line of sight check - in a real implementation, this would use raycasting
    // For now, we'll assume line of sight is always available
    return true;
}

void PvPMechanics::update(float deltaTime) {
    if (!combatEnabled) return;
    
    updateAttacks(deltaTime);
    updateProjectiles(deltaTime);
    updateVisualEffects(deltaTime);
    cleanupExpiredAttacks();
    cleanupExpiredProjectiles();
}

void PvPMechanics::render() {
    renderAttackEffects();
    renderProjectiles();
}

void PvPMechanics::renderAttackEffects() {
    // Render laser beams
    for (const auto& trail : laserTrails) {
        // This would render laser beam effects
    }
    
    // Render shield effects
    for (const auto& effect : shieldEffects) {
        // This would render shield bubble effects
    }
    
    // Render teleport effects
    for (const auto& effect : teleportEffects) {
        // This would render teleport particle effects
    }
}

void PvPMechanics::renderLaserBeam(const Vector3& start, const Vector3& end) {
    // This would render a laser beam from start to end
}

void PvPMechanics::renderShieldEffect(Car* player) {
    // This would render a shield bubble around the player
}

void PvPMechanics::renderTeleportEffect(const Vector3& position) {
    // This would render teleport particle effects at the position
}

void PvPMechanics::clearAllAttacks() {
    activeAttacks.clear();
}

void PvPMechanics::clearAllProjectiles() {
    activeProjectiles.clear();
}

void PvPMechanics::resetCombat() {
    clearAllAttacks();
    clearAllProjectiles();
    laserTrails.clear();
    shieldEffects.clear();
    teleportEffects.clear();
}

void PvPMechanics::updateAttacks(float deltaTime) {
    for (auto& attack : activeAttacks) {
        if (!attack.isActive) continue;
        
        attack.duration -= deltaTime;
        if (attack.duration <= 0.0f) {
            attack.isActive = false;
        }
    }
}

void PvPMechanics::updateVisualEffects(float deltaTime) {
    // Update laser trails
    for (auto& trail : laserTrails) {
        // Fade out laser trails over time
    }
    
    // Update shield effects
    for (auto& effect : shieldEffects) {
        // Update shield effect animations
    }
    
    // Update teleport effects
    for (auto& effect : teleportEffects) {
        // Update teleport particle effects
    }
}

void PvPMechanics::cleanupExpiredAttacks() {
    activeAttacks.erase(
        std::remove_if(activeAttacks.begin(), activeAttacks.end(),
            [](const Attack& attack) { return !attack.isActive; }),
        activeAttacks.end()
    );
}

void PvPMechanics::cleanupExpiredProjectiles() {
    activeProjectiles.erase(
        std::remove_if(activeProjectiles.begin(), activeProjectiles.end(),
            [](const Projectile& projectile) { return !projectile.isActive; }),
        activeProjectiles.end()
    );
}

Attack PvPMechanics::createLaserAttack(Car* attacker, const Vector3& direction) {
    Attack attack;
    attack.type = Attack::LASER;
    attack.origin = attacker->getPosition() + attacker->getForward() * 2.0f;
    attack.direction = direction;
    attack.damage = 30.0f; // This would come from player stats
    attack.range = 50.0f;
    attack.duration = 0.1f;
    attack.cooldown = 2.0f;
    attack.isActive = true;
    attack.attacker = attacker;
    attack.target = nullptr;
    return attack;
}

Attack PvPMechanics::createFistAttack(Car* attacker, Car* target) {
    Attack attack;
    attack.type = Attack::FIST;
    attack.origin = attacker->getPosition();
    attack.direction = (target->getPosition() - attacker->getPosition()).normalized();
    attack.damage = 25.0f; // This would come from player stats
    attack.range = FIST_RANGE;
    attack.duration = 0.5f;
    attack.cooldown = 1.0f;
    attack.isActive = true;
    attack.attacker = attacker;
    attack.target = target;
    return attack;
}

Attack PvPMechanics::createShieldAttack(Car* player) {
    Attack attack;
    attack.type = Attack::SHIELD;
    attack.origin = player->getPosition();
    attack.direction = Vector3::zero();
    attack.damage = 0.0f;
    attack.range = 0.0f;
    attack.duration = SHIELD_DURATION;
    attack.cooldown = 8.0f;
    attack.isActive = true;
    attack.attacker = player;
    attack.target = nullptr;
    return attack;
}

Attack PvPMechanics::createTeleportAttack(Car* player, const Vector3& target) {
    Attack attack;
    attack.type = Attack::TELEPORT;
    attack.origin = player->getPosition();
    attack.direction = (target - player->getPosition()).normalized();
    attack.damage = 0.0f;
    attack.range = TELEPORT_RANGE;
    attack.duration = 0.1f;
    attack.cooldown = 5.0f;
    attack.isActive = true;
    attack.attacker = player;
    attack.target = nullptr;
    return attack;
}

Projectile PvPMechanics::createLaserProjectile(Car* owner, const Vector3& direction) {
    Projectile projectile;
    projectile.position = owner->getPosition() + owner->getForward() * 2.0f;
    projectile.velocity = direction * LASER_SPEED;
    projectile.direction = direction;
    projectile.damage = 30.0f; // This would come from player stats
    projectile.range = 50.0f;
    projectile.speed = LASER_SPEED;
    projectile.lifetime = LASER_LIFETIME;
    projectile.isActive = true;
    projectile.owner = owner;
    projectile.type = Attack::LASER;
    return projectile;
}

Projectile PvPMechanics::createFistProjectile(Car* owner, const Vector3& direction) {
    Projectile projectile;
    projectile.position = owner->getPosition() + owner->getForward() * 1.0f;
    projectile.velocity = direction * 20.0f; // Slower than laser
    projectile.direction = direction;
    projectile.damage = 25.0f; // This would come from player stats
    projectile.range = FIST_RANGE;
    projectile.speed = 20.0f;
    projectile.lifetime = 0.5f;
    projectile.isActive = true;
    projectile.owner = owner;
    projectile.type = Attack::FIST;
    return projectile;
}

bool PvPMechanics::checkProjectileCarCollision(const Projectile& projectile, Car* car) {
    if (!car) return false;
    
    float distance = (projectile.position - car->getPosition()).length();
    return distance < 2.0f; // Car collision radius
}

bool PvPMechanics::checkAttackCarCollision(const Attack& attack, Car* car) {
    if (!car) return false;
    
    switch (attack.type) {
        case Attack::FIST:
            return isInRange(attack.origin, car->getPosition(), attack.range);
        default:
            return false;
    }
}

void PvPMechanics::addLaserTrail(const Vector3& start, const Vector3& end) {
    laserTrails.push_back(start);
    laserTrails.push_back(end);
}

void PvPMechanics::addShieldEffect(Car* player) {
    shieldEffects.push_back(player->getPosition());
}

void PvPMechanics::addTeleportEffect(const Vector3& position) {
    teleportEffects.push_back(position);
}