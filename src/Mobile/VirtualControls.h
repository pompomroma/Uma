#pragma once

#ifdef MOBILE_PLATFORM

#include "../Math/Vector2.h"
#include "../Math/Vector3.h"
#include "MobileRenderer.h"
#include <vector>
#include <string>
#include <memory>

class VirtualControls {
public:
    // Button types for different actions
    enum class ButtonType {
        Accelerate,
        Brake,
        Boost,
        Handbrake,
        Pause,
        Reset,
        Attack1,
        Attack2,
        Shield,
        Teleport,
        Interact,
        StatMenu
    };
    
    // Button states
    enum class ButtonState {
        Normal,
        Pressed,
        Disabled
    };
    
    // Virtual button structure
    struct VirtualButton {
        ButtonType type;
        Vector2 position;
        Vector2 size;
        ButtonState state;
        Vector3 normalColor;
        Vector3 pressedColor;
        Vector3 disabledColor;
        std::string label;
        std::string iconPath;
        float alpha;
        bool isVisible;
        bool isEnabled;
        
        VirtualButton() : state(ButtonState::Normal), alpha(1.0f), 
                         isVisible(true), isEnabled(true) {}
    };
    
    // Virtual joystick structure
    struct VirtualJoystick {
        Vector2 center;
        float outerRadius;
        float innerRadius;
        Vector2 knobPosition;
        Vector3 baseColor;
        Vector3 knobColor;
        Vector3 deadZoneColor;
        float baseAlpha;
        float knobAlpha;
        bool isActive;
        bool isVisible;
        bool showDeadZone;
        std::string label;
        
        VirtualJoystick() : outerRadius(60.0f), innerRadius(20.0f),
                           baseAlpha(0.6f), knobAlpha(0.8f), isActive(false),
                           isVisible(true), showDeadZone(true) {}
    };
    
    // HUD element for racing info
    struct HUDElement {
        Vector2 position;
        Vector2 size;
        Vector3 color;
        std::string text;
        float fontSize;
        bool isVisible;
        
        HUDElement() : fontSize(24.0f), isVisible(true) {}
    };
    
    // Speedometer
    struct Speedometer {
        Vector2 center;
        float radius;
        float currentSpeed;
        float maxSpeed;
        Vector3 needleColor;
        Vector3 dialColor;
        Vector3 textColor;
        bool isVisible;
        
        Speedometer() : radius(80.0f), currentSpeed(0.0f), maxSpeed(200.0f),
                       needleColor(1.0f, 0.0f, 0.0f), dialColor(1.0f, 1.0f, 1.0f),
                       textColor(1.0f, 1.0f, 1.0f), isVisible(true) {}
    };
    
    // Minimap
    struct Minimap {
        Vector2 position;
        Vector2 size;
        Vector3 backgroundColor;
        Vector3 playerColor;
        Vector3 trackColor;
        float zoom;
        bool isVisible;
        
        Minimap() : size(120.0f, 120.0f), backgroundColor(0.0f, 0.0f, 0.0f),
                   playerColor(1.0f, 0.0f, 0.0f), trackColor(0.5f, 0.5f, 0.5f),
                   zoom(1.0f), isVisible(true) {}
    };

private:
    // Renderer reference
    MobileRenderer* renderer;
    
    // Screen properties
    Vector2 screenSize;
    float screenDensity;
    
    // Control elements
    std::vector<VirtualButton> buttons;
    VirtualJoystick steeringJoystick;
    VirtualJoystick cameraJoystick;
    
    // HUD elements
    std::vector<HUDElement> hudElements;
    Speedometer speedometer;
    Minimap minimap;
    
    // Racing info
    float currentSpeed;
    int currentLap;
    int totalLaps;
    float lapTime;
    float bestTime;
    int position;
    int totalCars;
    
    // Combat info
    float health;
    float shield;
    float energy;
    int level;
    int experience;
    
    // UI settings
    float globalAlpha;
    bool showDebugInfo;
    bool adaptiveUI;        // Adjust UI based on device performance
    float uiScale;          // Scale factor for different screen sizes
    
    // Animation system
    struct Animation {
        float duration;
        float currentTime;
        float startValue;
        float endValue;
        bool isActive;
        
        Animation() : duration(0.0f), currentTime(0.0f), startValue(0.0f),
                     endValue(0.0f), isActive(false) {}
    };
    
    std::vector<Animation> buttonAnimations;

public:
    VirtualControls();
    ~VirtualControls();
    
    // Initialization
    void initialize(MobileRenderer* renderer, const Vector2& screenSize, float screenDensity = 160.0f);
    void shutdown();
    
    // Screen management
    void setScreenSize(const Vector2& size);
    void setScreenDensity(float density);
    void handleScreenRotation();
    void setUIScale(float scale);
    
