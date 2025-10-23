#include "TouchInputManager.h"
#include <algorithm>
#include <cmath>
#include <string>

TouchInputManager::TouchInputManager()
    : screenWidth(1920.0f)
    , screenHeight(1080.0f)
    , touchSensitivity(1.0f)
    , joystickDeadzone(0.15f)
    , cameraSensitivity(2.0f)
    , doubleTapTimeThreshold(0.3)
    , longPressTimeThreshold(0.5)
    , swipeDistanceThreshold(50.0f) {
    
    leftJoystick = {Vector2(0, 0), Vector2(0, 0), 100.0f, 40.0f, false, -1, Vector2(0, 0), 0.0f, true};
    rightJoystick = {Vector2(0, 0), Vector2(0, 0), 100.0f, 40.0f, false, -1, Vector2(0, 0), 0.0f, false};
    lastCameraTouchPosition = Vector2(0, 0);
    cameraTouchDelta = Vector2(0, 0);
    smoothedMovementInput = Vector2(0, 0);
    movementSmoothingFactor = 0.1f;
}

TouchInputManager::~TouchInputManager() {
    shutdown();
}

void TouchInputManager::initialize(float width, float height) {
    screenWidth = width;
    screenHeight = height;
    
    // Setup screen halves
    setupScreenHalves();
    
    // Setup default virtual controls
    // Left joystick for movement (dynamic, appears where touched)
    setupDynamicJoystick(80.0f, 30.0f);
    
    // Right half screen for camera control (large area covering right half)
    setupRightJoystick(Vector2(width * 0.75f, height * 0.5f), width * 0.5f, 0.0f);
    
    // Set sensitivity and smoothing
    setCameraSensitivity(2.0f);
    setMovementSmoothing(0.1f);
    
    // Action buttons (right side)
    addButton("accelerate", Vector2(width - 100.0f, height - 400.0f), 60.0f);
    addButton("brake", Vector2(width - 100.0f, height - 280.0f), 60.0f);
    addButton("boost", Vector2(width - 220.0f, height - 340.0f), 50.0f);
    addButton("handbrake", Vector2(width - 340.0f, height - 340.0f), 50.0f);
    
    // Combat buttons (if needed)
    addButton("attack1", Vector2(width - 100.0f, 200.0f), 50.0f);
    addButton("attack2", Vector2(width - 220.0f, 200.0f), 50.0f);
    addButton("shield", Vector2(width - 100.0f, 320.0f), 50.0f);
    addButton("teleport", Vector2(width - 220.0f, 320.0f), 50.0f);
    
    // Menu buttons (top)
    addButton("pause", Vector2(width - 70.0f, 70.0f), 40.0f);
    addButton("reset", Vector2(width - 150.0f, 70.0f), 40.0f);
}

void TouchInputManager::shutdown() {
    activeTouches.clear();
    currentGestures.clear();
    buttons.clear();
}

void TouchInputManager::update(float deltaTime) {
    updateVirtualControls();
    detectGestures();
    
    // Smooth movement input
    Vector2 targetMovement = leftJoystick.direction * leftJoystick.magnitude;
    smoothedMovementInput = smoothedMovementInput + (targetMovement - smoothedMovementInput) * movementSmoothingFactor;
    
    // Apply deadzone to smoothed movement
    if (smoothedMovementInput.magnitude() < joystickDeadzone) {
        smoothedMovementInput = Vector2(0, 0);
    } else {
        // Normalize after deadzone
        float magnitude = smoothedMovementInput.magnitude();
        smoothedMovementInput = smoothedMovementInput.normalized() * std::min(1.0f, (magnitude - joystickDeadzone) / (1.0f - joystickDeadzone));
    }
    
    // Reset camera delta
    cameraTouchDelta = Vector2(0, 0);
}

void TouchInputManager::registerTouch(int touchId, float x, float y, TouchPhase phase, float pressure) {
    Touch touch;
    touch.id = touchId;
    touch.position = Vector2(x, y);
    touch.previousPosition = Vector2(x, y);
    touch.deltaPosition = Vector2(0, 0);
    touch.phase = phase;
    touch.pressure = pressure;
    touch.timestamp = 0.0; // Should be set from platform
    
    activeTouches[touchId] = touch;
    
    if (onTouchBegan) {
        onTouchBegan(touch);
    }
    
    // Process for virtual controls
    processTouchForJoystick(touch, leftJoystick);
    processTouchForJoystick(touch, rightJoystick);
    processTouchForButtons(touch);
}

