#include "CombatSystem.h"
#include <algorithm>

CombatSystem::CombatSystem() {
}

void CombatSystem::update(float dt) {
    for (auto& l : lasers) {
        l.lifetime -= dt;
    }
    lasers.erase(std::remove_if(lasers.begin(), lasers.end(), [](const LaserShot& l){ return l.lifetime <= 0.0f; }), lasers.end());
}

void CombatSystem::applyDamage(Player& defender, float rawDamage) {
    float damage = rawDamage;
    if (defender.shieldActive) {
        damage *= 0.2f; // 80% reduction while shielding
    }
    float applied = defender.stats.applyDamage(damage);
    // Award a bit of experience to defender for surviving hits
    defender.stats.addExperience(applied * 0.1f);
}

void CombatSystem::fireLaser(Player& attacker, Player& defender) {
    if (!attacker.tryFireLaser()) return;

    // Laser direction is attacker's forward
    Vector3 dir = attacker.rotation * Vector3::forward();
    Vector3 start = attacker.position + dir * 0.5f;
    Vector3 end = start + dir * 20.0f;

    // Simple hit check: distance from defender position to laser line
    Vector3 toDef = defender.position - start;
    // Project onto dir
    float t = std::max(0.0f, std::min(1.0f, toDef.dot(dir.normalized()) / 20.0f));
    Vector3 closest = start + dir.normalized() * (20.0f * t);
    float dist = (defender.position - closest).length();

    float baseDamage = 15.0f * attacker.stats.outgoingDamageMultiplier();
    if (dist < 1.5f) {
        applyDamage(defender, baseDamage);
    }

    lasers.push_back({start, end, 0.15f});
}

void CombatSystem::meleePunch(Player& attacker, Player& defender) {
    if (!attacker.tryMeleePunch()) return;

    Vector3 dir = attacker.rotation * Vector3::forward();
    Vector3 fist = attacker.position + dir * 1.0f;
    float dist = (defender.position - fist).length();

    if (dist < 2.0f) {
        float baseDamage = 25.0f * attacker.stats.outgoingDamageMultiplier();
        applyDamage(defender, baseDamage);
    }
}

void CombatSystem::setShield(Player& player, bool active) {
    player.setShield(active);
}

void CombatSystem::teleport(Player& player, const Vector3& target) {
    player.tryTeleport(target);
}
