#pragma once
#include "../Math/Vector3.h"
#include "../Math/Quaternion.h"
#include "PlayerStats.h"
#include <vector>
#include <memory>

class Car; // Forward declaration

struct Attack {
    enum Type {
        LASER,
        FIST,
        SHIELD,
        TELEPORT
    };
    
    Type type;
    Vector3 origin;
    Vector3 direction;
    float damage;
    float range;
    float duration;
    float cooldown;
    bool isActive;
    Car* attacker;
    Car* target;
};

struct Projectile {
    Vector3 position;
    Vector3 velocity;
    Vector3 direction;
    float damage;
    float range;
    float speed;
    float lifetime;
    bool isActive;
    Car* owner;
    Attack::Type type;
};

class PvPMechanics {
public:
    // Attack types
    static constexpr float LASER_SPEED = 100.0f;
    static constexpr float LASER_LIFETIME = 2.0f;
    static constexpr float FIST_RANGE = 3.0f;
    static constexpr float SHIELD_DURATION = 3.0f;
    static constexpr float TELEPORT_RANGE = 15.0f;
    
private:
    std::vector<Attack> activeAttacks;
    std::vector<Projectile> activeProjectiles;
    std::vector<Car*> players;
    
    // Combat state
    bool combatEnabled;
    float globalCooldown;
    float lastGlobalUpdate;
    
    // Visual effects
    std::vector<Vector3> laserTrails;
    std::vector<Vector3> shieldEffects;
    std::vector<Vector3> teleportEffects;

public:
    PvPMechanics();
    ~PvPMechanics();
    
    // Initialization
    void initialize();
    void shutdown();
    
    // Player management
    void addPlayer(Car* player);
    void removePlayer(Car* player);
    void clearPlayers();
    
    // Combat system
    void enableCombat(bool enable);
    bool isCombatEnabled() const { return combatEnabled; }
    
    // Attack methods
    bool performLaserAttack(Car* attacker, const Vector3& direction);
    bool performFistAttack(Car* attacker, Car* target);
    bool activateShield(Car* player);
    bool performTeleport(Car* player, const Vector3& targetPosition);
    
    // Projectile system
    void updateProjectiles(float deltaTime);
    void renderProjectiles();
    void checkProjectileCollisions();
    
    // Combat resolution
    void resolveAttack(const Attack& attack);
    void applyDamage(Car* target, float damage, Car* attacker);
    void applyKnockback(Car* target, const Vector3& direction, float force);
    
    // Range and targeting
    Car* findNearestEnemy(Car* player, float maxRange = 50.0f);
    Car* findTargetInDirection(Car* player, const Vector3& direction, float maxRange = 50.0f);
    bool isInRange(const Vector3& from, const Vector3& to, float range);
    bool hasLineOfSight(const Vector3& from, const Vector3& to);
    
    // Update and rendering
    void update(float deltaTime);
    void render();
    void renderAttackEffects();
    void renderLaserBeam(const Vector3& start, const Vector3& end);
    void renderShieldEffect(Car* player);
    void renderTeleportEffect(const Vector3& position);
    
    // Getters
    const std::vector<Attack>& getActiveAttacks() const { return activeAttacks; }
    const std::vector<Projectile>& getActiveProjectiles() const { return activeProjectiles; }
    const std::vector<Car*>& getPlayers() const { return players; }
    
    // Utility
    void clearAllAttacks();
    void clearAllProjectiles();
    void resetCombat();
    
private:
    void updateAttacks(float deltaTime);
    void updateVisualEffects(float deltaTime);
    void cleanupExpiredAttacks();
    void cleanupExpiredProjectiles();
    
    // Attack creation helpers
    Attack createLaserAttack(Car* attacker, const Vector3& direction);
    Attack createFistAttack(Car* attacker, Car* target);
    Attack createShieldAttack(Car* player);
    Attack createTeleportAttack(Car* player, const Vector3& target);
    
    // Projectile creation
    Projectile createLaserProjectile(Car* owner, const Vector3& direction);
    Projectile createFistProjectile(Car* owner, const Vector3& direction);
    
    // Collision detection
    bool checkProjectileCarCollision(const Projectile& projectile, Car* car);
    bool checkAttackCarCollision(const Attack& attack, Car* car);
    
    // Visual effect helpers
    void addLaserTrail(const Vector3& start, const Vector3& end);
    void addShieldEffect(Car* player);
    void addTeleportEffect(const Vector3& position);
};