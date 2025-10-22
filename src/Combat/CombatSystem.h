#pragma once
#include "../Math/Vector3.h"
#include "Projectile.h"
#include "PlayerStats.h"
#include "PlayerAbilities.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

class Car;
class Game;

class CombatSystem {
public:
    struct CombatPlayer {
        Car* car;
        std::unique_ptr<PlayerStats> stats;
        std::unique_ptr<PlayerAbilities> abilities;
        int playerId;
        int kills;
        int deaths;
        float totalDamageDealt;
        float totalDamageTaken;
        bool isAlive;
        float respawnTimer;
        Vector3 respawnPosition;
        
        CombatPlayer(Car* c, int id) 
            : car(c), playerId(id), kills(0), deaths(0), 
              totalDamageDealt(0.0f), totalDamageTaken(0.0f), 
              isAlive(true), respawnTimer(0.0f), respawnPosition(Vector3::zero()) {
            stats = std::make_unique<PlayerStats>();
            abilities = std::make_unique<PlayerAbilities>(car, stats.get());
        }
    };

    struct DamageEvent {
        Car* attacker;
        Car* victim;
        float damage;
        Projectile::ProjectileType weaponType;
        Vector3 hitPosition;
        float timestamp;
        
        DamageEvent(Car* att, Car* vic, float dmg, Projectile::ProjectileType weapon, 
                   const Vector3& pos, float time)
            : attacker(att), victim(vic), damage(dmg), weaponType(weapon), 
              hitPosition(pos), timestamp(time) {}
    };

private:
    Game* game;
    std::vector<std::unique_ptr<CombatPlayer>> players;
    std::vector<std::unique_ptr<Projectile>> projectiles;
    std::vector<DamageEvent> recentDamageEvents;
    std::unordered_map<int, CombatPlayer*> playerMap; // playerId -> CombatPlayer
    
    // Combat settings
    float respawnTime;
    bool friendlyFire;
    float maxCombatRange;
    int maxProjectiles;
    
    // Match settings
    bool matchActive;
    float matchTime;
    float maxMatchTime;
    int scoreLimit;
    bool teamMode;
    
    // Area of effect tracking
    struct AoEEffect {
        Vector3 position;
        float radius;
        float damage;
        float duration;
        float timeRemaining;
        Car* owner;
        Projectile::ProjectileType type;
        
        AoEEffect(const Vector3& pos, float rad, float dmg, float dur, Car* own, Projectile::ProjectileType t)
            : position(pos), radius(rad), damage(dmg), duration(dur), 
              timeRemaining(dur), owner(own), type(t) {}
    };
    std::vector<AoEEffect> activeAoEEffects;
    
    // Callbacks
    std::function<void(Car*, Car*, float)> onPlayerDamaged;
    std::function<void(Car*, Car*)> onPlayerKilled;
    std::function<void(Car*)> onPlayerRespawned;
    std::function<void(Projectile*)> onProjectileCreated;
    std::function<void(Projectile*)> onProjectileDestroyed;
    std::function<void(const Vector3&, float, Projectile::ProjectileType)> onExplosion;
    std::function<void()> onMatchEnd;

public:
    CombatSystem(Game* game);
    ~CombatSystem();
    
    // System management
    void initialize();
    void shutdown();
    void update(float deltaTime);
    void reset();
    
    // Player management
    CombatPlayer* addPlayer(Car* car);
    void removePlayer(Car* car);
    CombatPlayer* getPlayer(Car* car);
    CombatPlayer* getPlayer(int playerId);
    const std::vector<std::unique_ptr<CombatPlayer>>& getPlayers() const { return players; }
    int getPlayerCount() const { return static_cast<int>(players.size()); }
    
    // Combat mechanics
    void fireProjectile(Car* owner, Projectile::ProjectileType type, const Vector3& direction);
    void dealDamage(Car* attacker, Car* victim, float damage, Projectile::ProjectileType weaponType, const Vector3& hitPos);
    void killPlayer(Car* victim, Car* attacker = nullptr);
    void respawnPlayer(Car* player);
    void healPlayer(Car* player, float amount);
    
    // Projectile management
    void updateProjectiles(float deltaTime);
    void checkProjectileCollisions();
    void destroyProjectile(Projectile* projectile);
    void createExplosion(const Vector3& position, float radius, float damage, Car* owner, Projectile::ProjectileType type);
    
