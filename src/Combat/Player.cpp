#include "Player.h"
#include "Projectile.h"
#include "Shield.h"
#include <algorithm>
#include <iostream>
#include <cmath>

// Stats implementation
Player::Stats::Stats() 
    : strength(10.0f)
    , defense(10.0f)
    , stamina(10.0f)
    , agility(10.0f)
    , maxHealth(100.0f)
    , currentHealth(100.0f)
    , maxStamina(100.0f)
    , currentStamina(100.0f)
    , healthRegen(1.0f)
    , staminaRegen(5.0f)
    , level(1)
    , experience(0)
    , experienceToNextLevel(100)
    , statPoints(0) {
    recalculateDerivedStats();
}

void Player::Stats::recalculateDerivedStats() {
    maxHealth = 100.0f + (defense * 10.0f);
    maxStamina = 100.0f + (stamina * 5.0f);
    healthRegen = 1.0f + (defense * 0.2f);
    staminaRegen = 5.0f + (stamina * 0.5f) + (agility * 0.3f);
}

void Player::Stats::levelUp() {
    level++;
    statPoints += 3;  // Grant 3 stat points per level
    experienceToNextLevel = level * 100;  // Simple scaling
    
    // Full heal on level up
    currentHealth = maxHealth;
    currentStamina = maxStamina;
}

void Player::Stats::applyStatPoint(const std::string& statName) {
    if (statPoints <= 0) return;
    
    if (statName == "strength") {
        strength += 1.0f;
        statPoints--;
    } else if (statName == "defense") {
        defense += 1.0f;
        statPoints--;
    } else if (statName == "stamina") {
        stamina += 1.0f;
        statPoints--;
    } else if (statName == "agility") {
        agility += 1.0f;
        statPoints--;
    }
    
    recalculateDerivedStats();
}

// Player implementation
Player::Player(int id, const std::string& name, const Vector3& startPos)
    : playerId(id)
    , playerName(name)
    , isLocalPlayer(false)
    , position(startPos)
    , velocity(Vector3::zero())
    , rotation(Quaternion::identity())
    , lookDirection(Vector3::forward())
    , moveDirection(Vector3::zero())
    , moveSpeed(10.0f)
    , rotationSpeed(5.0f)
    , currentState(CombatState::Idle)
    , currentAttack(AttackType::None)
    , stateTimer(0.0f)
    , lastAttackTime(0.0f)
    , lastDamageTime(0.0f)
    , teleportCooldown(0.0f)
    , teleportRange(20.0f)
    , isTeleporting(false)
    , teleportProgress(0.0f)
    , fistDamage(15.0f)
    , fistRange(3.0f)
    , fistCooldown(0.5f)
    , laserDamage(25.0f)
    , laserRange(50.0f)
    , laserSpeed(30.0f)
    , laserCooldown(1.0f)
    , shieldStrength(50.0f)
    , shieldMaxStrength(50.0f)
    , shieldRegenRate(5.0f)
    , shieldCooldown(0.0f)
    , isShielding(false)
    , color(Vector3(0.2f, 0.5f, 1.0f))
    , damageFlashTimer(0.0f)
    , healFlashTimer(0.0f)
    , comboTimer(0.0f)
    , comboMultiplier(1) {
    
    // Create shield component
    shield = std::make_unique<Shield>(this, shieldMaxStrength);
}

Player::~Player() {
    activeProjectiles.clear();
}

void Player::update(float deltaTime) {
    updateCombat(deltaTime);
    // Movement is now updated externally with camera info
    updateStats(deltaTime);
    updateProjectiles(deltaTime);
    updateBuffs(deltaTime);
    updateVisualEffects(deltaTime);
    updateCooldowns(deltaTime);
    checkComboTimeout(deltaTime);
}

