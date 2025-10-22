#include "Shield.h"
#include "Player.h"
#include <algorithm>
#include <cmath>

Shield::Shield(Player* playerOwner)
    : owner(playerOwner)
    , active(false)
    , health(100.0f)
    , maxHealth(100.0f)
    , regenRate(10.0f)
    , damageAbsorption(0.8f)
    , radius(2.0f)
    , type(ShieldType::Basic)
    , color(0.0f, 0.5f, 1.0f)
    , opacity(0.5f)
    , pulseFrequency(2.0f)
    , pulseTimer(0.0f)
    , cooldownTime(3.0f)
    , timeSinceDeactivated(0.0f)
    , onCooldown(false) {
}

Shield::~Shield() {
}

void Shield::update(float deltaTime) {
    if (active) {
        regenerate(deltaTime);
        updateVisuals(deltaTime);
    } else if (onCooldown) {
        timeSinceDeactivated += deltaTime;
        if (timeSinceDeactivated >= cooldownTime) {
            onCooldown = false;
            timeSinceDeactivated = 0.0f;
        }
    }
}

void Shield::activate() {
    if (onCooldown) return;
    if (health <= 0.0f) return;
    
    active = true;
}

void Shield::deactivate() {
    active = false;
    onCooldown = true;
    timeSinceDeactivated = 0.0f;
}

float Shield::absorbDamage(float incomingDamage) {
    if (!active || health <= 0.0f) return 0.0f;
    
    float damageToAbsorb = incomingDamage * damageAbsorption;
    
    // Different shield types handle damage differently
    switch (type) {
        case ShieldType::Basic:
            // Basic shield just absorbs damage
            break;
            
        case ShieldType::Energy:
            // Energy shield absorbs more but drains faster
            damageToAbsorb *= 1.2f;
            break;
            
        case ShieldType::Reflective:
            // Reflective shield absorbs less but could reflect damage
            damageToAbsorb *= 0.8f;
            // TODO: Implement reflection mechanic
            break;
            
        case ShieldType::Absorbing:
            // Absorbing shield converts damage to shield health
            float converted = damageToAbsorb * 0.1f;
            health = std::min(health + converted, maxHealth);
            break;
    }
    
    // Reduce shield health
    health -= damageToAbsorb;
    
    if (health <= 0.0f) {
        health = 0.0f;
        deactivate();
    }
    
    return damageToAbsorb;
}

void Shield::regenerate(float deltaTime) {
    if (!active) {
        // Regenerate faster when shield is down
        health = std::min(health + regenRate * 2.0f * deltaTime, maxHealth);
    } else {
        // Slow regeneration when active
        health = std::min(health + regenRate * 0.5f * deltaTime, maxHealth);
    }
}

void Shield::updateVisuals(float deltaTime) {
    pulseTimer += deltaTime;
    
    // Create pulsing effect
    float pulse = 0.5f + 0.5f * std::sin(pulseTimer * pulseFrequency);
    opacity = 0.3f + 0.3f * pulse;
    
    // Adjust opacity based on health
    opacity *= (health / maxHealth);
}

void Shield::setType(ShieldType shieldType) {
    type = shieldType;
    applyTypeEffects();
}

void Shield::applyTypeEffects() {
    switch (type) {
        case ShieldType::Basic:
            maxHealth = 100.0f;
            damageAbsorption = 0.8f;
            color = Vector3(0.0f, 0.5f, 1.0f);
            regenRate = 10.0f;
            break;
            
        case ShieldType::Energy:
            maxHealth = 150.0f;
            damageAbsorption = 0.9f;
            color = Vector3(0.0f, 1.0f, 1.0f);
            regenRate = 15.0f;
            break;
            
        case ShieldType::Reflective:
            maxHealth = 80.0f;
            damageAbsorption = 0.6f;
            color = Vector3(1.0f, 1.0f, 0.0f);
            regenRate = 8.0f;
            break;
            
        case ShieldType::Absorbing:
            maxHealth = 120.0f;
            damageAbsorption = 0.75f;
            color = Vector3(0.5f, 0.0f, 1.0f);
            regenRate = 12.0f;
            break;
    }
    
    health = maxHealth;
}

void Shield::setMaxHealth(float maxHp) {
    maxHealth = std::max(1.0f, maxHp);
    health = std::min(health, maxHealth);
}

void Shield::setDamageAbsorption(float absorption) {
    damageAbsorption = std::clamp(absorption, 0.0f, 1.0f);
}

float Shield::getCooldownPercentage() const {
    if (!onCooldown) return 1.0f;
    return timeSinceDeactivated / cooldownTime;
}
