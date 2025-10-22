#pragma once
#include <string>
#include <functional>
#include <memory>

// Platform detection
#if defined(__ANDROID__)
    #define PLATFORM_ANDROID 1
#elif defined(__APPLE__)
    #define PLATFORM_IOS 1
#elif defined(_WIN32) || defined(_WIN64)
    #define PLATFORM_WINDOWS 1
#elif defined(__linux__)
    #define PLATFORM_LINUX 1
#else
    #define PLATFORM_UNKNOWN 1
#endif

// Mobile platform detection
#if PLATFORM_ANDROID || PLATFORM_IOS
    #define PLATFORM_MOBILE 1
#else
    #define PLATFORM_MOBILE 0
#endif

// Graphics API detection
#if PLATFORM_MOBILE
    #define GRAPHICS_API_OPENGL_ES 1
    #define GRAPHICS_API_OPENGL 0
#else
    #define GRAPHICS_API_OPENGL_ES 0
    #define GRAPHICS_API_OPENGL 1
#endif

namespace Platform {
    enum class PlatformType {
        Android,
        iOS,
        Windows,
        Linux,
        Unknown
    };

    enum class GraphicsAPI {
        OpenGL,
        OpenGL_ES,
        Vulkan,
        Metal
    };

    struct PlatformInfo {
        PlatformType type;
        GraphicsAPI graphicsAPI;
        std::string version;
        std::string deviceModel;
        int screenWidth;
        int screenHeight;
        float screenDensity;
        bool hasTouch;
        bool hasAccelerometer;
        bool hasGyroscope;
        int maxTextureSize;
        int maxVertexAttribs;
        bool supportsInstancing;
        bool supportsVAO;
    };

    // Platform initialization and management
    class PlatformManager {
    public:
        static bool initialize();
        static void shutdown();
        static PlatformInfo getPlatformInfo();
        static PlatformType getCurrentPlatform();
        
        // Window management
        static bool createWindow(int width, int height, const std::string& title);
        static void destroyWindow();
        static bool isWindowOpen();
        static void swapBuffers();
        static void pollEvents();
        
        // Input handling
        static void setTouchCallback(std::function<void(int, float, float, bool)> callback);
        static void setKeyCallback(std::function<void(int, bool)> callback);
        static void setAccelerometerCallback(std::function<void(float, float, float)> callback);
        static void setGyroscopeCallback(std::function<void(float, float, float)> callback);
        
        // File system
        static std::string getAssetPath(const std::string& filename);
        static std::string getDataPath();
        static std::string getCachePath();
        static bool fileExists(const std::string& path);
        static std::vector<uint8_t> readFile(const std::string& path);
        
        // Audio
        static bool initializeAudio();
        static void shutdownAudio();
        static void playSound(const std::string& filename);
        static void playMusic(const std::string& filename);
        static void stopMusic();
        static void setVolume(float volume);
        
        // Utilities
        static void showToast(const std::string& message);
        static void vibrate(int duration = 100);
        static void setScreenOrientation(int orientation);
        static void setKeepScreenOn(bool keepOn);
        
        // Performance
        static void setTargetFPS(int fps);
        static int getCurrentFPS();
        static float getBatteryLevel();
        static bool isCharging();
        
    private:
        static PlatformInfo s_platformInfo;
        static bool s_initialized;
    };

    // Mobile-specific input handling
    class TouchInput {
    public:
        struct TouchPoint {
            int id;
            float x, y;
            float pressure;
            bool isActive;
        };

        static void update();
        static bool isTouching();
        static TouchPoint getTouchPoint(int index);
        static int getTouchCount();
        static Vector2 getTouchPosition(int index = 0);
        static Vector2 getTouchDelta(int index = 0);
        static bool isTouchJustPressed(int index = 0);
        static bool isTouchJustReleased(int index = 0);
        static bool isTouchHeld(int index = 0);
        
        // Gesture recognition
        static bool isPinching();
        static float getPinchScale();
        static bool isPanning();
        static Vector2 getPanDelta();
        static bool isRotating();
        static float getRotationAngle();
        
    private:
        static std::vector<TouchPoint> s_touchPoints;
        static std::vector<TouchPoint> s_previousTouchPoints;
        static bool s_gestureActive;
    };

    // Mobile UI elements
    class MobileUI {
    public:
        enum class ButtonType {
            Accelerate,
            Brake,
            SteerLeft,
            SteerRight,
            Boost,
            Handbrake,
            Attack1,
            Attack2,
            Shield,
            Teleport,
            Pause,
            Menu
        };

        struct UIButton {
            ButtonType type;
            float x, y, width, height;
            bool isPressed;
            bool isVisible;
            std::string label;
            Vector3 color;
        };

        static void initialize();
        static void shutdown();
        static void update(float deltaTime);
        static void render();
        
        // Button management
        static void addButton(ButtonType type, float x, float y, float width, float height, const std::string& label);
        static void removeButton(ButtonType type);
        static void setButtonVisible(ButtonType type, bool visible);
        static void setButtonPosition(ButtonType type, float x, float y);
        static void setButtonSize(ButtonType type, float width, float height);
        
        // Input handling
        static bool isButtonPressed(ButtonType type);
        static bool isButtonJustPressed(ButtonType type);
        static bool isButtonJustReleased(ButtonType type);
        static Vector2 getButtonPosition(ButtonType type);
        
        // Virtual joystick
        static void setJoystickVisible(bool visible);
        static void setJoystickPosition(float x, float y);
        static Vector2 getJoystickInput();
        static bool isJoystickActive();
        
        // UI scaling
        static void setUIScale(float scale);
        static float getUIScale();
        static void setSafeArea(float top, float bottom, float left, float right);
        
    private:
        static std::vector<UIButton> s_buttons;
        static bool s_joystickVisible;
        static Vector2 s_joystickPosition;
        static Vector2 s_joystickInput;
        static float s_uiScale;
        static bool s_initialized;
    };
}