void Player::updateCombat(float deltaTime) {
    // Update state timer
    if (stateTimer > 0) {
        stateTimer -= deltaTime;
        
        // Check if state should change
        switch (currentState) {
            case CombatState::Attacking:
                if (stateTimer <= 0) {
                    currentState = CombatState::Idle;
                    currentAttack = AttackType::None;
                }
                break;
                
            case CombatState::Teleporting:
                if (teleportProgress >= 1.0f) {
                    position = teleportTarget;
                    currentState = CombatState::Idle;
                    isTeleporting = false;
                    teleportProgress = 0.0f;
                }
                break;
                
            case CombatState::Stunned:
                if (stateTimer <= 0) {
                    currentState = CombatState::Idle;
                }
                break;
                
            default:
                break;
        }
    }
    
    // Update shield
    if (shield) {
        shield->update(deltaTime);
        
        if (isShielding) {
            // Consume stamina while shielding
            consumeStamina(10.0f * deltaTime);
            if (!hasStamina(1.0f)) {
                deactivateShield();
            }
        }
    }
}

void Player::updateMovement(float deltaTime, const Vector3& inputDirection, const Vector3& cameraForward, const Vector3& cameraRight) {
    if (currentState == CombatState::Teleporting) {
        // Smooth teleport animation
        teleportProgress += deltaTime * 3.0f;  // Teleport takes ~0.33 seconds
        teleportProgress = std::min(1.0f, teleportProgress);
        
        // Interpolate position
        Vector3 startPos = position;
        position = startPos + (teleportTarget - startPos) * teleportProgress;
    } else if (currentState != CombatState::Stunned && currentState != CombatState::Dead) {
        // Calculate movement direction relative to camera
        Vector3 forward = cameraForward;
        forward.y = 0;  // Keep movement on horizontal plane
        forward = forward.normalized();
        
        Vector3 right = cameraRight;
        right.y = 0;
        right = right.normalized();
        
        // Apply input relative to camera
        moveDirection = forward * inputDirection.y + right * inputDirection.x;
        
        if (moveDirection.length() > 0.01f) {
            moveDirection = moveDirection.normalized();
            
            // Smooth rotation toward movement direction
            lookDirection = lookDirection + (moveDirection - lookDirection) * rotationSpeed * deltaTime;
            lookDirection = lookDirection.normalized();
            
            // Update rotation quaternion to face movement direction
            float angle = std::atan2(lookDirection.x, lookDirection.z);
            rotation = Quaternion::fromAxisAngle(Vector3::up(), angle);
        }
        
        // Apply movement with speed modifier based on agility
        float speedModifier = 1.0f + (stats.agility * 0.02f);  // 2% speed per agility point
        velocity = moveDirection * moveSpeed * speedModifier;
        position = position + velocity * deltaTime;
    }
}

void Player::updateStats(float deltaTime) {
    // Health regeneration
    if (stats.currentHealth < stats.maxHealth && currentState != CombatState::Dead) {
        stats.currentHealth = std::min(stats.maxHealth, 
            stats.currentHealth + stats.healthRegen * deltaTime);
    }
    
    // Stamina regeneration
    if (stats.currentStamina < stats.maxStamina) {
        float regenMultiplier = isShielding ? 0.2f : 1.0f;  // Slower regen while shielding
        stats.currentStamina = std::min(stats.maxStamina,
            stats.currentStamina + stats.staminaRegen * deltaTime * regenMultiplier);
    }
}

void Player::updateProjectiles(float deltaTime) {
    // Update all active projectiles
    for (auto it = activeProjectiles.begin(); it != activeProjectiles.end();) {
        (*it)->update(deltaTime);
        
        if (!(*it)->isActive()) {
            it = activeProjectiles.erase(it);
        } else {
            ++it;
        }
    }
}

void Player::updateBuffs(float deltaTime) {
    for (auto it = activeBuffs.begin(); it != activeBuffs.end();) {
        it->duration -= deltaTime;
        
        if (it->duration <= 0) {
            // Remove expired buff
            it = activeBuffs.erase(it);
            stats.recalculateDerivedStats();
        } else {
            ++it;
        }
    }
}

