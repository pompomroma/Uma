#pragma once

#include "Stats.h"
#include "../Math/Vector3.h"
#include "../Math/Quaternion.h"
#include <vector>

class Player {
public:
    enum class AbilityState { Ready, Cooldown, Active };

    struct AbilityTimers {
        float laserCooldown;
        float meleeCooldown;
        float shieldCooldown;
        float teleportCooldown;

        float shieldActiveTime;

        AbilityTimers()
            : laserCooldown(0.0f)
            , meleeCooldown(0.0f)
            , shieldCooldown(0.0f)
            , teleportCooldown(0.0f)
            , shieldActiveTime(0.0f) {}
    };

    Player();

    // Position/orientation for PvP avatar (separate from car)
    Vector3 position;
    Quaternion rotation;

    // Combat state
    bool shieldActive;

    // Stats and progression
    Stats stats;

    // Timers and state
    AbilityTimers timers;

    // Config
    float laserCost;
    float meleeCost;
    float shieldCostPerSecond;
    float teleportCost;

    // Methods
    void update(float dt);

    bool tryFireLaser();
    bool tryMeleePunch();
    void setShield(bool active);
    bool tryTeleport(const Vector3& target);

private:
    bool spendStaminaWithAgility(float baseCost);
};