void TouchInputManager::updateTouch(int touchId, float x, float y) {
    auto it = activeTouches.find(touchId);
    if (it != activeTouches.end()) {
        Touch& touch = it->second;
        touch.previousPosition = touch.position;
        touch.position = Vector2(x, y);
        touch.deltaPosition = touch.position - touch.previousPosition;
        touch.phase = TouchPhase::Moved;
        
        // Track camera touch delta for right half screen
        if (rightJoystick.isActive && rightJoystick.touchId == touchId) {
            cameraTouchDelta = touch.deltaPosition;
            lastCameraTouchPosition = touch.position;
        }
        
        if (onTouchMoved) {
            onTouchMoved(touch);
        }
    }
}

void TouchInputManager::endTouch(int touchId) {
    auto it = activeTouches.find(touchId);
    if (it != activeTouches.end()) {
        Touch& touch = it->second;
        touch.phase = TouchPhase::Ended;
        
        if (onTouchEnded) {
            onTouchEnded(touch);
        }
        
        // Release joysticks
        if (leftJoystick.touchId == touchId) {
            leftJoystick.isActive = false;
            leftJoystick.touchId = -1;
            leftJoystick.currentPosition = leftJoystick.centerPosition;
            leftJoystick.direction = Vector2(0, 0);
            leftJoystick.magnitude = 0.0f;
        }
        if (rightJoystick.touchId == touchId) {
            rightJoystick.isActive = false;
            rightJoystick.touchId = -1;
            rightJoystick.currentPosition = rightJoystick.centerPosition;
            rightJoystick.direction = Vector2(0, 0);
            rightJoystick.magnitude = 0.0f;
        }
        
        // Release buttons
        for (auto& button : buttons) {
            if (button.touchId == touchId) {
                button.wasPressed = button.isPressed;
                button.isPressed = false;
                button.touchId = -1;
            }
        }
        
        activeTouches.erase(it);
    }
}

void TouchInputManager::cancelTouch(int touchId) {
    endTouch(touchId);
}

bool TouchInputManager::isTouchActive(int touchId) const {
    return activeTouches.find(touchId) != activeTouches.end();
}

TouchInputManager::Touch TouchInputManager::getTouch(int touchId) const {
    auto it = activeTouches.find(touchId);
    if (it != activeTouches.end()) {
        return it->second;
    }
    return Touch{-1, Vector2(0, 0), Vector2(0, 0), Vector2(0, 0), TouchPhase::Ended, 0.0f, 0.0};
}

std::vector<TouchInputManager::Touch> TouchInputManager::getAllTouches() const {
    std::vector<Touch> touches;
    for (const auto& pair : activeTouches) {
        touches.push_back(pair.second);
    }
    return touches;
}

int TouchInputManager::getTouchCount() const {
    return static_cast<int>(activeTouches.size());
}

void TouchInputManager::setupLeftJoystick(Vector2 center, float outerRadius, float innerRadius) {
    leftJoystick.centerPosition = center;
    leftJoystick.currentPosition = center;
    leftJoystick.outerRadius = outerRadius;
    leftJoystick.innerRadius = innerRadius;
    leftJoystick.isActive = false;
    leftJoystick.touchId = -1;
    leftJoystick.direction = Vector2(0, 0);
    leftJoystick.magnitude = 0.0f;
}

void TouchInputManager::setupRightJoystick(Vector2 center, float outerRadius, float innerRadius) {
    rightJoystick.centerPosition = center;
    rightJoystick.currentPosition = center;
    rightJoystick.outerRadius = outerRadius;
    rightJoystick.innerRadius = innerRadius;
    rightJoystick.isActive = false;
    rightJoystick.touchId = -1;
    rightJoystick.direction = Vector2(0, 0);
    rightJoystick.magnitude = 0.0f;
    rightJoystick.isDynamic = false;
}

void TouchInputManager::setupDynamicJoystick(float outerRadius, float innerRadius) {
    leftJoystick.outerRadius = outerRadius;
    leftJoystick.innerRadius = innerRadius;
    leftJoystick.isActive = false;
    leftJoystick.touchId = -1;
    leftJoystick.direction = Vector2(0, 0);
    leftJoystick.magnitude = 0.0f;
    leftJoystick.isDynamic = true;
}

