#include "MobileInputManager.h"
#include <algorithm>
#include <cmath>

MobileInputManager::MobileInputManager()
    : m_controlSensitivity(1.0f)
    , m_controlDeadZone(0.1f)
    , m_controlScale(1.0f)
    , m_controlOpacity(0.8f)
    , m_currentLayout("default")
    , m_joystickId(-1)
    , m_accelerateButtonId(-1)
    , m_brakeButtonId(-1)
    , m_boostButtonId(-1)
    , m_handbrakeButtonId(-1)
    , m_attack1ButtonId(-1)
    , m_attack2ButtonId(-1)
    , m_shieldButtonId(-1)
    , m_teleportButtonId(-1)
    , m_pauseButtonId(-1)
    , m_menuButtonId(-1)
    , m_initialized(false) {
    
    // Initialize action states
    for (int i = 0; i < (int)Action::Count; i++) {
        m_actionStates[(Action)i] = false;
        m_previousActionStates[(Action)i] = false;
    }
}

MobileInputManager::~MobileInputManager() {
    shutdown();
}

bool MobileInputManager::initialize() {
    if (m_initialized) return true;
    
    // Initialize mobile input systems
    if (!MobileInput::TouchManager::initialize()) {
        return false;
    }
    
    if (!MobileInput::VirtualControls::initialize()) {
        return false;
    }
    
    // Setup default controls
    setupRacingControls();
    
    m_initialized = true;
    return true;
}

void MobileInputManager::shutdown() {
    if (!m_initialized) return;
    
    MobileInput::TouchManager::shutdown();
    MobileInput::VirtualControls::shutdown();
    
    m_actionBindings.clear();
    m_actionStates.clear();
    m_previousActionStates.clear();
    m_actionCallbacks.clear();
    
    m_initialized = false;
}

void MobileInputManager::update(float deltaTime) {
    if (!m_initialized) return;
    
    // Update mobile input systems
    MobileInput::TouchManager::update(deltaTime);
    MobileInput::VirtualControls::update(deltaTime);
    
    // Update our state
    updateTouchState();
    updateGestureState();
    updateActionStates();
    processActions();
}

void MobileInputManager::bindAction(Action action, MobileInput::VirtualControls::ControlType controlType, int controlId, float threshold) {
    ActionBinding binding;
    binding.action = action;
    binding.controlType = controlType;
    binding.controlId = controlId;
    binding.threshold = threshold;
    binding.isEnabled = true;
    
    // Remove existing binding for this action
    m_actionBindings.erase(
        std::remove_if(m_actionBindings.begin(), m_actionBindings.end(),
            [action](const ActionBinding& b) { return b.action == action; }),
        m_actionBindings.end()
    );
    
    m_actionBindings.push_back(binding);
}

void MobileInputManager::unbindAction(Action action) {
    m_actionBindings.erase(
        std::remove_if(m_actionBindings.begin(), m_actionBindings.end(),
            [action](const ActionBinding& b) { return b.action == action; }),
        m_actionBindings.end()
    );
}

void MobileInputManager::setActionEnabled(Action action, bool enabled) {
    for (auto& binding : m_actionBindings) {
        if (binding.action == action) {
            binding.isEnabled = enabled;
            break;
        }
    }
}

bool MobileInputManager::isActionPressed(Action action) const {
    auto it = m_actionStates.find(action);
    return it != m_actionStates.end() ? it->second : false;
}

bool MobileInputManager::isActionJustPressed(Action action) const {
    bool current = isActionPressed(action);
    bool previous = false;
    auto it = m_previousActionStates.find(action);
    if (it != m_previousActionStates.end()) {
        previous = it->second;
    }
    return current && !previous;
}

bool MobileInputManager::isActionJustReleased(Action action) const {
    bool current = isActionPressed(action);
    bool previous = false;
    auto it = m_previousActionStates.find(action);
    if (it != m_previousActionStates.end()) {
        previous = it->second;
    }
    return !current && previous;
}

float MobileInputManager::getActionValue(Action action) const {
    for (const auto& binding : m_actionBindings) {
        if (binding.action == action && binding.isEnabled) {
            return getActionValue(binding);
        }
    }
    return 0.0f;
}

