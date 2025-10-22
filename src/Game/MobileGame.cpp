#include "MobileGame.h"
#include <iostream>
#include <chrono>
#include <algorithm>

MobileGame::MobileGame() 
    : Game()
    , platform(nullptr)
    , touchInput(nullptr)
    , mobileRenderer(nullptr)
    , batteryLevel(1.0f)
    , isLowPowerMode(false)
    , thermalState(0.0f)
    , isPausedByOS(false)
    , isInBackground(false) {
    
    // Default mobile settings
    mobileSettings.useTiltControls = false;
    mobileSettings.tiltSensitivity = 1.0f;
    mobileSettings.useHapticFeedback = true;
    mobileSettings.autoAccelerate = false;
    mobileSettings.controlLayout = TouchInputManager::ControlLayout::Racing;
    mobileSettings.graphicsQuality = 1; // Medium
    mobileSettings.enableVibration = true;
    mobileSettings.uiScale = 1.0f;
}

MobileGame::~MobileGame() {
    shutdown();
}

bool MobileGame::initializeMobile(PlatformBase* platformInstance) {
    platform = platformInstance;
    if (!platform) {
        std::cerr << "Invalid platform instance" << std::endl;
        return false;
    }
    
    // Get device info
    const auto& deviceInfo = platform->getDeviceInfo();
    
    // Initialize with device screen size
    return initialize(deviceInfo.screenWidth, deviceInfo.screenHeight, "Racing Game 3D Mobile");
}

bool MobileGame::initialize(int width, int height, const std::string& title) {
    screenWidth = width;
    screenHeight = height;
    
    // Create window through platform
    if (platform && !platform->createWindow(width, height, title)) {
        std::cerr << "Failed to create platform window" << std::endl;
        return false;
    }
    
    // Create mobile renderer
    mobileRenderer = new MobileRenderer();
    renderer.reset(mobileRenderer);
    
    if (!mobileRenderer->initializeMobile(platform, width, height)) {
        std::cerr << "Failed to initialize mobile renderer" << std::endl;
        return false;
    }
    
    // Initialize touch input
    touchInput = new TouchInputManager();
    touchInput->initialize(width, height);
    
    // Calculate UI scale based on screen DPI
    if (platform) {
        const auto& deviceInfo = platform->getDeviceInfo();
        float dpiScale = deviceInfo.screenDPI / 160.0f; // 160 is baseline Android DPI
        mobileSettings.uiScale = std::min(2.0f, std::max(0.5f, dpiScale));
        
        // Adjust for screen size
        if (deviceInfo.screenWidth < 1280 || deviceInfo.screenHeight < 720) {
            mobileSettings.graphicsQuality = 0; // Low quality for low-res screens
        } else if (deviceInfo.screenWidth > 2048 || deviceInfo.screenHeight > 1536) {
            mobileSettings.graphicsQuality = 2; // High quality for high-res screens
        }
    }
    
    // Initialize physics engine
    physicsEngine = std::make_unique<PhysicsEngine>();
    
    // Initialize combat manager
    combatManager = std::make_unique<CombatManager>();
    
    // Setup mobile-specific callbacks
    setupMobileCallbacks();
    
    // Initialize game systems
    initializeGame();
    
    // Setup touch controls
    setupTouchControls();
    
    // Load mobile settings
    loadMobileSettings();
    
    // Optimize for device
    optimizeForDevice();
    
    isRunning = true;
    return true;
}

void MobileGame::shutdown() {
    isRunning = false;
    
    // Save settings before shutdown
    saveMobileSettings();
    
    if (touchInput) {
        touchInput->shutdown();
        delete touchInput;
        touchInput = nullptr;
    }
    
    // Call parent shutdown
    Game::shutdown();
    
    platform = nullptr;
    mobileRenderer = nullptr;
}

void MobileGame::run() {
    auto lastTime = std::chrono::high_resolution_clock::now();
    
    while (isRunning && platform && !platform->shouldClose()) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;
        
        // Cap delta time
        deltaTime = std::min(deltaTime, 0.033f); // Cap at 30 FPS minimum
        
        // Process platform events
        platform->processEvents();
        
        // Skip update if paused by OS
        if (!isPausedByOS && !isInBackground) {
            update(deltaTime);
            render();
            
            if (platform->supportsOpenGLES()) {
                platform->swapBuffers();
            }
        }
        
        // Update performance metrics
        frameCount++;
        lastFpsUpdate += deltaTime;
        if (lastFpsUpdate >= 1.0f) {
            fps = frameCount / lastFpsUpdate;
            frameCount = 0;
            lastFpsUpdate = 0.0f;
            
            // Auto-adjust quality if needed
            if (mobileRenderer) {
                mobileRenderer->autoOptimizeForPerformance(fps);
            }
        }
        
        // Update battery and thermal state
        updateBatteryStatus();
        updateThermalState();
    }
}

