#include "CombatSystem.h"
#include "../Physics/Car.h"
#include "../Game.h"
#include <algorithm>
#include <cmath>
#include <random>

CombatSystem::CombatSystem(Game* game)
    : game(game)
    , respawnTime(5.0f)
    , friendlyFire(false)
    , maxCombatRange(200.0f)
    , maxProjectiles(50)
    , matchActive(false)
    , matchTime(0.0f)
    , maxMatchTime(300.0f) // 5 minutes
    , scoreLimit(10)
    , teamMode(false) {
}

CombatSystem::~CombatSystem() {
    shutdown();
}

void CombatSystem::initialize() {
    reset();
}

void CombatSystem::shutdown() {
    players.clear();
    projectiles.clear();
    activeAoEEffects.clear();
    recentDamageEvents.clear();
    playerMap.clear();
}

void CombatSystem::update(float deltaTime) {
    updatePlayers(deltaTime);
    updateProjectiles(deltaTime);
    updateAoEEffects(deltaTime);
    updateRespawns(deltaTime);
    updateMatch(deltaTime);
    
    checkProjectileCollisions();
    checkAoEDamage();
    cleanupDestroyedProjectiles();
    limitProjectileCount();
}

void CombatSystem::reset() {
    matchActive = false;
    matchTime = 0.0f;
    
    for (auto& player : players) {
        player->kills = 0;
        player->deaths = 0;
        player->totalDamageDealt = 0.0f;
        player->totalDamageTaken = 0.0f;
        player->isAlive = true;
        player->respawnTimer = 0.0f;
        if (player->stats) {
            player->stats->respawn();
        }
        if (player->abilities) {
            player->abilities->reset();
        }
    }
    
    projectiles.clear();
    activeAoEEffects.clear();
    recentDamageEvents.clear();
}

CombatSystem::CombatPlayer* CombatSystem::addPlayer(Car* car) {
    if (!car) return nullptr;
    
    // Check if player already exists
    for (auto& player : players) {
        if (player->car == car) {
            return player.get();
        }
    }
    
    int playerId = getNextPlayerId();
    auto player = std::make_unique<CombatPlayer>(car, playerId);
    CombatPlayer* playerPtr = player.get();
    
    setupPlayerCallbacks(playerPtr);
    
    players.push_back(std::move(player));
    playerMap[playerId] = playerPtr;
    
    return playerPtr;
}

void CombatSystem::removePlayer(Car* car) {
    if (!car) return;
    
    for (auto it = players.begin(); it != players.end(); ++it) {
        if ((*it)->car == car) {
            playerMap.erase((*it)->playerId);
            players.erase(it);
            break;
        }
    }
    
    // Remove any projectiles owned by this player
    for (auto it = projectiles.begin(); it != projectiles.end();) {
        if ((*it)->getOwner() == car) {
            it = projectiles.erase(it);
        } else {
            ++it;
        }
    }
}

CombatSystem::CombatPlayer* CombatSystem::getPlayer(Car* car) {
    if (!car) return nullptr;
    
    for (auto& player : players) {
        if (player->car == car) {
            return player.get();
        }
    }
    return nullptr;
}

CombatSystem::CombatPlayer* CombatSystem::getPlayer(int playerId) {
    auto it = playerMap.find(playerId);
    return (it != playerMap.end()) ? it->second : nullptr;
}