bool MobileInputManager::isTouching() const {
    return MobileInput::TouchManager::isTouching();
}

int MobileInputManager::getTouchCount() const {
    return MobileInput::TouchManager::getTouchCount();
}

Vector2 MobileInputManager::getTouchPosition(int index) const {
    return MobileInput::TouchManager::getTouchPosition(index);
}

Vector2 MobileInputManager::getTouchDelta(int index) const {
    return MobileInput::TouchManager::getTouchDelta(index);
}

bool MobileInputManager::isTouchPressed(int index) const {
    return MobileInput::TouchManager::isTouchPressed(index);
}

bool MobileInputManager::isTouchJustPressed(int index) const {
    return MobileInput::TouchManager::isTouchJustPressed(index);
}

bool MobileInputManager::isTouchJustReleased(int index) const {
    return MobileInput::TouchManager::isTouchJustReleased(index);
}

float MobileInputManager::getTouchPressure(int index) const {
    return MobileInput::TouchManager::getTouchPressure(index);
}

bool MobileInputManager::isGestureActive() const {
    return MobileInput::TouchManager::isGestureActive();
}

MobileInputManager::GestureState MobileInputManager::getGestureState() const {
    return m_gestureState;
}

bool MobileInputManager::isTap() const {
    return MobileInput::TouchManager::isTap();
}

bool MobileInputManager::isDoubleTap() const {
    return MobileInput::TouchManager::isDoubleTap();
}

bool MobileInputManager::isLongPress() const {
    return MobileInput::TouchManager::isLongPress();
}

bool MobileInputManager::isPanning() const {
    return MobileInput::TouchManager::isPanning();
}

bool MobileInputManager::isPinching() const {
    return MobileInput::TouchManager::isPinching();
}

bool MobileInputManager::isRotating() const {
    return MobileInput::TouchManager::isRotating();
}

bool MobileInputManager::isSwipe() const {
    return MobileInput::TouchManager::isSwipe();
}

Vector2 MobileInputManager::getPanDelta() const {
    return MobileInput::TouchManager::getPanDelta();
}

float MobileInputManager::getPinchScale() const {
    return MobileInput::TouchManager::getPinchScale();
}

float MobileInputManager::getRotationAngle() const {
    return MobileInput::TouchManager::getRotationAngle();
}

Vector2 MobileInputManager::getSwipeDirection() const {
    return MobileInput::TouchManager::getSwipeDirection();
}

float MobileInputManager::getAccelerateInput() const {
    // Check joystick input
    if (m_joystickId >= 0) {
        Vector2 joystickInput = MobileInput::VirtualControls::getJoystickInput(m_joystickId);
        if (joystickInput.y > 0) {
            return joystickInput.y;
        }
    }
    
    // Check button input
    if (m_accelerateButtonId >= 0) {
        if (MobileInput::VirtualControls::isButtonPressed(m_accelerateButtonId)) {
            return 1.0f;
        }
    }
    
    return 0.0f;
}

float MobileInputManager::getBrakeInput() const {
    // Check joystick input
    if (m_joystickId >= 0) {
        Vector2 joystickInput = MobileInput::VirtualControls::getJoystickInput(m_joystickId);
        if (joystickInput.y < 0) {
            return -joystickInput.y;
        }
    }
    
    // Check button input
    if (m_brakeButtonId >= 0) {
        if (MobileInput::VirtualControls::isButtonPressed(m_brakeButtonId)) {
            return 1.0f;
        }
    }
    
    return 0.0f;
}

float MobileInputManager::getSteerInput() const {
    // Check joystick input
    if (m_joystickId >= 0) {
        Vector2 joystickInput = MobileInput::VirtualControls::getJoystickInput(m_joystickId);
        return joystickInput.x;
    }
    
    // Check button input
    float steerInput = 0.0f;
    if (isActionPressed(Action::SteerLeft)) {
        steerInput -= 1.0f;
    }
    if (isActionPressed(Action::SteerRight)) {
        steerInput += 1.0f;
    }
    
    return steerInput;
}

