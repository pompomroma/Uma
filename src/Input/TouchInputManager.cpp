#include "TouchInputManager.h"
#include <cmath>
#include <algorithm>

TouchInputManager::TouchInputManager() 
    : currentLayout(ControlLayout::Racing)
    , isRecognizingGesture(false)
    , doubleTapThreshold(0.3f)
    , longPressThreshold(0.5f)
    , swipeThreshold(50.0f)
    , pinchThreshold(0.1f)
    , useTiltControls(false)
    , tiltSensitivity(1.0f)
    , screenWidth(1920)
    , screenHeight(1080)
    , joystickDeadzone(0.15f) {
    
    currentGesture.type = GestureType::None;
    currentGesture.isActive = false;
    
    leftJoystick.isActive = false;
    rightJoystick.isActive = false;
}

TouchInputManager::~TouchInputManager() {
    shutdown();
}

void TouchInputManager::initialize(int width, int height) {
    screenWidth = width;
    screenHeight = height;
    
    // Set up default control layout based on screen orientation
    if (width > height) {
        setControlLayout(ControlLayout::Racing);
    } else {
        setControlLayout(ControlLayout::Racing);
    }
}

void TouchInputManager::shutdown() {
    clearTouches();
    virtualControls.clear();
    virtualButtons.clear();
    touchZones.clear();
}

void TouchInputManager::handleTouchBegin(const PlatformBase::TouchPoint& touch) {
    currentTouches.push_back(touch);
    
    // Check virtual controls first
    bool handled = false;
    
    // Check virtual buttons
    for (auto& button : virtualButtons) {
        if (isTouchInButton(touch, button)) {
            button.isPressed = true;
            button.touchId = touch.id;
            if (button.callback) button.callback(true);
            if (onButtonInput) onButtonInput(button.label, true);
            handled = true;
            break;
        }
    }
    
    // Check virtual joysticks
    if (!handled) {
        if (!leftJoystick.isActive && isTouchInJoystick(touch, leftJoystick)) {
            leftJoystick.isActive = true;
            leftJoystick.touchId = touch.id;
            updateJoystick(leftJoystick, touch);
            handled = true;
        } else if (!rightJoystick.isActive && isTouchInJoystick(touch, rightJoystick)) {
            rightJoystick.isActive = true;
            rightJoystick.touchId = touch.id;
            updateJoystick(rightJoystick, touch);
            handled = true;
        }
    }
    
    // Start gesture recognition
    if (!handled) {
        currentGesture.startPosition = Vector2(touch.x, touch.y);
        currentGesture.currentPosition = currentGesture.startPosition;
        currentGesture.startTime = std::chrono::steady_clock::now();
        currentGesture.touchCount = currentTouches.size();
        isRecognizingGesture = true;
        
        // Check for tap/double tap
        detectTap(touch);
    }
}

void TouchInputManager::handleTouchMove(const PlatformBase::TouchPoint& touch) {
    // Update touch in list
    auto it = std::find_if(currentTouches.begin(), currentTouches.end(),
        [&touch](const PlatformBase::TouchPoint& t) { return t.id == touch.id; });
    if (it != currentTouches.end()) {
        *it = touch;
    }
    
    // Update virtual joysticks
    if (leftJoystick.isActive && leftJoystick.touchId == touch.id) {
        updateJoystick(leftJoystick, touch);
        if (onMovementInput) onMovementInput(leftJoystick.axis);
    }
    if (rightJoystick.isActive && rightJoystick.touchId == touch.id) {
        updateJoystick(rightJoystick, touch);
        if (onCameraInput) onCameraInput(rightJoystick.axis);
    }
    
    // Update gesture recognition
    if (isRecognizingGesture && currentTouches.size() == 1) {
        currentGesture.currentPosition = Vector2(touch.x, touch.y);
        currentGesture.delta = currentGesture.currentPosition - currentGesture.startPosition;
        
        // Check for swipe
        if (currentGesture.delta.magnitude() > swipeThreshold) {
            detectSwipe(touch);
        }
    }
}

