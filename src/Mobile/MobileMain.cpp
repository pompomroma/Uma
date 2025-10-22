#ifdef MOBILE_PLATFORM

#include "../Game.h"
#include "MobileRenderer.h"
#include "TouchInputManager.h"
#include "VirtualControls.h"
#include <iostream>
#include <memory>

#ifdef ANDROID_PLATFORM
    #include "Android/AndroidMain.h"
    #include <android/log.h>
    #define LOG_TAG "RacingGame3D"
    #define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
    #define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#elif defined(IOS_PLATFORM)
    #include "iOS/iOSMain.h"
    #define LOGI(...) printf(__VA_ARGS__)
    #define LOGE(...) printf(__VA_ARGS__)
#else
    #define LOGI(...) printf(__VA_ARGS__)
    #define LOGE(...) printf(__VA_ARGS__)
#endif

class MobileGame : public Game {
private:
    std::unique_ptr<MobileRenderer> mobileRenderer;
    std::unique_ptr<TouchInputManager> touchInputManager;
    std::unique_ptr<VirtualControls> virtualControls;
    
    // Mobile-specific properties
    Vector2 screenSize;
    float screenDensity;
    bool isLandscape;
    bool isPaused;
    bool isInitialized;
    
public:
    MobileGame() : screenDensity(160.0f), isLandscape(true), isPaused(false), isInitialized(false) {
        LOGI("MobileGame constructor called");
    }
    
    ~MobileGame() {
        LOGI("MobileGame destructor called");
        shutdown();
    }
    
    bool initializeMobile(int width, int height, float density = 160.0f) {
        LOGI("Initializing mobile game: %dx%d, density: %.1f", width, height, density);
        
        screenSize = Vector2(static_cast<float>(width), static_cast<float>(height));
        screenDensity = density;
        isLandscape = width > height;
        
        // Initialize mobile renderer
        mobileRenderer = std::make_unique<MobileRenderer>();
        if (!mobileRenderer->initialize(width, height, density / 160.0f)) {
            LOGE("Failed to initialize mobile renderer");
            return false;
        }
        
        // Initialize touch input manager
        touchInputManager = std::make_unique<TouchInputManager>();
        touchInputManager->initialize(screenSize, screenDensity);
        
        // Initialize virtual controls
        virtualControls = std::make_unique<VirtualControls>();
        virtualControls->initialize(mobileRenderer.get(), screenSize, screenDensity);
        
        // Setup input callbacks
        setupMobileInputCallbacks();
        
        // Initialize base game with mobile-specific settings
        if (!initializeGameSystems()) {
            LOGE("Failed to initialize game systems");
            return false;
        }
        
        // Setup mobile-specific UI layout
        setupMobileUI();
        
        isInitialized = true;
        LOGI("Mobile game initialized successfully!");
        return true;
    }
    
    void shutdown() override {
        if (!isInitialized) return;
        
        LOGI("Shutting down mobile game...");
        
        virtualControls.reset();
        touchInputManager.reset();
        mobileRenderer.reset();
        
        Game::shutdown();
        
        isInitialized = false;
        LOGI("Mobile game shutdown complete");
    }
    
    void updateMobile(float deltaTime) {
        if (!isInitialized || isPaused) return;
        
        // Update touch input
        touchInputManager->update(deltaTime);
        
        // Update virtual controls
        virtualControls->update(deltaTime);
        
        // Update base game
        Game::update(deltaTime);
        
        // Update mobile-specific systems
        updateMobileSystems(deltaTime);
    }
    
    void renderMobile() {
        if (!isInitialized) return;
        
        mobileRenderer->beginFrame();
        mobileRenderer->clear();
        
        // Render game world
        renderGameWorld();
        
        // Render mobile UI
        virtualControls->render();
        
        mobileRenderer->endFrame();
    }
    
    // Touch input handlers
    void onTouchDown(int touchId, float x, float y, float pressure = 1.0f) {
        if (!isInitialized) return;
        touchInputManager->processTouchDown(touchId, Vector2(x, y), pressure);
    }
    
    void onTouchMove(int touchId, float x, float y, float pressure = 1.0f) {
        if (!isInitialized) return;
        touchInputManager->processTouchMove(touchId, Vector2(x, y), pressure);
    }
    
