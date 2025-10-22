#include "MobileInput.h"
#include <algorithm>
#include <cmath>

namespace MobileInput {
    // TouchManager implementation
    std::vector<TouchManager::TouchEvent> TouchManager::s_touchEvents;
    std::vector<TouchManager::TouchEvent> TouchManager::s_previousTouchEvents;
    std::unordered_map<int, TouchManager::TouchEvent> TouchManager::s_activeTouches;
    TouchManager::Gesture TouchManager::s_currentGesture;
    float TouchManager::s_tapThreshold = 10.0f;
    float TouchManager::s_longPressThreshold = 0.5f;
    float TouchManager::s_swipeThreshold = 50.0f;
    float TouchManager::s_pinchThreshold = 0.1f;
    bool TouchManager::s_initialized = false;

    bool TouchManager::initialize() {
        if (s_initialized) return true;
        
        s_touchEvents.clear();
        s_previousTouchEvents.clear();
        s_activeTouches.clear();
        s_currentGesture = Gesture{};
        
        s_initialized = true;
        return true;
    }

    void TouchManager::shutdown() {
        s_touchEvents.clear();
        s_previousTouchEvents.clear();
        s_activeTouches.clear();
        s_initialized = false;
    }

    void TouchManager::update(float deltaTime) {
        s_previousTouchEvents = s_touchEvents;
        s_touchEvents.clear();
        
        // Update active touches
        for (auto& pair : s_activeTouches) {
            TouchEvent& touch = pair.second;
            touch.isPressed = false;
            touch.isReleased = false;
            touch.isHeld = touch.isHeld && !touch.isReleased;
        }
        
        processGestures();
    }

    bool TouchManager::isTouching() {
        return !s_activeTouches.empty();
    }

    int TouchManager::getTouchCount() {
        return s_activeTouches.size();
    }

    TouchManager::TouchEvent TouchManager::getTouchEvent(int index) {
        if (index >= 0 && index < s_touchEvents.size()) {
            return s_touchEvents[index];
        }
        return TouchEvent{};
    }

    Vector2 TouchManager::getTouchPosition(int index) {
        if (index < s_activeTouches.size()) {
            auto it = s_activeTouches.begin();
            std::advance(it, index);
            return it->second.position;
        }
        return Vector2::zero();
    }

    Vector2 TouchManager::getTouchDelta(int index) {
        if (index < s_activeTouches.size()) {
            auto it = s_activeTouches.begin();
            std::advance(it, index);
            return it->second.delta;
        }
        return Vector2::zero();
    }

    bool TouchManager::isTouchPressed(int index) {
        if (index < s_activeTouches.size()) {
            auto it = s_activeTouches.begin();
            std::advance(it, index);
            return it->second.isHeld;
        }
        return false;
    }

    bool TouchManager::isTouchJustPressed(int index) {
        if (index < s_touchEvents.size()) {
            return s_touchEvents[index].isPressed;
        }
        return false;
    }

    bool TouchManager::isTouchJustReleased(int index) {
        if (index < s_touchEvents.size()) {
            return s_touchEvents[index].isReleased;
        }
        return false;
    }

    bool TouchManager::isTouchHeld(int index) {
        return isTouchPressed(index);
    }

    float TouchManager::getTouchPressure(int index) {
        if (index < s_activeTouches.size()) {
            auto it = s_activeTouches.begin();
            std::advance(it, index);
            return it->second.pressure;
        }
        return 0.0f;
    }

    bool TouchManager::isGestureActive() {
        return s_currentGesture.isActive;
    }

    TouchManager::Gesture TouchManager::getCurrentGesture() {
        return s_currentGesture;
    }

    bool TouchManager::isTap() {
        return s_currentGesture.type == Gesture::Tap;
    }

    bool TouchManager::isDoubleTap() {
        return s_currentGesture.type == Gesture::DoubleTap;
    }

    bool TouchManager::isLongPress() {
        return s_currentGesture.type == Gesture::LongPress;
    }

    bool TouchManager::isPanning() {
        return s_currentGesture.type == Gesture::Pan;
    }

    bool TouchManager::isPinching() {
        return s_currentGesture.type == Gesture::Pinch;
    }

    bool TouchManager::isRotating() {
        return s_currentGesture.type == Gesture::Rotation;
    }

    bool TouchManager::isSwipe() {
        return s_currentGesture.type == Gesture::Swipe;
    }

    Vector2 TouchManager::getPanDelta() {
        return s_currentGesture.delta;
    }

    float TouchManager::getPinchScale() {
        return s_currentGesture.scale;
    }

    float TouchManager::getRotationAngle() {
        return s_currentGesture.rotation;
    }

    Vector2 TouchManager::getSwipeDirection() {
        return s_currentGesture.delta.normalized();
    }

