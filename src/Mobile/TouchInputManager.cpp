#ifdef MOBILE_PLATFORM

#include "TouchInputManager.h"
#include <iostream>
#include <algorithm>
#include <cmath>

TouchInputManager::TouchInputManager() 
    : maxTouches(10), tapMaxDistance(20.0f), tapMaxDuration(0.5f),
      doubleTapMaxInterval(0.3f), longPressMinDuration(0.8f),
      swipeMinDistance(50.0f), swipeMaxDuration(1.0f), pinchMinDistance(10.0f),
      screenDensity(160.0f), isEnabled(true), showTouchZones(false) {
    
    activeTouches.reserve(maxTouches);
    previousTouches.reserve(maxTouches);
}

TouchInputManager::~TouchInputManager() {
    shutdown();
}

void TouchInputManager::initialize(const Vector2& screenSize, float screenDensity) {
    this->screenSize = screenSize;
    this->screenDensity = screenDensity;
    
    std::cout << "Initializing Touch Input Manager..." << std::endl;
    std::cout << "Screen Size: " << screenSize.x << "x" << screenSize.y << std::endl;
    std::cout << "Screen Density: " << screenDensity << " DPI" << std::endl;
    
    // Adjust thresholds based on screen density
    float densityScale = screenDensity / 160.0f;  // 160 DPI is baseline
    tapMaxDistance *= densityScale;
    swipeMinDistance *= densityScale;
    pinchMinDistance *= densityScale;
    
    // Setup racing controls
    setupRacingControls();
    
    std::cout << "Touch Input Manager initialized successfully!" << std::endl;
}

void TouchInputManager::shutdown() {
    activeTouches.clear();
    previousTouches.clear();
    activeGestures.clear();
    completedGestures.clear();
    touchZones.clear();
    
    std::cout << "Touch Input Manager shutdown complete." << std::endl;
}

void TouchInputManager::setScreenSize(const Vector2& size) {
    screenSize = size;
    
    // Recalculate touch zones for new screen size
    setupRacingControls();
}

void TouchInputManager::handleScreenRotation() {
    // Swap width and height
    std::swap(screenSize.x, screenSize.y);
    
    // Recalculate touch zones for rotated screen
    setupRacingControls();
    
    std::cout << "Screen rotation handled: " << screenSize.x << "x" << screenSize.y << std::endl;
}

void TouchInputManager::update(float deltaTime) {
    if (!isEnabled) return;
    
    // Store previous frame's touches
    previousTouches = activeTouches;
    
    // Update touch velocities
    updateTouchVelocities(deltaTime);
    
    // Update gesture recognition
    updateGestures(deltaTime);
    
    // Update racing controls
    processRacingControls();
    
    // Update virtual joysticks
    updateVirtualJoysticks();
    
    // Update touch zones
    updateTouchZones();
    
    // Trigger racing callbacks
    triggerRacingCallbacks();
}

void TouchInputManager::processTouchDown(int touchId, const Vector2& position, float pressure) {
    if (!isEnabled) return;
    
    addTouch(touchId, position, pressure);
    
    // Check if touch is in any virtual joystick
    if (!steeringJoystick.isActive && 
        isPointInCircle(position, steeringJoystick.center, steeringJoystick.outerRadius)) {
        steeringJoystick.isActive = true;
        steeringJoystick.activeTouchId = touchId;
        steeringJoystick.knobPosition = position;
    } else if (!cameraJoystick.isActive && 
               isPointInCircle(position, cameraJoystick.center, cameraJoystick.outerRadius)) {
        cameraJoystick.isActive = true;
        cameraJoystick.activeTouchId = touchId;
        cameraJoystick.knobPosition = position;
    }
}

void TouchInputManager::processTouchMove(int touchId, const Vector2& position, float pressure) {
    if (!isEnabled) return;
    
    updateTouch(touchId, position, pressure);
    
    // Update virtual joysticks
    if (steeringJoystick.isActive && steeringJoystick.activeTouchId == touchId) {
        steeringJoystick.knobPosition = clampToCircle(position, steeringJoystick.center, 
                                                     steeringJoystick.outerRadius);
    } else if (cameraJoystick.isActive && cameraJoystick.activeTouchId == touchId) {
        cameraJoystick.knobPosition = clampToCircle(position, cameraJoystick.center, 
                                                   cameraJoystick.outerRadius);
    }
}

