#include "PlayerAbilities.h"
#include "../Physics/Car.h"
#include "PlayerStats.h"
#include <algorithm>
#include <cmath>

PlayerAbilities::PlayerAbilities(Car* owner, PlayerStats* stats)
    : owner(owner)
    , stats(stats)
    , shieldActive(false)
    , shieldTimeRemaining(0.0f)
    , shieldStrength(0.0f)
    , dashActive(false)
    , dashTimeRemaining(0.0f)
    , dashDirection(Vector3::zero())
    , dashSpeed(0.0f)
    , teleportTargetPosition(Vector3::zero())
    , teleportPending(false)
    , teleportChargeTime(0.0f)
    , maxTeleportChargeTime(1.0f)
    , comboCount(0)
    , comboTimer(0.0f)
    , maxComboTime(3.0f)
    , comboMultiplier(1.0f) {
    
    initializeAbilities();
}

PlayerAbilities::~PlayerAbilities() {
    // Cleanup handled by unique_ptr
}

void PlayerAbilities::initializeAbilities() {
    abilities.clear();
    
    // Attack abilities
    abilities.push_back(std::make_unique<Ability>(
        AbilityType::LaserAttack, "Laser Attack", "Fire a fast laser projectile",
        1.0f, 10.0f, 25.0f, 150.0f, 0.0f, 1));
    
    abilities.push_back(std::make_unique<Ability>(
        AbilityType::PlasmaBlast, "Plasma Blast", "Fire a powerful plasma projectile",
        2.5f, 20.0f, 40.0f, 100.0f, 0.0f, 3));
    
    abilities.push_back(std::make_unique<Ability>(
        AbilityType::MissileStrike, "Missile Strike", "Launch a homing missile",
        4.0f, 30.0f, 60.0f, 200.0f, 0.0f, 5));
    
    abilities.push_back(std::make_unique<Ability>(
        AbilityType::EnergyBall, "Energy Ball", "Create an explosive energy ball",
        3.0f, 25.0f, 35.0f, 80.0f, 0.0f, 4));
    
    abilities.push_back(std::make_unique<Ability>(
        AbilityType::FistAttack, "Fist Attack", "Powerful melee punch",
        1.5f, 15.0f, 50.0f, 10.0f, 0.0f, 2));
    
    // Defensive abilities
    abilities.push_back(std::make_unique<Ability>(
        AbilityType::Shield, "Shield", "Activate protective energy shield",
        8.0f, 40.0f, 0.0f, 0.0f, 5.0f, 2));
    
    // Movement abilities
    abilities.push_back(std::make_unique<Ability>(
        AbilityType::Teleport, "Teleport", "Instantly move to target location",
        6.0f, 35.0f, 0.0f, 30.0f, 0.0f, 4));
    
    abilities.push_back(std::make_unique<Ability>(
        AbilityType::Dash, "Dash", "Quick forward movement burst",
        3.0f, 20.0f, 0.0f, 15.0f, 0.5f, 1));
    
    // Special abilities
    abilities.push_back(std::make_unique<Ability>(
        AbilityType::EnergyBurst, "Energy Burst", "Damage all nearby enemies",
        10.0f, 50.0f, 30.0f, 12.0f, 0.0f, 6));
    
    abilities.push_back(std::make_unique<Ability>(
        AbilityType::Heal, "Heal", "Restore health over time",
        15.0f, 30.0f, 0.0f, 0.0f, 3.0f, 3));
    
    checkLevelUnlocks();
}

void PlayerAbilities::update(float deltaTime) {
    updateCooldowns(deltaTime);
    updateActiveEffects(deltaTime);
    updateComboSystem(deltaTime);
    checkLevelUnlocks();
}

void PlayerAbilities::updateCooldowns(float deltaTime) {
    for (auto& ability : abilities) {
        if (ability->currentCooldown > 0.0f) {
            ability->currentCooldown -= deltaTime;
            ability->currentCooldown = std::max(0.0f, ability->currentCooldown);
        }
    }
}

void PlayerAbilities::updateActiveEffects(float deltaTime) {
    // Update shield
    if (shieldActive) {
        shieldTimeRemaining -= deltaTime;
        if (shieldTimeRemaining <= 0.0f) {
            deactivateShield();
        }
    }
    
    // Update dash
    if (dashActive) {
        dashTimeRemaining -= deltaTime;
        if (dashTimeRemaining <= 0.0f) {
            dashActive = false;
        } else if (owner) {
            // Apply dash movement
            Vector3 dashVelocity = dashDirection * dashSpeed;
            Vector3 currentVel = owner->getVelocity();
            owner->setVelocity(currentVel + dashVelocity * deltaTime);
        }
    }
    
    // Update teleport charging
    if (teleportPending) {
        teleportChargeTime += deltaTime;
        if (teleportChargeTime >= maxTeleportChargeTime) {
            executeTeleport();
        }
    }
}

