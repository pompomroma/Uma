#pragma once
#include "../Game.h"
#include "../Platform/Platform.h"
#include "../Input/MobileInputManager.h"
#include "../Rendering/MobileRenderer.h"
#include <memory>

class MobileGame : public Game {
public:
    enum class MobileGameState {
        Menu,
        Playing,
        Paused,
        GameOver,
        PvPMode,
        StatsMenu,
        Settings,
        Loading,
        Splash
    };

    enum class ControlScheme {
        Racing,
        Combat,
        Menu,
        Gesture
    };

    struct MobileSettings {
        // Touch settings
        float touchSensitivity;
        float touchDeadZone;
        bool touchVibration;
        bool touchHapticFeedback;
        
        // Graphics settings
        float graphicsQuality;
        float textureQuality;
        float modelQuality;
        bool shadowsEnabled;
        bool particlesEnabled;
        bool postProcessingEnabled;
        int targetFPS;
        bool vsyncEnabled;
        
        // Audio settings
        float masterVolume;
        float sfxVolume;
        float musicVolume;
        bool audioEnabled;
        
        // Control settings
        ControlScheme controlScheme;
        bool autoAccelerate;
        bool autoBrake;
        bool autoSteer;
        bool gestureControls;
        bool virtualJoystick;
        bool virtualButtons;
        
        // UI settings
        float uiScale;
        bool uiEnabled;
        bool showFPS;
        bool showDebugInfo;
        bool showTouchDebug;
        
        // Mobile-specific settings
        bool keepScreenOn;
        bool orientationLock;
        int screenOrientation;
        bool batteryOptimization;
        bool lowPowerMode;
    };

private:
    // Mobile-specific systems
    std::unique_ptr<MobileInputManager> m_mobileInputManager;
    std::unique_ptr<MobileRenderer> m_mobileRenderer;
    
    // Mobile state
    MobileGameState m_mobileState;
    ControlScheme m_currentControlScheme;
    MobileSettings m_mobileSettings;
    
    // Mobile UI
    bool m_uiEnabled;
    bool m_controlsVisible;
    bool m_menuVisible;
    bool m_settingsVisible;
    bool m_debugVisible;
    
    // Mobile performance
    int m_targetFPS;
    float m_frameTime;
    int m_frameCount;
    float m_fps;
    float m_lastFpsUpdate;
    
    // Mobile input
    bool m_touchEnabled;
    bool m_gestureEnabled;
    bool m_accelerometerEnabled;
    bool m_gyroscopeEnabled;
    
    // Mobile audio
    bool m_audioEnabled;
    float m_masterVolume;
    float m_sfxVolume;
    float m_musicVolume;
    
    // Mobile platform
    Platform::PlatformType m_platform;
    Platform::PlatformInfo m_platformInfo;
    
    // Mobile lifecycle
    bool m_isActive;
    bool m_isPaused;
    bool m_isBackground;
    bool m_isForeground;
    
    // Mobile memory
    int m_memoryUsage;
    int m_maxMemory;
    bool m_memoryWarning;
    
    // Mobile battery
    float m_batteryLevel;
    bool m_isCharging;
    bool m_lowBatteryMode;
    
    // Mobile network
    bool m_networkConnected;
    bool m_wifiConnected;
    bool m_mobileDataConnected;
    
    // Mobile storage
    std::string m_dataPath;
    std::string m_cachePath;
    std::string m_assetPath;
    
    // Mobile settings
    std::string m_settingsFile;
    bool m_settingsLoaded;
    bool m_settingsSaved;
    
    // Mobile debugging
    bool m_debugEnabled;
    bool m_performanceOverlay;
    bool m_touchDebug;
    bool m_memoryDebug;
    bool m_batteryDebug;
    bool m_networkDebug;

public:
    MobileGame();
    ~MobileGame();
    
    // Initialization
    bool initialize(int width, int height, const std::string& title) override;
    void shutdown() override;
    
    // Mobile-specific initialization
    bool initializeMobile();
    bool initializePlatform();
    bool initializeInput();
    bool initializeRendering();
    bool initializeAudio();
    bool initializeUI();
    bool initializeSettings();
    
