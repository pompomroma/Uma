#include "Game/MobileGame.h"
#include "Platform/PlatformBase.h"
#include <iostream>
#include <exception>
#include <memory>

std::unique_ptr<MobileGame> g_game;
std::unique_ptr<PlatformBase> g_platform;

bool InitializeGame() {
    try {
        // Create platform instance
        g_platform.reset(PlatformBase::createPlatform());
        if (!g_platform->initialize()) {
            std::cerr << "Failed to initialize platform" << std::endl;
            return false;
        }
        
        // Get device info
        const auto& deviceInfo = g_platform->getDeviceInfo();
        std::cout << "Device: " << deviceInfo.deviceModel << std::endl;
        std::cout << "Screen: " << deviceInfo.screenWidth << "x" << deviceInfo.screenHeight << std::endl;
        std::cout << "DPI: " << deviceInfo.screenDPI << std::endl;
        
        // Create game instance
        g_game = std::make_unique<MobileGame>();
        
        // Initialize game with mobile platform
        if (!g_game->initializeMobile(g_platform.get())) {
            std::cerr << "Failed to initialize mobile game" << std::endl;
            return false;
        }
        
        std::cout << "Mobile Racing Game initialized successfully!" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception during initialization: " << e.what() << std::endl;
        return false;
    } catch (...) {
        std::cerr << "Unknown exception during initialization" << std::endl;
        return false;
    }
}

void RunGame() {
    if (!g_game || !g_platform) {
        return;
    }
    
    try {
        g_game->run();
    } catch (const std::exception& e) {
        std::cerr << "Exception during game execution: " << e.what() << std::endl;
    }
}

void ShutdownGame() {
    if (g_game) {
        g_game->shutdown();
        g_game.reset();
    }
    
    if (g_platform) {
        g_platform->shutdown();
        g_platform.reset();
    }
    
    std::cout << "Game shutdown complete" << std::endl;
}

// Platform-specific entry points will call these functions
extern "C" {
    bool MobileGameInit() {
        return InitializeGame();
    }
    
    void MobileGameRun() {
        RunGame();
    }
    
    void MobileGameShutdown() {
        ShutdownGame();
    }
    
    void MobileGamePause() {
        if (g_game) {
            g_game->onPause();
        }
    }
    
    void MobileGameResume() {
        if (g_game) {
            g_game->onResume();
        }
    }
    
    void MobileGameTouch(int id, float x, float y, int state) {
        if (!g_game || !g_platform) return;
        
        PlatformBase::TouchPoint touch;
        touch.id = id;
        touch.x = x;
        touch.y = y;
        touch.pressure = 1.0f;
        touch.size = 1.0f;
        
        switch (state) {
            case 0: // Began
                touch.state = PlatformBase::TouchPoint::Began;
                g_game->handleTouch(touch);
                break;
            case 1: // Moved
                touch.state = PlatformBase::TouchPoint::Moved;
                g_game->handleTouch(touch);
                break;
            case 2: // Ended
                touch.state = PlatformBase::TouchPoint::Ended;
                g_game->handleTouch(touch);
                break;
            case 3: // Cancelled
                touch.state = PlatformBase::TouchPoint::Cancelled;
                g_game->handleTouch(touch);
                break;
        }
    }
    
    void MobileGameAccelerometer(float x, float y, float z) {
        if (g_game) {
            g_game->handleAccelerometer(x, y, z);
        }
    }
    
    void MobileGameGyroscope(float x, float y, float z) {
        if (g_game) {
            g_game->handleGyroscope(x, y, z);
        }
    }
    
    void MobileGameOrientationChange(int orientation) {
        if (!g_game || !g_platform) return;
        
        PlatformBase::DeviceOrientation deviceOrientation;
        switch (orientation) {
            case 0:
                deviceOrientation.orientation = PlatformBase::DeviceOrientation::Portrait;
                break;
            case 1:
                deviceOrientation.orientation = PlatformBase::DeviceOrientation::PortraitUpsideDown;
                break;
            case 2:
                deviceOrientation.orientation = PlatformBase::DeviceOrientation::LandscapeLeft;
                break;
            case 3:
                deviceOrientation.orientation = PlatformBase::DeviceOrientation::LandscapeRight;
                break;
            default:
                deviceOrientation.orientation = PlatformBase::DeviceOrientation::LandscapeLeft;
                break;
        }
        deviceOrientation.isLocked = false;
        
        g_game->onOrientationChange(deviceOrientation);
    }
    
    void MobileGameLowMemory() {
        if (g_game) {
            g_game->onLowMemory();
        }
    }
}