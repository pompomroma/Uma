#include "Player.h"
#include "Projectile.h"
#include "Shield.h"
#include <cmath>
#include <algorithm>
#include <iostream>

Player::Player()
    : playerCar(nullptr)
    , currentState(PlayerState::Idle)
    , handPosition(0.0f, 0.0f, 0.0f)
    , aimDirection(0.0f, 0.0f, 1.0f)
    , attackCooldown(0.5f)
    , lastAttackTime(0.0f)
    , isAttacking(false)
    , currentAttackType(AttackType::None)
    , isShielding(false)
    , shieldCooldown(5.0f)
    , shieldDuration(0.0f)
    , shieldMaxDuration(3.0f)
    , teleportRange(20.0f)
    , teleportCooldown(3.0f)
    , lastTeleportTime(-10.0f)
    , maxTeleportCharges(3)
    , currentTeleportCharges(3)
    , teleportChargeRegenTime(5.0f)
    , timeSinceLastChargeRegen(0.0f)
    , targetReticlePosition(0.0f, 0.0f, 0.0f)
    , showTargetReticle(false)
    , healthRegenRate(5.0f)
    , staminaRegenRate(20.0f)
    , manaRegenRate(10.0f)
    , combatTimer(0.0f)
    , isInCombat(false) {
    initializeStats();
}

Player::Player(Car* car) : Player() {
    playerCar = car;
}

Player::~Player() {
}

void Player::initialize() {
    initializeStats();
    shield = std::make_unique<Shield>(this);
    recalculateCombatStats();
}

void Player::initializeStats() {
    // Base stats
    stats.strength = 10.0f;
    stats.defense = 10.0f;
    stats.stamina = 10.0f;
    stats.agility = 10.0f;
    
    // Health, stamina, mana
    stats.maxHealth = 100.0f + (stats.stamina * 5.0f);
    stats.currentHealth = stats.maxHealth;
    stats.maxStamina = 100.0f + (stats.stamina * 3.0f);
    stats.currentStamina = stats.maxStamina;
    stats.maxMana = 100.0f + (stats.agility * 2.0f);
    stats.currentMana = stats.maxMana;
    
    // Level and experience
    stats.level = 1;
    stats.experience = 0.0f;
    stats.experienceToNextLevel = 100.0f;
    stats.statPoints = 0;
    
    recalculateCombatStats();
}

void Player::recalculateCombatStats() {
    // Calculate combat stats based on base stats
    combatStats.attackDamage = 10.0f + (stats.strength * 2.0f);
    combatStats.attackSpeed = 1.0f + (stats.agility * 0.02f);
    combatStats.criticalChance = 0.05f + (stats.agility * 0.005f);
    combatStats.criticalDamage = 1.5f + (stats.strength * 0.01f);
    combatStats.damageReduction = stats.defense * 0.5f;
    combatStats.healingBonus = 1.0f + (stats.stamina * 0.01f);
    
    // Update max values based on stats
    stats.maxHealth = 100.0f + (stats.stamina * 5.0f);
    stats.maxStamina = 100.0f + (stats.stamina * 3.0f);
    stats.maxMana = 100.0f + (stats.agility * 2.0f);
    
    // Update cooldowns based on agility
    attackCooldown = std::max(0.2f, 0.5f - (stats.agility * 0.01f));
    teleportCooldown = std::max(1.0f, 3.0f - (stats.agility * 0.05f));
}

void Player::update(float deltaTime) {
    if (isDead()) return;
    
    updateHandPosition();
    updateStats(deltaTime);
    updateCombat(deltaTime);
    updateCooldowns(deltaTime);
    updateProjectiles(deltaTime);
    updateRegeneration(deltaTime);
    checkCombatStatus(deltaTime);
    
    if (shield) {
        shield->update(deltaTime);
    }
}

void Player::updateHandPosition() {
    if (!playerCar) return;
    
    // Position hand slightly in front and to the side of the car
    Vector3 carPos = playerCar->getPosition();
    Vector3 carForward = playerCar->getForward();
    Vector3 carRight = playerCar->getRight();
    
    handPosition = carPos + carForward * 2.0f + carRight * 1.0f + Vector3(0.0f, 1.0f, 0.0f);
}