    // Main game loop
    void run() override;
    void update(float deltaTime) override;
    void render() override;
    void handleInput() override;
    
    // Mobile game loop
    void updateMobile(float deltaTime);
    void renderMobile();
    void handleMobileInput();
    void handleMobileEvents();
    
    // Mobile state management
    void setMobileState(MobileGameState state);
    MobileGameState getMobileState() const { return m_mobileState; }
    void setControlScheme(ControlScheme scheme);
    ControlScheme getControlScheme() const { return m_currentControlScheme; }
    
    // Mobile input handling
    void onTouchDown(int id, float x, float y, float pressure);
    void onTouchMove(int id, float x, float y, float pressure);
    void onTouchUp(int id, float x, float y);
    void onTouchCancel(int id);
    void onGesture(const Vector2& delta, float scale, float rotation);
    void onAccelerometer(float x, float y, float z);
    void onGyroscope(float x, float y, float z);
    
    // Mobile UI
    void showMobileMenu();
    void hideMobileMenu();
    void showMobileSettings();
    void hideMobileSettings();
    void showMobileDebug();
    void hideMobileDebug();
    void toggleMobileUI();
    void setUIVisible(bool visible);
    
    // Mobile controls
    void setupRacingControls();
    void setupCombatControls();
    void setupMenuControls();
    void setupGestureControls();
    void updateControlScheme();
    void showControls();
    void hideControls();
    void toggleControls();
    
    // Mobile settings
    void loadMobileSettings();
    void saveMobileSettings();
    void resetMobileSettings();
    void setMobileSetting(const std::string& key, const std::string& value);
    std::string getMobileSetting(const std::string& key, const std::string& defaultValue = "");
    
    // Mobile performance
    void setTargetFPS(int fps);
    void setVSync(bool enabled);
    void setGraphicsQuality(float quality);
    void setTextureQuality(float quality);
    void setModelQuality(float quality);
    void setShadowsEnabled(bool enabled);
    void setParticlesEnabled(bool enabled);
    void setPostProcessingEnabled(bool enabled);
    void optimizeForPerformance();
    void optimizeForBattery();
    void optimizeForMemory();
    
    // Mobile audio
    void setMasterVolume(float volume);
    void setSFXVolume(float volume);
    void setMusicVolume(float volume);
    void playMobileSound(const std::string& filename);
    void stopMobileSound(const std::string& filename);
    void playMobileMusic(const std::string& filename);
    void stopMobileMusic();
    void pauseMobileAudio();
    void resumeMobileAudio();
    
    // Mobile platform
    void handleOrientationChange(int orientation);
    void handleScreenSizeChange(int width, int height);
    void handleMemoryWarning();
    void handleAppStateChange(int state);
    void handleBatteryLevelChange(float level);
    void handleNetworkChange(bool connected);
    
    // Mobile lifecycle
    void onResume();
    void onPause();
    void onStop();
    void onDestroy();
    void onRestart();
    void onLowMemory();
    void onTrimMemory();
    
    // Mobile debugging
    void enableDebugMode(bool enabled);
    void showPerformanceOverlay(bool show);
    void showTouchDebug(bool show);
    void showMemoryDebug(bool show);
    void showBatteryDebug(bool show);
    void showNetworkDebug(bool show);
    void logMobileInfo();
    void logPerformanceInfo();
    void logMemoryInfo();
    void logBatteryInfo();
    void logNetworkInfo();
    
    // Mobile utilities
    void vibrate(int duration = 100);
    void showToast(const std::string& message);
    void showAlert(const std::string& title, const std::string& message);
    void showConfirm(const std::string& title, const std::string& message, std::function<void(bool)> callback);
    void showInput(const std::string& title, const std::string& message, std::function<void(const std::string&)> callback);
    
    // Mobile file system
    std::string getAssetPath(const std::string& filename);
    std::string getDataPath();
    std::string getCachePath();
    bool fileExists(const std::string& path);
    std::vector<uint8_t> readFile(const std::string& path);
    bool writeFile(const std::string& path, const std::vector<uint8_t>& data);
    
    // Mobile network
    bool isNetworkConnected();
    bool isWifiConnected();
    bool isMobileDataConnected();
    void setNetworkEnabled(bool enabled);
    