void MobileGame::update(float dt) {
    deltaTime = dt;
    gameTime += dt;
    
    if (!isPaused) {
        handleMobileInput();
        
        if (currentState == GameState::PvPMode) {
            updatePvPMode(dt);
        } else {
            updatePhysics(dt);
            updateGameplay(dt);
        }
        
        updateCamera(dt);
        updateParticles(dt);
        updateTrails(dt);
        
        // Update touch input
        if (touchInput) {
            touchInput->updateGestureRecognition(dt);
        }
    }
    
    updateUI(dt);
    updateAudio(dt);
}

void MobileGame::render() {
    if (!mobileRenderer) return;
    
    mobileRenderer->beginFrameMobile();
    
    switch (currentState) {
        case GameState::Menu:
            renderMobileMenu();
            break;
        case GameState::Playing:
            renderGame();
            renderMobileHUD();
            break;
        case GameState::Paused:
            renderGame();
            renderMobilePauseMenu();
            break;
        case GameState::GameOver:
            renderGame();
            break;
        case GameState::PvPMode:
            renderPvPMode();
            renderMobileHUD();
            break;
        case GameState::StatsMenu:
            renderStatsMenu();
            break;
    }
    
    // Render virtual controls
    if (touchInput && mobileRenderer) {
        mobileRenderer->renderVirtualControls(touchInput);
    }
    
    if (showDebugInfo) {
        renderDebugInfo();
    }
    
    mobileRenderer->endFrameMobile();
}

void MobileGame::handleMobileInput() {
    if (!touchInput) return;
    
    // Handle different input based on game state
    switch (currentState) {
        case GameState::Menu:
            // Menu navigation handled by touch
            break;
            
        case GameState::Playing:
            // Racing controls
            if (playerCar) {
                if (mobileSettings.autoAccelerate) {
                    playerCar->setThrottle(1.0f);
                } else {
                    playerCar->setThrottle(touchInput->getAccelerateInput());
                }
                
                playerCar->setBrake(touchInput->getBrakeInput());
                playerCar->setSteer(touchInput->getSteerInput());
                playerCar->setBoost(touchInput->isButtonPressed("Boost"));
                playerCar->setHandbrake(touchInput->isButtonPressed("Handbrake"));
            }
            
            // Camera control
            if (camera) {
                Vector2 cameraInput = touchInput->getCameraInput();
                if (cameraInput.magnitude() > 0) {
                    camera->handleMouseInput(cameraInput.x * 2.0f, cameraInput.y * 2.0f);
                }
            }
            break;
            
        case GameState::PvPMode:
            // Combat controls
            if (localPlayer) {
                Vector2 moveInput = touchInput->getMovementInput();
                if (moveInput.magnitude() > 0) {
                    Vector3 moveDir = Vector3(moveInput.x, 0, moveInput.y);
                    moveDir = moveDir.normalized() * 10.0f;
                    localPlayer->setVelocity(moveDir);
                } else {
                    localPlayer->setVelocity(Vector3::zero());
                }
                
                Vector2 lookInput = touchInput->getCameraInput();
                if (lookInput.magnitude() > 0 && camera) {
                    camera->handleMouseInput(lookInput.x * 2.0f, lookInput.y * 2.0f);
                    localPlayer->setLookDirection(camera->getForward());
                }
            }
            break;
    }
}

void MobileGame::handleTouch(const PlatformBase::TouchPoint& touch) {
    if (!touchInput) return;
    
    switch (touch.state) {
        case PlatformBase::TouchPoint::Began:
            touchInput->handleTouchBegin(touch);
            break;
        case PlatformBase::TouchPoint::Moved:
            touchInput->handleTouchMove(touch);
            break;
        case PlatformBase::TouchPoint::Ended:
        case PlatformBase::TouchPoint::Cancelled:
            touchInput->handleTouchEnd(touch);
            break;
    }
}