void TouchInputManager::handleTouchEnd(const PlatformBase::TouchPoint& touch) {
    // Remove touch from list
    auto it = std::find_if(currentTouches.begin(), currentTouches.end(),
        [&touch](const PlatformBase::TouchPoint& t) { return t.id == touch.id; });
    if (it != currentTouches.end()) {
        currentTouches.erase(it);
    }
    
    // Release virtual buttons
    for (auto& button : virtualButtons) {
        if (button.touchId == touch.id) {
            button.isPressed = false;
            button.touchId = -1;
            if (button.callback) button.callback(false);
            if (onButtonInput) onButtonInput(button.label, false);
            break;
        }
    }
    
    // Release virtual joysticks
    if (leftJoystick.touchId == touch.id) {
        leftJoystick.isActive = false;
        leftJoystick.axis = Vector2::zero();
        leftJoystick.touchId = -1;
        if (onMovementInput) onMovementInput(Vector2::zero());
    }
    if (rightJoystick.touchId == touch.id) {
        rightJoystick.isActive = false;
        rightJoystick.axis = Vector2::zero();
        rightJoystick.touchId = -1;
        if (onCameraInput) onCameraInput(Vector2::zero());
    }
    
    // End gesture recognition
    if (currentTouches.empty()) {
        isRecognizingGesture = false;
        currentGesture.isActive = false;
        currentGesture.type = GestureType::None;
    }
}

void TouchInputManager::handleMultiTouch(const std::vector<PlatformBase::TouchPoint>& touches) {
    currentTouches = touches;
    
    if (touches.size() == 2) {
        detectPinch(touches);
        detectRotate(touches);
    }
}

void TouchInputManager::setControlLayout(ControlLayout layout) {
    currentLayout = layout;
    virtualButtons.clear();
    touchZones.clear();
    
    switch (layout) {
        case ControlLayout::Racing:
            setupRacingControls();
            break;
        case ControlLayout::Combat:
            setupCombatControls();
            break;
        case ControlLayout::Menu:
            setupMenuControls();
            break;
    }
}

void TouchInputManager::setupRacingControls() {
    float buttonSize = screenHeight * 0.15f;
    float margin = screenHeight * 0.05f;
    
    // Accelerate button (bottom right)
    VirtualButton accelButton;
    accelButton.position = Vector2(screenWidth - buttonSize - margin, screenHeight - buttonSize - margin);
    accelButton.radius = buttonSize * 0.5f;
    accelButton.label = "Accelerate";
    accelButton.callback = nullptr;
    virtualButtons.push_back(accelButton);
    
    // Brake button (bottom right, left of accelerate)
    VirtualButton brakeButton;
    brakeButton.position = Vector2(screenWidth - buttonSize * 2.5f - margin, screenHeight - buttonSize - margin);
    brakeButton.radius = buttonSize * 0.5f;
    brakeButton.label = "Brake";
    brakeButton.callback = nullptr;
    virtualButtons.push_back(brakeButton);
    
    // Boost button (above accelerate)
    VirtualButton boostButton;
    boostButton.position = Vector2(screenWidth - buttonSize - margin, screenHeight - buttonSize * 2.5f - margin);
    boostButton.radius = buttonSize * 0.4f;
    boostButton.label = "Boost";
    boostButton.callback = nullptr;
    virtualButtons.push_back(boostButton);
    
    // Steering zone (left side of screen for touch steering, or use tilt)
    if (!useTiltControls) {
        // Virtual steering wheel or left joystick for steering
        leftJoystick.center = Vector2(buttonSize + margin, screenHeight - buttonSize - margin);
        leftJoystick.radius = buttonSize;
        leftJoystick.isActive = false;
    }
    
    // Camera control (right side, but not overlapping with buttons)
    rightJoystick.center = Vector2(screenWidth * 0.75f, screenHeight * 0.5f);
    rightJoystick.radius = buttonSize;
    rightJoystick.isActive = false;
}

