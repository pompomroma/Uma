#include "Combatant.h"
#include <algorithm>

Combatant::Combatant(Car* attachedCar)
    : car(attachedCar)
    , shieldActive(false)
    , shieldStaminaDrainPerSecond(12.0f)
    , laserCooldownTimer(0.0f)
    , punchCooldownTimer(0.0f)
    , teleportCooldownTimer(0.0f) {
    // Initialize stats from defaults and scale a bit
    stats.recalcDerived();
}

void Combatant::update(float deltaTime) {
    // Tick cooldowns
    if (laserCooldownTimer > 0.0f) laserCooldownTimer -= deltaTime;
    if (punchCooldownTimer > 0.0f) punchCooldownTimer -= deltaTime;
    if (teleportCooldownTimer > 0.0f) teleportCooldownTimer -= deltaTime;

    // Stamina regeneration
    stats.currentStamina = std::min(stats.maxStamina, stats.currentStamina + stats.staminaRegenPerSecond * deltaTime);

    // Shield drains stamina if active; toggle off if we run out
    if (shieldActive) {
        if (!tryConsumeStamina(shieldStaminaDrainPerSecond * deltaTime)) {
            shieldActive = false;
        }
    }
}

float Combatant::applyDamage(float rawDamage, const Vector3& hitDirection) {
    float mitigated = rawDamage;
    // Flat mitigation via defense
    float flatReduction = static_cast<float>(stats.defense) * 1.5f;
    mitigated = std::max(0.0f, mitigated - flatReduction);

    // Shield reduction (multiplicative)
    if (shieldActive) {
        mitigated *= (1.0f - stats.shieldDamageReduction);
    }

    stats.currentHealth = std::max(0.0f, stats.currentHealth - mitigated);
    return mitigated;
}

void Combatant::setShieldActive(bool active) {
    shieldActive = active && stats.currentStamina > 0.0f;
}

bool Combatant::tryConsumeStamina(float amount) {
    if (stats.currentStamina >= amount) {
        stats.currentStamina -= amount;
        return true;
    }
    return false;
}

void Combatant::triggerLaserCooldown() {
    // Base 0.6s; reduced by agility-derived CDR
    float base = 0.6f;
    laserCooldownTimer = base * stats.cooldownReductionFactor;
}

void Combatant::triggerPunchCooldown() {
    // Base 0.8s
    float base = 0.8f;
    punchCooldownTimer = base * stats.cooldownReductionFactor;
}

void Combatant::triggerTeleportCooldown() {
    // Base 5s
    float base = 5.0f;
    teleportCooldownTimer = base * stats.cooldownReductionFactor;
}