void TouchInputManager::processTouchUp(int touchId, const Vector2& position) {
    if (!isEnabled) return;
    
    // Check for tap gestures before removing touch
    TouchPoint* touch = findTouch(touchId);
    if (touch) {
        recognizeTap(*touch);
    }
    
    // Deactivate virtual joysticks
    if (steeringJoystick.isActive && steeringJoystick.activeTouchId == touchId) {
        steeringJoystick.isActive = false;
        steeringJoystick.activeTouchId = -1;
        steeringJoystick.knobPosition = steeringJoystick.center;
    } else if (cameraJoystick.isActive && cameraJoystick.activeTouchId == touchId) {
        cameraJoystick.isActive = false;
        cameraJoystick.activeTouchId = -1;
        cameraJoystick.knobPosition = cameraJoystick.center;
    }
    
    removeTouch(touchId);
}

void TouchInputManager::processTouchCancel(int touchId) {
    processTouchUp(touchId, Vector2(0, 0));  // Treat cancel as touch up
}

void TouchInputManager::setupRacingControls() {
    touchZones.clear();
    
    float screenWidth = screenSize.x;
    float screenHeight = screenSize.y;
    
    // Setup touch zones based on screen size
    float buttonSize = std::min(screenWidth, screenHeight) * 0.15f;  // 15% of smaller dimension
    float margin = buttonSize * 0.5f;
    
    // Right side controls (accelerate, brake, boost)
    Vector2 acceleratePos(screenWidth - margin - buttonSize * 0.5f, 
                         screenHeight - margin - buttonSize * 0.5f);
    addTouchZone(acceleratePos, Vector2(buttonSize, buttonSize), RacingAction::Accelerate);
    
    Vector2 brakePos(screenWidth - margin - buttonSize * 0.5f, 
                    screenHeight - margin - buttonSize * 2.5f);
    addTouchZone(brakePos, Vector2(buttonSize, buttonSize), RacingAction::Brake);
    
    Vector2 boostPos(screenWidth - margin - buttonSize * 2.5f, 
                    screenHeight - margin - buttonSize * 0.5f);
    addTouchZone(boostPos, Vector2(buttonSize, buttonSize), RacingAction::Boost);
    
    // Left side controls (handbrake, pause)
    Vector2 handbrakePos(margin + buttonSize * 0.5f, 
                        screenHeight - margin - buttonSize * 0.5f);
    addTouchZone(handbrakePos, Vector2(buttonSize, buttonSize), RacingAction::Handbrake);
    
    Vector2 pausePos(margin + buttonSize * 0.5f, margin + buttonSize * 0.5f);
    addTouchZone(pausePos, Vector2(buttonSize * 0.8f, buttonSize * 0.8f), RacingAction::Pause);
    
    // Setup virtual joysticks
    setupVirtualJoysticks();
    
    std::cout << "Racing controls setup complete." << std::endl;
}

void TouchInputManager::setupVirtualJoysticks() {
    float joystickRadius = std::min(screenSize.x, screenSize.y) * 0.12f;  // 12% of smaller dimension
    float margin = joystickRadius * 1.5f;
    
    // Steering joystick (left side)
    steeringJoystick.center = Vector2(margin + joystickRadius, 
                                     screenSize.y * 0.5f);
    steeringJoystick.outerRadius = joystickRadius;
    steeringJoystick.innerRadius = joystickRadius * 0.4f;
    steeringJoystick.knobPosition = steeringJoystick.center;
    steeringJoystick.isActive = false;
    
    // Camera joystick (right side)
    cameraJoystick.center = Vector2(screenSize.x - margin - joystickRadius, 
                                   screenSize.y * 0.3f);
    cameraJoystick.outerRadius = joystickRadius;
    cameraJoystick.innerRadius = joystickRadius * 0.4f;
    cameraJoystick.knobPosition = cameraJoystick.center;
    cameraJoystick.isActive = false;
    
    std::cout << "Virtual joysticks setup complete." << std::endl;
}

void TouchInputManager::addTouchZone(const Vector2& center, const Vector2& size, RacingAction action) {
    TouchZone zone(center, size, action);
    touchZones.push_back(zone);
}

void TouchInputManager::updateTouchZones() {
    for (auto& zone : touchZones) {
        zone.isActive = false;
        zone.currentValue = 0.0f;
        
        // Check if any active touch is in this zone
        for (const auto& touch : activeTouches) {
            if (touch.isActive && isPointInZone(touch.position, zone)) {
                zone.isActive = true;
                zone.currentValue = calculateTouchPressure(touch.position, zone);
                break;
            }
        }
    }
}