bool MobileInputManager::getBoostInput() const {
    if (m_boostButtonId >= 0) {
        return MobileInput::VirtualControls::isButtonPressed(m_boostButtonId);
    }
    return isActionPressed(Action::Boost);
}

bool MobileInputManager::getHandbrakeInput() const {
    if (m_handbrakeButtonId >= 0) {
        return MobileInput::VirtualControls::isButtonPressed(m_handbrakeButtonId);
    }
    return isActionPressed(Action::Handbrake);
}

Vector2 MobileInputManager::getCameraLookInput() const {
    // Use touch input for camera look
    if (isTouching()) {
        Vector2 touchDelta = getTouchDelta(0);
        return touchDelta * m_controlSensitivity;
    }
    
    // Use gesture input
    if (isPanning()) {
        return getPanDelta() * m_controlSensitivity;
    }
    
    return Vector2::zero();
}

float MobileInputManager::getCameraZoomInput() const {
    if (isPinching()) {
        return getPinchScale();
    }
    return 0.0f;
}

bool MobileInputManager::getAttack1Input() const {
    if (m_attack1ButtonId >= 0) {
        return MobileInput::VirtualControls::isButtonPressed(m_attack1ButtonId);
    }
    return isActionPressed(Action::Attack1);
}

bool MobileInputManager::getAttack2Input() const {
    if (m_attack2ButtonId >= 0) {
        return MobileInput::VirtualControls::isButtonPressed(m_attack2ButtonId);
    }
    return isActionPressed(Action::Attack2);
}

bool MobileInputManager::getShieldInput() const {
    if (m_shieldButtonId >= 0) {
        return MobileInput::VirtualControls::isButtonPressed(m_shieldButtonId);
    }
    return isActionPressed(Action::Shield);
}

bool MobileInputManager::getTeleportInput() const {
    if (m_teleportButtonId >= 0) {
        return MobileInput::VirtualControls::isButtonPressed(m_teleportButtonId);
    }
    return isActionPressed(Action::Teleport);
}

bool MobileInputManager::getInteractInput() const {
    return isActionPressed(Action::Interact);
}

void MobileInputManager::setActionCallback(Action action, std::function<void()> callback) {
    m_actionCallbacks[action] = callback;
}

void MobileInputManager::setupRacingControls() {
    createRacingControls();
    setDefaultRacingLayout();
}

void MobileInputManager::setupCombatControls() {
    createCombatControls();
    setDefaultCombatLayout();
}

void MobileInputManager::setupMenuControls() {
    createMenuControls();
    setDefaultMenuLayout();
}

void MobileInputManager::setupGestureControls() {
    createGestureControls();
    setDefaultGestureLayout();
}

void MobileInputManager::setControlSensitivity(float sensitivity) {
    m_controlSensitivity = std::max(0.1f, std::min(5.0f, sensitivity));
}

void MobileInputManager::setControlDeadZone(float deadZone) {
    m_controlDeadZone = std::max(0.0f, std::min(1.0f, deadZone));
}

void MobileInputManager::setControlScale(float scale) {
    m_controlScale = std::max(0.5f, std::min(2.0f, scale));
}

void MobileInputManager::setControlOpacity(float opacity) {
    m_controlOpacity = std::max(0.1f, std::min(1.0f, opacity));
}

void MobileInputManager::setControlLayout(const std::string& layout) {
    m_currentLayout = layout;
    applyLayout(layout);
}

void MobileInputManager::saveControlLayout(const std::string& layout) {
    saveCurrentLayout();
}

void MobileInputManager::loadControlLayout(const std::string& layout) {
    loadSavedLayout();
}

void MobileInputManager::resetToDefaultLayout() {
    if (m_currentLayout == "racing") {
        setDefaultRacingLayout();
    } else if (m_currentLayout == "combat") {
        setDefaultCombatLayout();
    } else if (m_currentLayout == "menu") {
        setDefaultMenuLayout();
    } else {
        setDefaultRacingLayout();
    }
}

void MobileInputManager::setJoystickVisible(bool visible) {
    if (m_joystickId >= 0) {
        MobileInput::VirtualControls::setJoystickVisible(m_joystickId, visible);
    }
}