void PlayerAbilities::updateComboSystem(float deltaTime) {
    if (comboCount > 0) {
        comboTimer -= deltaTime;
        if (comboTimer <= 0.0f) {
            resetCombo();
        }
    }
}

bool PlayerAbilities::useAbility(AbilityType type) {
    switch (type) {
        case AbilityType::LaserAttack: return useLaserAttack();
        case AbilityType::PlasmaBlast: return usePlasmaBlast();
        case AbilityType::MissileStrike: return useMissileStrike();
        case AbilityType::EnergyBall: return useEnergyBall();
        case AbilityType::FistAttack: return useFistAttack();
        case AbilityType::Shield: return useShield();
        case AbilityType::Teleport: return false; // Requires position parameter
        case AbilityType::Dash: return useDash();
        case AbilityType::EnergyBurst: return useEnergyBurst();
        case AbilityType::Heal: return useHeal();
        default: return false;
    }
}

bool PlayerAbilities::canUseAbility(AbilityType type) const {
    const Ability* ability = getAbility(type);
    if (!ability || !ability->isUnlocked || ability->currentCooldown > 0.0f) {
        return false;
    }
    
    if (!stats || !stats->hasEnergy(ability->energyCost)) {
        return false;
    }
    
    return true;
}

bool PlayerAbilities::useLaserAttack() {
    if (!canUseAbility(AbilityType::LaserAttack)) return false;
    
    Ability* ability = getAbility(AbilityType::LaserAttack);
    if (!consumeEnergy(ability->energyCost)) return false;
    
    float damage = calculateActualDamage(ability->baseDamage);
    createProjectile(Projectile::ProjectileType::Laser, damage);
    
    triggerCooldown(AbilityType::LaserAttack);
    addCombo();
    
    if (onAbilityUsed) onAbilityUsed(AbilityType::LaserAttack, true);
    return true;
}

bool PlayerAbilities::usePlasmaBlast() {
    if (!canUseAbility(AbilityType::PlasmaBlast)) return false;
    
    Ability* ability = getAbility(AbilityType::PlasmaBlast);
    if (!consumeEnergy(ability->energyCost)) return false;
    
    float damage = calculateActualDamage(ability->baseDamage);
    createProjectile(Projectile::ProjectileType::Plasma, damage);
    
    triggerCooldown(AbilityType::PlasmaBlast);
    addCombo();
    
    if (onAbilityUsed) onAbilityUsed(AbilityType::PlasmaBlast, true);
    return true;
}

bool PlayerAbilities::useMissileStrike() {
    if (!canUseAbility(AbilityType::MissileStrike)) return false;
    
    Ability* ability = getAbility(AbilityType::MissileStrike);
    if (!consumeEnergy(ability->energyCost)) return false;
    
    float damage = calculateActualDamage(ability->baseDamage);
    createProjectile(Projectile::ProjectileType::Missile, damage);
    
    triggerCooldown(AbilityType::MissileStrike);
    addCombo();
    
    if (onAbilityUsed) onAbilityUsed(AbilityType::MissileStrike, true);
    return true;
}

bool PlayerAbilities::useEnergyBall() {
    if (!canUseAbility(AbilityType::EnergyBall)) return false;
    
    Ability* ability = getAbility(AbilityType::EnergyBall);
    if (!consumeEnergy(ability->energyCost)) return false;
    
    float damage = calculateActualDamage(ability->baseDamage);
    createProjectile(Projectile::ProjectileType::EnergyBall, damage);
    
    triggerCooldown(AbilityType::EnergyBall);
    addCombo();
    
    if (onAbilityUsed) onAbilityUsed(AbilityType::EnergyBall, true);
    return true;
}

bool PlayerAbilities::useFistAttack() {
    if (!canUseAbility(AbilityType::FistAttack)) return false;
    
    Ability* ability = getAbility(AbilityType::FistAttack);
    if (!consumeEnergy(ability->energyCost)) return false;
    
    float damage = calculateActualDamage(ability->baseDamage);
    createProjectile(Projectile::ProjectileType::Fist, damage);
    
    triggerCooldown(AbilityType::FistAttack);
    addCombo();
    
    if (onAbilityUsed) onAbilityUsed(AbilityType::FistAttack, true);
    return true;
}

