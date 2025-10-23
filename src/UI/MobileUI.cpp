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
        // Render dynamic joystick (only when active)
        const auto& joystick = touchManager->getJoystickState();
        if (joystick.fadeAlpha > 0.01f) {
            renderVirtualJoystick(joystick);
        }
        
        // Render buttons
        for (const auto& button : touchManager->getButtons()) {
            renderVirtualButton(button);
        }
        
        // Visual indicator for camera drag zone (subtle overlay)
        // Could render a faint border or gradient on the right half
    }
}

void MobileUI::renderVirtualJoystick(const TouchInputManager::VirtualJoystick& joystick) {
    // This should be implemented in the actual renderer
    // For dynamic joystick with fade effect:
    // - Apply alpha based on joystick.fadeAlpha
    // - Draw outer circle at joystick.centerPosition with joystick.outerRadius
    // - Draw inner circle (knob) at joystick.currentPosition with joystick.innerRadius
    // - Optional: Draw direction line from center to knob
    // - Use semi-transparent colors for better visibility
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
    
    // Left joystick (bottom left)
    touchManager->setupLeftJoystick(
        Vector2(margin + 80.0f, screenHeight - margin - 80.0f),
        80.0f, 30.0f
    );
    
    // Right joystick (bottom right)
    touchManager->setupRightJoystick(
        Vector2(screenWidth - margin - 80.0f, screenHeight - margin - 80.0f),
        70.0f, 25.0f
    );
    
    // Action buttons on right side, middle
    float centerY = screenHeight * 0.5f;
    touchManager->addButton("accelerate", Vector2(screenWidth - 80.0f, centerY - 100.0f), 50.0f);
    touchManager->addButton("brake", Vector2(screenWidth - 80.0f, centerY), 50.0f);
    touchManager->addButton("boost", Vector2(screenWidth - 180.0f, centerY - 50.0f), 45.0f);
}

void MobileUI::layoutForLandscape() {
    if (!touchManager) return;
    
    // Landscape layout: joysticks at bottom corners, buttons on sides
    float margin = 80.0f;
    
    // Left joystick for steering (bottom left)
    touchManager->setupLeftJoystick(
        Vector2(margin + 100.0f, screenHeight - margin - 100.0f),
        100.0f, 40.0f
    );
    
    // Right joystick for camera (bottom right)  
    touchManager->setupRightJoystick(
        Vector2(screenWidth - margin - 100.0f, screenHeight - margin - 100.0f),
        80.0f, 30.0f
    );
    
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
