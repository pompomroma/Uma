#include "CombatManager.h"
#include "Player.h"
#include "Projectile.h"
#include <algorithm>
#include <random>
#include <iostream>

CombatManager::CombatManager()
    : respawnTime(3.0f)
    , friendlyFire(false)
    , maxPlayers(8)
    , matchDuration(600.0f)  // 10 minutes
    , currentMatchTime(0.0f)
    , nextSpawnIndex(0) {
    
    // Setup default spawn points
    addSpawnPoint(Vector3(0, 0, 0));
    addSpawnPoint(Vector3(20, 0, 20));
    addSpawnPoint(Vector3(-20, 0, 20));
    addSpawnPoint(Vector3(20, 0, -20));
    addSpawnPoint(Vector3(-20, 0, -20));
    
    // Setup power-up spawn points
    powerUps.push_back({Vector3(10, 1, 10), PowerUp::Health, true, 0});
    powerUps.push_back({Vector3(-10, 1, 10), PowerUp::Shield, true, 0});
    powerUps.push_back({Vector3(10, 1, -10), PowerUp::Damage, true, 0});
    powerUps.push_back({Vector3(-10, 1, -10), PowerUp::Speed, true, 0});
}

CombatManager::~CombatManager() {
    players.clear();
}

Player* CombatManager::addPlayer(const std::string& name) {
    if (players.size() >= maxPlayers) return nullptr;
    
    int playerId = players.size();
    Vector3 spawnPoint = getRandomSpawnPoint();
    
    auto player = std::make_unique<Player>(playerId, name, spawnPoint);
    Player* playerPtr = player.get();
    players.push_back(std::move(player));
    
    // Initialize stats for this player
    playerStats[playerId] = CombatStats();
    
    std::cout << "Player " << name << " joined the game!" << std::endl;
    
    return playerPtr;
}

void CombatManager::removePlayer(int playerId) {
    auto it = std::find_if(players.begin(), players.end(),
        [playerId](const std::unique_ptr<Player>& p) {
            return p->getPlayerId() == playerId;
        });
    
    if (it != players.end()) {
        std::cout << "Player " << (*it)->getPlayerName() << " left the game!" << std::endl;
        players.erase(it);
    }
}

Player* CombatManager::getPlayer(int playerId) {
    for (auto& player : players) {
        if (player->getPlayerId() == playerId) {
            return player.get();
        }
    }
    return nullptr;
}

std::vector<Player*> CombatManager::getAllPlayers() {
    std::vector<Player*> result;
    for (auto& player : players) {
        result.push_back(player.get());
    }
    return result;
}

std::vector<Player*> CombatManager::getAlivePlayers() {
    std::vector<Player*> result;
    for (auto& player : players) {
        if (player->isAlive()) {
            result.push_back(player.get());
        }
    }
    return result;
}

void CombatManager::update(float deltaTime) {
    currentMatchTime += deltaTime;
    
    // Update all players
    for (auto& player : players) {
        player->update(deltaTime);
        
        // Check for respawn
        if (!player->isAlive() && player->getCombatState() == Player::CombatState::Dead) {
            // Simple auto-respawn after delay (could track individual timers)
            static float respawnTimer = 0;
            respawnTimer += deltaTime;
            if (respawnTimer >= respawnTime) {
                respawnPlayer(player.get());
                respawnTimer = 0;
            }
        }
    }
    
    updateCombat(deltaTime);
    updateProjectiles(deltaTime);
    updatePowerUps(deltaTime);
    checkCollisions();
    processEvents();
}

void CombatManager::updateCombat(float deltaTime) {
    // Handle ongoing combat mechanics
    for (auto& player : players) {
        if (!player->isAlive()) continue;
        
        // Auto-target for testing (remove in production)
        if (player->getCombatState() == Player::CombatState::Idle) {
            Player* nearestEnemy = findNearestEnemy(player.get(), 50.0f);
            if (nearestEnemy) {
                // Face the enemy
                Vector3 toEnemy = nearestEnemy->getPosition() - player->getPosition();
                player->setLookDirection(toEnemy);
            }
        }
    }
}

void CombatManager::updateProjectiles(float deltaTime) {
    // Projectiles are updated within each player
}

void CombatManager::updatePowerUps(float deltaTime) {
    for (auto& powerUp : powerUps) {
        if (!powerUp.active) {
            powerUp.respawnTimer -= deltaTime;
            if (powerUp.respawnTimer <= 0) {
                powerUp.active = true;
                std::cout << "Power-up respawned at position (" 
                         << powerUp.position.x << ", " << powerUp.position.z << ")" << std::endl;
            }
        }
    }
}

void CombatManager::checkCollisions() {
    checkProjectileCollisions();
    checkMeleeCollisions();
    checkPowerUpCollisions();
}

void CombatManager::checkProjectileCollisions() {
    // Check all projectiles against all players
    for (auto& attacker : players) {
        if (!attacker->isAlive()) continue;
        
        // Get projectiles from player (would need getter in Player class)
        // For now, this is a placeholder
    }
}