    void onTouchUp(int touchId, float x, float y) {
        if (!isInitialized) return;
        touchInputManager->processTouchUp(touchId, Vector2(x, y));
    }
    
    void onTouchCancel(int touchId) {
        if (!isInitialized) return;
        touchInputManager->processTouchCancel(touchId);
    }
    
    // Screen management
    void onScreenSizeChanged(int width, int height) {
        LOGI("Screen size changed: %dx%d", width, height);
        
        screenSize = Vector2(static_cast<float>(width), static_cast<float>(height));
        isLandscape = width > height;
        
        if (mobileRenderer) {
            mobileRenderer->setScreenSize(width, height);
        }
        
        if (touchInputManager) {
            touchInputManager->setScreenSize(screenSize);
        }
        
        if (virtualControls) {
            virtualControls->setScreenSize(screenSize);
            setupMobileUI();  // Recalculate UI layout
        }
    }
    
    void onScreenRotation() {
        LOGI("Screen rotation detected");
        
        std::swap(screenSize.x, screenSize.y);
        isLandscape = !isLandscape;
        
        if (mobileRenderer) {
            mobileRenderer->handleScreenRotation();
        }
        
        if (touchInputManager) {
            touchInputManager->handleScreenRotation();
        }
        
        if (virtualControls) {
            virtualControls->handleScreenRotation();
            setupMobileUI();
        }
    }
    
    // App lifecycle
    void onPause() {
        LOGI("Game paused");
        isPaused = true;
        Game::pause();
    }
    
    void onResume() {
        LOGI("Game resumed");
        isPaused = false;
        Game::resume();
    }
    
    void onLowMemory() {
        LOGI("Low memory warning received");
        // Implement memory cleanup
        if (mobileRenderer) {
            // Free unused resources
        }
    }
    
    // Performance management
    void setPerformanceLevel(MobileRenderer::PerformanceLevel level) {
        if (mobileRenderer) {
            mobileRenderer->setPerformanceLevel(level);
        }
    }
    
    MobileRenderer::PerformanceLevel getPerformanceLevel() const {
        return mobileRenderer ? mobileRenderer->getPerformanceLevel() : MobileRenderer::PerformanceLevel::Medium;
    }
    
private:
    bool initializeGameSystems() {
        // Initialize physics with mobile-optimized settings
        physicsEngine = std::make_unique<PhysicsEngine>();
        physicsEngine->initialize();
        
        // Initialize camera
        camera = std::make_unique<Camera>();
        camera->setPosition(Vector3(0, 5, 10));
        camera->setTarget(Vector3(0, 0, 0));
        
        // Initialize track (simplified for mobile)
        track = std::make_unique<Track>();
        track->generateSimpleTrack();  // Use simpler track for mobile
        
        // Initialize player car
        auto playerCar = std::make_unique<Car>();
        playerCar->setPosition(Vector3(0, 1, 0));
        setPlayerCar(playerCar.get());
        cars.push_back(std::move(playerCar));
        
        // Initialize combat manager (optional for mobile)
        combatManager = std::make_unique<CombatManager>();
        combatManager->initialize();
        
        return true;
    }
    
    void setupMobileInputCallbacks() {
        // Racing controls
        touchInputManager->setAccelerateCallback([this](float input) {
            onAccelerate(input);
        });
        
        touchInputManager->setBrakeCallback([this](float input) {
            onBrake(input);
        });
        
        touchInputManager->setSteerCallback([this](float input) {
            onSteer(input);
        });
        
        touchInputManager->setBoostCallback([this](bool input) {
            onBoost(input);
        });
        
        touchInputManager->setHandbrakeCallback([this](bool input) {
            onHandbrake(input);
        });
        
        touchInputManager->setCameraLookCallback([this](float deltaX, float deltaY) {
            onCameraLook(deltaX, deltaY);
        });
        
        touchInputManager->setCameraZoomCallback([this](float delta) {
            onCameraZoom(delta);
        });
        
        touchInputManager->setPauseCallback([this]() {
            onPause();
        });
        
        touchInputManager->setResetCallback([this]() {
            onReset();
        });
    }
    
