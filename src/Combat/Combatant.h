#pragma once
#include "../Math/Vector3.h"
#include "../Physics/Car.h"
#include <string>
#include <algorithm>

struct CombatStats {
    int strength;
    int defense;
    int stamina;
    int agility;

    int availablePoints;

    float maxHealth;
    float currentHealth;

    float maxStamina;
    float currentStamina;
    float staminaRegenPerSecond;

    // Derived combat numbers
    float laserBaseDamage;
    float punchBaseDamage;
    float shieldDamageReduction; // 0..1 fraction
    float teleportDistance;
    float cooldownReductionFactor; // multiplier <= 1

    CombatStats()
        : strength(1), defense(1), stamina(1), agility(1),
          availablePoints(0),
          maxHealth(100.0f), currentHealth(100.0f),
          maxStamina(50.0f), currentStamina(50.0f), staminaRegenPerSecond(5.0f),
          laserBaseDamage(10.0f), punchBaseDamage(15.0f),
          shieldDamageReduction(0.2f), teleportDistance(8.0f),
          cooldownReductionFactor(1.0f) {}

    void recalcDerived() {
        // Health grows with defense
        maxHealth = 100.0f + static_cast<float>(defense) * 20.0f;
        if (currentHealth > maxHealth) currentHealth = maxHealth;

        // Stamina pool and regen grow with stamina stat
        maxStamina = 50.0f + static_cast<float>(stamina) * 15.0f;
        if (currentStamina > maxStamina) currentStamina = maxStamina;
        staminaRegenPerSecond = 5.0f + static_cast<float>(stamina) * 1.5f;

        // Damage scales with strength
        laserBaseDamage = 10.0f + static_cast<float>(strength) * 4.0f;
        punchBaseDamage = 15.0f + static_cast<float>(strength) * 6.0f;

        // Shield reduction scales with defense
        shieldDamageReduction = std::min(0.6f, 0.15f + static_cast<float>(defense) * 0.04f);

        // Teleport distance and cooldown reduction scale with agility
        teleportDistance = 8.0f + static_cast<float>(agility) * 1.0f;
        cooldownReductionFactor = std::max(0.4f, 1.0f - static_cast<float>(agility) * 0.05f);
    }
};

class Combatant {
public:
    explicit Combatant(Car* attachedCar);

    // Accessors
    Car* getCar() const { return car; }
    CombatStats& getStats() { return stats; }
    const CombatStats& getStats() const { return stats; }

    // State queries
    bool isAlive() const { return stats.currentHealth > 0.0f; }
    bool isShieldActive() const { return shieldActive; }

    // Updates
    void update(float deltaTime);

    // Damage handling
    // Returns actual damage applied after mitigation
    float applyDamage(float rawDamage, const Vector3& hitDirection);

    // Shield control
    void setShieldActive(bool active);

    // Stamina spend/regen
    bool tryConsumeStamina(float amount);

    // Cooldowns
    bool canLaser() const { return laserCooldownTimer <= 0.0f; }
    bool canPunch() const { return punchCooldownTimer <= 0.0f; }
    bool canTeleport() const { return teleportCooldownTimer <= 0.0f; }

    void triggerLaserCooldown();
    void triggerPunchCooldown();
    void triggerTeleportCooldown();

private:
    Car* car;
    CombatStats stats;

    bool shieldActive;
    float shieldStaminaDrainPerSecond;

    float laserCooldownTimer;
    float punchCooldownTimer;
    float teleportCooldownTimer;
};
