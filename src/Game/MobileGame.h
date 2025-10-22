#pragma once
#include "../Game.h"
#include "../Platform/PlatformBase.h"
#include "../Input/TouchInputManager.h"
#include "../Rendering/MobileRenderer.h"

class MobileGame : public Game {
private:
    PlatformBase* platform;
    TouchInputManager* touchInput;
    MobileRenderer* mobileRenderer;
    
    // Mobile-specific settings
    struct MobileSettings {
        bool useTiltControls;
        float tiltSensitivity;
        bool useHapticFeedback;
        bool autoAccelerate;
        TouchInputManager::ControlLayout controlLayout;
        int graphicsQuality; // 0 = low, 1 = medium, 2 = high
        bool enableVibration;
        float uiScale;
    } mobileSettings;
    
    // Performance monitoring
    float batteryLevel;
    bool isLowPowerMode;
    float thermalState; // 0 = normal, 1 = throttled
    
    // Mobile-specific game states
    bool isPausedByOS;
    bool isInBackground;
    
public:
    MobileGame();
    ~MobileGame() override;
    
    // Mobile initialization
    bool initializeMobile(PlatformBase* platformInstance);
    bool initialize(int width, int height, const std::string& title) override;
    void shutdown() override;
    
    // Mobile game loop
    void run() override;
    void update(float deltaTime) override;
    void render() override;
    
    // Mobile input handling
    void handleMobileInput();
    void handleTouch(const PlatformBase::TouchPoint& touch);
    void handleGesture(TouchInputManager::GestureType type, const TouchInputManager::Gesture& gesture);
    void handleAccelerometer(float x, float y, float z);
    void handleGyroscope(float x, float y, float z);
    
    // Mobile-specific callbacks
    void onPause() override;
    void onResume();
    void onLowMemory();
    void onOrientationChange(PlatformBase::DeviceOrientation orientation);
    
    // Touch control setup
    void setupTouchControls();
    void switchControlLayout(TouchInputManager::ControlLayout layout);
    
    // Mobile UI
    void renderMobileUI();
    void renderMobileMenu() override;
    void renderMobilePauseMenu();
    void renderMobileSettings();
    void renderMobileHUD();
    
    // Mobile optimizations
    void optimizeForDevice();
    void adjustGraphicsQuality(int quality);
    void enablePowerSaving(bool enable);
    
    // Settings management
    void loadMobileSettings();
    void saveMobileSettings();
    void setTiltControls(bool enabled);
    void setHapticFeedback(bool enabled);
    void setAutoAccelerate(bool enabled);
    
    // Platform integration
    void shareScore(int score);
    void showLeaderboard();
    void unlockAchievement(const std::string& achievementId);
    void showAds(bool show);
    void purchaseItem(const std::string& itemId);
    
    // Device-specific features
    void vibrate(int milliseconds);
    void playHapticFeedback(float intensity);
    float getBatteryLevel() const { return batteryLevel; }
    bool isPluggedIn() const;
    
    // Cloud save
    void saveToCloud();
    void loadFromCloud();
    void syncProgress();
    
private:
    void setupMobileCallbacks();
    void updateBatteryStatus();
    void updateThermalState();
    void handleMemoryWarning();
    
    // Touch helpers
    Vector2 screenToWorld(Vector2 screenPos);
    void processTouchForRacing(const PlatformBase::TouchPoint& touch);
    void processTouchForCombat(const PlatformBase::TouchPoint& touch);
    void processTouchForMenu(const PlatformBase::TouchPoint& touch);
};