#pragma once
#include <unordered_map>
#include <functional>
#include <vector>
#include "../Math/Vector2.h"

class InputManager {
public:
    enum class Key {
        W, A, S, D,
        Space, Shift, Ctrl,
        Up, Down, Left, Right,
        MouseLeft, MouseRight, MouseMiddle,
        Escape, Enter, Tab,
        F1, F2, F3, F4, F5,
        Q, E, R, T, Y, U, I, O, P,
        F, G, H, J, K, L,
        Z, X, C, V, B, N, M,
        Num1, Num2, Num3, Num4, Num5,
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
        FireLaser,
        FirePlasma,
        FireMissile,
        FireEnergyBall,
        FistAttack,
        ActivateShield,
        Teleport,
        Dash,
        EnergyBurst,
        Heal,
        ToggleCombatMode,
        AimUp,
        AimDown,
        AimLeft,
        AimRight,
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
    std::function<void()> onFireLaser;
    std::function<void()> onFirePlasma;
    std::function<void()> onFireMissile;
    std::function<void()> onFireEnergyBall;
    std::function<void()> onFistAttack;
    std::function<void()> onActivateShield;
    std::function<void(Vector2)> onTeleport; // Mouse position for teleport target
    std::function<void()> onDash;
    std::function<void()> onEnergyBurst;
    std::function<void()> onHeal;
    std::function<void(bool)> onToggleCombatMode;
    std::function<void(float, float)> onAim; // Aim direction input

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
    void setFireLaserCallback(std::function<void()> callback);
    void setFirePlasmaCallback(std::function<void()> callback);
    void setFireMissileCallback(std::function<void()> callback);
    void setFireEnergyBallCallback(std::function<void()> callback);
    void setFistAttackCallback(std::function<void()> callback);
    void setActivateShieldCallback(std::function<void()> callback);
    void setTeleportCallback(std::function<void(Vector2)> callback);
    void setDashCallback(std::function<void()> callback);
    void setEnergyBurstCallback(std::function<void()> callback);
    void setHealCallback(std::function<void()> callback);
    void setToggleCombatModeCallback(std::function<void(bool)> callback);
    void setAimCallback(std::function<void(float, float)> callback);
    
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
    
    // Combat input values
    Vector2 getAimInput() const;
    bool getCombatModeToggle() const;
    
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