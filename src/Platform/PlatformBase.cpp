#include "PlatformBase.h"
#include <algorithm>

PlatformBase::PlatformBase() {
    currentPlatform = Platform::Desktop;
    orientation.orientation = DeviceOrientation::LandscapeLeft;
    orientation.isLocked = false;
}

PlatformBase::~PlatformBase() {
    activeTouches.clear();
}

TouchPoint* PlatformBase::getTouchById(int id) {
    auto it = std::find_if(activeTouches.begin(), activeTouches.end(),
        [id](const TouchPoint& touch) { return touch.id == id; });
    return (it != activeTouches.end()) ? &(*it) : nullptr;
}

bool PlatformBase::isTouchActive(int id) const {
    return std::any_of(activeTouches.begin(), activeTouches.end(),
        [id](const TouchPoint& touch) { return touch.id == id; });
}

void PlatformBase::updateDeviceInfo() {
    // Base implementation - derived classes should override
}

void PlatformBase::handleTouchBegin(const TouchPoint& touch) {
    activeTouches.push_back(touch);
    if (onTouchBegin) onTouchBegin(touch);
    if (onMultiTouch) onMultiTouch(activeTouches);
}

void PlatformBase::handleTouchMove(const TouchPoint& touch) {
    TouchPoint* existingTouch = getTouchById(touch.id);
    if (existingTouch) {
        *existingTouch = touch;
        if (onTouchMove) onTouchMove(touch);
        if (onMultiTouch) onMultiTouch(activeTouches);
    }
}

void PlatformBase::handleTouchEnd(const TouchPoint& touch) {
    auto it = std::find_if(activeTouches.begin(), activeTouches.end(),
        [&touch](const TouchPoint& t) { return t.id == touch.id; });
    if (it != activeTouches.end()) {
        activeTouches.erase(it);
        if (onTouchEnd) onTouchEnd(touch);
        if (onMultiTouch) onMultiTouch(activeTouches);
    }
}

void PlatformBase::handleOrientationChange(DeviceOrientation newOrientation) {
    orientation = newOrientation;
    if (onOrientationChange) onOrientationChange(newOrientation);
}

// Platform factory implementation
#ifdef __ANDROID__
#include "AndroidPlatform.h"
#elif defined(__APPLE__)
#include "TargetConditionals.h"
#if TARGET_OS_IPHONE
#include "iOSPlatform.h"
#else
#include "DesktopPlatform.h"
#endif
#else
#include "DesktopPlatform.h"
#endif

PlatformBase* PlatformBase::createPlatform() {
#ifdef __ANDROID__
    return new AndroidPlatform();
#elif defined(__APPLE__)
#if TARGET_OS_IPHONE
    return new iOSPlatform();
#else
    return new DesktopPlatform();
#endif
#else
    return new DesktopPlatform();
#endif
}