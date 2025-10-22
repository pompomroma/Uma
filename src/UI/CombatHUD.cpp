#include "CombatHUD.h"
#include "../Rendering/Renderer.h"
#include "../Physics/Car.h"
#include "../Combat/CombatSystem.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

CombatHUD::CombatHUD(Renderer* renderer, CombatSystem* combatSystem)
    : renderer(renderer)
    , combatSystem(combatSystem)
    , playerCar(nullptr)
    , screenWidth(1920)
    , screenHeight(1080)
    , killCountElement(nullptr)
    , deathCountElement(nullptr)
    , comboCountElement(nullptr)
    , matchTimeElement(nullptr)
    , levelElement(nullptr)
    , statsElement(nullptr)
    , showPlayerStats(true)
    , showAbilityCooldowns(true)
    , showCombatInfo(true)
    , showMinimap(false)
    , hudScale(1.0f)
    , hudOpacity(1.0f) {
}

CombatHUD::~CombatHUD() {
    shutdown();
}

void CombatHUD::initialize(int screenWidth, int screenHeight) {
    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;
    
    createHealthBar();
    createEnergyBar();
    createShieldBar();
    createExperienceBar();
    createAbilityIcons();
    createCombatInfoElements();
    
    repositionElements();
}

void CombatHUD::shutdown() {
    healthBar.reset();
    energyBar.reset();
    shieldBar.reset();
    experienceBar.reset();
    abilityIcons.clear();
    hudElements.clear();
}

void CombatHUD::update(float deltaTime) {
    if (!playerCar || !combatSystem) return;
    
    updatePlayerData();
    updateAbilityData();
    updateCombatData();
}

void CombatHUD::setPlayerCar(Car* car) {
    playerCar = car;
}

void CombatHUD::render() {
    if (!renderer || !playerCar) return;
    
    if (showPlayerStats) {
        renderHealthBars();
    }
    
    if (showAbilityCooldowns) {
        renderAbilityIcons();
    }
    
    if (showCombatInfo) {
        renderCombatInfo();
        renderPlayerStats();
    }
    
    if (showMinimap) {
        renderMinimap();
    }
    
    renderCrosshair();
    renderDamageNumbers();
}

void CombatHUD::renderHealthBars() {
    if (healthBar) {
        renderProgressBar(healthBar.get());
    }
    if (energyBar) {
        renderProgressBar(energyBar.get());
    }
    if (shieldBar) {
        renderProgressBar(shieldBar.get());
    }
    if (experienceBar) {
        renderProgressBar(experienceBar.get());
    }
}

void CombatHUD::renderAbilityIcons() {
    for (const auto& icon : abilityIcons) {
        if (icon) {
            renderAbilityIcon(icon.get());
        }
    }
}

void CombatHUD::renderCombatInfo() {
    for (const auto& element : hudElements) {
        if (element && element->visible) {
            renderHUDElement(element.get());
        }
    }
}

void CombatHUD::renderPlayerStats() {
    if (!statsElement || !combatSystem) return;
    
    PlayerStats* stats = combatSystem->getPlayerStats(playerCar);
    if (!stats) return;
    
    std::stringstream ss;
    ss << "STR: " << static_cast<int>(stats->getStrength()) << " ";
    ss << "DEF: " << static_cast<int>(stats->getDefense()) << " ";
    ss << "STA: " << static_cast<int>(stats->getStamina()) << " ";
    ss << "AGI: " << static_cast<int>(stats->getAgility());
    
    statsElement->text = ss.str();
    renderHUDElement(statsElement);
}

void CombatHUD::renderMinimap() {
    // Placeholder for minimap rendering
    // Would show player positions, projectiles, etc.
}