void Player::updateVisualEffects(float deltaTime) {
    if (damageFlashTimer > 0) {
        damageFlashTimer -= deltaTime;
        // Flash red when taking damage
        color = Vector3(1.0f, 0.2f, 0.2f) * (damageFlashTimer / 0.3f) + 
                Vector3(0.2f, 0.5f, 1.0f) * (1.0f - damageFlashTimer / 0.3f);
    }
    
    if (healFlashTimer > 0) {
        healFlashTimer -= deltaTime;
        // Flash green when healing
        color = Vector3(0.2f, 1.0f, 0.2f) * (healFlashTimer / 0.3f) + 
                Vector3(0.2f, 0.5f, 1.0f) * (1.0f - healFlashTimer / 0.3f);
    }
}

void Player::performFistAttack() {
    if (!canAttack() || !hasStamina(5.0f)) return;
    
    currentState = CombatState::Attacking;
    currentAttack = AttackType::Fist;
    stateTimer = 0.3f;  // Attack animation duration
    lastAttackTime = 0.0f;
    
    consumeStamina(5.0f);
    
    // Add to combo
    comboSequence.push_back(AttackType::Fist);
    comboTimer = 1.5f;  // Time window for next combo input
    
    // Deal damage in front of player (implement collision detection in game logic)
    float damage = calculateDamage(fistDamage);
    
    std::cout << playerName << " performs fist attack! Damage: " << damage << std::endl;
}

void Player::fireLaser(const Vector3& direction) {
    if (!canAttack() || !hasStamina(10.0f)) return;
    
    currentState = CombatState::Attacking;
    currentAttack = AttackType::Laser;
    stateTimer = 0.5f;
    lastAttackTime = 0.0f;
    
    consumeStamina(10.0f);
    
    // Add to combo
    comboSequence.push_back(AttackType::Laser);
    comboTimer = 1.5f;
    
    // Create laser projectile
    auto projectile = std::make_unique<Projectile>(
        position + Vector3(0, 1.5f, 0),  // Spawn at hand height
        direction.normalized() * laserSpeed,
        calculateDamage(laserDamage),
        laserRange,
        this
    );
    activeProjectiles.push_back(std::move(projectile));
    
    std::cout << playerName << " fires laser!" << std::endl;
}

void Player::activateShield() {
    if (isShielding || shieldCooldown > 0 || !hasStamina(20.0f)) return;
    
    isShielding = true;
    currentState = CombatState::Defending;
    
    if (shield) {
        shield->activate();
    }
    
    consumeStamina(20.0f);
    
    std::cout << playerName << " activates shield!" << std::endl;
}

void Player::deactivateShield() {
    if (!isShielding) return;
    
    isShielding = false;
    currentState = CombatState::Idle;
    shieldCooldown = 3.0f;  // 3 second cooldown
    
    if (shield) {
        shield->deactivate();
    }
    
    std::cout << playerName << " deactivates shield!" << std::endl;
}

void Player::teleport(const Vector3& target) {
    if (!canTeleport() || !hasStamina(30.0f)) return;
    
    Vector3 direction = target - position;
    float distance = direction.magnitude();
    
    if (distance > teleportRange) {
        // Limit to max range
        direction = direction.normalized() * teleportRange;
    }
    
    teleportTarget = position + direction;
    currentState = CombatState::Teleporting;
    isTeleporting = true;
    teleportProgress = 0.0f;
    teleportCooldown = 5.0f;  // 5 second cooldown
    
    consumeStamina(30.0f);
    
    std::cout << playerName << " teleports!" << std::endl;
}

void Player::performCombo() {
    if (comboSequence.size() >= 3) {
        // Check for specific combo patterns
        if (comboSequence[0] == AttackType::Fist &&
            comboSequence[1] == AttackType::Fist &&
            comboSequence[2] == AttackType::Laser) {
            // Fist-Fist-Laser combo: Area blast
            comboMultiplier = 2;
            std::cout << playerName << " performs POWER COMBO!" << std::endl;
            
            // Create area effect damage
            float damage = calculateDamage(laserDamage * 2);
            // Game logic will handle area damage
        }
        
        comboSequence.clear();
        comboTimer = 0;
    }
}