void Player::updateStats(float deltaTime) {
    // Clamp all stats to valid ranges
    stats.currentHealth = std::clamp(stats.currentHealth, 0.0f, stats.maxHealth);
    stats.currentStamina = std::clamp(stats.currentStamina, 0.0f, stats.maxStamina);
    stats.currentMana = std::clamp(stats.currentMana, 0.0f, stats.maxMana);
}

void Player::updateCombat(float deltaTime) {
    lastAttackTime += deltaTime;
    
    if (currentState == PlayerState::Attacking) {
        if (lastAttackTime >= attackCooldown) {
            setState(PlayerState::Idle);
            isAttacking = false;
        }
    }
}

void Player::updateCooldowns(float deltaTime) {
    // Regenerate teleport charges
    if (currentTeleportCharges < maxTeleportCharges) {
        timeSinceLastChargeRegen += deltaTime;
        if (timeSinceLastChargeRegen >= teleportChargeRegenTime) {
            currentTeleportCharges++;
            timeSinceLastChargeRegen = 0.0f;
        }
    }
    
    // Update shield duration
    if (isShielding) {
        shieldDuration += deltaTime;
        if (shieldDuration >= shieldMaxDuration) {
            deactivateShield();
        }
    }
}

void Player::updateProjectiles(float deltaTime) {
    for (auto& projectile : activeProjectiles) {
        if (projectile) {
            projectile->update(deltaTime);
        }
    }
    removeDeadProjectiles();
}

void Player::updateRegeneration(float deltaTime) {
    if (isDead()) return;
    
    // Regenerate stamina (always)
    regenerateStamina(staminaRegenRate * deltaTime);
    
    // Regenerate mana (always)
    regenerateMana(manaRegenRate * deltaTime);
    
    // Regenerate health (only out of combat)
    if (!isInCombat) {
        heal(healthRegenRate * deltaTime);
    }
}

void Player::checkCombatStatus(float deltaTime) {
    if (isInCombat) {
        combatTimer += deltaTime;
        if (combatTimer >= 5.0f) {
            exitCombat();
        }
    }
}

void Player::enterCombat() {
    if (!isInCombat) {
        isInCombat = true;
        combatTimer = 0.0f;
    }
}

void Player::exitCombat() {
    isInCombat = false;
    combatTimer = 0.0f;
}

void Player::performLaserAttack(const Vector3& targetPosition) {
    if (!canAttack()) return;
    
    float manaCost = 10.0f;
    if (!consumeMana(manaCost)) return;
    
    setState(PlayerState::Attacking);
    currentAttackType = AttackType::Laser;
    lastAttackTime = 0.0f;
    isAttacking = true;
    
    // Calculate direction and fire projectile
    Vector3 direction = (targetPosition - handPosition).normalized();
    float damage = combatStats.attackDamage * 1.5f;
    fireProjectile(direction, damage, 50.0f);
    
    enterCombat();
}

void Player::performFistAttack() {
    if (!canAttack()) return;
    
    float staminaCost = 15.0f;
    if (!consumeStamina(staminaCost)) return;
    
    setState(PlayerState::Attacking);
    currentAttackType = AttackType::Fist;
    lastAttackTime = 0.0f;
    isAttacking = true;
    
    // Melee attack - check for enemies in range
    float meleeRange = 3.0f;
    float damage = combatStats.attackDamage * 2.0f; // Melee does more base damage
    
    // TODO: Check collision with enemies in range
    // For now, we just set the attack state
    
    enterCombat();
}