void CombatHUD::renderCrosshair() {
    if (!renderer || !playerCar || !playerCar->getCombatMode()) return;
    
    // Render simple crosshair in center of screen
    Vector2 center(screenWidth * 0.5f, screenHeight * 0.5f);
    Vector2 crosshairSize(20.0f, 20.0f);
    Vector3 crosshairColor(1.0f, 1.0f, 1.0f);
    
    // This would use the renderer to draw crosshair lines
    // For now, it's a placeholder
}

void CombatHUD::renderDamageNumbers() {
    // Placeholder for floating damage numbers
    // Would show damage dealt/received with animations
}

void CombatHUD::createHealthBar() {
    Vector2 position(50.0f, screenHeight - 150.0f);
    Vector2 size(200.0f, 20.0f);
    Vector3 fillColor(1.0f, 0.0f, 0.0f); // Red
    
    healthBar = std::make_unique<ProgressBar>(position, size, fillColor, "Health");
}

void CombatHUD::createEnergyBar() {
    Vector2 position(50.0f, screenHeight - 120.0f);
    Vector2 size(200.0f, 20.0f);
    Vector3 fillColor(0.0f, 0.0f, 1.0f); // Blue
    
    energyBar = std::make_unique<ProgressBar>(position, size, fillColor, "Energy");
}

void CombatHUD::createShieldBar() {
    Vector2 position(50.0f, screenHeight - 90.0f);
    Vector2 size(200.0f, 20.0f);
    Vector3 fillColor(0.0f, 1.0f, 1.0f); // Cyan
    
    shieldBar = std::make_unique<ProgressBar>(position, size, fillColor, "Shield");
}

void CombatHUD::createExperienceBar() {
    Vector2 position(50.0f, screenHeight - 60.0f);
    Vector2 size(200.0f, 15.0f);
    Vector3 fillColor(1.0f, 1.0f, 0.0f); // Yellow
    
    experienceBar = std::make_unique<ProgressBar>(position, size, fillColor, "XP");
}

void CombatHUD::createAbilityIcons() {
    abilityIcons.clear();
    
    // Create ability icons in a row at the bottom of the screen
    float iconSize = 64.0f;
    float spacing = 10.0f;
    float startX = (screenWidth - (5 * iconSize + 4 * spacing)) * 0.5f;
    float y = screenHeight - iconSize - 20.0f;
    
    abilityIcons.push_back(std::make_unique<AbilityIcon>(
        Vector2(startX, y), PlayerAbilities::AbilityType::LaserAttack, "1"));
    
    abilityIcons.push_back(std::make_unique<AbilityIcon>(
        Vector2(startX + iconSize + spacing, y), PlayerAbilities::AbilityType::PlasmaBlast, "2"));
    
    abilityIcons.push_back(std::make_unique<AbilityIcon>(
        Vector2(startX + 2 * (iconSize + spacing), y), PlayerAbilities::AbilityType::MissileStrike, "3"));
    
    abilityIcons.push_back(std::make_unique<AbilityIcon>(
        Vector2(startX + 3 * (iconSize + spacing), y), PlayerAbilities::AbilityType::Shield, "R"));
    
    abilityIcons.push_back(std::make_unique<AbilityIcon>(
        Vector2(startX + 4 * (iconSize + spacing), y), PlayerAbilities::AbilityType::Teleport, "MMB"));
}