    void TouchManager::onTouchDown(int id, float x, float y, float pressure) {
        TouchEvent event;
        event.id = id;
        event.position = Vector2(x, y);
        event.delta = Vector2::zero();
        event.pressure = pressure;
        event.isPressed = true;
        event.isReleased = false;
        event.isHeld = true;
        event.timestamp = 0.0f; // Would use actual timestamp
        
        s_touchEvents.push_back(event);
        s_activeTouches[id] = event;
    }

    void TouchManager::onTouchMove(int id, float x, float y, float pressure) {
        auto it = s_activeTouches.find(id);
        if (it != s_activeTouches.end()) {
            Vector2 newPosition(x, y);
            Vector2 delta = newPosition - it->second.position;
            
            it->second.delta = delta;
            it->second.position = newPosition;
            it->second.pressure = pressure;
        }
    }

    void TouchManager::onTouchUp(int id, float x, float y) {
        auto it = s_activeTouches.find(id);
        if (it != s_activeTouches.end()) {
            TouchEvent event = it->second;
            event.position = Vector2(x, y);
            event.isPressed = false;
            event.isReleased = true;
            event.isHeld = false;
            
            s_touchEvents.push_back(event);
            s_activeTouches.erase(it);
        }
    }

    void TouchManager::onTouchCancel(int id) {
        s_activeTouches.erase(id);
    }

    void TouchManager::processGestures() {
        // Process tap gestures
        if (s_touchEvents.size() == 1 && s_touchEvents[0].isReleased) {
            const TouchEvent& event = s_touchEvents[0];
            if (event.delta.magnitude() < s_tapThreshold) {
                s_currentGesture.type = Gesture::Tap;
                s_currentGesture.startPosition = event.position;
                s_currentGesture.endPosition = event.position;
                s_currentGesture.isActive = true;
            }
        }
        
        // Process pan gestures
        if (s_activeTouches.size() == 1) {
            auto it = s_activeTouches.begin();
            const TouchEvent& touch = it->second;
            if (touch.delta.magnitude() > s_swipeThreshold) {
                s_currentGesture.type = Gesture::Pan;
                s_currentGesture.delta = touch.delta;
                s_currentGesture.isActive = true;
            }
        }
        
        // Process pinch gestures
        if (s_activeTouches.size() == 2) {
            auto it1 = s_activeTouches.begin();
            auto it2 = std::next(it1);
            
            Vector2 pos1 = it1->second.position;
            Vector2 pos2 = it2->second.position;
            float distance = (pos1 - pos2).magnitude();
            
            // Calculate pinch scale based on distance change
            s_currentGesture.type = Gesture::Pinch;
            s_currentGesture.scale = distance / 100.0f; // Normalize
            s_currentGesture.isActive = true;
        }
    }

    // VirtualControls implementation
    std::vector<VirtualControls::VirtualJoystick> VirtualControls::s_joysticks;
    std::vector<VirtualControls::VirtualButton> VirtualControls::s_buttons;
    std::vector<VirtualControls::VirtualSlider> VirtualControls::s_sliders;
    bool VirtualControls::s_initialized = false;

    bool VirtualControls::initialize() {
        if (s_initialized) return true;
        
        s_joysticks.clear();
        s_buttons.clear();
        s_sliders.clear();
        
        s_initialized = true;
        return true;
    }

    void VirtualControls::shutdown() {
        s_joysticks.clear();
        s_buttons.clear();
        s_sliders.clear();
        s_initialized = false;
    }

    void VirtualControls::update(float deltaTime) {
        // Update button states
        for (auto& button : s_buttons) {
            button.isPressed = false; // Reset for next frame
        }
        
        // Update slider states
        for (auto& slider : s_sliders) {
            slider.isDragging = false; // Reset for next frame
        }
    }

    void VirtualControls::render() {
        // Render all controls
        for (const auto& joystick : s_joysticks) {
            if (joystick.isVisible) {
                renderJoystick(joystick);
            }
        }
        
        for (const auto& button : s_buttons) {
            if (button.isVisible) {
                renderButton(button);
            }
        }
        
        for (const auto& slider : s_sliders) {
            if (slider.isVisible) {
                renderSlider(slider);
            }
        }
    }

    int VirtualControls::addJoystick(const Vector2& center, float radius, const Vector3& color) {
        VirtualJoystick joystick;
        joystick.center = center;
        joystick.position = center;
        joystick.radius = radius;
        joystick.deadZone = radius * 0.3f;
        joystick.isActive = false;
        joystick.isVisible = true;
        joystick.color = color;
        joystick.opacity = 0.8f;
        
        s_joysticks.push_back(joystick);
        return s_joysticks.size() - 1;
    }

    void VirtualControls::removeJoystick(int id) {
        if (id >= 0 && id < s_joysticks.size()) {
            s_joysticks.erase(s_joysticks.begin() + id);
        }
    }

