#pragma once
#include "Combatant.h"
#include "../Math/Vector3.h"
#include <vector>
#include <memory>

struct LaserShot {
    Vector3 start;
    Vector3 end;
    float lifetime; // seconds remaining for debug render
    Vector3 color;
};

class CombatSystem {
public:
    CombatSystem();

    Combatant* registerCarCombatant(Car* car);
    void unregisterCarCombatant(Car* car);

    // High-level actions
    // Returns true if action fired
    bool fireLaser(Combatant* attacker);
    bool punch(Combatant* attacker);
    bool teleportForward(Combatant* actor);
    void setShield(Combatant* actor, bool active);

    // Tick/update
    void update(float deltaTime);

    // Queries
    Combatant* getCombatant(Car* car);
    const std::vector<std::unique_ptr<Combatant>>& getCombatants() const { return combatants; }
    const std::vector<LaserShot>& getActiveLasers() const { return lasers; }

private:
    std::vector<std::unique_ptr<Combatant>> combatants;
    std::vector<LaserShot> lasers;

    // Helpers
    Combatant* findClosestOpponent(Combatant* attacker, float maxDistance, float maxAngleDeg);
    void applyDamageToTarget(Combatant* attacker, Combatant* target, float damage);
};