void MobileGame::handleGesture(TouchInputManager::GestureType type, const TouchInputManager::Gesture& gesture) {
    switch (type) {
        case TouchInputManager::GestureType::Tap:
            // Handle tap
            break;
        case TouchInputManager::GestureType::DoubleTap:
            // Toggle camera mode
            if (camera) {
                auto mode = camera->getMode();
                if (mode == Camera::CameraMode::ThirdPerson) {
                    camera->setMode(Camera::CameraMode::FirstPerson);
                } else {
                    camera->setMode(Camera::CameraMode::ThirdPerson);
                }
            }
            break;
        case TouchInputManager::GestureType::Swipe:
            // Handle swipe for menu navigation
            if (currentState == GameState::Menu) {
                // Navigate menu options based on swipe direction
            }
            break;
        case TouchInputManager::GestureType::Pinch:
            // Zoom camera
            if (camera) {
                camera->handleScrollInput(gesture.scale > 1.0f ? -1.0f : 1.0f);
            }
            break;
    }
}

void MobileGame::handleAccelerometer(float x, float y, float z) {
    if (touchInput && mobileSettings.useTiltControls) {
        touchInput->handleAccelerometer(x, y, z);
    }
}

void MobileGame::handleGyroscope(float x, float y, float z) {
    if (touchInput) {
        touchInput->handleGyroscope(x, y, z);
    }
}

void MobileGame::onPause() {
    isPausedByOS = true;
    isPaused = true;
    
    // Save game state
    saveMobileSettings();
    
    // Pause audio
    // TODO: Implement audio pause
}

void MobileGame::onResume() {
    isPausedByOS = false;
    
    // Resume audio
    // TODO: Implement audio resume
}

void MobileGame::onLowMemory() {
    // Reduce texture quality
    if (mobileRenderer) {
        MobileRenderer::MobileOptimizations opts = mobileRenderer->getOptimizations();
        opts.textureQuality = 0; // Low quality
        opts.reducedParticles = true;
        opts.disablePostProcessing = true;
        mobileRenderer->setOptimizations(opts);
    }
}

void MobileGame::onOrientationChange(PlatformBase::DeviceOrientation orientation) {
    // Handle orientation change
    if (platform) {
        int width, height;
        platform->getWindowSize(width, height);
        
        screenWidth = width;
        screenHeight = height;
        
        if (renderer) {
            renderer->setViewport(0, 0, width, height);
        }
        
        if (touchInput) {
            touchInput->initialize(width, height);
            setupTouchControls(); // Re-setup controls for new orientation
        }
    }
}

void MobileGame::setupTouchControls() {
    if (!touchInput) return;
    
    // Set control layout based on game mode
    switch (currentState) {
        case GameState::Menu:
            touchInput->setControlLayout(TouchInputManager::ControlLayout::Menu);
            break;
        case GameState::Playing:
            touchInput->setControlLayout(TouchInputManager::ControlLayout::Racing);
            break;
        case GameState::PvPMode:
            touchInput->setControlLayout(TouchInputManager::ControlLayout::Combat);
            break;
        default:
            touchInput->setControlLayout(TouchInputManager::ControlLayout::Menu);
            break;
    }
    
    // Configure tilt controls
    touchInput->setTiltControlsEnabled(mobileSettings.useTiltControls);
    touchInput->setTiltSensitivity(mobileSettings.tiltSensitivity);
}

void MobileGame::switchControlLayout(TouchInputManager::ControlLayout layout) {
    if (touchInput) {
        touchInput->setControlLayout(layout);
        mobileSettings.controlLayout = layout;
    }
}

void MobileGame::renderMobileMenu() {
    if (!mobileRenderer) return;
    
    std::vector<std::string> menuOptions = {
        "Racing Mode",
        "PvP Combat",
        "Settings",
        "Leaderboard",
        "Exit"
    };
    
    mobileRenderer->renderMobileMenu(menuOptions, -1);
}

void MobileGame::renderMobilePauseMenu() {
    if (!mobileRenderer) return;
    
    std::vector<std::string> pauseOptions = {
        "Resume",
        "Settings",
        "Restart",
        "Main Menu"
    };
    
    mobileRenderer->renderMobileMenu(pauseOptions, -1);
}

void MobileGame::renderMobileSettings() {
    if (!mobileRenderer) return;
    
    // Render settings UI
    // TODO: Implement detailed settings menu
}

void MobileGame::renderMobileHUD() {
    if (!mobileRenderer || !playerCar) return;
    
    float speed = playerCar->getSpeed() * 3.6f; // Convert to km/h
    mobileRenderer->renderMobileHUD(speed, currentLap, currentLapTime, 1);
}

