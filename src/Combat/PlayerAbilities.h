#pragma once
#include "../Math/Vector3.h"
#include "Projectile.h"
#include <vector>
#include <memory>
#include <functional>

class Car;
class PlayerStats;

class PlayerAbilities {
public:
    enum class AbilityType {
        LaserAttack,        // Basic laser projectile
        PlasmaBlast,        // Powerful plasma projectile
        MissileStrike,      // Homing missile
        EnergyBall,         // Area of effect attack
        FistAttack,         // Melee punch attack
        Shield,             // Defensive shield
        Teleport,           // Instant movement
        Dash,               // Quick forward movement
        EnergyBurst,        // Area damage around player
        Heal                // Self-healing ability
    };

    struct Ability {
        AbilityType type;
        std::string name;
        std::string description;
        float baseCooldown;         // Base cooldown time in seconds
        float currentCooldown;      // Current remaining cooldown
        float energyCost;           // Energy required to use
        float baseDamage;           // Base damage (for attack abilities)
        float baseRange;            // Base range/distance
        float baseDuration;         // Base duration (for shields, etc.)
        bool isUnlocked;            // Whether the player has unlocked this ability
        int requiredLevel;          // Level required to unlock
        bool isActive;              // Whether the ability is currently active (for toggles)
        
        Ability(AbilityType t, const std::string& n, const std::string& desc, 
                float cooldown, float energy, float damage = 0.0f, float range = 0.0f, 
                float duration = 0.0f, int reqLevel = 1)
            : type(t), name(n), description(desc), baseCooldown(cooldown), 
              currentCooldown(0.0f), energyCost(energy), baseDamage(damage), 
              baseRange(range), baseDuration(duration), isUnlocked(reqLevel <= 1), 
              requiredLevel(reqLevel), isActive(false) {}
    };

private:
    Car* owner;
    PlayerStats* stats;
    std::vector<std::unique_ptr<Ability>> abilities;
    
    // Active effects
    bool shieldActive;
    float shieldTimeRemaining;
    float shieldStrength;
    
    bool dashActive;
    float dashTimeRemaining;
    Vector3 dashDirection;
    float dashSpeed;
    
    // Teleport system
    Vector3 teleportTargetPosition;
    bool teleportPending;
    float teleportChargeTime;
    float maxTeleportChargeTime;
    
    // Combo system
    int comboCount;
    float comboTimer;
    float maxComboTime;
    float comboMultiplier;
    
    // Callbacks
    std::function<void(AbilityType, bool)> onAbilityUsed;
    std::function<void(Projectile*)> onProjectileCreated;
    std::function<void(AbilityType)> onAbilityUnlocked;
    std::function<void(int)> onComboChanged;

public:
    PlayerAbilities(Car* owner, PlayerStats* stats);
    ~PlayerAbilities();
    
    // Update
    void update(float deltaTime);
    void updateCooldowns(float deltaTime);
    void updateActiveEffects(float deltaTime);
    void updateComboSystem(float deltaTime);
    
    // Ability usage
    bool useAbility(AbilityType type);
    bool canUseAbility(AbilityType type) const;
    void cancelAbility(AbilityType type);
    
    // Individual abilities
    bool useLaserAttack();
    bool usePlasmaBlast();
    bool useMissileStrike();
    bool useEnergyBall();
    bool useFistAttack();
    bool useShield();
    bool useTeleport(const Vector3& targetPosition);
    bool useDash();
    bool useEnergyBurst();
    bool useHeal();
    
    // Ability management
    void unlockAbility(AbilityType type);
    void lockAbility(AbilityType type);
    bool isAbilityUnlocked(AbilityType type) const;
    void upgradeAbility(AbilityType type);
    
    // Getters
    const std::vector<std::unique_ptr<Ability>>& getAbilities() const { return abilities; }
    Ability* getAbility(AbilityType type);
    const Ability* getAbility(AbilityType type) const;
    float getCooldownPercentage(AbilityType type) const;
    float getRemainingCooldown(AbilityType type) const;
    bool isAbilityActive(AbilityType type) const;
    
    // Shield system
    bool hasActiveShield() const { return shieldActive; }
    float getShieldTimeRemaining() const { return shieldTimeRemaining; }
    float getShieldStrength() const { return shieldStrength; }
    void deactivateShield();
    
    // Dash system
    bool hasActiveDash() const { return dashActive; }
    float getDashTimeRemaining() const { return dashTimeRemaining; }
    Vector3 getDashDirection() const { return dashDirection; }
    float getDashSpeed() const { return dashSpeed; }
    
    // Teleport system
    bool isTeleportPending() const { return teleportPending; }
    Vector3 getTeleportTarget() const { return teleportTargetPosition; }
    float getTeleportChargePercentage() const;
    void cancelTeleport();
    void executeTeleport();
    
    // Combo system
    int getComboCount() const { return comboCount; }
    float getComboMultiplier() const { return comboMultiplier; }
    float getComboTimeRemaining() const { return comboTimer; }
    void addCombo();
    void resetCombo();
    
    // Utility
    Vector3 getAimDirection() const;
    Vector3 getProjectileSpawnPosition() const;
    float calculateActualDamage(float baseDamage) const;
    float calculateActualCooldown(float baseCooldown) const;
    float calculateActualRange(float baseRange) const;
    float calculateActualDuration(float baseDuration) const;
    
    // Callbacks
    void setAbilityUsedCallback(std::function<void(AbilityType, bool)> callback) { onAbilityUsed = callback; }
    void setProjectileCreatedCallback(std::function<void(Projectile*)> callback) { onProjectileCreated = callback; }
    void setAbilityUnlockedCallback(std::function<void(AbilityType)> callback) { onAbilityUnlocked = callback; }
    void setComboChangedCallback(std::function<void(int)> callback) { onComboChanged = callback; }
    
    // Reset and initialization
    void reset();
    void resetCooldowns();
    void initializeAbilities();
    
private:
    void createProjectile(Projectile::ProjectileType projType, float damage);
    void applyStatModifications(Ability* ability);
    bool consumeEnergy(float amount);
    void triggerCooldown(AbilityType type);
    void checkLevelUnlocks();
};