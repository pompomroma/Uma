#pragma once

#include "Player.h"
#include "../Math/Vector3.h"
#include <vector>

struct LaserShot {
    Vector3 start;
    Vector3 end;
    float lifetime; // seconds remaining
};

class CombatSystem {
public:
    CombatSystem();

    // Update all transient effects
    void update(float dt);

    // Resolve actions between two players (simple PvP for now)
    void fireLaser(Player& attacker, Player& defender);
    void meleePunch(Player& attacker, Player& defender);
    void setShield(Player& player, bool active);
    void teleport(Player& player, const Vector3& target);

    const std::vector<LaserShot>& getActiveLasers() const { return lasers; }

private:
    std::vector<LaserShot> lasers;

    void applyDamage(Player& defender, float rawDamage);
};