void CombatSystem::fireProjectile(Car* owner, Projectile::ProjectileType type, const Vector3& direction) {
    if (!owner) return;
    
    CombatPlayer* player = getPlayer(owner);
    if (!player || !player->isAlive) return;
    
    // Check if we're at the projectile limit
    if (static_cast<int>(projectiles.size()) >= maxProjectiles) {
        // Remove oldest projectile
        if (!projectiles.empty()) {
            projectiles.erase(projectiles.begin());
        }
    }
    
    Vector3 spawnPos = owner->getPosition() + owner->getForward() * 2.0f + Vector3::up() * 1.0f;
    
    std::unique_ptr<Projectile> projectile;
    float damage = 25.0f; // Base damage, will be modified by stats
    
    switch (type) {
        case Projectile::ProjectileType::Laser:
            projectile = std::unique_ptr<Projectile>(Projectile::createLaser(owner, spawnPos, direction, damage));
            break;
        case Projectile::ProjectileType::Plasma:
            projectile = std::unique_ptr<Projectile>(Projectile::createPlasma(owner, spawnPos, direction, damage));
            break;
        case Projectile::ProjectileType::Missile:
            projectile = std::unique_ptr<Projectile>(Projectile::createMissile(owner, spawnPos, direction, nullptr, damage));
            break;
        case Projectile::ProjectileType::EnergyBall:
            projectile = std::unique_ptr<Projectile>(Projectile::createEnergyBall(owner, spawnPos, direction, damage));
            break;
        case Projectile::ProjectileType::Fist:
            projectile = std::unique_ptr<Projectile>(Projectile::createFist(owner, spawnPos, direction, damage));
            break;
    }
    
    if (projectile) {
        // Apply stat modifications to damage
        if (player->stats) {
            float modifiedDamage = player->stats->calculateAttackDamage(damage);
            projectile->setDamage(modifiedDamage);
        }
        
        // Set up projectile callbacks
        projectile->setHitTargetCallback([this](Projectile* proj, Car* target) {
            if (proj && target) {
                dealDamage(proj->getOwner(), target, proj->getDamage(), proj->getType(), proj->getPosition());
            }
        });
        
        projectile->setHitGroundCallback([this](Projectile* proj, const Vector3& pos) {
            if (proj && proj->getExplosionRadius() > 0.0f) {
                createExplosion(pos, proj->getExplosionRadius(), proj->getDamage(), proj->getOwner(), proj->getType());
            }
        });
        
        if (onProjectileCreated) {
            onProjectileCreated(projectile.get());
        }
        
        projectiles.push_back(std::move(projectile));
    }
}

void CombatSystem::dealDamage(Car* attacker, Car* victim, float damage, Projectile::ProjectileType weaponType, const Vector3& hitPos) {
    if (!victim) return;
    
    CombatPlayer* victimPlayer = getPlayer(victim);
    CombatPlayer* attackerPlayer = getPlayer(attacker);
    
    if (!victimPlayer || !victimPlayer->isAlive) return;
    
    // Check friendly fire
    if (!friendlyFire && attacker && victim != attacker) {
        // In team mode, check if they're on the same team (simplified - no team system implemented yet)
        if (teamMode) {
            // TODO: Implement team checking
        }
    }
    
    // Apply dodge chance
    if (victimPlayer->stats && victimPlayer->stats->rollDodgeChance()) {
        return; // Attack dodged
    }
    
    // Deal damage to player stats
    if (victimPlayer->stats) {
        victimPlayer->stats->takeDamage(damage);
        victimPlayer->totalDamageTaken += damage;
    }
    
    // Update attacker stats
    if (attackerPlayer) {
        attackerPlayer->totalDamageDealt += damage;
    }
    
    // Record damage event
    recentDamageEvents.emplace_back(attacker, victim, damage, weaponType, hitPos, matchTime);
    
    // Trigger callback
    if (onPlayerDamaged) {
        onPlayerDamaged(attacker, victim, damage);
    }
    
    // Check if player died
    if (victimPlayer->stats && !victimPlayer->stats->getIsAlive()) {
        killPlayer(victim, attacker);
    }
}

void CombatSystem::killPlayer(Car* victim, Car* attacker) {
    if (!victim) return;
    
    CombatPlayer* victimPlayer = getPlayer(victim);
    if (!victimPlayer || !victimPlayer->isAlive) return;
    
    victimPlayer->isAlive = false;
    victimPlayer->deaths++;
    victimPlayer->respawnTimer = respawnTime;
    
    if (attacker) {
        CombatPlayer* attackerPlayer = getPlayer(attacker);
        if (attackerPlayer) {
            attackerPlayer->kills++;
            
            // Award experience
            if (attackerPlayer->stats) {
                attackerPlayer->stats->gainExperience(100.0f);
            }
        }
    }
    
    // Trigger callback
    if (onPlayerKilled) {
        onPlayerKilled(attacker, victim);
    }
    
    // Check match end conditions
    checkMatchEndConditions();
}

void CombatSystem::respawnPlayer(Car* player) {
    if (!player) return;
    
    CombatPlayer* combatPlayer = getPlayer(player);
    if (!combatPlayer) return;
    
    combatPlayer->isAlive = true;
    combatPlayer->respawnTimer = 0.0f;
    
    if (combatPlayer->stats) {
        combatPlayer->stats->respawn();
    }
    
    if (combatPlayer->abilities) {
        combatPlayer->abilities->reset();
    }
    
    // Find safe respawn position
    Vector3 respawnPos = findSafeRespawnPosition(player);
    player->setPosition(respawnPos);
    player->setVelocity(Vector3::zero());
    
    // Trigger callback
    if (onPlayerRespawned) {
        onPlayerRespawned(player);
    }
}

void CombatSystem::updateProjectiles(float deltaTime) {
    for (auto& projectile : projectiles) {
        if (projectile) {
            projectile->update(deltaTime);
        }
    }
}

