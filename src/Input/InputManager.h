#pragma once
#include "../Math/Vector2.h"
#include <unordered_map>
#include <functional>
#include <vector>
#include <string>

#ifdef PLATFORM_MOBILE
#include <SDL2/SDL.h>
#endif

class InputManager {
public:
    enum class Key {
        W, A, S, D,
        Space, Shift, Ctrl,
        Up, Down, Left, Right,
        MouseLeft, MouseRight, MouseMiddle,
        Escape, Enter, Tab,
        F1, F2, F3, F4, F5,
        Count
    };

    enum class Action {
        Accelerate,
        Brake,
        SteerLeft,
        SteerRight,
        Boost,
        Handbrake,
        CameraLook,
        CameraZoom,
        Pause,
        Reset,
        // Combat actions
        Attack1,        // Fist attack
        Attack2,        // Laser attack
        Shield,         // Activate shield
        Teleport,       // Teleport ability
        Interact,       // Pick up items
        StatMenu,       // Open stats menu
        Count
    };

    struct MouseState {
        float x, y;
        float deltaX, deltaY;
        bool leftPressed, rightPressed, middlePressed;
        float scrollDelta;
    };

    struct GamepadState {
        float leftStickX, leftStickY;
        float rightStickX, rightStickY;
        float leftTrigger, rightTrigger;
        bool buttonA, buttonB, buttonX, buttonY;
        bool buttonStart, buttonSelect;
        bool dpadUp, dpadDown, dpadLeft, dpadRight;
    };

private:
    std::unordered_map<Key, bool> keyStates;
    std::unordered_map<Key, bool> previousKeyStates;
    std::unordered_map<Action, std::vector<Key>> actionBindings;
    
    MouseState mouseState;
    MouseState previousMouseState;
    GamepadState gamepadState;
    GamepadState previousGamepadState;
    
    bool isMouseLookActive;
    bool isInputEnabled;
    
    #ifdef PLATFORM_MOBILE
    // Touch input state (simple virtual controls)
    float touchSteerAxis = 0.0f;     // -1..1 (left/right)
    float touchThrottleAxis = 0.0f;  // 0..1
    float touchBrakeAxis = 0.0f;     // 0..1
    bool touchBoost = false;
    bool touchHandbrake = false;
    SDL_FingerID leftFingerId = -1;   // steering finger (left half)
    SDL_FingerID rightFingerId = -1;  // throttle/brake finger (right half)
    float leftStartX = 0.0f;
    float rightStartY = 0.0f;
    int screenWidthPx = 0;
    int screenHeightPx = 0;
    #endif
    
    // Input callbacks
    std::function<void(float)> onAccelerate;
    std::function<void(float)> onBrake;
    std::function<void(float)> onSteer;
    std::function<void(bool)> onBoost;
    std::function<void(bool)> onHandbrake;
    std::function<void(float, float)> onCameraLook;
    std::function<void(float)> onCameraZoom;
    std::function<void()> onPause;
    std::function<void()> onReset;
    
    // Combat callbacks
    std::function<void()> onAttack1;
    std::function<void()> onAttack2;
    std::function<void()> onShield;
    std::function<void()> onTeleport;
    std::function<void()> onInteract;
    std::function<void()> onStatMenu;

public:
    InputManager();
    ~InputManager();
    
    // Initialization
    void initialize();
    void shutdown();
    
    // Input processing
    void update(float deltaTime);
    void processKeyboardInput();
    void processMouseInput();
    void processGamepadInput();
    #ifdef PLATFORM_MOBILE
    void handleSDLEvent(const SDL_Event& e);
    void setScreenSize(int width, int height) { screenWidthPx = width; screenHeightPx = height; }
    #endif
    
    // Key state queries
    bool isKeyPressed(Key key) const;
    bool isKeyJustPressed(Key key) const;
    bool isKeyJustReleased(Key key) const;
    bool isActionPressed(Action action) const;
    bool isActionJustPressed(Action action) const;
    bool isActionJustReleased(Action action) const;
    
    // Mouse queries
    const MouseState& getMouseState() const { return mouseState; }
    float getMouseX() const { return mouseState.x; }
    float getMouseY() const { return mouseState.y; }
    float getMouseDeltaX() const { return mouseState.deltaX; }
    float getMouseDeltaY() const { return mouseState.deltaY; }
    bool isMouseButtonPressed(int button) const;
    bool isMouseButtonJustPressed(int button) const;
    bool isMouseButtonJustReleased(int button) const;
    float getMouseScrollDelta() const { return mouseState.scrollDelta; }
    
    // Gamepad queries
    const GamepadState& getGamepadState() const { return gamepadState; }
    float getLeftStickX() const { return gamepadState.leftStickX; }
    float getLeftStickY() const { return gamepadState.leftStickY; }
    float getRightStickX() const { return gamepadState.rightStickX; }
    float getRightStickY() const { return gamepadState.rightStickY; }
    float getLeftTrigger() const { return gamepadState.leftTrigger; }
    float getRightTrigger() const { return gamepadState.rightTrigger; }
    bool isGamepadButtonPressed(int button) const;
    bool isGamepadButtonJustPressed(int button) const;
    bool isGamepadButtonJustReleased(int button) const;
    
    // Input binding
    void bindKeyToAction(Key key, Action action);
    void unbindKeyFromAction(Key key, Action action);
    void setDefaultBindings();
    
    // Callback registration
    void setAccelerateCallback(std::function<void(float)> callback);
    void setBrakeCallback(std::function<void(float)> callback);
    void setSteerCallback(std::function<void(float)> callback);
    void setBoostCallback(std::function<void(bool)> callback);
    void setHandbrakeCallback(std::function<void(bool)> callback);
    void setCameraLookCallback(std::function<void(float, float)> callback);
    void setCameraZoomCallback(std::function<void(float)> callback);
    void setPauseCallback(std::function<void()> callback);
    void setResetCallback(std::function<void()> callback);
    
    // Combat callback registration
    void setAttack1Callback(std::function<void()> callback);
    void setAttack2Callback(std::function<void()> callback);
    void setShieldCallback(std::function<void()> callback);
    void setTeleportCallback(std::function<void()> callback);
    void setInteractCallback(std::function<void()> callback);
    void setStatMenuCallback(std::function<void()> callback);
    
    // Input state management
    void setMouseLookActive(bool active);
    void setInputEnabled(bool enabled);
    bool getMouseLookActive() const { return isMouseLookActive; }
    bool getInputEnabled() const { return isInputEnabled; }
    
    // Input values for racing game
    float getAccelerateInput() const;
    float getBrakeInput() const;
    float getSteerInput() const;
    bool getBoostInput() const;
    bool getHandbrakeInput() const;
    Vector2 getCameraLookInput() const;
    float getCameraZoomInput() const;
    
    // Utility functions
    void clearInputState();
    void resetToDefaults();
    
private:
    void updateKeyStates();
    void updateMouseState();
    void updateGamepadState();
    void processActionCallbacks();
    Key getKeyFromString(const std::string& keyName) const;
    Action getActionFromString(const std::string& actionName) const;
};