void TouchInputManager::setupCombatControls() {
    float stickSize = screenHeight * 0.2f;
    float buttonSize = screenHeight * 0.12f;
    float margin = screenHeight * 0.05f;
    
    // Movement joystick (bottom left)
    leftJoystick.center = Vector2(stickSize * 0.5f + margin, screenHeight - stickSize * 0.5f - margin);
    leftJoystick.radius = stickSize * 0.5f;
    leftJoystick.isActive = false;
    
    // Camera joystick (bottom right)
    rightJoystick.center = Vector2(screenWidth - stickSize * 0.5f - margin, screenHeight - stickSize * 0.5f - margin);
    rightJoystick.radius = stickSize * 0.5f;
    rightJoystick.isActive = false;
    
    // Action buttons (right side, above camera stick)
    float buttonX = screenWidth - buttonSize * 1.5f - margin;
    float buttonY = screenHeight - stickSize - buttonSize * 2 - margin;
    
    // Attack button
    VirtualButton attackBtn;
    attackBtn.position = Vector2(buttonX + buttonSize, buttonY);
    attackBtn.radius = buttonSize * 0.5f;
    attackBtn.label = "Attack";
    attackBtn.callback = nullptr;
    virtualButtons.push_back(attackBtn);
    
    // Shield button
    VirtualButton shieldBtn;
    shieldBtn.position = Vector2(buttonX, buttonY - buttonSize);
    shieldBtn.radius = buttonSize * 0.5f;
    shieldBtn.label = "Shield";
    shieldBtn.callback = nullptr;
    virtualButtons.push_back(shieldBtn);
    
    // Special button
    VirtualButton specialBtn;
    specialBtn.position = Vector2(buttonX + buttonSize * 2, buttonY - buttonSize);
    specialBtn.radius = buttonSize * 0.5f;
    specialBtn.label = "Special";
    specialBtn.callback = nullptr;
    virtualButtons.push_back(specialBtn);
    
    // Jump button
    VirtualButton jumpBtn;
    jumpBtn.position = Vector2(buttonX + buttonSize, buttonY - buttonSize * 2);
    jumpBtn.radius = buttonSize * 0.5f;
    jumpBtn.label = "Jump";
    jumpBtn.callback = nullptr;
    virtualButtons.push_back(jumpBtn);
}

void TouchInputManager::setupMenuControls() {
    // Simple touch zones for menu navigation
    virtualButtons.clear();
    // Menu controls are typically handled by direct touch on UI elements
}

void TouchInputManager::updateJoystick(VirtualJoystick& joystick, const PlatformBase::TouchPoint& touch) {
    Vector2 touchPos(touch.x, touch.y);
    Vector2 delta = touchPos - joystick.center;
    float distance = delta.magnitude();
    
    if (distance > joystick.radius) {
        delta = delta.normalized() * joystick.radius;
        distance = joystick.radius;
    }
    
    joystick.currentPosition = joystick.center + delta;
    
    // Apply deadzone
    if (distance < joystick.radius * joystickDeadzone) {
        joystick.axis = Vector2::zero();
    } else {
        float normalizedDistance = (distance - joystick.radius * joystickDeadzone) / 
                                  (joystick.radius * (1.0f - joystickDeadzone));
        joystick.axis = delta.normalized() * normalizedDistance;
    }
}

bool TouchInputManager::isTouchInButton(const PlatformBase::TouchPoint& touch, const VirtualButton& button) const {
    Vector2 touchPos(touch.x, touch.y);
    float distance = (touchPos - button.position).magnitude();
    return distance <= button.radius;
}

bool TouchInputManager::isTouchInJoystick(const PlatformBase::TouchPoint& touch, const VirtualJoystick& joystick) const {
    Vector2 touchPos(touch.x, touch.y);
    float distance = (touchPos - joystick.center).magnitude();
    return distance <= joystick.radius * 1.5f; // Slightly larger hit area
}

Vector2 TouchInputManager::getMovementInput() const {
    if (leftJoystick.isActive) {
        return leftJoystick.axis;
    }
    return Vector2::zero();
}