void CombatSystem::checkProjectileCollisions() {
    for (auto& projectile : projectiles) {
        if (!projectile || !projectile->isActive()) continue;
        
        // Check collision with cars
        for (auto& player : players) {
            if (!player->isAlive || player->car == projectile->getOwner()) continue;
            
            if (checkProjectileCarCollision(projectile.get(), player->car)) {
                projectile->handleCollision(player->car);
                break;
            }
        }
        
        // Check collision with world
        if (projectile->isActive() && checkProjectileWorldCollision(projectile.get())) {
            // Collision handled by projectile
        }
    }
}

bool CombatSystem::checkProjectileCarCollision(Projectile* projectile, Car* car) {
    if (!projectile || !car) return false;
    
    Vector3 projPos = projectile->getPosition();
    Vector3 carPos = car->getPosition();
    
    float distance = (projPos - carPos).length();
    float collisionRadius = projectile->getCollisionRadius() + 2.0f; // Car radius approximation
    
    return distance <= collisionRadius;
}

bool CombatSystem::checkProjectileWorldCollision(Projectile* projectile) {
    if (!projectile) return false;
    
    return projectile->checkCollisionWithGround();
}

void CombatSystem::createExplosion(const Vector3& position, float radius, float damage, Car* owner, Projectile::ProjectileType type) {
    // Get all cars in explosion radius
    std::vector<Car*> carsInRange = getCarsInRadius(position, radius, owner);
    
    for (Car* car : carsInRange) {
        float distance = (car->getPosition() - position).length();
        float damageMultiplier = 1.0f - (distance / radius); // Damage falls off with distance
        float actualDamage = damage * std::max(0.1f, damageMultiplier);
        
        dealDamage(owner, car, actualDamage, type, position);
    }
    
    // Trigger explosion callback
    if (onExplosion) {
        onExplosion(position, radius, type);
    }
}

void CombatSystem::updateAoEEffects(float deltaTime) {
    for (auto it = activeAoEEffects.begin(); it != activeAoEEffects.end();) {
        it->timeRemaining -= deltaTime;
        
        if (it->timeRemaining <= 0.0f) {
            it = activeAoEEffects.erase(it);
        } else {
            ++it;
        }
    }
}

void CombatSystem::addAoEEffect(const Vector3& position, float radius, float damage, float duration, Car* owner, Projectile::ProjectileType type) {
    activeAoEEffects.emplace_back(position, radius, damage, duration, owner, type);
}

void CombatSystem::checkAoEDamage() {
    for (const auto& effect : activeAoEEffects) {
        std::vector<Car*> carsInRange = getCarsInRadius(effect.position, effect.radius, effect.owner);
        
        for (Car* car : carsInRange) {
            // Apply damage over time (reduced per frame)
            float frameTime = 1.0f / 60.0f; // Assume 60 FPS
            float frameDamage = effect.damage * frameTime / effect.duration;
            
            dealDamage(effect.owner, car, frameDamage, effect.type, effect.position);
        }
    }
}

std::vector<Car*> CombatSystem::getCarsInRadius(const Vector3& center, float radius, Car* exclude) {
    std::vector<Car*> result;
    
    for (auto& player : players) {
        if (!player->isAlive || player->car == exclude) continue;
        
        float distance = (player->car->getPosition() - center).length();
        if (distance <= radius) {
            result.push_back(player->car);
        }
    }
    
    return result;
}

void CombatSystem::updatePlayers(float deltaTime) {
    for (auto& player : players) {
        if (player->stats) {
            player->stats->update(deltaTime);
        }
        if (player->abilities) {
            player->abilities->update(deltaTime);
        }
    }
}

void CombatSystem::updateRespawns(float deltaTime) {
    for (auto& player : players) {
        if (!player->isAlive && player->respawnTimer > 0.0f) {
            player->respawnTimer -= deltaTime;
            if (player->respawnTimer <= 0.0f) {
                respawnPlayer(player->car);
            }
        }
    }
}

void CombatSystem::updateMatch(float deltaTime) {
    if (!matchActive) return;
    
    matchTime += deltaTime;
    checkMatchEndConditions();
}

void CombatSystem::checkMatchEndConditions() {
    if (!matchActive) return;
    
    // Check time limit
    if (matchTime >= maxMatchTime) {
        endMatch();
        return;
    }
    
    // Check score limit
    for (auto& player : players) {
        if (player->kills >= scoreLimit) {
            endMatch();
            return;
        }
    }
}

