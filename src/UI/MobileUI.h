#pragma once
#include "../Math/Vector2.h"
#include "../Math/Vector3.h"
#include "../Input/TouchInputManager.h"
#include <string>
#include <vector>

// Mobile-specific UI elements for virtual controls
class MobileUI {
public:
    struct UIElement {
        Vector2 position;
        Vector2 size;
        Vector3 color;
        float alpha;
        bool visible;
        std::string label;
    };

private:
    TouchInputManager* touchManager;
    
    // Screen dimensions
    float screenWidth;
    float screenHeight;
    
    // UI elements
    bool showVirtualControls;
    bool showFPS;
    bool showDebugInfo;
    
    // Orientation
    enum class Orientation {
        Portrait,
        Landscape,
        LandscapeLeft,
        LandscapeRight
    };
    Orientation currentOrientation;
    
    // Safe area insets (for notches, etc.)
    struct SafeArea {
        float top;
        float bottom;
        float left;
        float right;
    };
    SafeArea safeArea;

public:
    MobileUI();
    ~MobileUI();
    
    // Initialization
    void initialize(TouchInputManager* touchMgr, float width, float height);
    void shutdown();
    
    // Update
    void update(float deltaTime);
    
    // Rendering
    void render();
    void renderVirtualJoystick(const TouchInputManager::VirtualJoystick& joystick);
    void renderVirtualButton(const TouchInputManager::VirtualButton& button);
    void renderFPS(float fps);
    void renderGameHUD(int lap, int totalLaps, float time, float speed);
    void renderCombatHUD(float health, float maxHealth, float stamina, float maxStamina);
    
    // Settings
    void setShowVirtualControls(bool show);
    void setShowFPS(bool show);
    void setShowDebugInfo(bool show);
    void setOrientation(Orientation orientation);
    void setSafeArea(float top, float bottom, float left, float right);
    void setScreenSize(float width, float height);
    
    // Layout
    void layoutForPortrait();
    void layoutForLandscape();
    void adjustForSafeArea();
    
    // Getters
    bool getShowVirtualControls() const { return showVirtualControls; }
    float getScreenWidth() const { return screenWidth; }
    float getScreenHeight() const { return screenHeight; }
    
private:
    void updateLayout();
};