void Player::performUltimateAttack() {
    if (!canAttack()) return;
    
    float manaCost = 50.0f;
    float staminaCost = 30.0f;
    if (!consumeMana(manaCost) || !consumeStamina(staminaCost)) return;
    
    setState(PlayerState::Attacking);
    currentAttackType = AttackType::Ultimate;
    lastAttackTime = 0.0f;
    isAttacking = true;
    
    // Fire multiple projectiles in a spread
    float damage = combatStats.attackDamage * 3.0f;
    for (int i = -2; i <= 2; i++) {
        float angle = i * 0.2f;
        Vector3 direction = aimDirection;
        // Rotate direction by angle
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);
        Vector3 rotatedDir(
            direction.x * cosA - direction.z * sinA,
            direction.y,
            direction.x * sinA + direction.z * cosA
        );
        fireProjectile(rotatedDir.normalized(), damage, 60.0f);
    }
    
    enterCombat();
}

bool Player::canAttack() const {
    return !isDead() && 
           !isAttacking && 
           currentState != PlayerState::Stunned &&
           currentState != PlayerState::Teleporting &&
           lastAttackTime >= attackCooldown;
}

void Player::fireProjectile(const Vector3& direction, float damage, float speed) {
    auto projectile = std::make_unique<Projectile>(handPosition, direction, damage, speed, this);
    activeProjectiles.push_back(std::move(projectile));
}

void Player::activateShield() {
    if (isShielding) return;
    
    float manaCost = 20.0f;
    if (!consumeMana(manaCost)) return;
    
    isShielding = true;
    shieldDuration = 0.0f;
    
    if (shield) {
        shield->activate();
    }
}

void Player::deactivateShield() {
    isShielding = false;
    
    if (shield) {
        shield->deactivate();
    }
}

void Player::takeDamage(float damage, Player* attacker) {
    if (isDead()) return;
    
    // Check if shield is active
    if (isShielding && shield && shield->isActive()) {
        float absorbedDamage = shield->absorbDamage(damage);
        damage -= absorbedDamage;
        
        if (damage <= 0.0f) {
            return; // All damage absorbed
        }
    }
    
    // Apply damage reduction
    float actualDamage = calculateDamageReduction(damage);
    
    stats.currentHealth -= actualDamage;
    enterCombat();
    
    if (stats.currentHealth <= 0.0f) {
        stats.currentHealth = 0.0f;
        setState(PlayerState::Dead);
    }
}

void Player::heal(float amount) {
    if (isDead()) return;
    
    float healAmount = amount * combatStats.healingBonus;
    stats.currentHealth = std::min(stats.currentHealth + healAmount, stats.maxHealth);
}

void Player::teleportToPosition(const Vector3& targetPosition) {
    if (!canTeleport()) return;
    
    Vector3 currentPos = getPosition();
    float distance = (targetPosition - currentPos).length();
    
    if (distance > teleportRange) {
        // Teleport max range in that direction
        Vector3 direction = (targetPosition - currentPos).normalized();
        teleportInDirection(direction, teleportRange);
        return;
    }
    
    float staminaCost = 25.0f;
    if (!consumeStamina(staminaCost)) return;
    
    setState(PlayerState::Teleporting);
    
    if (playerCar) {
        playerCar->setPosition(targetPosition);
    }
    
    currentTeleportCharges--;
    lastTeleportTime = 0.0f;
    timeSinceLastChargeRegen = 0.0f;
    
    // Brief teleport state, then return to idle
    setState(PlayerState::Idle);
}

void Player::teleportInDirection(const Vector3& direction, float distance) {
    Vector3 targetPosition = getPosition() + direction.normalized() * distance;
    teleportToPosition(targetPosition);
}

bool Player::canTeleport() const {
    return !isDead() && 
           currentTeleportCharges > 0 &&
           currentState != PlayerState::Stunned &&
           currentState != PlayerState::Attacking;
}

bool Player::consumeStamina(float amount) {
    if (stats.currentStamina >= amount) {
        stats.currentStamina -= amount;
        return true;
    }
    return false;
}

bool Player::consumeMana(float amount) {
    if (stats.currentMana >= amount) {
        stats.currentMana -= amount;
        return true;
    }
    return false;
}

void Player::regenerateStamina(float amount) {
    stats.currentStamina = std::min(stats.currentStamina + amount, stats.maxStamina);
}

