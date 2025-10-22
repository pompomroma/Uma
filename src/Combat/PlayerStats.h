#pragma once
#include <algorithm>
#include <functional>

class PlayerStats {
public:
    struct StatSet {
        float strength;     // Affects attack damage and projectile power
        float defense;      // Reduces incoming damage
        float stamina;      // Affects ability cooldowns and energy regeneration
        float agility;      // Affects movement speed, teleportation distance, and dodge chance
        
        StatSet() : strength(10.0f), defense(10.0f), stamina(10.0f), agility(10.0f) {}
        StatSet(float str, float def, float sta, float agi) 
            : strength(str), defense(def), stamina(sta), agility(agi) {}
    };

private:
    StatSet baseStats;          // Base stats without modifiers
    StatSet currentStats;       // Current stats with all modifiers applied
    StatSet statModifiers;      // Temporary stat modifiers
    
    // Health and Energy
    float maxHealth;
    float currentHealth;
    float maxEnergy;
    float currentEnergy;
    float healthRegenRate;
    float energyRegenRate;
    
    // Experience and leveling
    int level;
    float experience;
    float experienceToNextLevel;
    int availableStatPoints;
    
    // Combat state
    bool isAlive;
    bool isShielded;
    float shieldStrength;
    float maxShieldStrength;
    float shieldRegenRate;
    float shieldRegenDelay;
    float timeSinceLastDamage;
    
    // Callbacks for stat changes
    std::function<void(const StatSet&)> onStatsChanged;
    std::function<void(float, float)> onHealthChanged;
    std::function<void(float, float)> onEnergyChanged;
    std::function<void(int)> onLevelUp;

public:
    PlayerStats();
    PlayerStats(const StatSet& initialStats);
    
    // Update
    void update(float deltaTime);
    
    // Stat management
    const StatSet& getBaseStats() const { return baseStats; }
    const StatSet& getCurrentStats() const { return currentStats; }
    void setBaseStats(const StatSet& stats);
    void addStatModifier(const StatSet& modifier);
    void removeStatModifier(const StatSet& modifier);
    void clearStatModifiers();
    void recalculateStats();
    
    // Individual stat getters
    float getStrength() const { return currentStats.strength; }
    float getDefense() const { return currentStats.defense; }
    float getStamina() const { return currentStats.stamina; }
    float getAgility() const { return currentStats.agility; }
    
    // Health system
    float getHealth() const { return currentHealth; }
    float getMaxHealth() const { return maxHealth; }
    float getHealthPercentage() const { return maxHealth > 0.0f ? currentHealth / maxHealth : 0.0f; }
    void setHealth(float health);
    void setMaxHealth(float maxHealth);
    void heal(float amount);
    void takeDamage(float damage);
    bool isHealthFull() const { return currentHealth >= maxHealth; }
    
    // Energy system
    float getEnergy() const { return currentEnergy; }
    float getMaxEnergy() const { return maxEnergy; }
    float getEnergyPercentage() const { return maxEnergy > 0.0f ? currentEnergy / maxEnergy : 0.0f; }
    void setEnergy(float energy);
    void setMaxEnergy(float maxEnergy);
    void restoreEnergy(float amount);
    void consumeEnergy(float amount);
    bool hasEnergy(float amount) const { return currentEnergy >= amount; }
    bool isEnergyFull() const { return currentEnergy >= maxEnergy; }
    
    // Shield system
    float getShieldStrength() const { return shieldStrength; }
    float getMaxShieldStrength() const { return maxShieldStrength; }
    float getShieldPercentage() const { return maxShieldStrength > 0.0f ? shieldStrength / maxShieldStrength : 0.0f; }
    bool hasShield() const { return isShielded && shieldStrength > 0.0f; }
    void activateShield();
    void deactivateShield();
    void setShieldStrength(float strength);
    void damageShield(float damage);
    void rechargeShield(float amount);
    
    // Experience and leveling
    int getLevel() const { return level; }
    float getExperience() const { return experience; }
    float getExperienceToNextLevel() const { return experienceToNextLevel; }
    float getExperiencePercentage() const { return experienceToNextLevel > 0.0f ? experience / experienceToNextLevel : 1.0f; }
    int getAvailableStatPoints() const { return availableStatPoints; }
    void gainExperience(float exp);
    void levelUp();
    void spendStatPoint(const std::string& statName);
    
    // Combat utilities
    float calculateDamageReduction(float incomingDamage) const;
    float calculateAttackDamage(float baseDamage) const;
    float calculateAbilityCooldown(float baseCooldown) const;
    float calculateMovementSpeed(float baseSpeed) const;
    float calculateTeleportDistance(float baseDistance) const;
    bool rollDodgeChance() const;
    
    // State queries
    bool getIsAlive() const { return isAlive; }
    void setAlive(bool alive) { isAlive = alive; }
    void kill();
    void respawn();
    
    // Regeneration rates
    void setHealthRegenRate(float rate) { healthRegenRate = rate; }
    void setEnergyRegenRate(float rate) { energyRegenRate = rate; }
    void setShieldRegenRate(float rate) { shieldRegenRate = rate; }
    void setShieldRegenDelay(float delay) { shieldRegenDelay = delay; }
    
    // Callbacks
    void setStatsChangedCallback(std::function<void(const StatSet&)> callback) { onStatsChanged = callback; }
    void setHealthChangedCallback(std::function<void(float, float)> callback) { onHealthChanged = callback; }
    void setEnergyChangedCallback(std::function<void(float, float)> callback) { onEnergyChanged = callback; }
    void setLevelUpCallback(std::function<void(int)> callback) { onLevelUp = callback; }
    
    // Utility
    void reset();
    void resetToLevel(int targetLevel);
    
private:
    void updateRegeneration(float deltaTime);
    void updateShieldRegeneration(float deltaTime);
    void calculateMaxHealthAndEnergy();
    void triggerCallbacks();
};