    Vector2 VirtualControls::getJoystickInput(int id) {
        if (id >= 0 && id < s_joysticks.size()) {
            const VirtualJoystick& joystick = s_joysticks[id];
            if (joystick.isActive) {
                Vector2 input = joystick.position - joystick.center;
                float distance = input.magnitude();
                
                if (distance > joystick.deadZone) {
                    input = input.normalized() * ((distance - joystick.deadZone) / (joystick.radius - joystick.deadZone));
                    return input;
                }
            }
        }
        return Vector2::zero();
    }

    bool VirtualControls::isJoystickActive(int id) {
        if (id >= 0 && id < s_joysticks.size()) {
            return s_joysticks[id].isActive;
        }
        return false;
    }

    int VirtualControls::addButton(const Vector2& position, const Vector2& size, const std::string& label, const Vector3& color) {
        VirtualButton button;
        button.position = position;
        button.size = size;
        button.isPressed = false;
        button.isVisible = true;
        button.label = label;
        button.color = color;
        button.pressedColor = color * 0.7f;
        button.opacity = 0.8f;
        button.pressScale = 0.9f;
        
        s_buttons.push_back(button);
        return s_buttons.size() - 1;
    }

    bool VirtualControls::isButtonPressed(int id) {
        if (id >= 0 && id < s_buttons.size()) {
            return s_buttons[id].isPressed;
        }
        return false;
    }

    bool VirtualControls::isButtonJustPressed(int id) {
        // This would need to track previous frame state
        return false; // Simplified for now
    }

    bool VirtualControls::isButtonJustReleased(int id) {
        // This would need to track previous frame state
        return false; // Simplified for now
    }

    void VirtualControls::onTouchDown(const Vector2& position) {
        // Check joysticks
        for (auto& joystick : s_joysticks) {
            if (joystick.isVisible && isPointInRect(position, joystick.center, Vector2(joystick.radius * 2, joystick.radius * 2))) {
                joystick.isActive = true;
                joystick.position = position;
                break;
            }
        }
        
        // Check buttons
        for (auto& button : s_buttons) {
            if (button.isVisible && isPointInRect(position, button.position, button.size)) {
                button.isPressed = true;
                break;
            }
        }
        
        // Check sliders
        for (auto& slider : s_sliders) {
            if (slider.isVisible && isPointInRect(position, slider.position, slider.size)) {
                slider.isDragging = true;
                // Update slider value based on position
                float normalizedX = (position.x - slider.position.x) / slider.size.x;
                slider.value = slider.minValue + normalizedX * (slider.maxValue - slider.minValue);
                slider.value = std::max(slider.minValue, std::min(slider.maxValue, slider.value));
                break;
            }
        }
    }

    void VirtualControls::onTouchMove(const Vector2& position) {
        // Update active joysticks
        for (auto& joystick : s_joysticks) {
            if (joystick.isActive) {
                joystick.position = clampToCircle(position, joystick.center, joystick.radius);
            }
        }
        
        // Update dragging sliders
        for (auto& slider : s_sliders) {
            if (slider.isDragging) {
                float normalizedX = (position.x - slider.position.x) / slider.size.x;
                slider.value = slider.minValue + normalizedX * (slider.maxValue - slider.minValue);
                slider.value = std::max(slider.minValue, std::min(slider.maxValue, slider.value));
            }
        }
    }

    void VirtualControls::onTouchUp(const Vector2& position) {
        // Deactivate all controls
        for (auto& joystick : s_joysticks) {
            joystick.isActive = false;
            joystick.position = joystick.center;
        }
        
        for (auto& button : s_buttons) {
            button.isPressed = false;
        }
        
        for (auto& slider : s_sliders) {
            slider.isDragging = false;
        }
    }

    bool VirtualControls::isPointInRect(const Vector2& point, const Vector2& rectPos, const Vector2& rectSize) {
        return point.x >= rectPos.x && point.x <= rectPos.x + rectSize.x &&
               point.y >= rectPos.y && point.y <= rectPos.y + rectSize.y;
    }

    float VirtualControls::getDistance(const Vector2& a, const Vector2& b) {
        return (a - b).magnitude();
    }

    Vector2 VirtualControls::clampToCircle(const Vector2& point, const Vector2& center, float radius) {
        Vector2 direction = point - center;
        float distance = direction.magnitude();
        
        if (distance <= radius) {
            return point;
        }
        
        return center + direction.normalized() * radius;
    }

    void VirtualControls::renderJoystick(const VirtualJoystick& joystick) {
        // This would render the joystick using the renderer
        // For now, it's a placeholder
    }

    void VirtualControls::renderButton(const VirtualButton& button) {
        // This would render the button using the renderer
        // For now, it's a placeholder
    }

    void VirtualControls::renderSlider(const VirtualSlider& slider) {
        // This would render the slider using the renderer
        // For now, it's a placeholder
    }
}