    void setupMobileUI() {
        if (!virtualControls) return;
        
        // Clear existing UI elements
        virtualControls->setJoystickVisible(false, false);
        
        if (isLandscape) {
            virtualControls->setupLandscapeLayout();
        } else {
            virtualControls->setupPortraitLayout();
        }
        
        // Add racing buttons
        float buttonSize = std::min(screenSize.x, screenSize.y) * 0.12f;
        
        // Accelerate button (bottom right)
        virtualControls->addButton(VirtualControls::ButtonType::Accelerate,
                                 Vector2(screenSize.x - buttonSize * 1.5f, screenSize.y - buttonSize * 1.5f),
                                 Vector2(buttonSize, buttonSize), "GAS");
        
        // Brake button (above accelerate)
        virtualControls->addButton(VirtualControls::ButtonType::Brake,
                                 Vector2(screenSize.x - buttonSize * 1.5f, screenSize.y - buttonSize * 3.0f),
                                 Vector2(buttonSize, buttonSize), "BRAKE");
        
        // Boost button (left of accelerate)
        virtualControls->addButton(VirtualControls::ButtonType::Boost,
                                 Vector2(screenSize.x - buttonSize * 3.0f, screenSize.y - buttonSize * 1.5f),
                                 Vector2(buttonSize, buttonSize), "BOOST");
        
        // Handbrake button (bottom left)
        virtualControls->addButton(VirtualControls::ButtonType::Handbrake,
                                 Vector2(buttonSize * 1.5f, screenSize.y - buttonSize * 1.5f),
                                 Vector2(buttonSize, buttonSize), "HAND");
        
        // Pause button (top left)
        virtualControls->addButton(VirtualControls::ButtonType::Pause,
                                 Vector2(buttonSize * 1.5f, buttonSize * 1.5f),
                                 Vector2(buttonSize * 0.8f, buttonSize * 0.8f), "PAUSE");
        
        // Setup virtual joysticks
        float joystickRadius = buttonSize * 1.2f;
        
        // Steering joystick (left side)
        virtualControls->setupSteeringJoystick(
            Vector2(joystickRadius * 2.0f, screenSize.y * 0.5f), joystickRadius);
        
        // Camera joystick (right side, upper)
        virtualControls->setupCameraJoystick(
            Vector2(screenSize.x - joystickRadius * 2.0f, screenSize.y * 0.3f), joystickRadius);
        
        virtualControls->setJoystickVisible(true, true);
        
        // Setup HUD elements
        setupMobileHUD();
    }
    
    void setupMobileHUD() {
        if (!virtualControls) return;
        
        // Speedometer (top center)
        virtualControls->setSpeedometerPosition(Vector2(screenSize.x * 0.5f, screenSize.y * 0.15f));
        virtualControls->setSpeedometerVisible(true);
        
        // Minimap (top right)
        virtualControls->setMinimapPosition(Vector2(screenSize.x - 80.0f, 80.0f));
        virtualControls->setMinimapSize(Vector2(120.0f, 120.0f));
        virtualControls->setMinimapVisible(true);
        
        // Add HUD text elements
        virtualControls->addHUDElement(Vector2(20.0f, screenSize.y - 60.0f), Vector2(200.0f, 30.0f), "Lap: 1/3");
        virtualControls->addHUDElement(Vector2(20.0f, screenSize.y - 30.0f), Vector2(200.0f, 30.0f), "Time: 00:00");
        virtualControls->addHUDElement(Vector2(screenSize.x - 120.0f, screenSize.y - 30.0f), Vector2(100.0f, 30.0f), "Pos: 1/8");
    }
    
    void updateMobileSystems(float deltaTime) {
        // Update HUD with current game state
        if (virtualControls && playerCar) {
            // Update speed
            Vector3 velocity = playerCar->getVelocity();
            float speed = velocity.length() * 3.6f;  // Convert to km/h
            virtualControls->updateSpeed(speed);
            
            // Update lap info
            virtualControls->updateLapInfo(currentLap, totalLaps, currentLapTime, bestTime);
            
            // Update race position (placeholder)
            virtualControls->updateRacePosition(1, static_cast<int>(cars.size()));
            
            // Update minimap
            Vector3 carPos = playerCar->getPosition();
            float carRotation = playerCar->getRotation().y;
            virtualControls->updateMinimapPlayerPosition(Vector2(carPos.x, carPos.z), carRotation);
        }
    }
    