void TouchInputManager::updateVirtualJoysticks() {
    // Update steering joystick
    if (steeringJoystick.isActive) {
        TouchPoint* touch = findTouch(steeringJoystick.activeTouchId);
        if (touch && touch->isActive) {
            processVirtualJoystick(steeringJoystick, *touch);
        }
    }
    
    // Update camera joystick
    if (cameraJoystick.isActive) {
        TouchPoint* touch = findTouch(cameraJoystick.activeTouchId);
        if (touch && touch->isActive) {
            processVirtualJoystick(cameraJoystick, *touch);
        }
    }
}

void TouchInputManager::processVirtualJoystick(VirtualJoystick& joystick, const TouchPoint& touch) {
    Vector2 offset = touch.position - joystick.center;
    float distance = getDistance(Vector2(0, 0), offset);
    
    if (distance > joystick.outerRadius) {
        // Clamp to outer radius
        offset = offset * (joystick.outerRadius / distance);
        joystick.knobPosition = joystick.center + offset;
    } else {
        joystick.knobPosition = touch.position;
    }
}

Vector2 TouchInputManager::getSteeringInput() const {
    if (!steeringJoystick.isActive) {
        return Vector2(0, 0);
    }
    
    Vector2 offset = steeringJoystick.knobPosition - steeringJoystick.center;
    float distance = getDistance(Vector2(0, 0), offset);
    
    if (distance < steeringJoystick.innerRadius) {
        return Vector2(0, 0);  // Dead zone
    }
    
    // Normalize to -1 to 1 range
    return offset / steeringJoystick.outerRadius;
}

Vector2 TouchInputManager::getCameraInput() const {
    if (!cameraJoystick.isActive) {
        return Vector2(0, 0);
    }
    
    Vector2 offset = cameraJoystick.knobPosition - cameraJoystick.center;
    float distance = getDistance(Vector2(0, 0), offset);
    
    if (distance < cameraJoystick.innerRadius) {
        return Vector2(0, 0);  // Dead zone
    }
    
    // Normalize to -1 to 1 range
    return offset / cameraJoystick.outerRadius;
}

float TouchInputManager::getAccelerateInput() const {
    TouchZone* zone = const_cast<TouchInputManager*>(this)->getTouchZone(RacingAction::Accelerate);
    return zone ? zone->currentValue : 0.0f;
}

float TouchInputManager::getBrakeInput() const {
    TouchZone* zone = const_cast<TouchInputManager*>(this)->getTouchZone(RacingAction::Brake);
    return zone ? zone->currentValue : 0.0f;
}

float TouchInputManager::getSteerInput() const {
    Vector2 steerInput = getSteeringInput();
    return steerInput.x;  // Horizontal component for steering
}

bool TouchInputManager::getBoostInput() const {
    TouchZone* zone = const_cast<TouchInputManager*>(this)->getTouchZone(RacingAction::Boost);
    return zone ? zone->isActive : false;
}

bool TouchInputManager::getHandbrakeInput() const {
    TouchZone* zone = const_cast<TouchInputManager*>(this)->getTouchZone(RacingAction::Handbrake);
    return zone ? zone->isActive : false;
}

Vector2 TouchInputManager::getCameraLookInput() const {
    return getCameraInput();
}

float TouchInputManager::getCameraZoomInput() const {
    // Implement pinch-to-zoom gesture
    for (const auto& gesture : activeGestures) {
        if (gesture.type == GestureType::Pinch) {
            return (gesture.scale - 1.0f) * 2.0f;  // Convert scale to zoom delta
        }
    }
    return 0.0f;
}

TouchInputManager::TouchZone* TouchInputManager::getTouchZone(RacingAction action) {
    for (auto& zone : touchZones) {
        if (zone.action == action) {
            return &zone;
        }
    }
    return nullptr;
}

void TouchInputManager::processRacingControls() {
    updateTouchZoneValues();
}

void TouchInputManager::updateTouchZoneValues() {
    for (auto& zone : touchZones) {
        if (zone.isActive) {
            // Apply smooth step for better feel
            zone.currentValue = smoothStep(0.0f, 1.0f, zone.currentValue);
        }
    }
}

