#include "InputManager.h"
#include <algorithm>
#include <cstring>

InputManager::InputManager() 
    : isMouseLookActive(false)
    , isInputEnabled(true) {
    // Initialize key states
    for (int i = 0; i < (int)Key::Count; i++) {
        keyStates[(Key)i] = false;
        previousKeyStates[(Key)i] = false;
    }
    
    // Initialize mouse state
    mouseState = {0.0f, 0.0f, 0.0f, 0.0f, false, false, false, 0.0f};
    previousMouseState = mouseState;
    
    // Initialize gamepad state
    gamepadState = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, false, false, false, false, false, false, false, false, false, false};
    previousGamepadState = gamepadState;
    
    setDefaultBindings();
}

InputManager::~InputManager() {
    shutdown();
}

void InputManager::initialize() {
    // Initialize input system
    // This would typically initialize GLFW input, DirectInput, XInput, etc.
    clearInputState();
}

void InputManager::shutdown() {
    // Cleanup input system
    clearInputState();
}

void InputManager::update(float deltaTime) {
    if (!isInputEnabled) return;
    
    updateKeyStates();
    updateMouseState();
    updateGamepadState();
    processActionCallbacks();
}

void InputManager::processKeyboardInput() {
    // This would process keyboard input from GLFW, DirectInput, etc.
    // For now, it's a placeholder
}

void InputManager::processMouseInput() {
    // This would process mouse input from GLFW, DirectInput, etc.
    // For now, it's a placeholder
}

void InputManager::processGamepadInput() {
    // This would process gamepad input from XInput, DirectInput, etc.
    // For now, it's a placeholder
}

bool InputManager::isKeyPressed(Key key) const {
    auto it = keyStates.find(key);
    return it != keyStates.end() ? it->second : false;
}

bool InputManager::isKeyJustPressed(Key key) const {
    bool current = isKeyPressed(key);
    bool previous = false;
    auto it = previousKeyStates.find(key);
    if (it != previousKeyStates.end()) {
        previous = it->second;
    }
    return current && !previous;
}

bool InputManager::isKeyJustReleased(Key key) const {
    bool current = isKeyPressed(key);
    bool previous = false;
    auto it = previousKeyStates.find(key);
    if (it != previousKeyStates.end()) {
        previous = it->second;
    }
    return !current && previous;
}

bool InputManager::isActionPressed(Action action) const {
    auto it = actionBindings.find(action);
    if (it == actionBindings.end()) return false;
    
    for (Key key : it->second) {
        if (isKeyPressed(key)) return true;
    }
    return false;
}

bool InputManager::isActionJustPressed(Action action) const {
    auto it = actionBindings.find(action);
    if (it == actionBindings.end()) return false;
    
    for (Key key : it->second) {
        if (isKeyJustPressed(key)) return true;
    }
    return false;
}

bool InputManager::isActionJustReleased(Action action) const {
    auto it = actionBindings.find(action);
    if (it == actionBindings.end()) return false;
    
    for (Key key : it->second) {
        if (isKeyJustReleased(key)) return true;
    }
    return false;
}

bool InputManager::isMouseButtonPressed(int button) const {
    switch (button) {
        case 0: return mouseState.leftPressed;
        case 1: return mouseState.rightPressed;
        case 2: return mouseState.middlePressed;
        default: return false;
    }
}

bool InputManager::isMouseButtonJustPressed(int button) const {
    bool current = isMouseButtonPressed(button);
    bool previous = false;
    switch (button) {
        case 0: previous = previousMouseState.leftPressed; break;
        case 1: previous = previousMouseState.rightPressed; break;
        case 2: previous = previousMouseState.middlePressed; break;
    }
    return current && !previous;
}

bool InputManager::isMouseButtonJustReleased(int button) const {
    bool current = isMouseButtonPressed(button);
    bool previous = false;
    switch (button) {
        case 0: previous = previousMouseState.leftPressed; break;
        case 1: previous = previousMouseState.rightPressed; break;
        case 2: previous = previousMouseState.middlePressed; break;
    }
    return !current && previous;
}

bool InputManager::isGamepadButtonPressed(int button) const {
    switch (button) {
        case 0: return gamepadState.buttonA;
        case 1: return gamepadState.buttonB;
        case 2: return gamepadState.buttonX;
        case 3: return gamepadState.buttonY;
        case 4: return gamepadState.buttonStart;
        case 5: return gamepadState.buttonSelect;
        default: return false;
    }
}

