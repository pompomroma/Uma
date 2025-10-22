#include "CombatSystem.h"
#include "../Math/Vector3.h"
#include <algorithm>
#include <cmath>

CombatSystem::CombatSystem() {}

Combatant* CombatSystem::registerCarCombatant(Car* car) {
    if (!car) return nullptr;
    // Avoid duplicates
    for (auto& c : combatants) {
        if (c->getCar() == car) return c.get();
    }
    auto ptr = std::make_unique<Combatant>(car);
    Combatant* raw = ptr.get();
    combatants.push_back(std::move(ptr));
    return raw;
}

void CombatSystem::unregisterCarCombatant(Car* car) {
    combatants.erase(std::remove_if(combatants.begin(), combatants.end(), [car](const std::unique_ptr<Combatant>& c){
        return c->getCar() == car;
    }), combatants.end());
}

Combatant* CombatSystem::getCombatant(Car* car) {
    for (auto& c : combatants) {
        if (c->getCar() == car) return c.get();
    }
    return nullptr;
}

static float clampDot(float d) { return std::max(-1.0f, std::min(1.0f, d)); }

Combatant* CombatSystem::findClosestOpponent(Combatant* attacker, float maxDistance, float maxAngleDeg) {
    if (!attacker) return nullptr;
    Car* aCar = attacker->getCar();
    Vector3 aPos = aCar->getPosition();
    Vector3 aFwd = aCar->getForward();

    Combatant* best = nullptr;
    float bestDist = maxDistance;
    float cosMaxAngle = std::cos(maxAngleDeg * M_PI / 180.0f);

    for (auto& candidate : combatants) {
        if (candidate.get() == attacker) continue;
        if (!candidate->isAlive()) continue;
        Car* bCar = candidate->getCar();
        float dist = (bCar->getPosition() - aPos).length();
        if (dist > bestDist) continue;
        // Angle check
        Vector3 dir = (bCar->getPosition() - aPos).normalized();
        float dot = aFwd.dot(dir);
        if (dot < cosMaxAngle) continue;
        best = candidate.get();
        bestDist = dist;
    }
    return best;
}

void CombatSystem::applyDamageToTarget(Combatant* attacker, Combatant* target, float damage) {
    if (!attacker || !target) return;
    Car* aCar = attacker->getCar();
    Car* tCar = target->getCar();
    Vector3 dir = (tCar->getPosition() - aCar->getPosition()).normalized();
    target->applyDamage(damage, dir);
}

bool CombatSystem::fireLaser(Combatant* attacker) {
    if (!attacker || !attacker->isAlive()) return false;
    if (!attacker->canLaser()) return false;

    Car* car = attacker->getCar();
    Vector3 start = car->getPosition() + car->getUp() * 1.2f + car->getRight() * 0.6f; // roughly "hand"
    Vector3 dir = car->getForward();

    // Find target in a forward cone
    Combatant* target = findClosestOpponent(attacker, 30.0f, 25.0f);

    // Stamina cost
    const float staminaCost = 8.0f;
    if (!attacker->tryConsumeStamina(staminaCost)) return false;

    // Register visual laser
    LaserShot shot;
    shot.start = start;
    shot.end = start + dir * 25.0f;
    shot.lifetime = 0.15f;
    shot.color = Vector3(1.0f, 0.2f, 0.2f);
    lasers.push_back(shot);

    // Apply damage
    float dmg = attacker->getStats().laserBaseDamage;
    if (target) {
        applyDamageToTarget(attacker, target, dmg);
    }

    attacker->triggerLaserCooldown();
    return true;
}

bool CombatSystem::punch(Combatant* attacker) {
    if (!attacker || !attacker->isAlive()) return false;
    if (!attacker->canPunch()) return false;

    // Short-range melee in a very close cone
    Combatant* target = findClosestOpponent(attacker, 4.0f, 35.0f);

    // Stamina cost
    const float staminaCost = 6.0f;
    if (!attacker->tryConsumeStamina(staminaCost)) return false;

    if (target) {
        float dmg = attacker->getStats().punchBaseDamage;
        applyDamageToTarget(attacker, target, dmg);
    }

    attacker->triggerPunchCooldown();
    return true;
}

bool CombatSystem::teleportForward(Combatant* actor) {
    if (!actor || !actor->isAlive()) return false;
    if (!actor->canTeleport()) return false;

    // Stamina cost
    const float staminaCost = 15.0f;
    if (!actor->tryConsumeStamina(staminaCost)) return false;

    Car* car = actor->getCar();
    Vector3 pos = car->getPosition();
    Vector3 fwd = car->getForward();
    float dist = actor->getStats().teleportDistance;

    car->setPosition(pos + fwd * dist);

    actor->triggerTeleportCooldown();
    return true;
}

void CombatSystem::setShield(Combatant* actor, bool active) {
    if (!actor || !actor->isAlive()) return;
    actor->setShieldActive(active);
}

void CombatSystem::update(float deltaTime) {
    // Update combatants
    for (auto& c : combatants) {
        c->update(deltaTime);
    }

    // Tick lasers for brief debug render lifetime
    for (auto& l : lasers) {
        l.lifetime -= deltaTime;
    }
    lasers.erase(std::remove_if(lasers.begin(), lasers.end(), [](const LaserShot& l){ return l.lifetime <= 0.0f; }), lasers.end());
}