void MobileInputManager::setJoystickPosition(const Vector2& position) {
    if (m_joystickId >= 0) {
        MobileInput::VirtualControls::setJoystickPosition(m_joystickId, position);
    }
}

void MobileInputManager::setJoystickRadius(float radius) {
    if (m_joystickId >= 0) {
        MobileInput::VirtualControls::setJoystickRadius(m_joystickId, radius);
    }
}

Vector2 MobileInputManager::getJoystickInput() const {
    if (m_joystickId >= 0) {
        return MobileInput::VirtualControls::getJoystickInput(m_joystickId);
    }
    return Vector2::zero();
}

bool MobileInputManager::isJoystickActive() const {
    if (m_joystickId >= 0) {
        return MobileInput::VirtualControls::isJoystickActive(m_joystickId);
    }
    return false;
}

void MobileInputManager::renderControls() {
    MobileInput::VirtualControls::render();
}

void MobileInputManager::onTouchDown(int id, float x, float y, float pressure) {
    MobileInput::TouchManager::onTouchDown(id, x, y, pressure);
    MobileInput::VirtualControls::onTouchDown(Vector2(x, y));
}

void MobileInputManager::onTouchMove(int id, float x, float y, float pressure) {
    MobileInput::TouchManager::onTouchMove(id, x, y, pressure);
    MobileInput::VirtualControls::onTouchMove(Vector2(x, y));
}

void MobileInputManager::onTouchUp(int id, float x, float y) {
    MobileInput::TouchManager::onTouchUp(id, x, y);
    MobileInput::VirtualControls::onTouchUp(Vector2(x, y));
}

void MobileInputManager::onTouchCancel(int id) {
    MobileInput::TouchManager::onTouchCancel(id);
}

void MobileInputManager::clearInputState() {
    MobileInput::TouchManager::shutdown();
    MobileInput::TouchManager::initialize();
    
    for (auto& pair : m_actionStates) {
        pair.second = false;
    }
    for (auto& pair : m_previousActionStates) {
        pair.second = false;
    }
}

void MobileInputManager::resetToDefaults() {
    clearInputState();
    setControlSensitivity(1.0f);
    setControlDeadZone(0.1f);
    setControlScale(1.0f);
    setControlOpacity(0.8f);
    resetToDefaultLayout();
}

void MobileInputManager::processActions() {
    for (const auto& binding : m_actionBindings) {
        if (binding.isEnabled) {
            bool actionState = evaluateAction(binding);
            m_actionStates[binding.action] = actionState;
        }
    }
}

void MobileInputManager::updateActionStates() {
    m_previousActionStates = m_actionStates;
}

void MobileInputManager::updateTouchState() {
    m_touchState.position = getTouchPosition(0);
    m_touchState.delta = getTouchDelta(0);
    m_touchState.isPressed = isTouchPressed(0);
    m_touchState.isJustPressed = isTouchJustPressed(0);
    m_touchState.isJustReleased = isTouchJustReleased(0);
    m_touchState.pressure = getTouchPressure(0);
}

void MobileInputManager::updateGestureState() {
    if (isGestureActive()) {
        auto gesture = MobileInput::TouchManager::getCurrentGesture();
        m_gestureState.type = (GestureState::Type)gesture.type;
        m_gestureState.startPosition = gesture.startPosition;
        m_gestureState.endPosition = gesture.endPosition;
        m_gestureState.delta = gesture.delta;
        m_gestureState.scale = gesture.scale;
        m_gestureState.rotation = gesture.rotation;
        m_gestureState.duration = gesture.duration;
        m_gestureState.isActive = gesture.isActive;
    } else {
        m_gestureState.isActive = false;
    }
}

