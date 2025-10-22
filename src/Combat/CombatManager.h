#pragma once
#include "../Math/Vector3.h"
#include "Player.h"  // Include Player to access AttackType enum
#include <vector>
#include <memory>
#include <unordered_map>

// Forward declarations
class Projectile;

class CombatManager {
public:
    struct CombatEvent {
        enum Type {
            PlayerAttack,
            PlayerDamaged,
            PlayerKilled,
            PlayerRespawned,
            ProjectileHit,
            ShieldBreak,
            LevelUp,
            ComboPerformed
        };
        
        Type type;
        int playerId;
        int targetId;
        float value;
        Vector3 position;
    };
    
    struct CombatStats {
        int kills;
        int deaths;
        int assists;
        float damageDealt;
        float damageTaken;
        int highestCombo;
        int projectilesHit;
        int projectilesFired;
        
        CombatStats() : kills(0), deaths(0), assists(0), damageDealt(0),
                       damageTaken(0), highestCombo(0), projectilesHit(0), projectilesFired(0) {}
    };
    
private:
    std::vector<std::unique_ptr<Player>> players;
    std::unordered_map<int, CombatStats> playerStats;
    std::vector<CombatEvent> eventQueue;
    
    // Combat settings
    float respawnTime;
    bool friendlyFire;
    int maxPlayers;
    float matchDuration;
    float currentMatchTime;
    
    // Spawn points
    std::vector<Vector3> spawnPoints;
    int nextSpawnIndex;
    
    // Power-ups
    struct PowerUp {
        Vector3 position;
        enum Type { Health, Shield, Damage, Speed } type;
        bool active;
        float respawnTimer;
    };
    std::vector<PowerUp> powerUps;
    
public:
    CombatManager();
    ~CombatManager();
    
    // Player management
    Player* addPlayer(const std::string& name);
    void removePlayer(int playerId);
    Player* getPlayer(int playerId);
    std::vector<Player*> getAllPlayers();
    std::vector<Player*> getAlivePlayers();
    
    // Update
    void update(float deltaTime);
    void updateCombat(float deltaTime);
    void updateProjectiles(float deltaTime);
    void updatePowerUps(float deltaTime);
    void checkCollisions();
    void processEvents();
    
    // Combat actions
    void handlePlayerAttack(Player* attacker, Player::AttackType type, const Vector3& direction);
    void handleFistAttack(Player* attacker);
    void handleLaserAttack(Player* attacker, const Vector3& direction);
    void handleAreaAttack(Player* attacker, const Vector3& center, float radius, float damage);
    void handlePlayerDamage(Player* victim, float damage, Player* attacker = nullptr);
    void handlePlayerDeath(Player* victim, Player* killer = nullptr);
    void respawnPlayer(Player* player);
    
    // Teleportation
    bool validateTeleportTarget(Player* player, const Vector3& target);
    void executeTeleport(Player* player, const Vector3& target);
    
    // Power-ups
    void spawnPowerUp(const Vector3& position, PowerUp::Type type);
    void collectPowerUp(Player* player, PowerUp& powerUp);
    
    // Match management
    void startMatch();
    void endMatch();
    bool isMatchActive() const;
    float getMatchTime() const { return currentMatchTime; }
    Player* getMatchLeader() const;
    
    // Statistics
    CombatStats getPlayerStats(int playerId) const;
    void recordKill(int killerId, int victimId);
    void recordDamage(int attackerId, int victimId, float damage);
    void recordProjectileHit(int playerId);
    void recordProjectileFired(int playerId);
    
    // Spawn management
    void addSpawnPoint(const Vector3& point);
    Vector3 getRandomSpawnPoint();
    Vector3 getBestSpawnPoint(Player* player);
    
    // Settings
    void setRespawnTime(float time) { respawnTime = time; }
    void setFriendlyFire(bool enabled) { friendlyFire = enabled; }
    void setMaxPlayers(int max) { maxPlayers = max; }
    void setMatchDuration(float duration) { matchDuration = duration; }
    
    // Events
    void pushEvent(const CombatEvent& event);
    std::vector<CombatEvent> getRecentEvents(float timeWindow = 5.0f);
    
private:
    void checkProjectileCollisions();
    void checkMeleeCollisions();
    void checkPowerUpCollisions();
    float calculateDistance(const Vector3& a, const Vector3& b);
    bool isInRange(const Vector3& pos1, const Vector3& pos2, float range);
    Player* findNearestEnemy(Player* player, float maxRange = -1.0f);
    std::vector<Player*> findPlayersInRadius(const Vector3& center, float radius);
};