void TouchInputManager::triggerRacingCallbacks() {
    // Trigger callbacks based on current input state
    if (onAccelerate) onAccelerate(getAccelerateInput());
    if (onBrake) onBrake(getBrakeInput());
    if (onSteer) onSteer(getSteerInput());
    if (onBoost) onBoost(getBoostInput());
    if (onHandbrake) onHandbrake(getHandbrakeInput());
    
    Vector2 cameraInput = getCameraLookInput();
    if (onCameraLook && (cameraInput.x != 0.0f || cameraInput.y != 0.0f)) {
        onCameraLook(cameraInput.x, cameraInput.y);
    }
    
    float zoomInput = getCameraZoomInput();
    if (onCameraZoom && zoomInput != 0.0f) {
        onCameraZoom(zoomInput);
    }
}

// Gesture recognition
void TouchInputManager::updateGestures(float deltaTime) {
    // Clear completed gestures from previous frame
    activeGestures.clear();
    
    // Recognize gestures from active touches
    for (const auto& touch : activeTouches) {
        if (touch.isActive) {
            recognizeLongPress(touch);
            recognizeSwipe(touch);
        }
    }
    
    // Multi-touch gestures
    if (activeTouches.size() >= 2) {
        recognizePinch();
        recognizePan();
        recognizeRotation();
    }
}

void TouchInputManager::recognizeTap(const TouchPoint& touch) {
    auto now = std::chrono::steady_clock::now();
    float duration = std::chrono::duration<float>(now - touch.startTime).count();
    float distance = getDistance(touch.position, touch.startPosition);
    
    if (duration <= tapMaxDuration && distance <= tapMaxDistance) {
        Gesture gesture;
        gesture.type = GestureType::Tap;
        gesture.position = touch.position;
        gesture.startPosition = touch.startPosition;
        gesture.isComplete = true;
        completedGestures.push_back(gesture);
        
        // Check for pause action
        TouchZone* pauseZone = getTouchZone(RacingAction::Pause);
        if (pauseZone && isPointInZone(touch.position, *pauseZone)) {
            if (onPause) onPause();
        }
    }
}

void TouchInputManager::recognizeLongPress(const TouchPoint& touch) {
    auto now = std::chrono::steady_clock::now();
    float duration = std::chrono::duration<float>(now - touch.startTime).count();
    float distance = getDistance(touch.position, touch.startPosition);
    
    if (duration >= longPressMinDuration && distance <= tapMaxDistance) {
        Gesture gesture;
        gesture.type = GestureType::LongPress;
        gesture.position = touch.position;
        gesture.startPosition = touch.startPosition;
        gesture.isComplete = false;
        activeGestures.push_back(gesture);
    }
}

void TouchInputManager::recognizeSwipe(const TouchPoint& touch) {
    auto now = std::chrono::steady_clock::now();
    float duration = std::chrono::duration<float>(now - touch.startTime).count();
    float distance = getDistance(touch.position, touch.startPosition);
    
    if (duration <= swipeMaxDuration && distance >= swipeMinDistance) {
        Gesture gesture;
        gesture.type = GestureType::Swipe;
        gesture.position = touch.position;
        gesture.startPosition = touch.startPosition;
        gesture.deltaPosition = touch.position - touch.startPosition;
        gesture.velocity = distance / duration;
        gesture.isComplete = false;
        activeGestures.push_back(gesture);
    }
}

void TouchInputManager::recognizePinch() {
    if (activeTouches.size() < 2) return;
    
    // Calculate distance between first two touches
    float currentDistance = getDistance(activeTouches[0].position, activeTouches[1].position);
    float startDistance = getDistance(activeTouches[0].startPosition, activeTouches[1].startPosition);
    
    if (std::abs(currentDistance - startDistance) >= pinchMinDistance) {
        Gesture gesture;
        gesture.type = GestureType::Pinch;
        gesture.position = (activeTouches[0].position + activeTouches[1].position) * 0.5f;
        gesture.scale = currentDistance / startDistance;
        gesture.touchIds = {activeTouches[0].id, activeTouches[1].id};
        gesture.isComplete = false;
        activeGestures.push_back(gesture);
    }
}

// Utility functions
bool TouchInputManager::isPointInZone(const Vector2& point, const TouchZone& zone) const {
    Vector2 halfSize = zone.size * 0.5f;
    return (point.x >= zone.center.x - halfSize.x && point.x <= zone.center.x + halfSize.x &&
            point.y >= zone.center.y - halfSize.y && point.y <= zone.center.y + halfSize.y);
}