void TouchInputManager::setupScreenHalves() {
    leftHalfStart = Vector2(0, 0);
    leftHalfEnd = Vector2(screenWidth * 0.5f, screenHeight);
    rightHalfStart = Vector2(screenWidth * 0.5f, 0);
    rightHalfEnd = Vector2(screenWidth, screenHeight);
}

void TouchInputManager::addButton(const std::string& label, Vector2 position, float radius) {
    VirtualButton button;
    button.position = position;
    button.radius = radius;
    button.isPressed = false;
    button.wasPressed = false;
    button.touchId = -1;
    button.label = label;
    buttons.push_back(button);
}

void TouchInputManager::removeButton(const std::string& label) {
    buttons.erase(
        std::remove_if(buttons.begin(), buttons.end(),
            [&label](const VirtualButton& btn) { return btn.label == label; }),
        buttons.end()
    );
}

Vector2 TouchInputManager::getLeftJoystickDirection() const {
    return leftJoystick.direction;
}

float TouchInputManager::getLeftJoystickMagnitude() const {
    return leftJoystick.magnitude;
}

Vector2 TouchInputManager::getRightJoystickDirection() const {
    return rightJoystick.direction;
}

float TouchInputManager::getRightJoystickMagnitude() const {
    return rightJoystick.magnitude;
}

Vector2 TouchInputManager::getCameraTouchDirection() const {
    return rightJoystick.direction;
}

float TouchInputManager::getCameraTouchMagnitude() const {
    return rightJoystick.magnitude;
}

Vector2 TouchInputManager::getCameraTouchDelta() const {
    return cameraTouchDelta;
}

Vector2 TouchInputManager::getMovementInput() const {
    return smoothedMovementInput;
}

bool TouchInputManager::isButtonPressed(const std::string& label) const {
    for (const auto& button : buttons) {
        if (button.label == label) {
            return button.isPressed;
        }
    }
    return false;
}

bool TouchInputManager::isButtonJustPressed(const std::string& label) const {
    for (const auto& button : buttons) {
        if (button.label == label) {
            return button.isPressed && !button.wasPressed;
        }
    }
    return false;
}

bool TouchInputManager::isButtonJustReleased(const std::string& label) const {
    for (const auto& button : buttons) {
        if (button.label == label) {
            return !button.isPressed && button.wasPressed;
        }
    }
    return false;
}

bool TouchInputManager::hasGesture(GestureType type) const {
    for (const auto& gesture : currentGestures) {
        if (gesture.type == type) {
            return true;
        }
    }
    return false;
}

TouchInputManager::Gesture TouchInputManager::getGesture(GestureType type) const {
    for (const auto& gesture : currentGestures) {
        if (gesture.type == type) {
            return gesture;
        }
    }
    return Gesture{type, Vector2(0, 0), Vector2(0, 0), 1.0f, 0.0f, 0.0};
}

void TouchInputManager::setScreenSize(float width, float height) {
    screenWidth = width;
    screenHeight = height;
}

void TouchInputManager::setTouchSensitivity(float sensitivity) {
    touchSensitivity = sensitivity;
}

void TouchInputManager::setJoystickDeadzone(float deadzone) {
    joystickDeadzone = deadzone;
}

void TouchInputManager::setCameraSensitivity(float sensitivity) {
    cameraSensitivity = std::max(0.1f, sensitivity);
}

void TouchInputManager::setMovementSmoothing(float factor) {
    movementSmoothingFactor = std::clamp(factor, 0.01f, 1.0f);
}

void TouchInputManager::setTouchBeganCallback(std::function<void(const Touch&)> callback) {
    onTouchBegan = callback;
}

void TouchInputManager::setTouchMovedCallback(std::function<void(const Touch&)> callback) {
    onTouchMoved = callback;
}

void TouchInputManager::setTouchEndedCallback(std::function<void(const Touch&)> callback) {
    onTouchEnded = callback;
}

void TouchInputManager::setGestureCallback(std::function<void(const Gesture&)> callback) {
    onGesture = callback;
}

void TouchInputManager::updateVirtualControls() {
    updateJoystick(leftJoystick);
    updateJoystick(rightJoystick);
    updateButtons();
}