void CombatSystem::cleanupDestroyedProjectiles() {
    projectiles.erase(
        std::remove_if(projectiles.begin(), projectiles.end(),
            [this](const std::unique_ptr<Projectile>& proj) {
                if (proj && proj->isDestroyed()) {
                    if (onProjectileDestroyed) {
                        onProjectileDestroyed(proj.get());
                    }
                    return true;
                }
                return false;
            }),
        projectiles.end()
    );
}

void CombatSystem::limitProjectileCount() {
    while (static_cast<int>(projectiles.size()) > maxProjectiles) {
        if (!projectiles.empty()) {
            if (onProjectileDestroyed) {
                onProjectileDestroyed(projectiles.front().get());
            }
            projectiles.erase(projectiles.begin());
        }
    }
}

void CombatSystem::startMatch() {
    matchActive = true;
    matchTime = 0.0f;
    reset();
}

void CombatSystem::endMatch() {
    matchActive = false;
    
    if (onMatchEnd) {
        onMatchEnd();
    }
}

Car* CombatSystem::getLeadingPlayer() {
    Car* leader = nullptr;
    int maxKills = -1;
    
    for (auto& player : players) {
        if (player->kills > maxKills) {
            maxKills = player->kills;
            leader = player->car;
        }
    }
    
    return leader;
}

Vector3 CombatSystem::findSafeRespawnPosition(Car* player) {
    // Simple implementation - find a position away from other players
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(-50.0f, 50.0f);
    
    for (int attempts = 0; attempts < 10; attempts++) {
        Vector3 candidate(dis(gen), 2.0f, dis(gen));
        
        if (isPositionSafe(candidate)) {
            return candidate;
        }
    }
    
    // Fallback to origin if no safe position found
    return Vector3(0.0f, 2.0f, 0.0f);
}

bool CombatSystem::isPositionSafe(const Vector3& position, float safeRadius) {
    for (auto& player : players) {
        if (!player->isAlive) continue;
        
        float distance = (player->car->getPosition() - position).length();
        if (distance < safeRadius) {
            return false;
        }
    }
    return true;
}

void CombatSystem::setupPlayerCallbacks(CombatPlayer* player) {
    if (!player || !player->abilities) return;
    
    player->abilities->setProjectileCreatedCallback([this](Projectile* proj) {
        if (proj && onProjectileCreated) {
            onProjectileCreated(proj);
        }
    });
}

int CombatSystem::getNextPlayerId() {
    static int nextId = 1;
    return nextId++;
}

// Ability usage methods
bool CombatSystem::usePlayerAbility(Car* player, PlayerAbilities::AbilityType ability) {
    CombatPlayer* combatPlayer = getPlayer(player);
    if (!combatPlayer || !combatPlayer->abilities) return false;
    
    return combatPlayer->abilities->useAbility(ability);
}

bool CombatSystem::usePlayerAbilityWithTarget(Car* player, PlayerAbilities::AbilityType ability, const Vector3& target) {
    CombatPlayer* combatPlayer = getPlayer(player);
    if (!combatPlayer || !combatPlayer->abilities) return false;
    
    if (ability == PlayerAbilities::AbilityType::Teleport) {
        return combatPlayer->abilities->useTeleport(target);
    }
    
    return false;
}

bool CombatSystem::canPlayerUseAbility(Car* player, PlayerAbilities::AbilityType ability) {
    CombatPlayer* combatPlayer = getPlayer(player);
    if (!combatPlayer || !combatPlayer->abilities) return false;
    
    return combatPlayer->abilities->canUseAbility(ability);
}

// Getter methods for player stats
bool CombatSystem::isPlayerAlive(Car* player) {
    CombatPlayer* combatPlayer = getPlayer(player);
    return combatPlayer ? combatPlayer->isAlive : false;
}

float CombatSystem::getPlayerHealth(Car* player) {
    CombatPlayer* combatPlayer = getPlayer(player);
    return (combatPlayer && combatPlayer->stats) ? combatPlayer->stats->getHealth() : 0.0f;
}

float CombatSystem::getPlayerEnergy(Car* player) {
    CombatPlayer* combatPlayer = getPlayer(player);
    return (combatPlayer && combatPlayer->stats) ? combatPlayer->stats->getEnergy() : 0.0f;
}

PlayerStats* CombatSystem::getPlayerStats(Car* player) {
    CombatPlayer* combatPlayer = getPlayer(player);
    return combatPlayer ? combatPlayer->stats.get() : nullptr;
}

PlayerAbilities* CombatSystem::getPlayerAbilities(Car* player) {
    CombatPlayer* combatPlayer = getPlayer(player);
    return combatPlayer ? combatPlayer->abilities.get() : nullptr;
}