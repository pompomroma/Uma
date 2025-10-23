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
    , swipeDistanceThreshold(50.0f)
    , isCameraDragging(false)
    , cameraDragTouchId(-1)
    , cameraDragStartPos(0, 0)
    , cameraDragCurrentPos(0, 0)
    , cameraDragDelta(0, 0) {
    
    // Initialize dynamic joystick (position will be set when touched)
    leftJoystick = {Vector2(0, 0), Vector2(0, 0), 100.0f, 40.0f, false, -1, Vector2(0, 0), 0.0f, true, 0.0f};
}

TouchInputManager::~TouchInputManager() {
    shutdown();
}

void TouchInputManager::initialize(float width, float height) {
    screenWidth = width;
    screenHeight = height;
    
    // Setup dynamic joystick (no fixed position)
    setupDynamicJoystick(100.0f, 40.0f);
    
    // Combat buttons (bottom right corner for easy access)
    addButton("attack1", Vector2(width - 100.0f, height - 200.0f), 50.0f);
    addButton("attack2", Vector2(width - 220.0f, height - 200.0f), 50.0f);
    addButton("shield", Vector2(width - 100.0f, height - 320.0f), 50.0f);
    addButton("teleport", Vector2(width - 220.0f, height - 320.0f), 50.0f);
    
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
    
    // Fade out joystick when not in use
    if (!leftJoystick.isActive && leftJoystick.fadeAlpha > 0.0f) {
        leftJoystick.fadeAlpha -= deltaTime * 2.0f;
        if (leftJoystick.fadeAlpha < 0.0f) {
            leftJoystick.fadeAlpha = 0.0f;
        }
    }
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
    processTouchForControls(touch);
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
        
        // Release joystick
        if (leftJoystick.touchId == touchId) {
            leftJoystick.isActive = false;
            leftJoystick.touchId = -1;
            leftJoystick.currentPosition = leftJoystick.centerPosition;
            leftJoystick.direction = Vector2(0, 0);
            leftJoystick.magnitude = 0.0f;
        }
        
        // Release camera drag
        if (cameraDragTouchId == touchId) {
            isCameraDragging = false;
            cameraDragTouchId = -1;
            cameraDragDelta = Vector2(0, 0);
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

void TouchInputManager::setupDynamicJoystick(float outerRadius, float innerRadius) {
    leftJoystick.outerRadius = outerRadius;
    leftJoystick.innerRadius = innerRadius;
    leftJoystick.isActive = false;
    leftJoystick.touchId = -1;
    leftJoystick.direction = Vector2(0, 0);
    leftJoystick.magnitude = 0.0f;
    leftJoystick.isDynamic = true;
    leftJoystick.fadeAlpha = 0.0f;
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

Vector2 TouchInputManager::getJoystickDirection() const {
    return leftJoystick.direction;
}

float TouchInputManager::getJoystickMagnitude() const {
    return leftJoystick.magnitude;
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

void TouchInputManager::updateVirtualControls() {
    updateDynamicJoystick();
    updateCameraDrag();
    updateButtons();
}

void TouchInputManager::updateDynamicJoystick() {
    if (leftJoystick.isActive && leftJoystick.touchId >= 0) {
        auto it = activeTouches.find(leftJoystick.touchId);
        if (it != activeTouches.end()) {
            const Touch& touch = it->second;
            leftJoystick.currentPosition = touch.position;
            
            // Calculate direction and magnitude
            Vector2 delta = leftJoystick.currentPosition - leftJoystick.centerPosition;
            float distance = delta.magnitude();
            
            // Clamp to outer radius
            if (distance > leftJoystick.outerRadius) {
                delta = delta.normalized() * leftJoystick.outerRadius;
                leftJoystick.currentPosition = leftJoystick.centerPosition + delta;
                distance = leftJoystick.outerRadius;
            }
            
            // Apply deadzone
            if (distance < joystickDeadzone * leftJoystick.outerRadius) {
                leftJoystick.direction = Vector2(0, 0);
                leftJoystick.magnitude = 0.0f;
            } else {
                leftJoystick.direction = delta.normalized();
                leftJoystick.magnitude = (distance - joystickDeadzone * leftJoystick.outerRadius) / 
                                        (leftJoystick.outerRadius * (1.0f - joystickDeadzone));
                leftJoystick.magnitude = std::min(1.0f, leftJoystick.magnitude);
            }
            
            leftJoystick.fadeAlpha = 1.0f;
        }
    }
}

void TouchInputManager::updateCameraDrag() {
    if (isCameraDragging && cameraDragTouchId >= 0) {
        auto it = activeTouches.find(cameraDragTouchId);
        if (it != activeTouches.end()) {
            const Touch& touch = it->second;
            cameraDragCurrentPos = touch.position;
            
            // Calculate delta with sensitivity
            Vector2 rawDelta = cameraDragCurrentPos - cameraDragStartPos;
            cameraDragDelta = rawDelta * touchSensitivity * 0.01f;
            
            // Update start position for continuous dragging
            cameraDragStartPos = cameraDragCurrentPos;
        }
    } else {
        // Gradually reduce camera delta when not dragging
        cameraDragDelta *= 0.9f;
        if (cameraDragDelta.magnitude() < 0.001f) {
            cameraDragDelta = Vector2(0, 0);
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

void TouchInputManager::processTouchForControls(const Touch& touch) {
    if (touch.phase == TouchPhase::Began) {
        // Check if touch is on a button first
        bool touchOnButton = false;
        for (const auto& button : buttons) {
            if (isTouchInCircle(touch.position, button.position, button.radius)) {
                touchOnButton = true;
                break;
            }
        }
        
        if (!touchOnButton) {
            if (isTouchInLeftHalf(touch)) {
                // Dynamic joystick - appears where user touches
                if (!leftJoystick.isActive) {
                    leftJoystick.isActive = true;
                    leftJoystick.touchId = touch.id;
                    leftJoystick.centerPosition = touch.position;
                    leftJoystick.currentPosition = touch.position;
                    leftJoystick.fadeAlpha = 1.0f;
                }
            } else if (isTouchInRightHalf(touch)) {
                // Camera drag on right half
                if (!isCameraDragging) {
                    isCameraDragging = true;
                    cameraDragTouchId = touch.id;
                    cameraDragStartPos = touch.position;
                    cameraDragCurrentPos = touch.position;
                    cameraDragDelta = Vector2(0, 0);
                }
            }
        }
    }
}

bool TouchInputManager::isTouchInLeftHalf(const Touch& touch) const {
    return touch.position.x < screenWidth * 0.5f;
}

bool TouchInputManager::isTouchInRightHalf(const Touch& touch) const {
    return touch.position.x >= screenWidth * 0.5f;
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

float TouchInputManager::calculateDistance(Vector2 a, Vector2 b) const {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy);
}