void TouchInputManager::updateJoystick(VirtualJoystick& joystick) {
    if (joystick.isActive && joystick.touchId >= 0) {
        auto it = activeTouches.find(joystick.touchId);
        if (it != activeTouches.end()) {
            const Touch& touch = it->second;
            joystick.currentPosition = touch.position;
            
            // Calculate direction and magnitude
            Vector2 delta = joystick.currentPosition - joystick.centerPosition;
            float distance = delta.magnitude();
            
            // Clamp to outer radius
            if (distance > joystick.outerRadius) {
                delta = delta.normalized() * joystick.outerRadius;
                joystick.currentPosition = joystick.centerPosition + delta;
                distance = joystick.outerRadius;
            }
            
            // Apply deadzone
            if (distance < joystickDeadzone * joystick.outerRadius) {
                joystick.direction = Vector2(0, 0);
                joystick.magnitude = 0.0f;
            } else {
                joystick.direction = delta.normalized();
                joystick.magnitude = (distance - joystickDeadzone * joystick.outerRadius) / 
                                    (joystick.outerRadius * (1.0f - joystickDeadzone));
                joystick.magnitude = std::min(1.0f, joystick.magnitude);
            }
        }
    }
}

void TouchInputManager::updateButtons() {
    for (auto& button : buttons) {
        button.wasPressed = button.isPressed;
        
        if (button.touchId >= 0) {
            // Check if touch is still on button
            auto it = activeTouches.find(button.touchId);
            if (it != activeTouches.end()) {
                const Touch& touch = it->second;
                button.isPressed = isTouchInCircle(touch.position, button.position, button.radius);
            }
        }
    }
}

void TouchInputManager::detectGestures() {
    currentGestures.clear();
    
    // Implement gesture detection (tap, swipe, pinch, etc.)
    // This is a simplified version - full implementation would track touch history
    
    if (activeTouches.size() == 2) {
        // Detect pinch/zoom gesture
        auto it = activeTouches.begin();
        const Touch& touch1 = it->second;
        ++it;
        const Touch& touch2 = it->second;
        
        float currentDistance = calculateDistance(touch1.position, touch2.position);
        float previousDistance = calculateDistance(touch1.previousPosition, touch2.previousPosition);
        
        if (std::abs(currentDistance - previousDistance) > 5.0f) {
            Gesture pinch;
            pinch.type = GestureType::Pinch;
            pinch.position = (touch1.position + touch2.position) * 0.5f;
            pinch.scale = currentDistance / std::max(1.0f, previousDistance);
            pinch.timestamp = 0.0;
            currentGestures.push_back(pinch);
        }
    }
}

void TouchInputManager::processTouchForJoystick(const Touch& touch, VirtualJoystick& joystick) {
    if (touch.phase == TouchPhase::Began) {
        if (!joystick.isActive) {
            bool shouldActivate = false;
            
            if (joystick.isDynamic) {
                // Dynamic joystick - check if touch is in left half of screen
                if (isTouchInLeftHalf(touch.position)) {
                    joystick.centerPosition = touch.position;
                    joystick.currentPosition = touch.position;
                    shouldActivate = true;
                }
            } else {
                // Fixed joystick - check if touch is in right half of screen for camera control
                if (isTouchInRightHalf(touch.position)) {
                    shouldActivate = true;
                }
            }
            
            if (shouldActivate) {
                joystick.isActive = true;
                joystick.touchId = touch.id;
                joystick.currentPosition = touch.position;
            }
        }
    }
}

void TouchInputManager::processTouchForButtons(const Touch& touch) {
    if (touch.phase == TouchPhase::Began) {
        for (auto& button : buttons) {
            if (!button.isPressed && isTouchInCircle(touch.position, button.position, button.radius)) {
                button.isPressed = true;
                button.touchId = touch.id;
                break; // Only one button per touch
            }
        }
    }
}

bool TouchInputManager::isTouchInCircle(Vector2 touchPos, Vector2 center, float radius) const {
    return calculateDistance(touchPos, center) <= radius;
}

bool TouchInputManager::isTouchInRightHalf(Vector2 touchPos) const {
    return touchPos.x >= rightHalfStart.x && touchPos.x <= rightHalfEnd.x &&
           touchPos.y >= rightHalfStart.y && touchPos.y <= rightHalfEnd.y;
}

bool TouchInputManager::isTouchInLeftHalf(Vector2 touchPos) const {
    return touchPos.x >= leftHalfStart.x && touchPos.x <= leftHalfEnd.x &&
           touchPos.y >= leftHalfStart.y && touchPos.y <= leftHalfEnd.y;
}

float TouchInputManager::calculateDistance(Vector2 a, Vector2 b) const {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy);
}
