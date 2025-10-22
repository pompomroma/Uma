#pragma once
#include "../Math/Vector3.h"

class Player;

class Shield {
public:
    enum class ShieldType {
        Basic,
        Energy,
        Reflective,
        Absorbing
    };

private:
    Player* owner;
    bool active;
    float health;
    float maxHealth;
    float regenRate;
    float damageAbsorption;  // Percentage of damage absorbed (0.0 - 1.0)
    float radius;
    ShieldType type;
    
    // Visual
    Vector3 color;
    float opacity;
    float pulseFrequency;
    float pulseTimer;
    
    // Cooldown
    float cooldownTime;
    float timeSinceDeactivated;
    bool onCooldown;

public:
    Shield(Player* playerOwner);
    ~Shield();
    
    void update(float deltaTime);
    void activate();
    void deactivate();
    
    // Damage handling
    float absorbDamage(float incomingDamage);
    void regenerate(float deltaTime);
    
    // Getters
    bool isActive() const { return active; }
    float getHealth() const { return health; }
    float getMaxHealth() const { return maxHealth; }
    float getHealthPercentage() const { return health / maxHealth; }
    float getRadius() const { return radius; }
    Vector3 getColor() const { return color; }
    float getOpacity() const { return opacity; }
    bool isOnCooldown() const { return onCooldown; }
    float getCooldownPercentage() const;
    ShieldType getType() const { return type; }
    
    // Setters
    void setType(ShieldType shieldType);
    void setMaxHealth(float maxHp);
    void setDamageAbsorption(float absorption);
    
private:
    void updateVisuals(float deltaTime);
    void applyTypeEffects();
};
