#include "PlayerStats.h"
#include <cmath>
#include <random>

PlayerStats::PlayerStats() 
    : baseStats()
    , currentStats()
    , statModifiers()
    , maxHealth(100.0f)
    , currentHealth(100.0f)
    , maxEnergy(100.0f)
    , currentEnergy(100.0f)
    , healthRegenRate(5.0f)
    , energyRegenRate(10.0f)
    , level(1)
    , experience(0.0f)
    , experienceToNextLevel(100.0f)
    , availableStatPoints(0)
    , isAlive(true)
    , isShielded(false)
    , shieldStrength(0.0f)
    , maxShieldStrength(50.0f)
    , shieldRegenRate(20.0f)
    , shieldRegenDelay(3.0f)
    , timeSinceLastDamage(0.0f) {
    
    recalculateStats();
    calculateMaxHealthAndEnergy();
}

PlayerStats::PlayerStats(const StatSet& initialStats) : PlayerStats() {
    baseStats = initialStats;
    recalculateStats();
    calculateMaxHealthAndEnergy();
}

void PlayerStats::update(float deltaTime) {
    if (!isAlive) return;
    
    timeSinceLastDamage += deltaTime;
    updateRegeneration(deltaTime);
    updateShieldRegeneration(deltaTime);
}

void PlayerStats::setBaseStats(const StatSet& stats) {
    baseStats = stats;
    recalculateStats();
    calculateMaxHealthAndEnergy();
    triggerCallbacks();
}

void PlayerStats::addStatModifier(const StatSet& modifier) {
    statModifiers.strength += modifier.strength;
    statModifiers.defense += modifier.defense;
    statModifiers.stamina += modifier.stamina;
    statModifiers.agility += modifier.agility;
    recalculateStats();
    triggerCallbacks();
}

void PlayerStats::removeStatModifier(const StatSet& modifier) {
    statModifiers.strength -= modifier.strength;
    statModifiers.defense -= modifier.defense;
    statModifiers.stamina -= modifier.stamina;
    statModifiers.agility -= modifier.agility;
    recalculateStats();
    triggerCallbacks();
}

void PlayerStats::clearStatModifiers() {
    statModifiers = StatSet();
    recalculateStats();
    triggerCallbacks();
}

void PlayerStats::recalculateStats() {
    currentStats.strength = std::max(1.0f, baseStats.strength + statModifiers.strength);
    currentStats.defense = std::max(1.0f, baseStats.defense + statModifiers.defense);
    currentStats.stamina = std::max(1.0f, baseStats.stamina + statModifiers.stamina);
    currentStats.agility = std::max(1.0f, baseStats.agility + statModifiers.agility);
}

void PlayerStats::setHealth(float health) {
    float oldHealth = currentHealth;
    currentHealth = std::clamp(health, 0.0f, maxHealth);
    
    if (currentHealth <= 0.0f && isAlive) {
        kill();
    }
    
    if (onHealthChanged && currentHealth != oldHealth) {
        onHealthChanged(currentHealth, maxHealth);
    }
}

void PlayerStats::setMaxHealth(float maxHealth) {
    this->maxHealth = std::max(1.0f, maxHealth);
    if (currentHealth > this->maxHealth) {
        setHealth(this->maxHealth);
    }
}

void PlayerStats::heal(float amount) {
    setHealth(currentHealth + amount);
}

void PlayerStats::takeDamage(float damage) {
    if (!isAlive) return;
    
    timeSinceLastDamage = 0.0f;
    
    // Apply shield first
    if (hasShield()) {
        float shieldDamage = std::min(damage, shieldStrength);
        damageShield(shieldDamage);
        damage -= shieldDamage;
    }
    
    // Apply defense reduction
    damage = calculateDamageReduction(damage);
    
    // Apply remaining damage to health
    if (damage > 0.0f) {
        setHealth(currentHealth - damage);
    }
}

void PlayerStats::setEnergy(float energy) {
    float oldEnergy = currentEnergy;
    currentEnergy = std::clamp(energy, 0.0f, maxEnergy);
    
    if (onEnergyChanged && currentEnergy != oldEnergy) {
        onEnergyChanged(currentEnergy, maxEnergy);
    }
}

void PlayerStats::setMaxEnergy(float maxEnergy) {
    this->maxEnergy = std::max(1.0f, maxEnergy);
    if (currentEnergy > this->maxEnergy) {
        setEnergy(this->maxEnergy);
    }
}

void PlayerStats::restoreEnergy(float amount) {
    setEnergy(currentEnergy + amount);
}

void PlayerStats::consumeEnergy(float amount) {
    setEnergy(currentEnergy - amount);
}

void PlayerStats::activateShield() {
    isShielded = true;
    if (shieldStrength <= 0.0f) {
        shieldStrength = maxShieldStrength;
    }
}

void PlayerStats::deactivateShield() {
    isShielded = false;
}

void PlayerStats::setShieldStrength(float strength) {
    shieldStrength = std::clamp(strength, 0.0f, maxShieldStrength);
}

void PlayerStats::damageShield(float damage) {
    shieldStrength = std::max(0.0f, shieldStrength - damage);
    if (shieldStrength <= 0.0f) {
        isShielded = false;
    }
}

void PlayerStats::rechargeShield(float amount) {
    if (isShielded) {
        setShieldStrength(shieldStrength + amount);
    }
}

void PlayerStats::gainExperience(float exp) {
    experience += exp;
    
    while (experience >= experienceToNextLevel) {
        levelUp();
    }
}

