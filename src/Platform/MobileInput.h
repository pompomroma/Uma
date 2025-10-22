#pragma once
#include "Platform.h"
#include "../Math/Vector2.h"
#include <vector>
#include <unordered_map>
#include <functional>

namespace MobileInput {
    // Touch input handling
    class TouchManager {
    public:
        struct TouchEvent {
            int id;
            Vector2 position;
            Vector2 delta;
            float pressure;
            bool isPressed;
            bool isReleased;
            bool isHeld;
            float timestamp;
        };

        struct Gesture {
            enum Type {
                None,
                Tap,
                DoubleTap,
                LongPress,
                Pan,
                Pinch,
                Rotation,
                Swipe
            };
            
            Type type;
            Vector2 startPosition;
            Vector2 endPosition;
            Vector2 delta;
            float scale;
            float rotation;
            float duration;
            bool isActive;
        };

    private:
        static std::vector<TouchEvent> s_touchEvents;
        static std::vector<TouchEvent> s_previousTouchEvents;
        static std::unordered_map<int, TouchEvent> s_activeTouches;
        static Gesture s_currentGesture;
        static float s_tapThreshold;
        static float s_longPressThreshold;
        static float s_swipeThreshold;
        static float s_pinchThreshold;
        static bool s_initialized;

    public:
        static bool initialize();
        static void shutdown();
        static void update(float deltaTime);
        
        // Touch queries
        static bool isTouching();
        static int getTouchCount();
        static TouchEvent getTouchEvent(int index);
        static Vector2 getTouchPosition(int index = 0);
        static Vector2 getTouchDelta(int index = 0);
        static bool isTouchPressed(int index = 0);
        static bool isTouchJustPressed(int index = 0);
        static bool isTouchJustReleased(int index = 0);
        static bool isTouchHeld(int index = 0);
        static float getTouchPressure(int index = 0);
        
        // Gesture recognition
        static bool isGestureActive();
        static Gesture getCurrentGesture();
        static bool isTap();
        static bool isDoubleTap();
        static bool isLongPress();
        static bool isPanning();
        static bool isPinching();
        static bool isRotating();
        static bool isSwipe();
        static Vector2 getPanDelta();
        static float getPinchScale();
        static float getRotationAngle();
        static Vector2 getSwipeDirection();
        
        // Configuration
        static void setTapThreshold(float threshold);
        static void setLongPressThreshold(float threshold);
        static void setSwipeThreshold(float threshold);
        static void setPinchThreshold(float threshold);
        
        // Event handling
        static void onTouchDown(int id, float x, float y, float pressure);
        static void onTouchMove(int id, float x, float y, float pressure);
        static void onTouchUp(int id, float x, float y);
        static void onTouchCancel(int id);

    private:
        static void processGestures();
        static void updateGestureState();
        static bool isWithinThreshold(const Vector2& pos1, const Vector2& pos2, float threshold);
    };

    // Virtual controls for mobile
    class VirtualControls {
    public:
        enum class ControlType {
            Joystick,
            Button,
            Slider,
            Toggle
        };

        struct VirtualJoystick {
            Vector2 center;
            Vector2 position;
            float radius;
            float deadZone;
            bool isActive;
            bool isVisible;
            Vector3 color;
            float opacity;
        };

        struct VirtualButton {
            Vector2 position;
            Vector2 size;
            bool isPressed;
            bool isVisible;
            std::string label;
            Vector3 color;
            Vector3 pressedColor;
            float opacity;
            float pressScale;
        };

        struct VirtualSlider {
            Vector2 position;
            Vector2 size;
            float value;
            float minValue;
            float maxValue;
            bool isDragging;
            bool isVisible;
            Vector3 color;
            Vector3 fillColor;
            float opacity;
        };

    private:
        static std::vector<VirtualJoystick> s_joysticks;
        static std::vector<VirtualButton> s_buttons;
        static std::vector<VirtualSlider> s_sliders;
        static bool s_initialized;

    public:
        static bool initialize();
        static void shutdown();
        static void update(float deltaTime);
        static void render();
        
        // Joystick management
        static int addJoystick(const Vector2& center, float radius, const Vector3& color = Vector3(1, 1, 1));
        static void removeJoystick(int id);
        static void setJoystickVisible(int id, bool visible);
        static void setJoystickPosition(int id, const Vector2& position);
        static void setJoystickRadius(int id, float radius);
        static Vector2 getJoystickInput(int id);
        static bool isJoystickActive(int id);
        
