#include "Player.h"
#include <algorithm>

Player::Player()
    : position(0.0f, 0.0f, 0.0f)
    , rotation(Quaternion::identity())
    , shieldActive(false)
    , laserCost(15.0f)
    , meleeCost(10.0f)
    , shieldCostPerSecond(20.0f)
    , teleportCost(40.0f) {
}

void Player::update(float dt) {
    // Update stats regen
    stats.update(dt);

    // Tick cooldowns, reduced by agility
    float cdMultiplier = std::max(0.5f, 1.0f - (stats.agility - 10.0f) * 0.03f); // down to 50%
    timers.laserCooldown = std::max(0.0f, timers.laserCooldown - dt * (1.0f / cdMultiplier));
    timers.meleeCooldown = std::max(0.0f, timers.meleeCooldown - dt * (1.0f / cdMultiplier));
    timers.shieldCooldown = std::max(0.0f, timers.shieldCooldown - dt * (1.0f / cdMultiplier));
    timers.teleportCooldown = std::max(0.0f, timers.teleportCooldown - dt * (1.0f / cdMultiplier));

    if (shieldActive) {
        // Drain stamina while shielding
        float drain = shieldCostPerSecond * dt;
        if (stats.canSpendStamina(drain)) {
            stats.spendStamina(drain);
            timers.shieldActiveTime += dt;
        } else {
            shieldActive = false;
            timers.shieldCooldown = 2.0f; // short cooldown after dropping
        }
    }
}

bool Player::spendStaminaWithAgility(float baseCost) {
    // Higher agility reduces stamina cost slightly
    float reduction = std::clamp((stats.agility - 10.0f) * 0.02f, -0.3f, 0.5f);
    float cost = baseCost * (1.0f - reduction);
    if (stats.canSpendStamina(cost)) { stats.spendStamina(cost); return true; }
    return false;
}

bool Player::tryFireLaser() {
    if (timers.laserCooldown > 0.0f) return false;
    if (!spendStaminaWithAgility(laserCost)) return false;
    timers.laserCooldown = 0.8f; // base cooldown
    return true;
}

bool Player::tryMeleePunch() {
    if (timers.meleeCooldown > 0.0f) return false;
    if (!spendStaminaWithAgility(meleeCost)) return false;
    timers.meleeCooldown = 0.6f;
    return true;
}

void Player::setShield(bool active) {
    if (active) {
        if (!shieldActive && timers.shieldCooldown <= 0.0f && stats.canSpendStamina(shieldCostPerSecond * 0.1f)) {
            shieldActive = true;
            timers.shieldActiveTime = 0.0f;
        }
    } else {
        if (shieldActive) {
            shieldActive = false;
            timers.shieldCooldown = 1.0f + std::min(2.0f, timers.shieldActiveTime * 0.5f);
        }
    }
}

bool Player::tryTeleport(const Vector3& target) {
    if (timers.teleportCooldown > 0.0f) return false;
    if (!spendStaminaWithAgility(teleportCost)) return false;
    position = target;
    timers.teleportCooldown = 3.0f;
    return true;
}
