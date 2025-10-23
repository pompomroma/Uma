#include "MobileUI.h"
#include <cmath>

MobileUI::MobileUI()
    : touchManager(nullptr)
    , screenWidth(1920.0f)
    , screenHeight(1080.0f)
    , showVirtualControls(true)
    , showFPS(false)
    , showDebugInfo(false)
    , currentOrientation(Orientation::Landscape) {
    
    safeArea = {0.0f, 0.0f, 0.0f, 0.0f};
}

MobileUI::~MobileUI() {
    shutdown();
}

void MobileUI::initialize(TouchInputManager* touchMgr, float width, float height) {
    touchManager = touchMgr;
    screenWidth = width;
    screenHeight = height;
    
    // Determine initial orientation
    if (width > height) {
        currentOrientation = Orientation::Landscape;
        layoutForLandscape();
    } else {
        currentOrientation = Orientation::Portrait;
        layoutForPortrait();
    }
}

void MobileUI::shutdown() {
    touchManager = nullptr;
}

void MobileUI::update(float deltaTime) {
    // Update UI animations, transitions, etc.
}

void MobileUI::render() {
    if (!touchManager) return;
    
    // This would be called from the renderer
    // For now, it's a placeholder that shows what should be rendered
    
    if (showVirtualControls) {
        // Render left joystick (only when active for dynamic joystick)
        const auto& leftJoystick = touchManager->getLeftJoystickState();
        if (leftJoystick.isActive || !leftJoystick.isDynamic) {
            renderVirtualJoystick(leftJoystick);
        }
        
        // Don't render right joystick - using camera drag instead
        // Right half of screen is for camera rotation
        
        // Render buttons
        for (const auto& button : touchManager->getButtons()) {
            renderVirtualButton(button);
        }
    }
}

void MobileUI::renderVirtualJoystick(const TouchInputManager::VirtualJoystick& joystick) {
    // This should be implemented in the actual renderer
    // Draw outer circle
    // Draw inner circle (knob)
    // Draw direction indicator if active
}

void MobileUI::renderVirtualButton(const TouchInputManager::VirtualButton& button) {
    // This should be implemented in the actual renderer
    // Draw button circle
    // Draw label
    // Highlight if pressed
}

void MobileUI::renderFPS(float fps) {
    // Render FPS counter in top-left corner
}

void MobileUI::renderGameHUD(int lap, int totalLaps, float time, float speed) {
    // Render racing game HUD
    // - Current lap / total laps
    // - Current time
    // - Speed
    // - Minimap (if applicable)
}

void MobileUI::renderCombatHUD(float health, float maxHealth, float stamina, float maxStamina) {
    // Render combat game HUD
    // - Health bar
    // - Stamina bar
    // - Ability cooldowns
}

void MobileUI::setShowVirtualControls(bool show) {
    showVirtualControls = show;
}

void MobileUI::setShowFPS(bool show) {
    showFPS = show;
}

void MobileUI::setShowDebugInfo(bool show) {
    showDebugInfo = show;
}

void MobileUI::setOrientation(Orientation orientation) {
    if (currentOrientation != orientation) {
        currentOrientation = orientation;
        updateLayout();
    }
}

void MobileUI::setSafeArea(float top, float bottom, float left, float right) {
    safeArea.top = top;
    safeArea.bottom = bottom;
    safeArea.left = left;
    safeArea.right = right;
    adjustForSafeArea();
}

void MobileUI::setScreenSize(float width, float height) {
    screenWidth = width;
    screenHeight = height;
    
    if (touchManager) {
        touchManager->setScreenSize(width, height);
    }
    
    updateLayout();
}

void MobileUI::layoutForPortrait() {
    if (!touchManager) return;
    
    // Portrait layout: controls at bottom
    float margin = 100.0f;
    
    // Left dynamic joystick (bottom left) - appears where you touch
    touchManager->setupLeftJoystick(
        Vector2(margin + 80.0f, screenHeight - margin - 80.0f),
        100.0f, 40.0f,
        true  // Dynamic joystick
    );
    
    // No right joystick - using camera drag on right half of screen
    
    // Action buttons on right side, middle
    float centerY = screenHeight * 0.5f;
    touchManager->addButton("accelerate", Vector2(screenWidth - 80.0f, centerY - 100.0f), 50.0f);
    touchManager->addButton("brake", Vector2(screenWidth - 80.0f, centerY), 50.0f);
    touchManager->addButton("boost", Vector2(screenWidth - 180.0f, centerY - 50.0f), 45.0f);
}

void MobileUI::layoutForLandscape() {
    if (!touchManager) return;
    
    // Landscape layout: dynamic joystick on left, camera drag on right
    float margin = 80.0f;
    
    // Left dynamic joystick for movement (bottom left) - appears where you touch
    touchManager->setupLeftJoystick(
        Vector2(margin + 100.0f, screenHeight - margin - 100.0f),
        100.0f, 40.0f,
        true  // Dynamic joystick
    );
    
    // No right joystick - using camera drag on right half of screen
    
    // Clear existing buttons and add new ones in landscape positions
    // Action buttons (right side, vertically centered)
    float rightX = screenWidth - 100.0f;
    float midY = screenHeight * 0.5f;
    
    // Note: addButton will append, so we'd need to clear first in a real implementation
    // For now, this shows the intended layout
}

void MobileUI::adjustForSafeArea() {
    // Adjust control positions to avoid safe area (notches, home indicators, etc.)
    if (!touchManager) return;
    
    // Re-layout based on safe area constraints
    updateLayout();
}

void MobileUI::updateLayout() {
    if (currentOrientation == Orientation::Portrait) {
        layoutForPortrait();
    } else {
        layoutForLandscape();
    }
    
    adjustForSafeArea();
}
