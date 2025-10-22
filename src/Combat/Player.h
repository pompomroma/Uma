#pragma once
#include "../Math/Vector3.h"
#include "../Math/Matrix4.h"
#include "../Math/Quaternion.h"
#include <memory>
#include <vector>
#include <string>

// Forward declarations
class Projectile;
class Shield;
class Ability;

class Player {
public:
    // Player stats structure
    struct Stats {
        // Core stats
        float strength;      // Affects damage output
        float defense;       // Reduces incoming damage
        float stamina;       // Affects ability usage and sprinting
        float agility;       // Affects movement speed and cooldowns
        
        // Derived stats
        float maxHealth;
        float currentHealth;
        float maxStamina;
        float currentStamina;
        float healthRegen;
        float staminaRegen;
        
        // Experience and leveling
        int level;
        int experience;
        int experienceToNextLevel;
        int statPoints;     // Available points to distribute
        
        Stats();
        void recalculateDerivedStats();
        void levelUp();
        void applyStatPoint(const std::string& statName);
    };
    
    // Combat state
    enum class CombatState {
        Idle,
        Attacking,
        Defending,
        Teleporting,
        Stunned,
        Dead
    };
    
    // Attack types
    enum class AttackType {
        None,
        Fist,
        Laser,
        AreaOfEffect
    };

private:
    // Player identification
    int playerId;
    std::string playerName;
    bool isLocalPlayer;
    
    // Position and orientation
    Vector3 position;
    Vector3 velocity;
    Quaternion rotation;
    Vector3 lookDirection;
    
    // Stats
    Stats stats;
    
    // Combat state
    CombatState currentState;
    AttackType currentAttack;
    float stateTimer;
    float lastAttackTime;
    float lastDamageTime;
    
    // Combat mechanics
    std::unique_ptr<Shield> shield;
    std::vector<std::unique_ptr<Projectile>> activeProjectiles;
    
    // Teleportation
    Vector3 teleportTarget;
    float teleportCooldown;
    float teleportRange;
    bool isTeleporting;
    float teleportProgress;
    
    // Attack properties
    float fistDamage;
    float fistRange;
    float fistCooldown;
    
    float laserDamage;
    float laserRange;
    float laserSpeed;
    float laserCooldown;
    
    // Shield properties
    float shieldStrength;
    float shieldMaxStrength;
    float shieldRegenRate;
    float shieldCooldown;
    bool isShielding;
    
    // Visual effects
    Vector3 color;
    float damageFlashTimer;
    float healFlashTimer;
    
    // Combo system
    std::vector<AttackType> comboSequence;
    float comboTimer;
    int comboMultiplier;
    
    // Buffs and debuffs
    struct Buff {
        std::string name;
        float duration;
        float statModifier;
        std::string affectedStat;
    };
    std::vector<Buff> activeBuffs;
    
public:
    Player(int id, const std::string& name, const Vector3& startPos);
    ~Player();
    
    // Update
    void update(float deltaTime);
    void updateCombat(float deltaTime);
    void updateMovement(float deltaTime);
    void updateStats(float deltaTime);
    void updateProjectiles(float deltaTime);
    void updateBuffs(float deltaTime);
    void updateVisualEffects(float deltaTime);
    
    // Combat actions
    void performFistAttack();
    void fireLaser(const Vector3& direction);
    void activateShield();
    void deactivateShield();
    void teleport(const Vector3& target);
    void performCombo();
    
    // Damage and healing
    void takeDamage(float damage, Player* attacker = nullptr);
    void heal(float amount);
    void die();
    void respawn(const Vector3& respawnPoint);
    
    // Stats management
    void addExperience(int amount);
    void levelUp();
    void increaseStrength(float amount);
    void increaseDefense(float amount);
    void increaseStamina(float amount);
    void increaseAgility(float amount);
    void resetStats();
    
    // Buff management
    void applyBuff(const std::string& buffName, float duration, float modifier, const std::string& stat);
    void removeBuff(const std::string& buffName);
    void clearBuffs();
    
    // Getters
    int getPlayerId() const { return playerId; }
    std::string getPlayerName() const { return playerName; }
    Vector3 getPosition() const { return position; }
    Vector3 getVelocity() const { return velocity; }
    Quaternion getRotation() const { return rotation; }
    Vector3 getLookDirection() const { return lookDirection; }
    
    Stats& getStats() { return stats; }
    const Stats& getStats() const { return stats; }
    float getHealth() const { return stats.currentHealth; }
    float getMaxHealth() const { return stats.maxHealth; }
    float getStamina() const { return stats.currentStamina; }
    float getMaxStamina() const { return stats.maxStamina; }
    int getLevel() const { return stats.level; }
    int getExperience() const { return stats.experience; }
    
    CombatState getCombatState() const { return currentState; }
    bool isAlive() const { return stats.currentHealth > 0; }
    bool canAttack() const;
    bool canTeleport() const;
    bool isShieldActive() const { return isShielding; }
    float getShieldStrength() const { return shieldStrength; }
    
    // Setters
    void setPosition(const Vector3& pos);
    void setVelocity(const Vector3& vel);
    void setRotation(const Quaternion& rot);
    void setLookDirection(const Vector3& dir);
    void setLocalPlayer(bool isLocal) { isLocalPlayer = isLocal; }
    bool getLocalPlayer() const { return isLocalPlayer; }
    
    // Collision detection
    bool checkHit(const Vector3& point, float radius) const;
    Vector3 getBoundingBoxMin() const;
    Vector3 getBoundingBoxMax() const;
    
    // Networking
    void serializeState(std::vector<uint8_t>& buffer) const;
    void deserializeState(const std::vector<uint8_t>& buffer);
    
    // Rendering
    Matrix4 getTransformMatrix() const;
    Vector3 getColor() const { return color; }
    
    // Debug
    void debugPrint() const;
    
private:
    void updateCooldowns(float deltaTime);
    void checkComboTimeout(float deltaTime);
    float calculateDamage(float baseDamage) const;
    float calculateDefense(float incomingDamage) const;
    void consumeStamina(float amount);
    bool hasStamina(float amount) const;
};