void Player::takeDamage(float damage, Player* attacker) {
    if (currentState == CombatState::Dead) return;
    
    float finalDamage = damage;
    
    // Apply shield first
    if (isShielding && shield) {
        finalDamage = shield->absorbDamage(damage);
        
        if (shield->getStrength() <= 0) {
            deactivateShield();
        }
    }
    
    // Apply defense stat
    finalDamage = calculateDefense(finalDamage);
    
    stats.currentHealth -= finalDamage;
    damageFlashTimer = 0.3f;
    lastDamageTime = 0.0f;
    
    std::cout << playerName << " takes " << finalDamage << " damage! Health: " 
              << stats.currentHealth << "/" << stats.maxHealth << std::endl;
    
    if (stats.currentHealth <= 0) {
        die();
        
        // Attacker gains experience
        if (attacker && attacker != this) {
            attacker->addExperience(50 + stats.level * 10);
        }
    }
}

void Player::heal(float amount) {
    stats.currentHealth = std::min(stats.maxHealth, stats.currentHealth + amount);
    healFlashTimer = 0.3f;
    
    std::cout << playerName << " heals for " << amount << "! Health: "
              << stats.currentHealth << "/" << stats.maxHealth << std::endl;
}

void Player::die() {
    currentState = CombatState::Dead;
    stats.currentHealth = 0;
    velocity = Vector3::zero();
    activeProjectiles.clear();
    
    std::cout << playerName << " has died!" << std::endl;
}

void Player::respawn(const Vector3& respawnPoint) {
    position = respawnPoint;
    velocity = Vector3::zero();
    stats.currentHealth = stats.maxHealth;
    stats.currentStamina = stats.maxStamina;
    currentState = CombatState::Idle;
    activeProjectiles.clear();
    clearBuffs();
    
    // Reset cooldowns
    teleportCooldown = 0;
    shieldCooldown = 0;
    lastAttackTime = 0;
    
    std::cout << playerName << " respawns!" << std::endl;
}

void Player::addExperience(int amount) {
    stats.experience += amount;
    
    std::cout << playerName << " gains " << amount << " experience!" << std::endl;
    
    while (stats.experience >= stats.experienceToNextLevel) {
        stats.experience -= stats.experienceToNextLevel;
        levelUp();
    }
}

void Player::levelUp() {
    stats.levelUp();
    
    std::cout << playerName << " levels up to level " << stats.level << "!" << std::endl;
    std::cout << "Stat points available: " << stats.statPoints << std::endl;
}

void Player::increaseStrength(float amount) {
    stats.strength += amount;
    stats.recalculateDerivedStats();
}

void Player::increaseDefense(float amount) {
    stats.defense += amount;
    stats.recalculateDerivedStats();
}

void Player::increaseStamina(float amount) {
    stats.stamina += amount;
    stats.recalculateDerivedStats();
}

void Player::increaseAgility(float amount) {
    stats.agility += amount;
    stats.recalculateDerivedStats();
}

void Player::resetStats() {
    stats = Stats();
}

void Player::applyBuff(const std::string& buffName, float duration, float modifier, const std::string& stat) {
    Buff buff = { buffName, duration, modifier, stat };
    activeBuffs.push_back(buff);
    
    // Apply buff effect
    if (stat == "strength") {
        stats.strength += modifier;
    } else if (stat == "defense") {
        stats.defense += modifier;
    } else if (stat == "stamina") {
        stats.stamina += modifier;
    } else if (stat == "agility") {
        stats.agility += modifier;
    }
    
    stats.recalculateDerivedStats();
}

void Player::removeBuff(const std::string& buffName) {
    for (auto it = activeBuffs.begin(); it != activeBuffs.end();) {
        if (it->name == buffName) {
            // Remove buff effect
            if (it->affectedStat == "strength") {
                stats.strength -= it->statModifier;
            } else if (it->affectedStat == "defense") {
                stats.defense -= it->statModifier;
            } else if (it->affectedStat == "stamina") {
                stats.stamina -= it->statModifier;
            } else if (it->affectedStat == "agility") {
                stats.agility -= it->statModifier;
            }
            
            it = activeBuffs.erase(it);
        } else {
            ++it;
        }
    }
    
    stats.recalculateDerivedStats();
}

