#pragma once
#include "../Math/Vector3.h"
#include "../Math/Matrix4.h"
#include "../Math/Quaternion.h"
#include "../Physics/Car.h"
#include <memory>
#include <vector>

class Projectile;
class Shield;

class Player {
public:
    struct Stats {
        float strength;      // Affects attack damage
        float defense;       // Reduces incoming damage
        float stamina;       // Affects ability usage and regeneration
        float agility;       // Affects movement speed and teleport cooldown
        
        float maxHealth;
        float currentHealth;
        float maxStamina;
        float currentStamina;
        float maxMana;       // For special abilities
        float currentMana;
        
        // Level and experience
        int level;
        float experience;
        float experienceToNextLevel;
        int statPoints;      // Unspent stat points
    };
    
    struct CombatStats {
        float attackDamage;
        float attackSpeed;
        float criticalChance;
        float criticalDamage;
        float damageReduction;
        float healingBonus;
    };
    
    enum class AttackType {
        None,
        Laser,
        Fist,
        Ultimate
    };
    
    enum class PlayerState {
        Idle,
        Attacking,
        Shielding,
        Teleporting,
        Stunned,
        Dead
    };

private:
    // Reference to player's car for movement
    Car* playerCar;
    
    // Stats
    Stats stats;
    CombatStats combatStats;
    
    // State
    PlayerState currentState;
    Vector3 handPosition;  // Position for laser origin
    Vector3 aimDirection;
    
    // Combat
    float attackCooldown;
    float lastAttackTime;
    bool isAttacking;
    AttackType currentAttackType;
    
    // Shield
    std::unique_ptr<Shield> shield;
    bool isShielding;
    float shieldCooldown;
    float shieldDuration;
    float shieldMaxDuration;
    
    // Teleportation
    float teleportRange;
    float teleportCooldown;
    float lastTeleportTime;
    int maxTeleportCharges;
    int currentTeleportCharges;
    float teleportChargeRegenTime;
    float timeSinceLastChargeRegen;
    
    // Projectiles
    std::vector<std::unique_ptr<Projectile>> activeProjectiles;
    
    // Visual effects
    Vector3 targetReticlePosition;
    bool showTargetReticle;
    
    // Regeneration
    float healthRegenRate;
    float staminaRegenRate;
    float manaRegenRate;
    float combatTimer;  // Time since last damage taken/dealt
    bool isInCombat;

public:
    Player();
    Player(Car* car);
    ~Player();
    
    // Initialization
    void initialize();
    
    // Update
    void update(float deltaTime);
    void updateStats(float deltaTime);
    void updateCombat(float deltaTime);
    void updateCooldowns(float deltaTime);
    void updateProjectiles(float deltaTime);
    void updateRegeneration(float deltaTime);
    
    // Stats management
    const Stats& getStats() const { return stats; }
    const CombatStats& getCombatStats() const { return combatStats; }
    void setBaseStat(const std::string& statName, float value);
    void increaseStat(const std::string& statName, float amount);
    void levelUp();
    void gainExperience(float exp);
    void spendStatPoint(const std::string& statName);
    void recalculateCombatStats();
    
    // Combat - Attacks
    void performLaserAttack(const Vector3& targetPosition);
    void performFistAttack();
    void performUltimateAttack();
    bool canAttack() const;
    void fireProjectile(const Vector3& direction, float damage, float speed);
    
    // Combat - Defense
    void activateShield();
    void deactivateShield();
    bool isShieldActive() const { return isShielding; }
    void takeDamage(float damage, Player* attacker = nullptr);
    void heal(float amount);
    
    // Mobility
    void teleportToPosition(const Vector3& targetPosition);
    void teleportInDirection(const Vector3& direction, float distance);
    bool canTeleport() const;
    
    // Stamina and Mana management
    bool consumeStamina(float amount);
    bool consumeMana(float amount);
    void regenerateStamina(float amount);
    void regenerateMana(float amount);
    
    // State management
    PlayerState getState() const { return currentState; }
    void setState(PlayerState state);
    bool isDead() const { return currentState == PlayerState::Dead; }
    void respawn(const Vector3& position);
    
    // Position and orientation
    Vector3 getPosition() const;
    Vector3 getForward() const;
    Vector3 getHandPosition() const { return handPosition; }
    void setAimDirection(const Vector3& direction);
    Vector3 getAimDirection() const { return aimDirection; }
    
    // Car management
    void setCar(Car* car) { playerCar = car; }
    Car* getCar() const { return playerCar; }
    
    // Projectile management
    const std::vector<std::unique_ptr<Projectile>>& getProjectiles() const { return activeProjectiles; }
    void removeDeadProjectiles();
    
    // UI/Visual
    void updateTargetReticle(const Vector3& target);
    Vector3 getTargetReticlePosition() const { return targetReticlePosition; }
    bool shouldShowTargetReticle() const { return showTargetReticle; }
    
    // Getters for UI
    float getHealthPercentage() const { return stats.currentHealth / stats.maxHealth; }
    float getStaminaPercentage() const { return stats.currentStamina / stats.maxStamina; }
    float getManaPercentage() const { return stats.currentMana / stats.maxMana; }
    float getShieldCooldownPercentage() const;
    float getTeleportCooldownPercentage() const;
    int getTeleportCharges() const { return currentTeleportCharges; }
    float getAttackCooldownPercentage() const;
    
    // Debug
    void debugDraw() const;
    
private:
    void updateHandPosition();
    void enterCombat();
    void exitCombat();
    void checkCombatStatus(float deltaTime);
    void initializeStats();
    float calculateDamageOutput(float baseDamage) const;
    float calculateDamageReduction(float incomingDamage) const;
};
