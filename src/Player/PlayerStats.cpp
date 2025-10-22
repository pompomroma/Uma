#include "PlayerStats.h"
#include <algorithm>
#include <cmath>

PlayerStats::PlayerStats() 
    : strength(10.0f)
    , defense(10.0f)
    , stamina(10.0f)
    , agility(10.0f)
    , maxHealth(100.0f)
    , currentHealth(100.0f)
    , maxStamina(100.0f)
    , currentStamina(100.0f)
    , attackDamage(20.0f)
    , defenseValue(5.0f)
    , movementSpeed(1.0f)
    , attackSpeed(1.0f)
    , dodgeChance(0.1f)
    , level(1)
    , experience(0.0f)
    , experienceToNextLevel(100.0f)
    , statPoints(0)
    , lastAttackTime(0.0f)
    , lastAbilityTime(0.0f)
    , isShielded(false)
    , shieldDuration(0.0f)
    , shieldCooldown(0.0f)
    , teleportRange(10.0f)
    , teleportCooldown(0.0f)
    , lastTeleportTime(0.0f)
    , laserDamage(30.0f)
    , laserRange(50.0f)
    , laserCooldown(0.0f)
    , lastLaserTime(0.0f)
    , fistDamage(25.0f)
    , fistRange(3.0f)
    , fistCooldown(0.0f)
    , lastFistTime(0.0f) {
    initialize();
}

PlayerStats::~PlayerStats() {
}

void PlayerStats::initialize() {
    recalculateDerivedStats();
    currentHealth = maxHealth;
    currentStamina = maxStamina;
}

void PlayerStats::reset() {
    strength = 10.0f;
    defense = 10.0f;
    stamina = 10.0f;
    agility = 10.0f;
    level = 1;
    experience = 0.0f;
    experienceToNextLevel = 100.0f;
    statPoints = 0;
    currentHealth = maxHealth;
    currentStamina = maxStamina;
    isShielded = false;
    shieldDuration = 0.0f;
    shieldCooldown = 0.0f;
    lastTeleportTime = 0.0f;
    lastLaserTime = 0.0f;
    lastFistTime = 0.0f;
    recalculateDerivedStats();
}

void PlayerStats::addStatPoint(const std::string& statName, int points) {
    if (statPoints < points) return;
    
    if (statName == "strength") {
        strength += points;
    } else if (statName == "defense") {
        defense += points;
    } else if (statName == "stamina") {
        stamina += points;
    } else if (statName == "agility") {
        agility += points;
    }
    
    statPoints -= points;
    recalculateDerivedStats();
}

void PlayerStats::levelUp() {
    if (!canLevelUp()) return;
    
    level++;
    experience -= experienceToNextLevel;
    experienceToNextLevel *= 1.2f; // Exponential growth
    statPoints += 3; // Gain 3 stat points per level
    
    // Increase base stats slightly on level up
    strength += 1.0f;
    defense += 1.0f;
    stamina += 1.0f;
    agility += 1.0f;
    
    // Restore health and stamina
    currentHealth = maxHealth;
    currentStamina = maxStamina;
    
    recalculateDerivedStats();
}

void PlayerStats::gainExperience(float exp) {
    experience += exp;
    while (canLevelUp()) {
        levelUp();
    }
}

void PlayerStats::takeDamage(float damage) {
    if (isShielded) {
        damage *= 0.3f; // Shield reduces damage by 70%
    }
    
    float actualDamage = std::max(0.0f, damage - defenseValue);
    currentHealth = std::max(0.0f, currentHealth - actualDamage);
}

void PlayerStats::heal(float amount) {
    currentHealth = std::min(maxHealth, currentHealth + amount);
}

void PlayerStats::consumeStamina(float amount) {
    currentStamina = std::max(0.0f, currentStamina - amount);
}

void PlayerStats::restoreStamina(float amount) {
    currentStamina = std::min(maxStamina, currentStamina + amount);
}

bool PlayerStats::canAttack() const {
    return currentStamina >= 10.0f;
}

bool PlayerStats::canUseAbility() const {
    return currentStamina >= 20.0f;
}

bool PlayerStats::canTeleport() const {
    return teleportCooldown <= 0.0f && currentStamina >= 30.0f;
}

bool PlayerStats::canUseLaser() const {
    return laserCooldown <= 0.0f && currentStamina >= 15.0f;
}

bool PlayerStats::canUseFist() const {
    return fistCooldown <= 0.0f && currentStamina >= 10.0f;
}

