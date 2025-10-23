#include "TouchInputManager.h"
#include <algorithm>
#include <cmath>

TouchInputManager::TouchInputManager()
    : screenWidth(1920.0f)
    , screenHeight(1080.0f)
    , touchSensitivity(1.0f)
    , joystickDeadzone(0.15f)
    , doubleTapTimeThreshold(0.3)
    , longPressTimeThreshold(0.5)
    , swipeDistanceThreshold(50.0f) {
    
    leftJoystick = {Vector2(0, 0), Vector2(0, 0), 100.0f, 40.0f, false, -1, Vector2(0, 0), 0.0f, true, Vector2(0, 0)};
    rightJoystick = {Vector2(0, 0), Vector2(0, 0), 100.0f, 40.0f, false, -1, Vector2(0, 0), 0.0f, false, Vector2(0, 0)};
    
    // Initialize camera control area (right half of screen)
    cameraControlArea = {Vector2(0, 0), Vector2(0, 0), false, -1, Vector2(0, 0), Vector2(0, 0)};
}

TouchInputManager::~TouchInputManager() {
    shutdown();
}

void TouchInputManager::initialize(float width, float height) {
    screenWidth = width;
    screenHeight = height;
    
    // Setup dynamic joystick (will be created when user touches left side)
    setupDynamicJoystick(80.0f, 30.0f);
    
    // Setup camera control area (right half of screen)
    setupCameraControlArea(Vector2(width * 0.5f, 0), Vector2(width, height));
    
    // Combat buttons (right side, but not in camera drag area)
    addButton("attack1", Vector2(width - 80.0f, height - 200.0f), 40.0f);
    addButton("attack2", Vector2(width - 80.0f, height - 150.0f), 40.0f);
    addButton("shield", Vector2(width - 80.0f, height - 100.0f), 40.0f);
    addButton("teleport", Vector2(width - 80.0f, height - 50.0f), 40.0f);
    
    // Menu buttons (top left, outside camera area)
    addButton("pause", Vector2(70.0f, 70.0f), 40.0f);
    addButton("reset", Vector2(150.0f, 70.0f), 40.0f);
}

void TouchInputManager::shutdown() {
    activeTouches.clear();
    currentGestures.clear();
    buttons.clear();
}

void TouchInputManager::update(float deltaTime) {
    updateVirtualControls();
    updateScreenDrag();
    detectGestures();
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
    processTouchForButtons(touch);
    processTouchForScreenDrag(touch);
}

void TouchInputManager::updateTouch(int touchId, float x, float y) {
    auto it = activeTouches.find(touchId);
    if (it != activeTouches.end()) {
        Touch& touch = it->second;
        touch.previousPosition = touch.position;
        touch.position = Vector2(x, y);
        touch.deltaPosition = touch.position - touch.previousPosition;
        touch.phase = TouchPhase::Moved;
        
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
        
        // Release camera drag
        if (cameraControlArea.touchId == touchId) {
            cameraControlArea.isActive = false;
            cameraControlArea.touchId = -1;
            cameraControlArea.deltaPosition = Vector2(0, 0);
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
    leftJoystick.isDynamic = false;
    leftJoystick.initialTouchPosition = Vector2(0, 0);
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
    rightJoystick.initialTouchPosition = Vector2(0, 0);
}

void TouchInputManager::setupDynamicJoystick(float outerRadius, float innerRadius) {
    leftJoystick.outerRadius = outerRadius;
    leftJoystick.innerRadius = innerRadius;
    leftJoystick.isActive = false;
    leftJoystick.touchId = -1;
    leftJoystick.direction = Vector2(0, 0);
    leftJoystick.magnitude = 0.0f;
    leftJoystick.isDynamic = true;
    leftJoystick.initialTouchPosition = Vector2(0, 0);
}

void TouchInputManager::setupCameraControlArea(Vector2 topLeft, Vector2 bottomRight) {
    cameraControlArea.topLeft = topLeft;
    cameraControlArea.bottomRight = bottomRight;
    cameraControlArea.isActive = false;
    cameraControlArea.touchId = -1;
    cameraControlArea.lastPosition = Vector2(0, 0);
    cameraControlArea.deltaPosition = Vector2(0, 0);
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

Vector2 TouchInputManager::getCameraDragDelta() const {
    return cameraControlArea.deltaPosition;
}

bool TouchInputManager::isCameraDragActive() const {
    return cameraControlArea.isActive;
}

void TouchInputManager::updateVirtualControls() {
    updateJoystick(leftJoystick);
    updateButtons();
}

void TouchInputManager::updateScreenDrag() {
    if (cameraControlArea.isActive && cameraControlArea.touchId >= 0) {
        auto it = activeTouches.find(cameraControlArea.touchId);
        if (it != activeTouches.end()) {
            const Touch& touch = it->second;
            cameraControlArea.deltaPosition = touch.position - cameraControlArea.lastPosition;
            cameraControlArea.lastPosition = touch.position;
        }
    } else {
        cameraControlArea.deltaPosition = Vector2(0, 0);
    }
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
            if (joystick.isDynamic) {
                // For dynamic joystick, check if touch is in left half of screen and not in camera area
                if (touch.position.x < screenWidth * 0.5f && 
                    !isTouchInRectangle(touch.position, cameraControlArea.topLeft, cameraControlArea.bottomRight)) {
                    createDynamicJoystick(touch.position);
                    joystick.isActive = true;
                    joystick.touchId = touch.id;
                    joystick.currentPosition = touch.position;
                }
            } else {
                // For static joystick
                if (isTouchInCircle(touch.position, joystick.centerPosition, joystick.outerRadius)) {
                    joystick.isActive = true;
                    joystick.touchId = touch.id;
                    joystick.currentPosition = touch.position;
                }
            }
        }
    }
}

void TouchInputManager::processTouchForScreenDrag(const Touch& touch) {
    if (touch.phase == TouchPhase::Began) {
        if (!cameraControlArea.isActive && 
            isTouchInRectangle(touch.position, cameraControlArea.topLeft, cameraControlArea.bottomRight)) {
            // Make sure this touch isn't already used by buttons
            bool touchUsedByButton = false;
            for (const auto& button : buttons) {
                if (button.touchId == touch.id) {
                    touchUsedByButton = true;
                    break;
                }
            }
            
            if (!touchUsedByButton) {
                cameraControlArea.isActive = true;
                cameraControlArea.touchId = touch.id;
                cameraControlArea.lastPosition = touch.position;
                cameraControlArea.deltaPosition = Vector2(0, 0);
            }
        }
    }
}

void TouchInputManager::createDynamicJoystick(Vector2 touchPosition) {
    leftJoystick.centerPosition = touchPosition;
    leftJoystick.currentPosition = touchPosition;
    leftJoystick.initialTouchPosition = touchPosition;
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

bool TouchInputManager::isTouchInRectangle(Vector2 touchPos, Vector2 topLeft, Vector2 bottomRight) const {
    return touchPos.x >= topLeft.x && touchPos.x <= bottomRight.x &&
           touchPos.y >= topLeft.y && touchPos.y <= bottomRight.y;
}

float TouchInputManager::calculateDistance(Vector2 a, Vector2 b) const {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy);
}