void Player::clearBuffs() {
    for (const auto& buff : activeBuffs) {
        if (buff.affectedStat == "strength") {
            stats.strength -= buff.statModifier;
        } else if (buff.affectedStat == "defense") {
            stats.defense -= buff.statModifier;
        } else if (buff.affectedStat == "stamina") {
            stats.stamina -= buff.statModifier;
        } else if (buff.affectedStat == "agility") {
            stats.agility -= buff.statModifier;
        }
    }
    
    activeBuffs.clear();
    stats.recalculateDerivedStats();
}

bool Player::canAttack() const {
    return currentState == CombatState::Idle && 
           lastAttackTime <= 0 &&
           stats.currentHealth > 0;
}

bool Player::canTeleport() const {
    return currentState == CombatState::Idle &&
           teleportCooldown <= 0 &&
           stats.currentHealth > 0;
}

void Player::setPosition(const Vector3& pos) {
    position = pos;
}

void Player::setVelocity(const Vector3& vel) {
    velocity = vel;
}

void Player::setRotation(const Quaternion& rot) {
    rotation = rot;
    // Update look direction from rotation
    lookDirection = rotation * Vector3::forward();
}

void Player::setLookDirection(const Vector3& dir) {
    lookDirection = dir.normalized();
}

bool Player::checkHit(const Vector3& point, float radius) const {
    // Simple sphere collision
    return (point - position).magnitude() <= radius + 1.0f;  // Player radius ~1.0
}

Vector3 Player::getBoundingBoxMin() const {
    return position - Vector3(0.5f, 0, 0.5f);
}

Vector3 Player::getBoundingBoxMax() const {
    return position + Vector3(0.5f, 2.0f, 0.5f);  // Player is ~2 units tall
}

Matrix4 Player::getTransformMatrix() const {
    Matrix4 transform;
    transform = Matrix4::translation(position) * rotation.toMatrix4();
    return transform;
}

void Player::updateCooldowns(float deltaTime) {
    if (lastAttackTime > 0) {
        lastAttackTime -= deltaTime;
    }
    
    if (teleportCooldown > 0) {
        teleportCooldown -= deltaTime;
    }
    
    if (shieldCooldown > 0) {
        shieldCooldown -= deltaTime;
    }
    
    if (lastDamageTime < 5.0f) {
        lastDamageTime += deltaTime;
    }
}

void Player::checkComboTimeout(float deltaTime) {
    if (comboTimer > 0) {
        comboTimer -= deltaTime;
        
        if (comboTimer <= 0) {
            // Combo timed out
            comboSequence.clear();
            comboMultiplier = 1;
        }
    }
}

float Player::calculateDamage(float baseDamage) const {
    // Apply strength modifier and combo multiplier
    float strengthBonus = 1.0f + (stats.strength * 0.05f);  // 5% per strength point
    return baseDamage * strengthBonus * comboMultiplier;
}

float Player::calculateDefense(float incomingDamage) const {
    // Defense reduces damage by percentage
    float defenseReduction = stats.defense / (stats.defense + 50.0f);  // Diminishing returns
    return incomingDamage * (1.0f - defenseReduction * 0.5f);  // Max 50% reduction
}

void Player::consumeStamina(float amount) {
    stats.currentStamina = std::max(0.0f, stats.currentStamina - amount);
}

bool Player::hasStamina(float amount) const {
    return stats.currentStamina >= amount;
}

void Player::debugPrint() const {
    std::cout << "=== Player: " << playerName << " ===" << std::endl;
    std::cout << "Level: " << stats.level << " | Exp: " << stats.experience 
              << "/" << stats.experienceToNextLevel << std::endl;
    std::cout << "Health: " << stats.currentHealth << "/" << stats.maxHealth << std::endl;
    std::cout << "Stamina: " << stats.currentStamina << "/" << stats.maxStamina << std::endl;
    std::cout << "Stats - STR: " << stats.strength << " | DEF: " << stats.defense 
              << " | STA: " << stats.stamina << " | AGI: " << stats.agility << std::endl;
    std::cout << "State: " << static_cast<int>(currentState) << std::endl;
    std::cout << "Position: (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
}