bool InputManager::isGamepadButtonJustPressed(int button) const {
    bool current = isGamepadButtonPressed(button);
    bool previous = false;
    switch (button) {
        case 0: previous = previousGamepadState.buttonA; break;
        case 1: previous = previousGamepadState.buttonB; break;
        case 2: previous = previousGamepadState.buttonX; break;
        case 3: previous = previousGamepadState.buttonY; break;
        case 4: previous = previousGamepadState.buttonStart; break;
        case 5: previous = previousGamepadState.buttonSelect; break;
    }
    return current && !previous;
}

bool InputManager::isGamepadButtonJustReleased(int button) const {
    bool current = isGamepadButtonPressed(button);
    bool previous = false;
    switch (button) {
        case 0: previous = previousGamepadState.buttonA; break;
        case 1: previous = previousGamepadState.buttonB; break;
        case 2: previous = previousGamepadState.buttonX; break;
        case 3: previous = previousGamepadState.buttonY; break;
        case 4: previous = previousGamepadState.buttonStart; break;
        case 5: previous = previousGamepadState.buttonSelect; break;
    }
    return !current && previous;
}

void InputManager::bindKeyToAction(Key key, Action action) {
    actionBindings[action].push_back(key);
}

void InputManager::unbindKeyFromAction(Key key, Action action) {
    auto it = actionBindings.find(action);
    if (it != actionBindings.end()) {
        auto& keys = it->second;
        keys.erase(std::remove(keys.begin(), keys.end(), key), keys.end());
    }
}

void InputManager::setDefaultBindings() {
    // Clear existing bindings
    actionBindings.clear();
    
    // Racing game controls
    bindKeyToAction(Key::W, Action::Accelerate);
    bindKeyToAction(Key::Up, Action::Accelerate);
    bindKeyToAction(Key::S, Action::Brake);
    bindKeyToAction(Key::Down, Action::Brake);
    bindKeyToAction(Key::A, Action::SteerLeft);
    bindKeyToAction(Key::Left, Action::SteerLeft);
    bindKeyToAction(Key::D, Action::SteerRight);
    bindKeyToAction(Key::Right, Action::SteerRight);
    bindKeyToAction(Key::Space, Action::Boost);
    bindKeyToAction(Key::Shift, Action::Handbrake);
    bindKeyToAction(Key::MouseLeft, Action::CameraLook);
    bindKeyToAction(Key::Escape, Action::Pause);
    bindKeyToAction(Key::F1, Action::Reset);

    // PvP combat controls
    bindKeyToAction(Key::Q, Action::Laser);      // Laser from hand
    bindKeyToAction(Key::F, Action::Punch);      // Fist punch
    bindKeyToAction(Key::E, Action::Shield);     // Hold to shield
    bindKeyToAction(Key::R, Action::Teleport);   // Teleport forward

    // Stat allocation (press while playing to spend points)
    bindKeyToAction(Key::Digit1, Action::IncreaseStrength);
    bindKeyToAction(Key::Digit2, Action::IncreaseDefense);
    bindKeyToAction(Key::Digit3, Action::IncreaseStamina);
    bindKeyToAction(Key::Digit4, Action::IncreaseAgility);
}

void InputManager::setAccelerateCallback(std::function<void(float)> callback) {
    onAccelerate = callback;
}

void InputManager::setBrakeCallback(std::function<void(float)> callback) {
    onBrake = callback;
}

void InputManager::setSteerCallback(std::function<void(float)> callback) {
    onSteer = callback;
}

void InputManager::setBoostCallback(std::function<void(bool)> callback) {
    onBoost = callback;
}

void InputManager::setHandbrakeCallback(std::function<void(bool)> callback) {
    onHandbrake = callback;
}

void InputManager::setCameraLookCallback(std::function<void(float, float)> callback) {
    onCameraLook = callback;
}

void InputManager::setCameraZoomCallback(std::function<void(float)> callback) {
    onCameraZoom = callback;
}

void InputManager::setPauseCallback(std::function<void()> callback) {
    onPause = callback;
}

void InputManager::setResetCallback(std::function<void()> callback) {
    onReset = callback;
}

void InputManager::setMouseLookActive(bool active) {
    isMouseLookActive = active;
}

void InputManager::setInputEnabled(bool enabled) {
    isInputEnabled = enabled;
}