void PlayerStats::levelUp() {
    level++;
    experience -= experienceToNextLevel;
    experienceToNextLevel = 100.0f * std::pow(1.2f, level - 1); // Exponential scaling
    availableStatPoints += 3; // 3 stat points per level
    
    // Increase base stats slightly on level up
    baseStats.strength += 1.0f;
    baseStats.defense += 1.0f;
    baseStats.stamina += 1.0f;
    baseStats.agility += 1.0f;
    
    recalculateStats();
    calculateMaxHealthAndEnergy();
    
    // Restore health and energy on level up
    setHealth(maxHealth);
    setEnergy(maxEnergy);
    
    if (onLevelUp) {
        onLevelUp(level);
    }
    
    triggerCallbacks();
}

void PlayerStats::spendStatPoint(const std::string& statName) {
    if (availableStatPoints <= 0) return;
    
    if (statName == "strength") {
        baseStats.strength += 1.0f;
        availableStatPoints--;
    } else if (statName == "defense") {
        baseStats.defense += 1.0f;
        availableStatPoints--;
    } else if (statName == "stamina") {
        baseStats.stamina += 1.0f;
        availableStatPoints--;
    } else if (statName == "agility") {
        baseStats.agility += 1.0f;
        availableStatPoints--;
    }
    
    recalculateStats();
    calculateMaxHealthAndEnergy();
    triggerCallbacks();
}

float PlayerStats::calculateDamageReduction(float incomingDamage) const {
    // Defense reduces damage by a percentage based on defense stat
    float defenseReduction = currentStats.defense / (currentStats.defense + 100.0f);
    return incomingDamage * (1.0f - defenseReduction);
}

float PlayerStats::calculateAttackDamage(float baseDamage) const {
    // Strength increases damage
    return baseDamage * (1.0f + currentStats.strength / 100.0f);
}

float PlayerStats::calculateAbilityCooldown(float baseCooldown) const {
    // Stamina reduces cooldowns
    float cooldownReduction = currentStats.stamina / (currentStats.stamina + 200.0f);
    return baseCooldown * (1.0f - cooldownReduction);
}

float PlayerStats::calculateMovementSpeed(float baseSpeed) const {
    // Agility increases movement speed
    return baseSpeed * (1.0f + currentStats.agility / 200.0f);
}

float PlayerStats::calculateTeleportDistance(float baseDistance) const {
    // Agility increases teleport distance
    return baseDistance * (1.0f + currentStats.agility / 150.0f);
}

bool PlayerStats::rollDodgeChance() const {
    // Agility provides dodge chance (max 50% at high agility)
    float dodgeChance = std::min(0.5f, currentStats.agility / 200.0f);
    
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    
    return dis(gen) < dodgeChance;
}

void PlayerStats::kill() {
    isAlive = false;
    currentHealth = 0.0f;
    isShielded = false;
    shieldStrength = 0.0f;
    
    if (onHealthChanged) {
        onHealthChanged(currentHealth, maxHealth);
    }
}

void PlayerStats::respawn() {
    isAlive = true;
    setHealth(maxHealth);
    setEnergy(maxEnergy);
    shieldStrength = 0.0f;
    isShielded = false;
    timeSinceLastDamage = 0.0f;
}

void PlayerStats::reset() {
    level = 1;
    experience = 0.0f;
    experienceToNextLevel = 100.0f;
    availableStatPoints = 0;
    baseStats = StatSet();
    clearStatModifiers();
    recalculateStats();
    calculateMaxHealthAndEnergy();
    respawn();
}

void PlayerStats::resetToLevel(int targetLevel) {
    reset();
    for (int i = 1; i < targetLevel; i++) {
        levelUp();
    }
}

void PlayerStats::updateRegeneration(float deltaTime) {
    // Health regeneration (only when not at full health and not recently damaged)
    if (currentHealth < maxHealth && timeSinceLastDamage > 5.0f) {
        float regenAmount = healthRegenRate * deltaTime;
        heal(regenAmount);
    }
    
    // Energy regeneration (affected by stamina)
    if (currentEnergy < maxEnergy) {
        float staminaMultiplier = 1.0f + currentStats.stamina / 100.0f;
        float regenAmount = energyRegenRate * staminaMultiplier * deltaTime;
        restoreEnergy(regenAmount);
    }
}

void PlayerStats::updateShieldRegeneration(float deltaTime) {
    // Shield regeneration (only when shield is active and not recently damaged)
    if (isShielded && shieldStrength < maxShieldStrength && timeSinceLastDamage > shieldRegenDelay) {
        float regenAmount = shieldRegenRate * deltaTime;
        rechargeShield(regenAmount);
    }
}

void PlayerStats::calculateMaxHealthAndEnergy() {
    // Base health and energy scale with stats
    float baseHealthValue = 100.0f;
    float baseEnergyValue = 100.0f;
    
    // Stamina affects both health and energy
    maxHealth = baseHealthValue + (currentStats.stamina * 5.0f);
    maxEnergy = baseEnergyValue + (currentStats.stamina * 3.0f);
    
    // Defense affects shield strength
    maxShieldStrength = 50.0f + (currentStats.defense * 2.0f);
    
    // Ensure current values don't exceed new maximums
    if (currentHealth > maxHealth) {
        setHealth(maxHealth);
    }
    if (currentEnergy > maxEnergy) {
        setEnergy(maxEnergy);
    }
    if (shieldStrength > maxShieldStrength) {
        setShieldStrength(maxShieldStrength);
    }
}

void PlayerStats::triggerCallbacks() {
    if (onStatsChanged) {
        onStatsChanged(currentStats);
    }
    if (onHealthChanged) {
        onHealthChanged(currentHealth, maxHealth);
    }
    if (onEnergyChanged) {
        onEnergyChanged(currentEnergy, maxEnergy);
    }
}