void CombatHUD::createCombatInfoElements() {
    // Kill count
    auto killElement = std::make_unique<HUDElement>(
        Vector2(screenWidth - 200.0f, 50.0f),
        Vector2(150.0f, 30.0f),
        Vector3(1.0f, 1.0f, 1.0f),
        "Kills: 0"
    );
    killCountElement = killElement.get();
    hudElements.push_back(std::move(killElement));
    
    // Death count
    auto deathElement = std::make_unique<HUDElement>(
        Vector2(screenWidth - 200.0f, 80.0f),
        Vector2(150.0f, 30.0f),
        Vector3(1.0f, 1.0f, 1.0f),
        "Deaths: 0"
    );
    deathCountElement = deathElement.get();
    hudElements.push_back(std::move(deathElement));
    
    // Combo count
    auto comboElement = std::make_unique<HUDElement>(
        Vector2(screenWidth - 200.0f, 110.0f),
        Vector2(150.0f, 30.0f),
        Vector3(1.0f, 1.0f, 0.0f),
        "Combo: 0x"
    );
    comboCountElement = comboElement.get();
    hudElements.push_back(std::move(comboElement));
    
    // Match time
    auto timeElement = std::make_unique<HUDElement>(
        Vector2(screenWidth * 0.5f - 75.0f, 50.0f),
        Vector2(150.0f, 30.0f),
        Vector3(1.0f, 1.0f, 1.0f),
        "Time: 5:00"
    );
    matchTimeElement = timeElement.get();
    hudElements.push_back(std::move(timeElement));
    
    // Level
    auto levelElem = std::make_unique<HUDElement>(
        Vector2(50.0f, 50.0f),
        Vector2(100.0f, 30.0f),
        Vector3(1.0f, 1.0f, 1.0f),
        "Level: 1"
    );
    levelElement = levelElem.get();
    hudElements.push_back(std::move(levelElem));
    
    // Stats
    auto statsElem = std::make_unique<HUDElement>(
        Vector2(50.0f, 80.0f),
        Vector2(300.0f, 30.0f),
        Vector3(0.8f, 0.8f, 0.8f),
        "STR: 10 DEF: 10 STA: 10 AGI: 10"
    );
    statsElement = statsElem.get();
    hudElements.push_back(std::move(statsElem));
}

void CombatHUD::updatePlayerData() {
    if (!combatSystem) return;
    
    PlayerStats* stats = combatSystem->getPlayerStats(playerCar);
    if (!stats) return;
    
    // Update health bar
    if (healthBar) {
        updateProgressBar(healthBar.get(), stats->getHealth(), stats->getMaxHealth());
    }
    
    // Update energy bar
    if (energyBar) {
        updateProgressBar(energyBar.get(), stats->getEnergy(), stats->getMaxEnergy());
    }
    
    // Update shield bar
    if (shieldBar) {
        updateProgressBar(shieldBar.get(), stats->getShieldStrength(), stats->getMaxShieldStrength());
    }
    
    // Update experience bar
    if (experienceBar) {
        updateProgressBar(experienceBar.get(), stats->getExperience(), stats->getExperienceToNextLevel());
    }
    
    // Update level
    if (levelElement) {
        levelElement->text = "Level: " + std::to_string(stats->getLevel());
    }
}

void CombatHUD::updateAbilityData() {
    if (!combatSystem) return;
    
    PlayerAbilities* abilities = combatSystem->getPlayerAbilities(playerCar);
    if (!abilities) return;
    
    for (auto& icon : abilityIcons) {
        if (!icon) continue;
        
        float cooldown = abilities->getCooldownPercentage(icon->abilityType);
        bool available = abilities->canUseAbility(icon->abilityType);
        bool unlocked = abilities->isAbilityUnlocked(icon->abilityType);
        
        updateAbilityIcon(icon.get(), cooldown, available, unlocked);
    }
}

void CombatHUD::updateCombatData() {
    if (!combatSystem) return;
    
    // Update kill count
    if (killCountElement) {
        int kills = combatSystem->getPlayerKills(playerCar);
        killCountElement->text = "Kills: " + std::to_string(kills);
    }
    
    // Update death count
    if (deathCountElement) {
        int deaths = combatSystem->getPlayerDeaths(playerCar);
        deathCountElement->text = "Deaths: " + std::to_string(deaths);
    }
    
    // Update combo count
    if (comboCountElement) {
        PlayerAbilities* abilities = combatSystem->getPlayerAbilities(playerCar);
        if (abilities) {
            int combo = abilities->getComboCount();
            comboCountElement->text = "Combo: " + std::to_string(combo) + "x";
            comboCountElement->visible = combo > 0;
        }
    }
    
    // Update match time
    if (matchTimeElement && combatSystem->isMatchActive()) {
        float remainingTime = combatSystem->getRemainingTime();
        int minutes = static_cast<int>(remainingTime) / 60;
        int seconds = static_cast<int>(remainingTime) % 60;
        
        std::stringstream ss;
        ss << "Time: " << minutes << ":" << std::setfill('0') << std::setw(2) << seconds;
        matchTimeElement->text = ss.str();
    }
}