float PlayerStats::performLaserAttack() {
    if (!canUseLaser()) return 0.0f;
    
    lastLaserTime = 0.0f;
    laserCooldown = 2.0f / attackSpeed; // Base 2 second cooldown modified by attack speed
    consumeStamina(15.0f);
    
    return laserDamage * (1.0f + strength * 0.02f); // Strength increases laser damage
}

float PlayerStats::performFistAttack() {
    if (!canUseFist()) return 0.0f;
    
    lastFistTime = 0.0f;
    fistCooldown = 1.0f / attackSpeed; // Base 1 second cooldown modified by attack speed
    consumeStamina(10.0f);
    
    return fistDamage * (1.0f + strength * 0.03f); // Strength increases fist damage more
}

void PlayerStats::activateShield() {
    if (shieldCooldown <= 0.0f && currentStamina >= 25.0f) {
        isShielded = true;
        shieldDuration = 3.0f + defense * 0.1f; // Defense increases shield duration
        shieldCooldown = 8.0f;
        consumeStamina(25.0f);
    }
}

void PlayerStats::deactivateShield() {
    isShielded = false;
    shieldDuration = 0.0f;
}

bool PlayerStats::performTeleport(const Vector3& targetPosition) {
    if (!canTeleport()) return false;
    
    lastTeleportTime = 0.0f;
    teleportCooldown = 5.0f - agility * 0.05f; // Agility reduces cooldown
    teleportCooldown = std::max(1.0f, teleportCooldown);
    consumeStamina(30.0f);
    
    return true;
}

void PlayerStats::update(float deltaTime) {
    updateCooldowns(deltaTime);
    updateRegeneration(deltaTime);
    updateShield(deltaTime);
}

void PlayerStats::updateCooldowns(float deltaTime) {
    if (shieldCooldown > 0.0f) {
        shieldCooldown -= deltaTime;
    }
    if (teleportCooldown > 0.0f) {
        teleportCooldown -= deltaTime;
    }
    if (laserCooldown > 0.0f) {
        laserCooldown -= deltaTime;
    }
    if (fistCooldown > 0.0f) {
        fistCooldown -= deltaTime;
    }
}

void PlayerStats::updateRegeneration(float deltaTime) {
    // Health regeneration (slow)
    if (currentHealth < maxHealth) {
        float regenRate = 2.0f + stamina * 0.1f; // Stamina increases health regen
        heal(regenRate * deltaTime);
    }
    
    // Stamina regeneration
    if (currentStamina < maxStamina) {
        float regenRate = 15.0f + stamina * 0.5f; // Stamina increases stamina regen
        restoreStamina(regenRate * deltaTime);
    }
}

void PlayerStats::updateShield(float deltaTime) {
    if (isShielded) {
        shieldDuration -= deltaTime;
        if (shieldDuration <= 0.0f) {
            deactivateShield();
        }
    }
}

float PlayerStats::getHealthPercentage() const {
    return maxHealth > 0.0f ? currentHealth / maxHealth : 0.0f;
}

float PlayerStats::getStaminaPercentage() const {
    return maxStamina > 0.0f ? currentStamina / maxStamina : 0.0f;
}

bool PlayerStats::isAlive() const {
    return currentHealth > 0.0f;
}

bool PlayerStats::isShieldActive() const {
    return isShielded && shieldDuration > 0.0f;
}

void PlayerStats::recalculateDerivedStats() {
    // Health based on stamina and defense
    maxHealth = 100.0f + stamina * 5.0f + defense * 3.0f;
    
    // Stamina based on stamina stat
    maxStamina = 100.0f + stamina * 8.0f;
    
    // Attack damage based on strength
    attackDamage = 20.0f + strength * 2.0f;
    
    // Defense value based on defense stat
    defenseValue = 5.0f + defense * 1.5f;
    
    // Movement speed based on agility
    movementSpeed = 1.0f + agility * 0.05f;
    
    // Attack speed based on agility
    attackSpeed = 1.0f + agility * 0.03f;
    
    // Dodge chance based on agility
    dodgeChance = 0.1f + agility * 0.01f;
    dodgeChance = std::min(0.5f, dodgeChance); // Cap at 50%
    
    // Teleport range based on agility
    teleportRange = 10.0f + agility * 0.5f;
    
    // Laser damage based on strength
    laserDamage = 30.0f + strength * 1.5f;
    
    // Fist damage based on strength
    fistDamage = 25.0f + strength * 2.0f;
}

void PlayerStats::distributeStatPoints(int points) {
    statPoints += points;
}

bool PlayerStats::canLevelUp() const {
    return experience >= experienceToNextLevel;
}