float InputManager::getAccelerateInput() const {
    float input = 0.0f;
    
    // Keyboard input
    if (isKeyPressed(Key::W) || isKeyPressed(Key::Up)) {
        input = 1.0f;
    }
    
    // Gamepad input
    float rightTrigger = getRightTrigger();
    if (rightTrigger > 0.1f) {
        input = std::max(input, rightTrigger);
    }
    
    return input;
}

float InputManager::getBrakeInput() const {
    float input = 0.0f;
    
    // Keyboard input
    if (isKeyPressed(Key::S) || isKeyPressed(Key::Down)) {
        input = 1.0f;
    }
    
    // Gamepad input
    float leftTrigger = getLeftTrigger();
    if (leftTrigger > 0.1f) {
        input = std::max(input, leftTrigger);
    }
    
    return input;
}

float InputManager::getSteerInput() const {
    float input = 0.0f;
    
    // Keyboard input
    if (isKeyPressed(Key::A) || isKeyPressed(Key::Left)) {
        input -= 1.0f;
    }
    if (isKeyPressed(Key::D) || isKeyPressed(Key::Right)) {
        input += 1.0f;
    }
    
    // Gamepad input
    float leftStickX = getLeftStickX();
    if (std::abs(leftStickX) > 0.1f) {
        input = leftStickX;
    }
    
    return input;
}

bool InputManager::getBoostInput() const {
    return isKeyPressed(Key::Space) || isGamepadButtonPressed(0); // A button
}

bool InputManager::getHandbrakeInput() const {
    return isKeyPressed(Key::Shift) || isGamepadButtonPressed(1); // B button
}

Vector2 InputManager::getCameraLookInput() const {
    Vector2 input(0.0f, 0.0f);
    
    if (isMouseLookActive) {
        input.x = getMouseDeltaX();
        input.y = getMouseDeltaY();
    }
    
    // Gamepad right stick
    input.x += getRightStickX();
    input.y += getRightStickY();
    
    return input;
}

float InputManager::getCameraZoomInput() const {
    return getMouseScrollDelta();
}

void InputManager::clearInputState() {
    for (auto& pair : keyStates) {
        pair.second = false;
    }
    for (auto& pair : previousKeyStates) {
        pair.second = false;
    }
    
    mouseState = {0.0f, 0.0f, 0.0f, 0.0f, false, false, false, 0.0f};
    previousMouseState = mouseState;
    
    gamepadState = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, false, false, false, false, false, false, false, false, false, false};
    previousGamepadState = gamepadState;
}

void InputManager::resetToDefaults() {
    clearInputState();
    setDefaultBindings();
}

void InputManager::updateKeyStates() {
    // Store previous key states
    previousKeyStates = keyStates;
    
    // Update current key states
    // This would be implemented with actual input system calls
    processKeyboardInput();
}

void InputManager::updateMouseState() {
    // Store previous mouse state
    previousMouseState = mouseState;
    
    // Update current mouse state
    // This would be implemented with actual input system calls
    processMouseInput();
}

void InputManager::updateGamepadState() {
    // Store previous gamepad state
    previousGamepadState = gamepadState;
    
    // Update current gamepad state
    // This would be implemented with actual input system calls
    processGamepadInput();
}

void InputManager::processActionCallbacks() {
    // Process action callbacks
    if (onAccelerate) {
        onAccelerate(getAccelerateInput());
    }
    if (onBrake) {
        onBrake(getBrakeInput());
    }
    if (onSteer) {
        onSteer(getSteerInput());
    }
    if (onBoost) {
        onBoost(getBoostInput());
    }
    if (onHandbrake) {
        onHandbrake(getHandbrakeInput());
    }
    if (onCameraLook) {
        Vector2 lookInput = getCameraLookInput();
        onCameraLook(lookInput.x, lookInput.y);
    }
    if (onCameraZoom) {
        onCameraZoom(getCameraZoomInput());
    }
    if (onPause && isActionJustPressed(Action::Pause)) {
        onPause();
    }
    if (onReset && isActionJustPressed(Action::Reset)) {
        onReset();
    }
}

InputManager::Key InputManager::getKeyFromString(const std::string& keyName) const {
    // This would convert string names to Key enum values
    // For now, return a default
    return Key::W;
}

InputManager::Action InputManager::getActionFromString(const std::string& actionName) const {
    // This would convert string names to Action enum values
    // For now, return a default
    return Action::Accelerate;
}