void CombatHUD::updateProgressBar(ProgressBar* bar, float current, float max) {
    if (!bar) return;
    
    bar->currentValue = current;
    bar->maxValue = max;
}

void CombatHUD::updateAbilityIcon(AbilityIcon* icon, float cooldown, bool available, bool unlocked) {
    if (!icon) return;
    
    icon->cooldownPercentage = cooldown;
    icon->available = available;
    icon->unlocked = unlocked;
    
    // Update color based on state
    if (!unlocked) {
        icon->color = Vector3(0.3f, 0.3f, 0.3f); // Dark gray
    } else if (!available) {
        icon->color = Vector3(0.6f, 0.6f, 0.6f); // Gray
    } else {
        icon->color = Vector3(1.0f, 1.0f, 1.0f); // White
    }
}

void CombatHUD::repositionElements() {
    // Reposition elements based on screen size and scale
    // This would be called when screen resolution changes
}

void CombatHUD::setHUDScale(float scale) {
    hudScale = std::clamp(scale, 0.5f, 2.0f);
    scaleElements(hudScale);
}

void CombatHUD::scaleElements(float scale) {
    // Scale all HUD elements
    // This would adjust sizes and positions based on the scale factor
}

void CombatHUD::setHUDOpacity(float opacity) {
    hudOpacity = std::clamp(opacity, 0.0f, 1.0f);
}

void CombatHUD::renderProgressBar(const ProgressBar* bar) {
    if (!bar || !renderer) return;
    
    // This would use the renderer to draw the progress bar
    // For now, it's a placeholder that would call renderer methods like:
    // renderer->renderRect(bar->position, bar->size, bar->backgroundColor);
    // renderer->renderRect(bar->position, Vector2(bar->size.x * bar->getPercentage(), bar->size.y), bar->fillColor);
    // renderer->renderText(bar->label, bar->position, Vector3(1.0f, 1.0f, 1.0f));
}

void CombatHUD::renderAbilityIcon(const AbilityIcon* icon) {
    if (!icon || !renderer) return;
    
    // This would use the renderer to draw the ability icon
    // Including the icon image, cooldown overlay, and key binding text
}

void CombatHUD::renderHUDElement(const HUDElement* element) {
    if (!element || !renderer) return;
    
    // This would use the renderer to draw the HUD element
    // renderer->renderText(element->text, element->position, element->color);
}

void CombatHUD::renderText(const std::string& text, const Vector2& position, const Vector3& color, float scale) {
    if (!renderer) return;
    
    // This would use the renderer to draw text
    // renderer->renderText(text, position, color, scale);
}

Vector2 CombatHUD::worldToScreenPosition(const Vector3& worldPos) {
    // Convert world position to screen coordinates
    // This would use the camera's view and projection matrices
    // For now, return a placeholder
    return Vector2(screenWidth * 0.5f, screenHeight * 0.5f);
}

void CombatHUD::showDamageNumber(const Vector3& worldPos, float damage, bool isCritical) {
    // Show floating damage number at world position
    // This would create a temporary UI element that animates upward and fades out
}

void CombatHUD::showHealNumber(const Vector3& worldPos, float healing) {
    // Show floating healing number at world position
}

void CombatHUD::showExperienceGain(float experience) {
    // Show experience gain notification
}

void CombatHUD::showLevelUp(int newLevel) {
    // Show level up notification with effects
}

void CombatHUD::showAbilityUnlocked(PlayerAbilities::AbilityType ability) {
    // Show ability unlocked notification
}