    void renderGameWorld() {
        if (!mobileRenderer) return;
        
        // Set camera matrices
        if (camera) {
            mobileRenderer->setViewMatrix(camera->getViewMatrix());
            mobileRenderer->setProjectionMatrix(camera->getProjectionMatrix());
            mobileRenderer->setCameraPosition(camera->getPosition());
            mobileRenderer->updateMatrices();
        }
        
        // Render track
        if (track) {
            // Simplified track rendering for mobile
            mobileRenderer->renderTrack(track->getVertices(), track->getIndices());
        }
        
        // Render cars
        for (const auto& car : cars) {
            if (car) {
                Matrix4 carTransform = Matrix4::translation(car->getPosition()) * 
                                     Matrix4::rotationY(car->getRotation().y);
                mobileRenderer->renderCar(carTransform, Vector3(1.0f, 0.0f, 0.0f));
            }
        }
        
        // Render environment (simplified)
        renderMobileEnvironment();
    }
    
    void renderMobileEnvironment() {
        // Render simplified environment for mobile
        // Ground plane
        mobileRenderer->renderPlane(Vector3(0, -0.1f, 0), Vector3(0, 1, 0), 100.0f, Vector3(0.3f, 0.7f, 0.3f));
        
        // Simple skybox color (no actual skybox for performance)
        mobileRenderer->setClearColor(Vector3(0.5f, 0.7f, 1.0f));  // Sky blue
    }
};

// Global game instance
static std::unique_ptr<MobileGame> g_mobileGame;

// C-style interface for platform-specific code
extern "C" {
    
    int mobile_game_initialize(int width, int height, float density) {
        LOGI("mobile_game_initialize called: %dx%d, density: %.1f", width, height, density);
        
        try {
            g_mobileGame = std::make_unique<MobileGame>();
            
            if (!g_mobileGame->initializeMobile(width, height, density)) {
                LOGE("Failed to initialize mobile game");
                g_mobileGame.reset();
                return 0;
            }
            
            LOGI("Mobile game initialized successfully");
            return 1;
        } catch (const std::exception& e) {
            LOGE("Exception during initialization: %s", e.what());
            g_mobileGame.reset();
            return 0;
        }
    }
    
    void mobile_game_shutdown() {
        LOGI("mobile_game_shutdown called");
        
        if (g_mobileGame) {
            g_mobileGame.reset();
        }
    }
    
    void mobile_game_update(float deltaTime) {
        if (g_mobileGame) {
            g_mobileGame->updateMobile(deltaTime);
        }
    }
    
    void mobile_game_render() {
        if (g_mobileGame) {
            g_mobileGame->renderMobile();
        }
    }
    
    void mobile_game_touch_down(int touchId, float x, float y, float pressure) {
        if (g_mobileGame) {
            g_mobileGame->onTouchDown(touchId, x, y, pressure);
        }
    }
    
    void mobile_game_touch_move(int touchId, float x, float y, float pressure) {
        if (g_mobileGame) {
            g_mobileGame->onTouchMove(touchId, x, y, pressure);
        }
    }
    
    void mobile_game_touch_up(int touchId, float x, float y) {
        if (g_mobileGame) {
            g_mobileGame->onTouchUp(touchId, x, y);
        }
    }
    
    void mobile_game_screen_size_changed(int width, int height) {
        if (g_mobileGame) {
            g_mobileGame->onScreenSizeChanged(width, height);
        }
    }
    
    void mobile_game_screen_rotation() {
        if (g_mobileGame) {
            g_mobileGame->onScreenRotation();
        }
    }
    
    void mobile_game_pause() {
        if (g_mobileGame) {
            g_mobileGame->onPause();
        }
    }
    
    void mobile_game_resume() {
        if (g_mobileGame) {
            g_mobileGame->onResume();
        }
    }
    
    void mobile_game_low_memory() {
        if (g_mobileGame) {
            g_mobileGame->onLowMemory();
        }
    }
    
    void mobile_game_set_performance_level(int level) {
        if (g_mobileGame) {
            g_mobileGame->setPerformanceLevel(static_cast<MobileRenderer::PerformanceLevel>(level));
        }
    }
}

#endif // MOBILE_PLATFORM