        // Button management
        static int addButton(const Vector2& position, const Vector2& size, const std::string& label, const Vector3& color = Vector3(0.5f, 0.5f, 0.5f));
        static void removeButton(int id);
        static void setButtonVisible(int id, bool visible);
        static void setButtonPosition(int id, const Vector2& position);
        static void setButtonSize(int id, const Vector2& size);
        static void setButtonLabel(int id, const std::string& label);
        static bool isButtonPressed(int id);
        static bool isButtonJustPressed(int id);
        static bool isButtonJustReleased(int id);
        
        // Slider management
        static int addSlider(const Vector2& position, const Vector2& size, float minValue, float maxValue, const Vector3& color = Vector3(0.5f, 0.5f, 0.5f));
        static void removeSlider(int id);
        static void setSliderVisible(int id, bool visible);
        static void setSliderPosition(int id, const Vector2& position);
        static void setSliderSize(int id, const Vector2& size);
        static void setSliderValue(int id, float value);
        static float getSliderValue(int id);
        static bool isSliderDragging(int id);
        
        // Input handling
        static void onTouchDown(const Vector2& position);
        static void onTouchMove(const Vector2& position);
        static void onTouchUp(const Vector2& position);
        
        // Rendering
        static void renderJoystick(const VirtualJoystick& joystick);
        static void renderButton(const VirtualButton& button);
        static void renderSlider(const VirtualSlider& slider);
        
        // Utility
        static bool isPointInRect(const Vector2& point, const Vector2& rectPos, const Vector2& rectSize);
        static float getDistance(const Vector2& a, const Vector2& b);
        static Vector2 clampToCircle(const Vector2& point, const Vector2& center, float radius);
    };

    // Mobile-specific input manager
    class MobileInputManager {
    public:
        enum class Action {
            // Racing controls
            Accelerate,
            Brake,
            SteerLeft,
            SteerRight,
            Boost,
            Handbrake,
            CameraLook,
            CameraZoom,
            
            // Combat controls
            Attack1,
            Attack2,
            Shield,
            Teleport,
            Interact,
            
            // UI controls
            Pause,
            Menu,
            Back,
            Select,
            
            // Gestures
            Tap,
            DoubleTap,
            LongPress,
            SwipeLeft,
            SwipeRight,
            SwipeUp,
            SwipeDown,
            PinchIn,
            PinchOut,
            RotateLeft,
            RotateRight
        };

        struct ActionBinding {
            Action action;
            ControlType controlType;
            int controlId;
            float threshold;
            bool isEnabled;
        };

    private:
        static std::vector<ActionBinding> s_actionBindings;
        static std::unordered_map<Action, bool> s_actionStates;
        static std::unordered_map<Action, bool> s_previousActionStates;
        static std::unordered_map<Action, std::function<void()>> s_actionCallbacks;
        static bool s_initialized;

    public:
        static bool initialize();
        static void shutdown();
        static void update(float deltaTime);
        
        // Action management
        static void bindAction(Action action, ControlType controlType, int controlId, float threshold = 0.1f);
        static void unbindAction(Action action);
        static void setActionEnabled(Action action, bool enabled);
        
        // Action queries
        static bool isActionPressed(Action action);
        static bool isActionJustPressed(Action action);
        static bool isActionJustReleased(Action action);
        static float getActionValue(Action action);
        
        // Callback registration
        static void setActionCallback(Action action, std::function<void()> callback);
        
        // Mobile-specific controls
        static void setupRacingControls();
        static void setupCombatControls();
        static void setupMenuControls();
        static void setupGestureControls();
        
        // Configuration
        static void setControlSensitivity(float sensitivity);
        static void setControlDeadZone(float deadZone);
        static void setControlScale(float scale);
        static void setControlOpacity(float opacity);
        
        // Layout management
        static void setControlLayout(const std::string& layout);
        static void saveControlLayout(const std::string& layout);
        static void loadControlLayout(const std::string& layout);
        static void resetToDefaultLayout();

    private:
        static void processActions();
        static void updateActionStates();
        static bool evaluateAction(const ActionBinding& binding);
        static float getActionValue(const ActionBinding& binding);
    };
}