bool MobileInputManager::evaluateAction(const ActionBinding& binding) const {
    switch (binding.controlType) {
        case MobileInput::VirtualControls::ControlType::Joystick:
            if (binding.controlId >= 0) {
                Vector2 input = MobileInput::VirtualControls::getJoystickInput(binding.controlId);
                return input.magnitude() > binding.threshold;
            }
            break;
            
        case MobileInput::VirtualControls::ControlType::Button:
            if (binding.controlId >= 0) {
                return MobileInput::VirtualControls::isButtonPressed(binding.controlId);
            }
            break;
            
        case MobileInput::VirtualControls::ControlType::Slider:
            if (binding.controlId >= 0) {
                float value = MobileInput::VirtualControls::getSliderValue(binding.controlId);
                return value > binding.threshold;
            }
            break;
            
        case MobileInput::VirtualControls::ControlType::Toggle:
            // Toggle logic would be implemented here
            break;
    }
    
    return false;
}

float MobileInputManager::getActionValue(const ActionBinding& binding) const {
    switch (binding.controlType) {
        case MobileInput::VirtualControls::ControlType::Joystick:
            if (binding.controlId >= 0) {
                Vector2 input = MobileInput::VirtualControls::getJoystickInput(binding.controlId);
                return input.magnitude();
            }
            break;
            
        case MobileInput::VirtualControls::ControlType::Slider:
            if (binding.controlId >= 0) {
                return MobileInput::VirtualControls::getSliderValue(binding.controlId);
            }
            break;
    }
    
    return 0.0f;
}

void MobileInputManager::createRacingControls() {
    // Create joystick for steering and acceleration
    m_joystickId = MobileInput::VirtualControls::addJoystick(
        Vector2(200, 200), 80.0f, Vector3(0.3f, 0.3f, 0.3f)
    );
    
    // Create buttons for boost and handbrake
    m_boostButtonId = MobileInput::VirtualControls::addButton(
        Vector2(600, 150), Vector2(80, 80), "BOOST", Vector3(0.8f, 0.2f, 0.2f)
    );
    
    m_handbrakeButtonId = MobileInput::VirtualControls::addButton(
        Vector2(700, 150), Vector2(80, 80), "BRAKE", Vector3(0.2f, 0.2f, 0.8f)
    );
    
    // Bind actions
    bindAction(Action::SteerLeft, MobileInput::VirtualControls::ControlType::Joystick, m_joystickId);
    bindAction(Action::SteerRight, MobileInput::VirtualControls::ControlType::Joystick, m_joystickId);
    bindAction(Action::Accelerate, MobileInput::VirtualControls::ControlType::Joystick, m_joystickId);
    bindAction(Action::Brake, MobileInput::VirtualControls::ControlType::Joystick, m_joystickId);
    bindAction(Action::Boost, MobileInput::VirtualControls::ControlType::Button, m_boostButtonId);
    bindAction(Action::Handbrake, MobileInput::VirtualControls::ControlType::Button, m_handbrakeButtonId);
}

void MobileInputManager::createCombatControls() {
    // Create joystick for movement
    m_joystickId = MobileInput::VirtualControls::addJoystick(
        Vector2(200, 200), 80.0f, Vector3(0.3f, 0.3f, 0.3f)
    );
    
    // Create combat buttons
    m_attack1ButtonId = MobileInput::VirtualControls::addButton(
        Vector2(600, 200), Vector2(80, 80), "ATK1", Vector3(0.8f, 0.2f, 0.2f)
    );
    
    m_attack2ButtonId = MobileInput::VirtualControls::addButton(
        Vector2(700, 200), Vector2(80, 80), "ATK2", Vector3(0.2f, 0.8f, 0.2f)
    );
    
    m_shieldButtonId = MobileInput::VirtualControls::addButton(
        Vector2(600, 100), Vector2(80, 80), "SHIELD", Vector3(0.2f, 0.2f, 0.8f)
    );
    
    m_teleportButtonId = MobileInput::VirtualControls::addButton(
        Vector2(700, 100), Vector2(80, 80), "TELE", Vector3(0.8f, 0.8f, 0.2f)
    );
    
    // Bind actions
    bindAction(Action::SteerLeft, MobileInput::VirtualControls::ControlType::Joystick, m_joystickId);
    bindAction(Action::SteerRight, MobileInput::VirtualControls::ControlType::Joystick, m_joystickId);
    bindAction(Action::Accelerate, MobileInput::VirtualControls::ControlType::Joystick, m_joystickId);
    bindAction(Action::Brake, MobileInput::VirtualControls::ControlType::Joystick, m_joystickId);
    bindAction(Action::Attack1, MobileInput::VirtualControls::ControlType::Button, m_attack1ButtonId);
    bindAction(Action::Attack2, MobileInput::VirtualControls::ControlType::Button, m_attack2ButtonId);
    bindAction(Action::Shield, MobileInput::VirtualControls::ControlType::Button, m_shieldButtonId);
    bindAction(Action::Teleport, MobileInput::VirtualControls::ControlType::Button, m_teleportButtonId);
}