    // Mobile storage
    void setDataPath(const std::string& path);
    void setCachePath(const std::string& path);
    void setAssetPath(const std::string& path);
    void clearCache();
    void clearData();
    
    // Mobile platform info
    Platform::PlatformType getPlatform() const { return m_platform; }
    Platform::PlatformInfo getPlatformInfo() const { return m_platformInfo; }
    bool isMobile() const { return m_platform == Platform::PlatformType::Android || m_platform == Platform::PlatformType::iOS; }
    
    // Mobile settings access
    MobileSettings& getMobileSettings() { return m_mobileSettings; }
    const MobileSettings& getMobileSettings() const { return m_mobileSettings; }
    
    // Mobile state queries
    bool isUIVisible() const { return m_uiEnabled; }
    bool areControlsVisible() const { return m_controlsVisible; }
    bool isMenuVisible() const { return m_menuVisible; }
    bool isSettingsVisible() const { return m_settingsVisible; }
    bool isDebugVisible() const { return m_debugVisible; }
    
    // Mobile performance queries
    float getFPS() const { return m_fps; }
    float getFrameTime() const { return m_frameTime; }
    int getMemoryUsage() const { return m_memoryUsage; }
    float getBatteryLevel() const { return m_batteryLevel; }
    bool isLowBattery() const { return m_lowBatteryMode; }
    
    // Mobile input queries
    bool isTouchEnabled() const { return m_touchEnabled; }
    bool isGestureEnabled() const { return m_gestureEnabled; }
    bool isAccelerometerEnabled() const { return m_accelerometerEnabled; }
    bool isGyroscopeEnabled() const { return m_gyroscopeEnabled; }
    
    // Mobile audio queries
    bool isAudioEnabled() const { return m_audioEnabled; }
    float getMasterVolume() const { return m_masterVolume; }
    float getSFXVolume() const { return m_sfxVolume; }
    float getMusicVolume() const { return m_musicVolume; }

private:
    void initializeMobileDefaults();
    void setupMobileCallbacks();
    void updateMobilePerformance(float deltaTime);
    void updateMobileInput(float deltaTime);
    void updateMobileAudio(float deltaTime);
    void updateMobileUI(float deltaTime);
    void updateMobilePlatform(float deltaTime);
    
    void renderMobileUI();
    void renderMobileControls();
    void renderMobileDebug();
    void renderMobileHUD();
    void renderMobileMenu();
    void renderMobileSettings();
    
    void handleMobileTouchInput();
    void handleMobileGestureInput();
    void handleMobileAccelerometerInput();
    void handleMobileGyroscopeInput();
    void handleMobileButtonInput();
    void handleMobileJoystickInput();
    
    void processMobileEvents();
    void processMobileInput();
    void processMobileGestures();
    void processMobileAccelerometer();
    void processMobileGyroscope();
    
    void setupMobileRacingControls();
    void setupMobileCombatControls();
    void setupMobileMenuControls();
    void setupMobileGestureControls();
    
    void updateMobileControlScheme();
    void updateMobileUI();
    void updateMobileSettings();
    void updateMobilePerformance();
    void updateMobileMemory();
    void updateMobileBattery();
    void updateMobileNetwork();
    
    void saveMobileSettings();
    void loadMobileSettings();
    void resetMobileSettings();
    void applyMobileSettings();
    
    void optimizeMobilePerformance();
    void optimizeMobileBattery();
    void optimizeMobileMemory();
    void optimizeMobileRendering();
    void optimizeMobileAudio();
    void optimizeMobileInput();
    
    void handleMobileOrientationChange();
    void handleMobileScreenSizeChange();
    void handleMobileMemoryWarning();
    void handleMobileAppStateChange();
    void handleMobileBatteryLevelChange();
    void handleMobileNetworkChange();
    
    void logMobileDebugInfo();
    void logMobilePerformanceInfo();
    void logMobileMemoryInfo();
    void logMobileBatteryInfo();
    void logMobileNetworkInfo();
    void logMobileInputInfo();
    void logMobileAudioInfo();
    void logMobileRenderingInfo();
};