    // Update and rendering
    void update(float deltaTime);
    void render();
    void renderButtons();
    void renderJoysticks();
    void renderHUD();
    void renderSpeedometer();
    void renderMinimap();
    void renderDebugInfo();
    
    // Button management
    void addButton(ButtonType type, const Vector2& position, const Vector2& size, 
                  const std::string& label = "", const std::string& iconPath = "");
    void removeButton(ButtonType type);
    VirtualButton* getButton(ButtonType type);
    void setButtonState(ButtonType type, ButtonState state);
    void setButtonVisible(ButtonType type, bool visible);
    void setButtonEnabled(ButtonType type, bool enabled);
    
    // Joystick management
    void setupSteeringJoystick(const Vector2& center, float outerRadius);
    void setupCameraJoystick(const Vector2& center, float outerRadius);
    void setJoystickVisible(bool steering, bool camera);
    void updateJoystickPosition(bool isSteeringJoystick, const Vector2& knobPosition, bool isActive);
    
    // HUD management
    void addHUDElement(const Vector2& position, const Vector2& size, const std::string& text);
    void updateHUDElement(int index, const std::string& text);
    void setHUDElementVisible(int index, bool visible);
    
    // Racing info updates
    void updateSpeed(float speed);
    void updateLapInfo(int currentLap, int totalLaps, float lapTime, float bestTime);
    void updateRacePosition(int position, int totalCars);
    
    // Combat info updates
    void updateHealth(float health);
    void updateShield(float shield);
    void updateEnergy(float energy);
    void updateLevel(int level, int experience);
    
    // Speedometer
    void setSpeedometerVisible(bool visible);
    void setSpeedometerPosition(const Vector2& center);
    void setSpeedometerMaxSpeed(float maxSpeed);
    
    // Minimap
    void setMinimapVisible(bool visible);
    void setMinimapPosition(const Vector2& position);
    void setMinimapSize(const Vector2& size);
    void updateMinimapPlayerPosition(const Vector2& position, float rotation);
    
    // UI settings
    void setGlobalAlpha(float alpha);
    void setShowDebugInfo(bool show);
    void setAdaptiveUI(bool adaptive);
    
    // Layout presets for different screen orientations
    void setupPortraitLayout();
    void setupLandscapeLayout();
    void setupTabletLayout();
    
    // Touch interaction
    bool isPointInButton(const Vector2& point, ButtonType type) const;
    bool isPointInJoystick(const Vector2& point, bool isSteeringJoystick) const;
    ButtonType getTouchedButton(const Vector2& point) const;
    
    // Animation system
    void animateButton(ButtonType type, float fromAlpha, float toAlpha, float duration);
    void animateJoystick(bool isSteeringJoystick, float fromAlpha, float toAlpha, float duration);
    void updateAnimations(float deltaTime);
    
    // Utility functions
    Vector2 getButtonCenter(ButtonType type) const;
    Vector2 getJoystickCenter(bool isSteeringJoystick) const;
    float getJoystickRadius(bool isSteeringJoystick) const;
    
    // Adaptive UI helpers
    void adjustForPerformance();
    void adjustForScreenSize();
    void adjustForOrientation();
    
private:
    // Internal rendering helpers
    void renderButton(const VirtualButton& button);
    void renderJoystick(const VirtualJoystick& joystick);
    void renderCircle(const Vector2& center, float radius, const Vector3& color, float alpha = 1.0f);
    void renderRing(const Vector2& center, float innerRadius, float outerRadius, 
                   const Vector3& color, float alpha = 1.0f);
    void renderText(const std::string& text, const Vector2& position, float fontSize, 
                   const Vector3& color, float alpha = 1.0f);
    void renderProgressBar(const Vector2& position, const Vector2& size, float progress,
                          const Vector3& fillColor, const Vector3& bgColor, float alpha = 1.0f);
    
    // Layout calculation helpers
    Vector2 calculateButtonPosition(ButtonType type, const Vector2& screenSize) const;
    Vector2 calculateButtonSize(ButtonType type, const Vector2& screenSize) const;
    Vector2 calculateJoystickPosition(bool isSteeringJoystick, const Vector2& screenSize) const;
    float calculateJoystickRadius(const Vector2& screenSize) const;
    
    // Color and style helpers
    Vector3 getButtonColor(ButtonType type, ButtonState state) const;
    float getButtonAlpha(ButtonType type, ButtonState state) const;
    std::string getButtonLabel(ButtonType type) const;
    
    // Performance optimization
    void optimizeForLowEnd();
    void optimizeForHighEnd();
    bool shouldSkipFrame() const;
    
    // Debug helpers
    void renderTouchDebugInfo();
    void renderPerformanceInfo();
    void logUIMetrics() const;
};

#endif // MOBILE_PLATFORM