void MobileInputManager::createMenuControls() {
    // Create menu buttons
    m_menuButtonId = MobileInput::VirtualControls::addButton(
        Vector2(400, 300), Vector2(120, 60), "MENU", Vector3(0.5f, 0.5f, 0.5f)
    );
    
    m_pauseButtonId = MobileInput::VirtualControls::addButton(
        Vector2(400, 200), Vector2(120, 60), "PAUSE", Vector3(0.8f, 0.8f, 0.2f)
    );
    
    // Bind actions
    bindAction(Action::Menu, MobileInput::VirtualControls::ControlType::Button, m_menuButtonId);
    bindAction(Action::Pause, MobileInput::VirtualControls::ControlType::Button, m_pauseButtonId);
}

void MobileInputManager::createGestureControls() {
    // Gesture controls are handled by the touch manager
    // No virtual controls needed for gestures
}

void MobileInputManager::applyLayout(const std::string& layout) {
    if (layout == "racing") {
        setDefaultRacingLayout();
    } else if (layout == "combat") {
        setDefaultCombatLayout();
    } else if (layout == "menu") {
        setDefaultMenuLayout();
    } else if (layout == "gesture") {
        setDefaultGestureLayout();
    }
}

void MobileInputManager::saveCurrentLayout() {
    // Save current layout to file
    // Implementation would save to platform-specific storage
}

void MobileInputManager::loadSavedLayout() {
    // Load saved layout from file
    // Implementation would load from platform-specific storage
}

void MobileInputManager::setDefaultRacingLayout() {
    // Set default racing control positions
    if (m_joystickId >= 0) {
        MobileInput::VirtualControls::setJoystickPosition(m_joystickId, Vector2(200, 200));
    }
    if (m_boostButtonId >= 0) {
        MobileInput::VirtualControls::setButtonPosition(m_boostButtonId, Vector2(600, 150));
    }
    if (m_handbrakeButtonId >= 0) {
        MobileInput::VirtualControls::setButtonPosition(m_handbrakeButtonId, Vector2(700, 150));
    }
}

void MobileInputManager::setDefaultCombatLayout() {
    // Set default combat control positions
    if (m_joystickId >= 0) {
        MobileInput::VirtualControls::setJoystickPosition(m_joystickId, Vector2(200, 200));
    }
    if (m_attack1ButtonId >= 0) {
        MobileInput::VirtualControls::setButtonPosition(m_attack1ButtonId, Vector2(600, 200));
    }
    if (m_attack2ButtonId >= 0) {
        MobileInput::VirtualControls::setButtonPosition(m_attack2ButtonId, Vector2(700, 200));
    }
    if (m_shieldButtonId >= 0) {
        MobileInput::VirtualControls::setButtonPosition(m_shieldButtonId, Vector2(600, 100));
    }
    if (m_teleportButtonId >= 0) {
        MobileInput::VirtualControls::setButtonPosition(m_teleportButtonId, Vector2(700, 100));
    }
}

void MobileInputManager::setDefaultMenuLayout() {
    // Set default menu control positions
    if (m_menuButtonId >= 0) {
        MobileInput::VirtualControls::setButtonPosition(m_menuButtonId, Vector2(400, 300));
    }
    if (m_pauseButtonId >= 0) {
        MobileInput::VirtualControls::setButtonPosition(m_pauseButtonId, Vector2(400, 200));
    }
}

void MobileInputManager::setDefaultGestureLayout() {
    // Gesture layout doesn't need positioning
}