void CombatManager::checkMeleeCollisions() {
    // Check fist attacks
    for (auto& attacker : players) {
        if (!attacker->isAlive()) continue;
        if (attacker->getCombatState() != Player::CombatState::Attacking) continue;
        
        // Check if any enemy is in melee range
        for (auto& target : players) {
            if (target.get() == attacker.get()) continue;
            if (!target->isAlive()) continue;
            if (!friendlyFire && target->getPlayerId() % 2 == attacker->getPlayerId() % 2) continue;  // Simple team check
            
            float distance = calculateDistance(attacker->getPosition(), target->getPosition());
            if (distance <= 3.0f) {  // Fist range
                // Check if facing the target
                Vector3 toTarget = (target->getPosition() - attacker->getPosition()).normalized();
                float dot = attacker->getLookDirection().dot(toTarget);
                
                if (dot > 0.7f) {  // ~45 degree cone
                    handlePlayerDamage(target.get(), 15.0f, attacker.get());
                    recordProjectileHit(attacker->getPlayerId());
                }
            }
        }
    }
}

void CombatManager::checkPowerUpCollisions() {
    for (auto& player : players) {
        if (!player->isAlive()) continue;
        
        for (auto& powerUp : powerUps) {
            if (!powerUp.active) continue;
            
            float distance = calculateDistance(player->getPosition(), powerUp.position);
            if (distance <= 2.0f) {
                collectPowerUp(player.get(), powerUp);
            }
        }
    }
}

void CombatManager::processEvents() {
    // Process and clear event queue
    for (const auto& event : eventQueue) {
        switch (event.type) {
            case CombatEvent::PlayerKilled:
                std::cout << "Player " << event.targetId << " was killed by Player " 
                         << event.playerId << "!" << std::endl;
                break;
                
            case CombatEvent::LevelUp:
                std::cout << "Player " << event.playerId << " leveled up!" << std::endl;
                break;
                
            default:
                break;
        }
    }
    
    eventQueue.clear();
}

void CombatManager::handlePlayerAttack(Player* attacker, Player::AttackType type, const Vector3& direction) {
    if (!attacker) return;
    
    switch (type) {
        case Player::AttackType::Fist:
            handleFistAttack(attacker);
            break;
            
        case Player::AttackType::Laser:
            handleLaserAttack(attacker, direction);
            break;
            
        default:
            break;
    }
}

void CombatManager::handleFistAttack(Player* attacker) {
    attacker->performFistAttack();
    recordProjectileFired(attacker->getPlayerId());
}

void CombatManager::handleLaserAttack(Player* attacker, const Vector3& direction) {
    attacker->fireLaser(direction);
    recordProjectileFired(attacker->getPlayerId());
}

void CombatManager::handleAreaAttack(Player* attacker, const Vector3& center, float radius, float damage) {
    std::vector<Player*> targets = findPlayersInRadius(center, radius);
    
    for (auto& target : targets) {
        if (target == attacker) continue;
        if (!friendlyFire && target->getPlayerId() % 2 == attacker->getPlayerId() % 2) continue;
        
        handlePlayerDamage(target, damage, attacker);
    }
}

void CombatManager::handlePlayerDamage(Player* victim, float damage, Player* attacker) {
    if (!victim) return;
    
    victim->takeDamage(damage, attacker);
    
    if (attacker) {
        recordDamage(attacker->getPlayerId(), victim->getPlayerId(), damage);
    }
    
    CombatEvent event;
    event.type = CombatEvent::PlayerDamaged;
    event.playerId = attacker ? attacker->getPlayerId() : -1;
    event.targetId = victim->getPlayerId();
    event.value = damage;
    event.position = victim->getPosition();
    pushEvent(event);
}

void CombatManager::handlePlayerDeath(Player* victim, Player* killer) {
    if (!victim) return;
    
    victim->die();
    
    if (killer) {
        recordKill(killer->getPlayerId(), victim->getPlayerId());
    }
    
    CombatEvent event;
    event.type = CombatEvent::PlayerKilled;
    event.playerId = killer ? killer->getPlayerId() : -1;
    event.targetId = victim->getPlayerId();
    event.position = victim->getPosition();
    pushEvent(event);
}

void CombatManager::respawnPlayer(Player* player) {
    if (!player) return;
    
    Vector3 spawnPoint = getBestSpawnPoint(player);
    player->respawn(spawnPoint);
    
    CombatEvent event;
    event.type = CombatEvent::PlayerRespawned;
    event.playerId = player->getPlayerId();
    event.position = spawnPoint;
    pushEvent(event);
}

bool CombatManager::validateTeleportTarget(Player* player, const Vector3& target) {
    // Check if target is within bounds
    // Check for obstacles
    // For now, always allow
    return true;
}

void CombatManager::executeTeleport(Player* player, const Vector3& target) {
    if (!player) return;
    
    if (validateTeleportTarget(player, target)) {
        player->teleport(target);
    }
}

