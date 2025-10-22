#pragma once
#include "../Math/Vector3.h"
#include "../Math/Matrix4.h"

// Forward declaration
class Player;

class Shield {
public:
    enum class ShieldType {
        Standard,
        Heavy,
        Energy,
        Reflective
    };
    
    enum class ShieldState {
        Inactive,
        Active,
        Damaged,
        Broken,
        Recharging
    };
    
private:
    // Owner
    Player* owner;
    
    // Shield properties
    float strength;
    float maxStrength;
    float rechargeRate;
    float rechargeDelay;
    float timeSinceLastHit;
    
    // State
    ShieldState state;
    ShieldType type;
    bool isActive;
    
    // Visual properties
    float radius;
    Vector3 color;
    float opacity;
    float pulseTimer;
    float flickerTimer;
    
    // Special properties
    bool canReflect;
    float reflectChance;
    bool canAbsorb;
    float absorptionRate;
    
public:
    Shield(Player* owner, float maxStrength);
    ~Shield();
    
    // Update
    void update(float deltaTime);
    void updateRecharge(float deltaTime);
    void updateVisuals(float deltaTime);
    
    // Actions
    void activate();
    void deactivate();
    float absorbDamage(float incomingDamage);
    void takeDamage(float damage);
    void repair(float amount);
    void overcharge(float amount);
    void breakShield();
    
    // Getters
    float getStrength() const { return strength; }
    float getMaxStrength() const { return maxStrength; }
    float getStrengthPercentage() const { return strength / maxStrength; }
    ShieldState getState() const { return state; }
    ShieldType getType() const { return type; }
    bool isShieldActive() const { return isActive && state == ShieldState::Active; }
    Vector3 getColor() const { return color; }
    float getOpacity() const { return opacity; }
    float getRadius() const { return radius; }
    
    // Setters
    void setType(ShieldType t);
    void setMaxStrength(float str);
    void setRechargeRate(float rate) { rechargeRate = rate; }
    void setReflective(float chance);
    void setAbsorption(float rate);
    
    // Rendering
    Matrix4 getTransformMatrix() const;
    bool shouldRender() const { return isActive && opacity > 0.01f; }
};