#include "Shield.h"
#include "Player.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>

Shield::Shield(Player* own, float maxStr)
    : owner(own)
    , strength(maxStr)
    , maxStrength(maxStr)
    , rechargeRate(5.0f)
    , rechargeDelay(2.0f)
    , timeSinceLastHit(0.0f)
    , state(ShieldState::Inactive)
    , type(ShieldType::Standard)
    , isActive(false)
    , radius(2.0f)
    , color(Vector3(0.3f, 0.6f, 1.0f))  // Blue shield
    , opacity(0.0f)
    , pulseTimer(0.0f)
    , flickerTimer(0.0f)
    , canReflect(false)
    , reflectChance(0.0f)
    , canAbsorb(false)
    , absorptionRate(0.0f) {
}

Shield::~Shield() {
}

void Shield::update(float deltaTime) {
    if (!owner) return;
    
    updateRecharge(deltaTime);
    updateVisuals(deltaTime);
    
    timeSinceLastHit += deltaTime;
}

void Shield::updateRecharge(float deltaTime) {
    if (state == ShieldState::Broken) {
        // Wait for recharge delay after breaking
        if (timeSinceLastHit >= rechargeDelay * 2) {
            state = ShieldState::Recharging;
        }
    } else if (state == ShieldState::Recharging || 
              (state == ShieldState::Damaged && timeSinceLastHit >= rechargeDelay)) {
        // Recharge shield
        strength = std::min(maxStrength, strength + rechargeRate * deltaTime);
        
        if (strength >= maxStrength) {
            strength = maxStrength;
            if (isActive) {
                state = ShieldState::Active;
            } else {
                state = ShieldState::Inactive;
            }
        }
    }
}

void Shield::updateVisuals(float deltaTime) {
    if (!isActive) {
        opacity = std::max(0.0f, opacity - deltaTime * 3.0f);
        return;
    }
    
    // Target opacity based on shield strength
    float targetOpacity = 0.3f + (strength / maxStrength) * 0.4f;
    
    if (state == ShieldState::Damaged) {
        // Flicker when damaged
        flickerTimer += deltaTime * 20.0f;
        targetOpacity *= 0.5f + std::sin(flickerTimer) * 0.5f;
    }
    
    // Smooth opacity transition
    opacity = opacity + (targetOpacity - opacity) * deltaTime * 5.0f;
    
    // Pulse effect
    pulseTimer += deltaTime;
    radius = 2.0f + std::sin(pulseTimer * 2.0f) * 0.1f;
    
    // Color based on strength
    if (strength < maxStrength * 0.3f) {
        color = Vector3(1.0f, 0.3f, 0.3f);  // Red when low
    } else if (strength < maxStrength * 0.6f) {
        color = Vector3(1.0f, 1.0f, 0.3f);  // Yellow when medium
    } else {
        color = Vector3(0.3f, 0.6f, 1.0f);  // Blue when high
    }
}

void Shield::activate() {
    if (state == ShieldState::Broken) return;
    
    isActive = true;
    if (strength > 0) {
        state = ShieldState::Active;
    } else {
        state = ShieldState::Damaged;
    }
}

void Shield::deactivate() {
    isActive = false;
    state = ShieldState::Inactive;
}

float Shield::absorbDamage(float incomingDamage) {
    if (!isActive || state == ShieldState::Broken) {
        return incomingDamage;
    }
    
    float absorbed = std::min(strength, incomingDamage);
    takeDamage(absorbed);
    
    // Handle reflection
    if (canReflect && (rand() / (float)RAND_MAX) < reflectChance) {
        // Reflect damage back (game logic handles this)
        return -absorbed * 0.5f;  // Negative indicates reflection
    }
    
    // Handle absorption
    if (canAbsorb) {
        // Convert some damage to player health or stamina
        if (owner) {
            owner->heal(absorbed * absorptionRate);
        }
    }
    
    return incomingDamage - absorbed;
}

void Shield::takeDamage(float damage) {
    strength -= damage;
    timeSinceLastHit = 0.0f;
    
    if (strength <= 0) {
        strength = 0;
        breakShield();
    } else if (strength < maxStrength * 0.3f) {
        state = ShieldState::Damaged;
    }
}

void Shield::repair(float amount) {
    strength = std::min(maxStrength, strength + amount);
    
    if (strength > 0 && state == ShieldState::Broken) {
        state = ShieldState::Recharging;
    }
}

void Shield::overcharge(float amount) {
    strength = maxStrength + amount;
    // Temporary overcharge that will decay
}

void Shield::breakShield() {
    state = ShieldState::Broken;
    strength = 0;
    deactivate();
    timeSinceLastHit = 0.0f;
}

void Shield::setType(ShieldType t) {
    type = t;
    
    switch (type) {
        case ShieldType::Heavy:
            maxStrength *= 1.5f;
            rechargeRate *= 0.7f;
            break;
            
        case ShieldType::Energy:
            maxStrength *= 0.8f;
            rechargeRate *= 1.5f;
            rechargeDelay *= 0.5f;
            break;
            
        case ShieldType::Reflective:
            setReflective(0.3f);
            break;
            
        default:
            break;
    }
}

void Shield::setMaxStrength(float str) {
    maxStrength = str;
    strength = std::min(strength, maxStrength);
}

void Shield::setReflective(float chance) {
    canReflect = true;
    reflectChance = std::clamp(chance, 0.0f, 1.0f);
}

void Shield::setAbsorption(float rate) {
    canAbsorb = true;
    absorptionRate = std::clamp(rate, 0.0f, 1.0f);
}

Matrix4 Shield::getTransformMatrix() const {
    if (!owner) return Matrix4::identity();
    
    return Matrix4::translation(owner->getPosition()) * 
           Matrix4::scale(Vector3(radius, radius, radius));
}