void CombatManager::collectPowerUp(Player* player, PowerUp& powerUp) {
    switch (powerUp.type) {
        case PowerUp::Health:
            player->heal(50.0f);
            std::cout << player->getPlayerName() << " collected health power-up!" << std::endl;
            break;
            
        case PowerUp::Shield:
            // Restore shield
            std::cout << player->getPlayerName() << " collected shield power-up!" << std::endl;
            break;
            
        case PowerUp::Damage:
            player->applyBuff("Damage Boost", 30.0f, 10.0f, "strength");
            std::cout << player->getPlayerName() << " collected damage power-up!" << std::endl;
            break;
            
        case PowerUp::Speed:
            player->applyBuff("Speed Boost", 30.0f, 10.0f, "agility");
            std::cout << player->getPlayerName() << " collected speed power-up!" << std::endl;
            break;
    }
    
    powerUp.active = false;
    powerUp.respawnTimer = 30.0f;  // 30 seconds respawn
}

void CombatManager::startMatch() {
    currentMatchTime = 0.0f;
    
    // Reset all player stats
    for (auto& [id, stats] : playerStats) {
        stats = CombatStats();
    }
    
    // Respawn all players
    for (auto& player : players) {
        respawnPlayer(player.get());
    }
    
    std::cout << "Match started!" << std::endl;
}

void CombatManager::endMatch() {
    // Display final scores
    Player* winner = getMatchLeader();
    if (winner) {
        std::cout << "Match ended! Winner: " << winner->getPlayerName() << std::endl;
    }
}

bool CombatManager::isMatchActive() const {
    return currentMatchTime < matchDuration;
}

Player* CombatManager::getMatchLeader() const {
    Player* leader = nullptr;
    int maxKills = -1;
    
    for (const auto& player : players) {
        int playerId = player->getPlayerId();
        auto it = playerStats.find(playerId);
        if (it != playerStats.end() && it->second.kills > maxKills) {
            maxKills = it->second.kills;
            leader = player.get();
        }
    }
    
    return leader;
}

CombatManager::CombatStats CombatManager::getPlayerStats(int playerId) const {
    auto it = playerStats.find(playerId);
    if (it != playerStats.end()) {
        return it->second;
    }
    return CombatStats();
}

void CombatManager::recordKill(int killerId, int victimId) {
    playerStats[killerId].kills++;
    playerStats[victimId].deaths++;
}

void CombatManager::recordDamage(int attackerId, int victimId, float damage) {
    playerStats[attackerId].damageDealt += damage;
    playerStats[victimId].damageTaken += damage;
}

void CombatManager::recordProjectileHit(int playerId) {
    playerStats[playerId].projectilesHit++;
}

void CombatManager::recordProjectileFired(int playerId) {
    playerStats[playerId].projectilesFired++;
}

void CombatManager::addSpawnPoint(const Vector3& point) {
    spawnPoints.push_back(point);
}

Vector3 CombatManager::getRandomSpawnPoint() {
    if (spawnPoints.empty()) {
        return Vector3::zero();
    }
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, spawnPoints.size() - 1);
    
    return spawnPoints[dis(gen)];
}

Vector3 CombatManager::getBestSpawnPoint(Player* player) {
    // Find spawn point furthest from enemies
    Vector3 bestPoint = getRandomSpawnPoint();
    float maxMinDistance = 0;
    
    for (const auto& spawn : spawnPoints) {
        float minDistance = std::numeric_limits<float>::max();
        
        for (const auto& other : players) {
            if (other.get() == player || !other->isAlive()) continue;
            
            float distance = calculateDistance(spawn, other->getPosition());
            minDistance = std::min(minDistance, distance);
        }
        
        if (minDistance > maxMinDistance) {
            maxMinDistance = minDistance;
            bestPoint = spawn;
        }
    }
    
    return bestPoint;
}

void CombatManager::pushEvent(const CombatEvent& event) {
    eventQueue.push_back(event);
}

std::vector<CombatManager::CombatEvent> CombatManager::getRecentEvents(float timeWindow) {
    // In a full implementation, would filter by timestamp
    return eventQueue;
}

float CombatManager::calculateDistance(const Vector3& a, const Vector3& b) {
    return (a - b).magnitude();
}

bool CombatManager::isInRange(const Vector3& pos1, const Vector3& pos2, float range) {
    return calculateDistance(pos1, pos2) <= range;
}

Player* CombatManager::findNearestEnemy(Player* player, float maxRange) {
    if (!player) return nullptr;
    
    Player* nearest = nullptr;
    float minDistance = maxRange > 0 ? maxRange : std::numeric_limits<float>::max();
    
    for (auto& other : players) {
        if (other.get() == player || !other->isAlive()) continue;
        if (!friendlyFire && other->getPlayerId() % 2 == player->getPlayerId() % 2) continue;
        
        float distance = calculateDistance(player->getPosition(), other->getPosition());
        if (distance < minDistance) {
            minDistance = distance;
            nearest = other.get();
        }
    }
    
    return nearest;
}

std::vector<Player*> CombatManager::findPlayersInRadius(const Vector3& center, float radius) {
    std::vector<Player*> result;
    
    for (auto& player : players) {
        if (!player->isAlive()) continue;
        
        float distance = calculateDistance(center, player->getPosition());
        if (distance <= radius) {
            result.push_back(player.get());
        }
    }
    
    return result;
}