bool TouchInputManager::isPointInCircle(const Vector2& point, const Vector2& center, float radius) const {
    return getDistance(point, center) <= radius;
}

float TouchInputManager::getDistance(const Vector2& a, const Vector2& b) const {
    Vector2 diff = a - b;
    return std::sqrt(diff.x * diff.x + diff.y * diff.y);
}

Vector2 TouchInputManager::clampToCircle(const Vector2& position, const Vector2& center, float radius) const {
    Vector2 offset = position - center;
    float distance = getDistance(Vector2(0, 0), offset);
    
    if (distance <= radius) {
        return position;
    }
    
    return center + (offset / distance) * radius;
}

float TouchInputManager::calculateTouchPressure(const Vector2& position, const TouchZone& zone) const {
    // Calculate pressure based on distance from zone center
    Vector2 offset = position - zone.center;
    Vector2 halfSize = zone.size * 0.5f;
    
    float normalizedX = std::abs(offset.x) / halfSize.x;
    float normalizedY = std::abs(offset.y) / halfSize.y;
    
    // Use maximum component for pressure calculation
    float maxComponent = std::max(normalizedX, normalizedY);
    return 1.0f - std::min(maxComponent, 1.0f);
}

float TouchInputManager::smoothStep(float edge0, float edge1, float x) const {
    float t = std::max(0.0f, std::min(1.0f, (x - edge0) / (edge1 - edge0)));
    return t * t * (3.0f - 2.0f * t);
}

// Touch management helpers
TouchInputManager::TouchPoint* TouchInputManager::findTouch(int touchId) {
    for (auto& touch : activeTouches) {
        if (touch.id == touchId) {
            return &touch;
        }
    }
    return nullptr;
}

void TouchInputManager::addTouch(int touchId, const Vector2& position, float pressure) {
    TouchPoint newTouch;
    newTouch.id = touchId;
    newTouch.position = position;
    newTouch.startPosition = position;
    newTouch.deltaPosition = Vector2(0, 0);
    newTouch.velocity = Vector2(0, 0);
    newTouch.pressure = pressure;
    newTouch.isActive = true;
    newTouch.startTime = std::chrono::steady_clock::now();
    newTouch.lastUpdateTime = newTouch.startTime;
    
    activeTouches.push_back(newTouch);
}

void TouchInputManager::updateTouch(int touchId, const Vector2& position, float pressure) {
    TouchPoint* touch = findTouch(touchId);
    if (touch) {
        touch->deltaPosition = position - touch->position;
        touch->position = position;
        touch->pressure = pressure;
        touch->lastUpdateTime = std::chrono::steady_clock::now();
    }
}

void TouchInputManager::removeTouch(int touchId) {
    activeTouches.erase(
        std::remove_if(activeTouches.begin(), activeTouches.end(),
                      [touchId](const TouchPoint& touch) { return touch.id == touchId; }),
        activeTouches.end());
}

void TouchInputManager::updateTouchVelocities(float deltaTime) {
    for (auto& touch : activeTouches) {
        if (deltaTime > 0.0f) {
            touch.velocity = touch.deltaPosition / deltaTime;
        }
    }
}

// Callback setters
void TouchInputManager::setAccelerateCallback(std::function<void(float)> callback) {
    onAccelerate = callback;
}

void TouchInputManager::setBrakeCallback(std::function<void(float)> callback) {
    onBrake = callback;
}

void TouchInputManager::setSteerCallback(std::function<void(float)> callback) {
    onSteer = callback;
}

void TouchInputManager::setBoostCallback(std::function<void(bool)> callback) {
    onBoost = callback;
}

void TouchInputManager::setHandbrakeCallback(std::function<void(bool)> callback) {
    onHandbrake = callback;
}

void TouchInputManager::setCameraLookCallback(std::function<void(float, float)> callback) {
    onCameraLook = callback;
}

void TouchInputManager::setCameraZoomCallback(std::function<void(float)> callback) {
    onCameraZoom = callback;
}

void TouchInputManager::setPauseCallback(std::function<void()> callback) {
    onPause = callback;
}

void TouchInputManager::setResetCallback(std::function<void()> callback) {
    onReset = callback;
}

#endif // MOBILE_PLATFORM