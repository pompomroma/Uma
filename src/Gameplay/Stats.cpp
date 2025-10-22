#include "Stats.h"
#include <algorithm>

Stats::Stats()
    : strength(10.0f)
    , defense(10.0f)
    , stamina(10.0f)
    , agility(10.0f)
    , maxHealth(100.0f)
    , currentHealth(100.0f)
    , maxStamina(100.0f)
    , currentStamina(100.0f)
    , level(1)
    , experiencePoints(0.0f)
    , experienceToNext(100.0f)
    , staminaRegenRate(10.0f)
    , healthRegenRate(1.0f) {
}

void Stats::update(float deltaTime) {
    // Regenerate stamina faster with agility
    float agilityFactor = 1.0f + (agility - 10.0f) * 0.05f; // +5% per point over 10
    currentStamina = std::min(maxStamina, currentStamina + staminaRegenRate * agilityFactor * deltaTime);

    // Light health regen when out of combat (simplification)
    currentHealth = std::min(maxHealth, currentHealth + healthRegenRate * deltaTime);
}

void Stats::spendStamina(float cost) {
    currentStamina = std::max(0.0f, currentStamina - cost);
}

float Stats::applyDamage(float rawDamage) {
    // Mitigation based on defense (diminishing returns)
    float mitigation = defense / (defense + 100.0f);
    float finalDamage = rawDamage * (1.0f - mitigation);
    currentHealth = std::max(0.0f, currentHealth - finalDamage);
    return finalDamage;
}

float Stats::outgoingDamageMultiplier() const {
    // Strength increases damage with diminishing returns
    return 1.0f + (strength / (strength + 50.0f));
}

void Stats::addExperience(float amount) {
    experiencePoints += amount;
    while (experiencePoints >= experienceToNext) {
        experiencePoints -= experienceToNext;
        levelUp();
    }
}

void Stats::levelUp() {
    level++;
    // Increase core attributes modestly
    strength += 2.0f;
    defense += 2.0f;
    stamina += 2.0f;
    agility += 2.0f;

    // Scale vitals
    maxHealth += 10.0f;
    maxStamina += 10.0f;
    currentHealth = maxHealth;
    currentStamina = maxStamina;

    // Increase next threshold
    experienceToNext *= 1.25f;
}