Vector2 TouchInputManager::getCameraInput() const {
    if (rightJoystick.isActive) {
        return rightJoystick.axis;
    }
    return Vector2::zero();
}

float TouchInputManager::getAccelerateInput() const {
    for (const auto& button : virtualButtons) {
        if (button.label == "Accelerate" && button.isPressed) {
            return 1.0f;
        }
    }
    return 0.0f;
}

float TouchInputManager::getBrakeInput() const {
    for (const auto& button : virtualButtons) {
        if (button.label == "Brake" && button.isPressed) {
            return 1.0f;
        }
    }
    return 0.0f;
}

float TouchInputManager::getSteerInput() const {
    if (useTiltControls) {
        return tiltInput.x;
    } else if (leftJoystick.isActive) {
        return leftJoystick.axis.x;
    }
    return 0.0f;
}

void TouchInputManager::handleAccelerometer(float x, float y, float z) {
    if (useTiltControls) {
        // Convert accelerometer input to steering
        // Assuming device is held in landscape
        tiltInput.x = -y * tiltSensitivity; // Tilt left/right
        tiltInput.y = x * tiltSensitivity;  // Tilt forward/back
        
        // Clamp values
        tiltInput.x = std::max(-1.0f, std::min(1.0f, tiltInput.x));
        tiltInput.y = std::max(-1.0f, std::min(1.0f, tiltInput.y));
    }
}

void TouchInputManager::detectTap(const PlatformBase::TouchPoint& touch) {
    auto now = std::chrono::steady_clock::now();
    Vector2 touchPos(touch.x, touch.y);
    
    // Check for double tap
    auto timeSinceLastTap = std::chrono::duration<float>(now - lastTapTime).count();
    float distanceFromLastTap = (touchPos - lastTapPosition).magnitude();
    
    if (timeSinceLastTap < doubleTapThreshold && distanceFromLastTap < 50.0f) {
        currentGesture.type = GestureType::DoubleTap;
        currentGesture.isActive = true;
        if (onGesture) onGesture(GestureType::DoubleTap, currentGesture);
    } else {
        currentGesture.type = GestureType::Tap;
        currentGesture.isActive = true;
        if (onGesture) onGesture(GestureType::Tap, currentGesture);
    }
    
    lastTapTime = now;
    lastTapPosition = touchPos;
}

void TouchInputManager::detectSwipe(const PlatformBase::TouchPoint& touch) {
    if (currentGesture.delta.magnitude() > swipeThreshold) {
        currentGesture.type = GestureType::Swipe;
        currentGesture.isActive = true;
        
        // Calculate swipe direction and velocity
        auto duration = std::chrono::duration<float>(
            std::chrono::steady_clock::now() - currentGesture.startTime).count();
        currentGesture.velocity = currentGesture.delta.magnitude() / duration;
        
        if (onGesture) onGesture(GestureType::Swipe, currentGesture);
    }
}

void TouchInputManager::detectPinch(const std::vector<PlatformBase::TouchPoint>& touches) {
    if (touches.size() != 2) return;
    
    Vector2 pos1(touches[0].x, touches[0].y);
    Vector2 pos2(touches[1].x, touches[1].y);
    float currentDistance = (pos2 - pos1).magnitude();
    
    if (currentGesture.type != GestureType::Pinch) {
        currentGesture.type = GestureType::Pinch;
        currentGesture.distance = currentDistance;
        currentGesture.scale = 1.0f;
        currentGesture.isActive = true;
    } else {
        float previousDistance = currentGesture.distance;
        currentGesture.scale = currentDistance / previousDistance;
        currentGesture.distance = currentDistance;
        
        if (onGesture) onGesture(GestureType::Pinch, currentGesture);
    }
}

void TouchInputManager::clearTouches() {
    currentTouches.clear();
    leftJoystick.isActive = false;
    rightJoystick.isActive = false;
    for (auto& button : virtualButtons) {
        button.isPressed = false;
        button.touchId = -1;
    }
}