bool PlayerAbilities::useShield() {
    if (!canUseAbility(AbilityType::Shield)) return false;
    
    Ability* ability = getAbility(AbilityType::Shield);
    if (!consumeEnergy(ability->energyCost)) return false;
    
    shieldActive = true;
    shieldTimeRemaining = calculateActualDuration(ability->baseDuration);
    shieldStrength = stats ? stats->getMaxShieldStrength() : 100.0f;
    
    if (stats) {
        stats->activateShield();
        stats->setShieldStrength(shieldStrength);
    }
    
    triggerCooldown(AbilityType::Shield);
    
    if (onAbilityUsed) onAbilityUsed(AbilityType::Shield, true);
    return true;
}

bool PlayerAbilities::useTeleport(const Vector3& targetPosition) {
    if (!canUseAbility(AbilityType::Teleport)) return false;
    
    Ability* ability = getAbility(AbilityType::Teleport);
    
    // Check if target is within range
    if (owner) {
        float distance = (targetPosition - owner->getPosition()).length();
        float maxRange = calculateActualRange(ability->baseRange);
        if (distance > maxRange) return false;
    }
    
    if (!consumeEnergy(ability->energyCost)) return false;
    
    teleportTargetPosition = targetPosition;
    teleportPending = true;
    teleportChargeTime = 0.0f;
    
    triggerCooldown(AbilityType::Teleport);
    
    if (onAbilityUsed) onAbilityUsed(AbilityType::Teleport, true);
    return true;
}

bool PlayerAbilities::useDash() {
    if (!canUseAbility(AbilityType::Dash)) return false;
    
    Ability* ability = getAbility(AbilityType::Dash);
    if (!consumeEnergy(ability->energyCost)) return false;
    
    dashActive = true;
    dashTimeRemaining = calculateActualDuration(ability->baseDuration);
    dashDirection = getAimDirection();
    dashSpeed = calculateActualRange(ability->baseRange) / dashTimeRemaining;
    
    triggerCooldown(AbilityType::Dash);
    
    if (onAbilityUsed) onAbilityUsed(AbilityType::Dash, true);
    return true;
}

bool PlayerAbilities::useEnergyBurst() {
    if (!canUseAbility(AbilityType::EnergyBurst)) return false;
    
    Ability* ability = getAbility(AbilityType::EnergyBurst);
    if (!consumeEnergy(ability->energyCost)) return false;
    
    // Energy burst would damage all nearby enemies
    // This would be handled by the combat system
    
    triggerCooldown(AbilityType::EnergyBurst);
    addCombo();
    
    if (onAbilityUsed) onAbilityUsed(AbilityType::EnergyBurst, true);
    return true;
}

bool PlayerAbilities::useHeal() {
    if (!canUseAbility(AbilityType::Heal)) return false;
    
    Ability* ability = getAbility(AbilityType::Heal);
    if (!consumeEnergy(ability->energyCost)) return false;
    
    if (stats) {
        float healAmount = stats->getMaxHealth() * 0.3f; // Heal 30% of max health
        stats->heal(healAmount);
    }
    
    triggerCooldown(AbilityType::Heal);
    
    if (onAbilityUsed) onAbilityUsed(AbilityType::Heal, true);
    return true;
}

void PlayerAbilities::createProjectile(Projectile::ProjectileType projType, float damage) {
    if (!owner) return;
    
    Vector3 spawnPos = getProjectileSpawnPosition();
    Vector3 direction = getAimDirection();
    
    Projectile* projectile = nullptr;
    
    switch (projType) {
        case Projectile::ProjectileType::Laser:
            projectile = Projectile::createLaser(owner, spawnPos, direction, damage);
            break;
        case Projectile::ProjectileType::Plasma:
            projectile = Projectile::createPlasma(owner, spawnPos, direction, damage);
            break;
        case Projectile::ProjectileType::Missile:
            projectile = Projectile::createMissile(owner, spawnPos, direction, nullptr, damage);
            break;
        case Projectile::ProjectileType::EnergyBall:
            projectile = Projectile::createEnergyBall(owner, spawnPos, direction, damage);
            break;
        case Projectile::ProjectileType::Fist:
            projectile = Projectile::createFist(owner, spawnPos, direction, damage);
            break;
    }
    
    if (projectile && onProjectileCreated) {
        onProjectileCreated(projectile);
    }
}

Vector3 PlayerAbilities::getAimDirection() const {
    if (!owner) return Vector3::forward();
    
    // For now, use the car's forward direction
    // In a full implementation, this might use camera or mouse direction
    return owner->getForward();
}

