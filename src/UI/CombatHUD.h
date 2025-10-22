#pragma once
#include "../Math/Vector2.h"
#include "../Math/Vector3.h"
#include "../Combat/PlayerStats.h"
#include "../Combat/PlayerAbilities.h"
#include <string>
#include <vector>
#include <memory>

class Renderer;
class Car;
class CombatSystem;

class CombatHUD {
public:
    struct HUDElement {
        Vector2 position;
        Vector2 size;
        Vector3 color;
        std::string text;
        bool visible;
        
        HUDElement(const Vector2& pos, const Vector2& sz, const Vector3& col, const std::string& txt = "")
            : position(pos), size(sz), color(col), text(txt), visible(true) {}
    };

    struct ProgressBar {
        Vector2 position;
        Vector2 size;
        float currentValue;
        float maxValue;
        Vector3 fillColor;
        Vector3 backgroundColor;
        Vector3 borderColor;
        std::string label;
        bool showText;
        
        ProgressBar(const Vector2& pos, const Vector2& sz, const Vector3& fill, const std::string& lbl = "")
            : position(pos), size(sz), currentValue(0.0f), maxValue(100.0f),
              fillColor(fill), backgroundColor(0.2f, 0.2f, 0.2f), 
              borderColor(0.5f, 0.5f, 0.5f), label(lbl), showText(true) {}
        
        float getPercentage() const { return maxValue > 0.0f ? currentValue / maxValue : 0.0f; }
    };

    struct AbilityIcon {
        Vector2 position;
        Vector2 size;
        PlayerAbilities::AbilityType abilityType;
        std::string keyBinding;
        float cooldownPercentage;
        bool available;
        bool unlocked;
        Vector3 color;
        
        AbilityIcon(const Vector2& pos, PlayerAbilities::AbilityType type, const std::string& key)
            : position(pos), size(64.0f, 64.0f), abilityType(type), keyBinding(key),
              cooldownPercentage(0.0f), available(true), unlocked(true), 
              color(1.0f, 1.0f, 1.0f) {}
    };

private:
    Renderer* renderer;
    CombatSystem* combatSystem;
    Car* playerCar;
    
    // Screen dimensions
    int screenWidth;
    int screenHeight;
    
    // HUD elements
    std::unique_ptr<ProgressBar> healthBar;
    std::unique_ptr<ProgressBar> energyBar;
    std::unique_ptr<ProgressBar> shieldBar;
    std::unique_ptr<ProgressBar> experienceBar;
    
    std::vector<std::unique_ptr<AbilityIcon>> abilityIcons;
    std::vector<std::unique_ptr<HUDElement>> hudElements;
    
    // Combat info
    HUDElement* killCountElement;
    HUDElement* deathCountElement;
    HUDElement* comboCountElement;
    HUDElement* matchTimeElement;
    HUDElement* levelElement;
    HUDElement* statsElement;
    
    // Settings
    bool showPlayerStats;
    bool showAbilityCooldowns;
    bool showCombatInfo;
    bool showMinimap;
    float hudScale;
    float hudOpacity;

public:
    CombatHUD(Renderer* renderer, CombatSystem* combatSystem);
    ~CombatHUD();
    
    // Initialization
    void initialize(int screenWidth, int screenHeight);
    void shutdown();
    
    // Update
    void update(float deltaTime);
    void setPlayerCar(Car* car);
    
    // Rendering
    void render();
    void renderHealthBars();
    void renderAbilityIcons();
    void renderCombatInfo();
    void renderPlayerStats();
    void renderMinimap();
    void renderCrosshair();
    void renderDamageNumbers();
    
    // HUD management
    void addHUDElement(std::unique_ptr<HUDElement> element);
    void removeHUDElement(HUDElement* element);
    void updateProgressBar(ProgressBar* bar, float current, float max);
    void updateAbilityIcon(AbilityIcon* icon, float cooldown, bool available, bool unlocked);
    
    // Layout
    void repositionElements();
    void scaleElements(float scale);
    void setHUDOpacity(float opacity);
    
    // Settings
    void setShowPlayerStats(bool show) { showPlayerStats = show; }
    void setShowAbilityCooldowns(bool show) { showAbilityCooldowns = show; }
    void setShowCombatInfo(bool show) { showCombatInfo = show; }
    void setShowMinimap(bool show) { showMinimap = show; }
    void setHUDScale(float scale);
    
    bool getShowPlayerStats() const { return showPlayerStats; }
    bool getShowAbilityCooldowns() const { return showAbilityCooldowns; }
    bool getShowCombatInfo() const { return showCombatInfo; }
    bool getShowMinimap() const { return showMinimap; }
    float getHUDScale() const { return hudScale; }
    
    // Utility
    void showDamageNumber(const Vector3& worldPos, float damage, bool isCritical = false);
    void showHealNumber(const Vector3& worldPos, float healing);
    void showExperienceGain(float experience);
    void showLevelUp(int newLevel);
    void showAbilityUnlocked(PlayerAbilities::AbilityType ability);
    
private:
    void createHealthBar();
    void createEnergyBar();
    void createShieldBar();
    void createExperienceBar();
    void createAbilityIcons();
    void createCombatInfoElements();
    void updatePlayerData();
    void updateAbilityData();
    void updateCombatData();
    Vector2 worldToScreenPosition(const Vector3& worldPos);
    void renderProgressBar(const ProgressBar* bar);
    void renderAbilityIcon(const AbilityIcon* icon);
    void renderHUDElement(const HUDElement* element);
    void renderText(const std::string& text, const Vector2& position, const Vector3& color, float scale = 1.0f);
};