    // Area of effect management
    void updateAoEEffects(float deltaTime);
    void addAoEEffect(const Vector3& position, float radius, float damage, float duration, Car* owner, Projectile::ProjectileType type);
    void checkAoEDamage();
    
    // Collision detection
    bool checkProjectileCarCollision(Projectile* projectile, Car* car);
    bool checkProjectileWorldCollision(Projectile* projectile);
    std::vector<Car*> getCarsInRadius(const Vector3& center, float radius, Car* exclude = nullptr);
    
    // Combat queries
    bool isPlayerAlive(Car* player);
    float getPlayerHealth(Car* player);
    float getPlayerEnergy(Car* player);
    PlayerStats* getPlayerStats(Car* player);
    PlayerAbilities* getPlayerAbilities(Car* player);
    
    // Match management
    void startMatch();
    void endMatch();
    void pauseMatch();
    void resumeMatch();
    bool isMatchActive() const { return matchActive; }
    float getMatchTime() const { return matchTime; }
    float getRemainingTime() const { return std::max(0.0f, maxMatchTime - matchTime); }
    
    // Scoring and statistics
    int getPlayerKills(Car* player);
    int getPlayerDeaths(Car* player);
    float getPlayerDamageDealt(Car* player);
    float getPlayerDamageTaken(Car* player);
    Car* getLeadingPlayer(); // Player with most kills
    void resetPlayerStats();
    
    // Settings
    void setRespawnTime(float time) { respawnTime = time; }
    void setFriendlyFire(bool enabled) { friendlyFire = enabled; }
    void setMaxCombatRange(float range) { maxCombatRange = range; }
    void setMaxProjectiles(int max) { maxProjectiles = max; }
    void setMatchTimeLimit(float time) { maxMatchTime = time; }
    void setScoreLimit(int limit) { scoreLimit = limit; }
    void setTeamMode(bool enabled) { teamMode = enabled; }
    
    float getRespawnTime() const { return respawnTime; }
    bool getFriendlyFire() const { return friendlyFire; }
    float getMaxCombatRange() const { return maxCombatRange; }
    int getMaxProjectiles() const { return maxProjectiles; }
    
    // Abilities
    bool usePlayerAbility(Car* player, PlayerAbilities::AbilityType ability);
    bool usePlayerAbilityWithTarget(Car* player, PlayerAbilities::AbilityType ability, const Vector3& target);
    bool canPlayerUseAbility(Car* player, PlayerAbilities::AbilityType ability);
    
    // Damage events
    const std::vector<DamageEvent>& getRecentDamageEvents() const { return recentDamageEvents; }
    void clearDamageEvents() { recentDamageEvents.clear(); }
    
    // Callbacks
    void setPlayerDamagedCallback(std::function<void(Car*, Car*, float)> callback) { onPlayerDamaged = callback; }
    void setPlayerKilledCallback(std::function<void(Car*, Car*)> callback) { onPlayerKilled = callback; }
    void setPlayerRespawnedCallback(std::function<void(Car*)> callback) { onPlayerRespawned = callback; }
    void setProjectileCreatedCallback(std::function<void(Projectile*)> callback) { onProjectileCreated = callback; }
    void setProjectileDestroyedCallback(std::function<void(Projectile*)> callback) { onProjectileDestroyed = callback; }
    void setExplosionCallback(std::function<void(const Vector3&, float, Projectile::ProjectileType)> callback) { onExplosion = callback; }
    void setMatchEndCallback(std::function<void()> callback) { onMatchEnd = callback; }
    
    // Utility
    Vector3 findSafeRespawnPosition(Car* player);
    bool isPositionSafe(const Vector3& position, float safeRadius = 10.0f);
    float calculateDistance(const Vector3& a, const Vector3& b);
    bool isInLineOfSight(const Vector3& from, const Vector3& to);
    
private:
    void updatePlayers(float deltaTime);
    void updateRespawns(float deltaTime);
    void updateMatch(float deltaTime);
    void checkMatchEndConditions();
    void cleanupDestroyedProjectiles();
    void limitProjectileCount();
    void setupPlayerCallbacks(CombatPlayer* player);
    int getNextPlayerId();
};