Vector3 PlayerAbilities::getProjectileSpawnPosition() const {
    if (!owner) return Vector3::zero();
    
    Vector3 carPos = owner->getPosition();
    Vector3 forward = owner->getForward();
    Vector3 right = owner->getRight();
    
    // Spawn projectiles slightly in front and to the side of the car
    return carPos + forward * 2.0f + right * 0.5f + Vector3::up() * 1.0f;
}

float PlayerAbilities::calculateActualDamage(float baseDamage) const {
    if (!stats) return baseDamage;
    
    float damage = stats->calculateAttackDamage(baseDamage);
    damage *= comboMultiplier;
    return damage;
}

float PlayerAbilities::calculateActualCooldown(float baseCooldown) const {
    if (!stats) return baseCooldown;
    return stats->calculateAbilityCooldown(baseCooldown);
}

float PlayerAbilities::calculateActualRange(float baseRange) const {
    if (!stats) return baseRange;
    return stats->calculateTeleportDistance(baseRange);
}

float PlayerAbilities::calculateActualDuration(float baseDuration) const {
    if (!stats) return baseDuration;
    
    // Stamina can extend duration of abilities
    float staminaMultiplier = 1.0f + stats->getStamina() / 200.0f;
    return baseDuration * staminaMultiplier;
}

bool PlayerAbilities::consumeEnergy(float amount) {
    if (!stats || !stats->hasEnergy(amount)) return false;
    
    stats->consumeEnergy(amount);
    return true;
}

void PlayerAbilities::triggerCooldown(AbilityType type) {
    Ability* ability = getAbility(type);
    if (ability) {
        ability->currentCooldown = calculateActualCooldown(ability->baseCooldown);
    }
}

void PlayerAbilities::addCombo() {
    comboCount++;
    comboTimer = maxComboTime;
    comboMultiplier = 1.0f + (comboCount - 1) * 0.1f; // 10% damage increase per combo
    comboMultiplier = std::min(comboMultiplier, 2.0f); // Cap at 200%
    
    if (onComboChanged) {
        onComboChanged(comboCount);
    }
}

void PlayerAbilities::resetCombo() {
    comboCount = 0;
    comboTimer = 0.0f;
    comboMultiplier = 1.0f;
    
    if (onComboChanged) {
        onComboChanged(comboCount);
    }
}

void PlayerAbilities::executeTeleport() {
    if (!teleportPending || !owner) return;
    
    owner->setPosition(teleportTargetPosition);
    owner->setVelocity(Vector3::zero()); // Stop movement on teleport
    
    teleportPending = false;
    teleportChargeTime = 0.0f;
}

void PlayerAbilities::deactivateShield() {
    shieldActive = false;
    shieldTimeRemaining = 0.0f;
    
    if (stats) {
        stats->deactivateShield();
    }
}

void PlayerAbilities::checkLevelUnlocks() {
    if (!stats) return;
    
    int currentLevel = stats->getLevel();
    
    for (auto& ability : abilities) {
        if (!ability->isUnlocked && currentLevel >= ability->requiredLevel) {
            ability->isUnlocked = true;
            if (onAbilityUnlocked) {
                onAbilityUnlocked(ability->type);
            }
        }
    }
}

PlayerAbilities::Ability* PlayerAbilities::getAbility(AbilityType type) {
    for (auto& ability : abilities) {
        if (ability->type == type) {
            return ability.get();
        }
    }
    return nullptr;
}

const PlayerAbilities::Ability* PlayerAbilities::getAbility(AbilityType type) const {
    for (const auto& ability : abilities) {
        if (ability->type == type) {
            return ability.get();
        }
    }
    return nullptr;
}

float PlayerAbilities::getCooldownPercentage(AbilityType type) const {
    const Ability* ability = getAbility(type);
    if (!ability || ability->baseCooldown <= 0.0f) return 0.0f;
    
    return ability->currentCooldown / ability->baseCooldown;
}

float PlayerAbilities::getRemainingCooldown(AbilityType type) const {
    const Ability* ability = getAbility(type);
    return ability ? ability->currentCooldown : 0.0f;
}

bool PlayerAbilities::isAbilityActive(AbilityType type) const {
    switch (type) {
        case AbilityType::Shield: return shieldActive;
        case AbilityType::Dash: return dashActive;
        case AbilityType::Teleport: return teleportPending;
        default: return false;
    }
}

void PlayerAbilities::reset() {
    resetCooldowns();
    resetCombo();
    
    shieldActive = false;
    shieldTimeRemaining = 0.0f;
    dashActive = false;
    dashTimeRemaining = 0.0f;
    teleportPending = false;
    teleportChargeTime = 0.0f;
}

void PlayerAbilities::resetCooldowns() {
    for (auto& ability : abilities) {
        ability->currentCooldown = 0.0f;
    }
}