void Player::regenerateMana(float amount) {
    stats.currentMana = std::min(stats.currentMana + amount, stats.maxMana);
}

void Player::gainExperience(float exp) {
    stats.experience += exp;
    
    while (stats.experience >= stats.experienceToNextLevel) {
        levelUp();
    }
}

void Player::levelUp() {
    stats.level++;
    stats.experience -= stats.experienceToNextLevel;
    stats.experienceToNextLevel = stats.level * 100.0f;
    stats.statPoints += 5; // Gain 5 stat points per level
    
    // Heal to full on level up
    stats.currentHealth = stats.maxHealth;
    stats.currentStamina = stats.maxStamina;
    stats.currentMana = stats.maxMana;
}

void Player::spendStatPoint(const std::string& statName) {
    if (stats.statPoints <= 0) return;
    
    increaseStat(statName, 1.0f);
    stats.statPoints--;
    recalculateCombatStats();
}

void Player::setBaseStat(const std::string& statName, float value) {
    if (statName == "strength") stats.strength = value;
    else if (statName == "defense") stats.defense = value;
    else if (statName == "stamina") stats.stamina = value;
    else if (statName == "agility") stats.agility = value;
    
    recalculateCombatStats();
}

void Player::increaseStat(const std::string& statName, float amount) {
    if (statName == "strength") stats.strength += amount;
    else if (statName == "defense") stats.defense += amount;
    else if (statName == "stamina") stats.stamina += amount;
    else if (statName == "agility") stats.agility += amount;
    
    recalculateCombatStats();
}

void Player::setState(PlayerState state) {
    currentState = state;
}

void Player::respawn(const Vector3& position) {
    setState(PlayerState::Idle);
    stats.currentHealth = stats.maxHealth;
    stats.currentStamina = stats.maxStamina;
    stats.currentMana = stats.maxMana;
    
    if (playerCar) {
        playerCar->setPosition(position);
    }
    
    currentTeleportCharges = maxTeleportCharges;
    isShielding = false;
    isAttacking = false;
    exitCombat();
}

Vector3 Player::getPosition() const {
    if (playerCar) {
        return playerCar->getPosition();
    }
    return Vector3::zero();
}

Vector3 Player::getForward() const {
    if (playerCar) {
        return playerCar->getForward();
    }
    return Vector3::forward();
}

void Player::setAimDirection(const Vector3& direction) {
    aimDirection = direction.normalized();
}

void Player::removeDeadProjectiles() {
    activeProjectiles.erase(
        std::remove_if(activeProjectiles.begin(), activeProjectiles.end(),
            [](const std::unique_ptr<Projectile>& p) { return p->isDead(); }),
        activeProjectiles.end()
    );
}

void Player::updateTargetReticle(const Vector3& target) {
    targetReticlePosition = target;
    showTargetReticle = true;
}

float Player::getShieldCooldownPercentage() const {
    if (!isShielding) return 1.0f;
    return 1.0f - (shieldDuration / shieldMaxDuration);
}

float Player::getTeleportCooldownPercentage() const {
    if (currentTeleportCharges >= maxTeleportCharges) return 1.0f;
    return timeSinceLastChargeRegen / teleportChargeRegenTime;
}

float Player::getAttackCooldownPercentage() const {
    return std::min(1.0f, lastAttackTime / attackCooldown);
}

float Player::calculateDamageOutput(float baseDamage) const {
    float damage = baseDamage;
    
    // Apply critical hit
    float critRoll = static_cast<float>(rand()) / RAND_MAX;
    if (critRoll < combatStats.criticalChance) {
        damage *= combatStats.criticalDamage;
    }
    
    return damage;
}

float Player::calculateDamageReduction(float incomingDamage) const {
    // Damage reduction formula: actualDamage = incomingDamage * (100 / (100 + defense))
    float reduction = combatStats.damageReduction;
    float multiplier = 100.0f / (100.0f + reduction);
    return incomingDamage * multiplier;
}

void Player::debugDraw() const {
    // Debug rendering would go here
}
