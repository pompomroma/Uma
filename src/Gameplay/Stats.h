#pragma once

class Stats {
public:
    // Core attributes
    float strength;  // Increases outgoing damage
    float defense;   // Reduces incoming damage
    float stamina;   // Affects max stamina pool
    float agility;   // Affects cooldowns and stamina regen

    // Vital resources
    float maxHealth;
    float currentHealth;
    float maxStamina;
    float currentStamina;

    // Progression
    int level;
    float experiencePoints;
    float experienceToNext;

    // Regeneration rates (per second)
    float staminaRegenRate;
    float healthRegenRate;

    Stats();

    void update(float deltaTime);

    bool isAlive() const { return currentHealth > 0.0f; }

    bool canSpendStamina(float cost) const { return currentStamina >= cost; }
    void spendStamina(float cost);

    // Returns actual damage applied after mitigation
    float applyDamage(float rawDamage);

    // Multiplier to outgoing damage based on strength
    float outgoingDamageMultiplier() const;

    // Gated stat progression via experience
    void addExperience(float amount);

private:
    void levelUp();
};
