#pragma once
#include "../Math/Vector3.h"

class PlayerStats {
public:
    // Core stats
    float strength;     // Affects attack damage and melee effectiveness
    float defense;      // Reduces incoming damage
    float stamina;      // Affects ability usage and movement speed
    float agility;      // Affects movement speed, dodge chance, and attack speed
    
    // Derived stats
    float maxHealth;
    float currentHealth;
    float maxStamina;
    float currentStamina;
    float attackDamage;
    float defenseValue;
    float movementSpeed;
    float attackSpeed;
    float dodgeChance;
    
    // Experience and leveling
    int level;
    float experience;
    float experienceToNextLevel;
    int statPoints;
    
    // Combat stats
    float lastAttackTime;
    float lastAbilityTime;
    bool isShielded;
    float shieldDuration;
    float shieldCooldown;
    
    // Teleportation stats
    float teleportRange;
    float teleportCooldown;
    float lastTeleportTime;
    
    // Laser attack stats
    float laserDamage;
    float laserRange;
    float laserCooldown;
    float lastLaserTime;
    
    // Fist attack stats
    float fistDamage;
    float fistRange;
    float fistCooldown;
    float lastFistTime;

public:
    PlayerStats();
    ~PlayerStats();
    
    // Initialization
    void initialize();
    void reset();
    
    // Stat management
    void addStatPoint(const std::string& statName, int points = 1);
    void levelUp();
    void gainExperience(float exp);
    
    // Health and stamina management
    void takeDamage(float damage);
    void heal(float amount);
    void consumeStamina(float amount);
    void restoreStamina(float amount);
    
    // Combat abilities
    bool canAttack() const;
    bool canUseAbility() const;
    bool canTeleport() const;
    bool canUseLaser() const;
    bool canUseFist() const;
    
    // Attack methods
    float performLaserAttack();
    float performFistAttack();
    void activateShield();
    void deactivateShield();
    bool performTeleport(const Vector3& targetPosition);
    
    // Update methods
    void update(float deltaTime);
    void updateCooldowns(float deltaTime);
    void updateRegeneration(float deltaTime);
    
    // Getters
    float getHealthPercentage() const;
    float getStaminaPercentage() const;
    bool isAlive() const;
    bool isShieldActive() const;
    
    // Stat calculation
    void recalculateDerivedStats();
    float calculateDamageReduction() const;
    float calculateAttackSpeed() const;
    float calculateMovementSpeed() const;
    
    // Leveling
    void distributeStatPoints(int points);
    bool canLevelUp() const;
    
private:
    void updateShield(float deltaTime);
    void updateTeleportCooldown(float deltaTime);
    void updateLaserCooldown(float deltaTime);
    void updateFistCooldown(float deltaTime);
};