void MobileGame::optimizeForDevice() {
    if (!platform || !mobileRenderer) return;
    
    const auto& deviceInfo = platform->getDeviceInfo();
    MobileRenderer::MobileOptimizations opts;
    
    // Determine optimization level based on device capabilities
    if (deviceInfo.memoryMB < 2048) {
        // Low-end device
        opts.useLowPolyModels = true;
        opts.useSimpleShaders = true;
        opts.reducedParticles = true;
        opts.lowResolutionShadows = true;
        opts.disablePostProcessing = true;
        opts.maxDrawDistance = 100;
        opts.textureQuality = 0;
        opts.renderScale = 0.75f;
    } else if (deviceInfo.memoryMB < 4096) {
        // Mid-range device
        opts.useLowPolyModels = false;
        opts.useSimpleShaders = false;
        opts.reducedParticles = true;
        opts.lowResolutionShadows = true;
        opts.disablePostProcessing = false;
        opts.maxDrawDistance = 200;
        opts.textureQuality = 1;
        opts.renderScale = 0.85f;
    } else {
        // High-end device
        opts.useLowPolyModels = false;
        opts.useSimpleShaders = false;
        opts.reducedParticles = false;
        opts.lowResolutionShadows = false;
        opts.disablePostProcessing = false;
        opts.maxDrawDistance = 500;
        opts.textureQuality = 2;
        opts.renderScale = 1.0f;
    }
    
    mobileRenderer->setOptimizations(opts);
    mobileRenderer->setTargetFPS(60.0f);
}

void MobileGame::adjustGraphicsQuality(int quality) {
    mobileSettings.graphicsQuality = quality;
    optimizeForDevice();
}

void MobileGame::vibrate(int milliseconds) {
    if (platform && mobileSettings.enableVibration) {
        platform->vibrate(milliseconds);
    }
}

void MobileGame::playHapticFeedback(float intensity) {
    if (mobileSettings.useHapticFeedback) {
        int duration = static_cast<int>(intensity * 50); // Scale intensity to duration
        vibrate(duration);
    }
}

void MobileGame::setupMobileCallbacks() {
    if (!platform) return;
    
    // Set platform callbacks
    platform->setTouchBeginCallback([this](const PlatformBase::TouchPoint& touch) {
        handleTouch(touch);
    });
    
    platform->setTouchMoveCallback([this](const PlatformBase::TouchPoint& touch) {
        handleTouch(touch);
    });
    
    platform->setTouchEndCallback([this](const PlatformBase::TouchPoint& touch) {
        handleTouch(touch);
    });
    
    platform->setAccelerometerCallback([this](float x, float y, float z) {
        handleAccelerometer(x, y, z);
    });
    
    platform->setGyroscopeCallback([this](float x, float y, float z) {
        handleGyroscope(x, y, z);
    });
    
    platform->setOrientationChangeCallback([this](PlatformBase::DeviceOrientation orientation) {
        onOrientationChange(orientation);
    });
    
    platform->setPauseCallback([this]() {
        onPause();
    });
    
    platform->setResumeCallback([this]() {
        onResume();
    });
    
    platform->setLowMemoryCallback([this]() {
        onLowMemory();
    });
    
    // Set touch input callbacks
    if (touchInput) {
        touchInput->setGestureCallback([this](TouchInputManager::GestureType type, 
                                             const TouchInputManager::Gesture& gesture) {
            handleGesture(type, gesture);
        });
        
        touchInput->setButtonCallback([this](const std::string& label, bool pressed) {
            if (pressed) {
                playHapticFeedback(0.5f);
            }
        });
    }
}

void MobileGame::loadMobileSettings() {
    // TODO: Load settings from persistent storage
    // For now, use defaults
}

void MobileGame::saveMobileSettings() {
    // TODO: Save settings to persistent storage
}

void MobileGame::updateBatteryStatus() {
    // TODO: Query battery level from platform
    // Adjust performance if battery is low
    if (batteryLevel < 0.2f && !isLowPowerMode) {
        enablePowerSaving(true);
    }
}

void MobileGame::updateThermalState() {
    // TODO: Query thermal state from platform
    // Reduce quality if device is overheating
}

void MobileGame::enablePowerSaving(bool enable) {
    isLowPowerMode = enable;
    
    if (mobileRenderer) {
        if (enable) {
            // Reduce quality for power saving
            MobileRenderer::MobileOptimizations opts = mobileRenderer->getOptimizations();
            opts.renderScale = 0.5f;
            opts.reducedParticles = true;
            opts.disablePostProcessing = true;
            mobileRenderer->setOptimizations(opts);
            mobileRenderer->setTargetFPS(30.0f);
        } else {
            // Restore normal quality
            optimizeForDevice();
            mobileRenderer->setTargetFPS(60.0f);
        }
    }
}