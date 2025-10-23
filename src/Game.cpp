#include "Game.h"
#include "Platform/PlatformDetect.h"
#include <iostream>
#include <chrono>
#include <algorithm>

#if !PLATFORM_MOBILE
#include <GLFW/glfw3.h>
#endif

Game::Game() 
    : currentState(GameState::Menu)
    , isRunning(false)
    , isPaused(false)
    , gameTime(0.0f)
    , deltaTime(0.0f)
    , currentLap(0)
    , totalLaps(3)
    , bestTime(0.0f)
    , currentLapTime(0.0f)
    , frameCount(0)
    , fps(0.0f)
    , lastFpsUpdate(0.0f)
    , screenWidth(1920)
    , screenHeight(1080)
    , fullscreen(false)
    , vsync(true)
    , masterVolume(1.0f)
    , sfxVolume(1.0f)
    , musicVolume(1.0f)
    , cameraDistance(15.0f)
    , cameraHeight(8.0f)
    , cameraSensitivity(2.0f)
    , cameraInverted(false)
    , difficulty(1.0f)
    , autoBrake(false)
    , autoSteer(false)
    , showHUD(true)
    , showDebugInfo(false)
    , playerCar(nullptr)
    , localPlayer(nullptr) {
}

Game::~Game() {
    shutdown();
}

bool Game::initialize(int width, int height, const std::string& title) {
    screenWidth = width;
    screenHeight = height;
    
    std::cout << "Initializing game for platform: " << Platform::getPlatformName() << std::endl;
    
#if !PLATFORM_MOBILE
    // Desktop: Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    
    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    
    // Create window
    GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(window);
#endif
    
    // Initialize renderer
    renderer = std::make_unique<Renderer>();
    if (!renderer->initialize(width, height)) {
        std::cerr << "Failed to initialize renderer" << std::endl;
        return false;
    }
    
    // Initialize input manager
    inputManager = std::make_unique<InputManager>();
    inputManager->initialize();
    
#if PLATFORM_MOBILE
    // Initialize mobile-specific input
    touchInputManager = std::make_unique<TouchInputManager>();
    touchInputManager->initialize(static_cast<float>(width), static_cast<float>(height));
    
    // Link touch manager to input manager
    inputManager->setTouchInputManager(touchInputManager.get());
    
    // Initialize mobile UI
    mobileUI = std::make_unique<MobileUI>();
    mobileUI->initialize(touchInputManager.get(), static_cast<float>(width), static_cast<float>(height));
    
    std::cout << "Mobile input and UI initialized" << std::endl;
#endif
    
    // Initialize physics engine
    physicsEngine = std::make_unique<PhysicsEngine>();
    
    // Initialize combat manager
    combatManager = std::make_unique<CombatManager>();
    
    // Initialize game systems
    initializeGame();
    
    isRunning = true;
    return true;
}

void Game::shutdown() {
    isRunning = false;
    
#if PLATFORM_MOBILE
    if (mobileUI) {
        mobileUI->shutdown();
        mobileUI.reset();
    }
    
    if (touchInputManager) {
        touchInputManager->shutdown();
        touchInputManager.reset();
    }
#endif
    
    if (renderer) {
        renderer->shutdown();
        renderer.reset();
    }
    
    if (inputManager) {
        inputManager->shutdown();
        inputManager.reset();
    }
    
    physicsEngine.reset();
    combatManager.reset();
    camera.reset();
    track.reset();
    cars.clear();
    playerCar = nullptr;
    localPlayer = nullptr;
    pvpPlayers.clear();
    
#if !PLATFORM_MOBILE
    glfwTerminate();
#endif
}

void Game::run() {
    auto lastTime = std::chrono::high_resolution_clock::now();
    
    while (isRunning) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;
        
        // Cap delta time to prevent large jumps
        deltaTime = std::min(deltaTime, 0.016f);
        
        update(deltaTime);
        render();
        
        // Update FPS counter
        frameCount++;
        lastFpsUpdate += deltaTime;
        if (lastFpsUpdate >= 1.0f) {
            fps = frameCount / lastFpsUpdate;
            frameCount = 0;
            lastFpsUpdate = 0.0f;
        }
    }
}

void Game::update(float dt) {
    deltaTime = dt;
    gameTime += dt;
    
    if (!isPaused) {
        handleInput();
        
        if (currentState == GameState::PvPMode) {
            updatePvPMode(dt);
        } else {
            updatePhysics(dt);
            updateGameplay(dt);
        }
        
        updateCamera(dt);
        updateParticles(dt);
        updateTrails(dt);
    }
    
    updateUI(dt);
    updateAudio(dt);
}

void Game::render() {
    renderer->beginFrame();
    
    switch (currentState) {
        case GameState::Menu:
            renderMenu();
            break;
        case GameState::Playing:
            renderGame();
            break;
        case GameState::Paused:
            renderGame();
            renderPauseMenu();
            break;
        case GameState::GameOver:
            renderGame();
            break;
        case GameState::PvPMode:
            renderPvPMode();
            break;
        case GameState::StatsMenu:
            renderStatsMenu();
            break;
    }
    
    if (showHUD) {
        renderHUD();
    }
    
    if (showDebugInfo) {
        renderDebugInfo();
    }
    
#if PLATFORM_MOBILE
    // Render mobile UI (virtual controls, HUD)
    if (mobileUI) {
        mobileUI->render();
    }
#endif
    
    renderer->endFrame();
}

void Game::handleInput() {
    if (!inputManager) return;
    
    inputManager->update(deltaTime);
    
#if PLATFORM_MOBILE
    // Update mobile touch input
    if (touchInputManager) {
        touchInputManager->update(deltaTime);
    }
#endif
    
    // Handle menu input
    if (currentState == GameState::Menu) {
        if (inputManager->isKeyJustPressed(InputManager::Key::F1)) {
            // Start racing mode
            setState(GameState::Playing);
        } else if (inputManager->isKeyJustPressed(InputManager::Key::F2)) {
            // Start PvP mode
            initializePvPMode();
        } else if (inputManager->isKeyJustPressed(InputManager::Key::Escape)) {
            quit();
        }
    }
    // Handle stats menu input
    else if (currentState == GameState::StatsMenu && localPlayer) {
        if (inputManager->isKeyJustPressed(InputManager::Key::F1)) {
            distributeStatPoint(localPlayer, "strength");
        } else if (inputManager->isKeyJustPressed(InputManager::Key::F2)) {
            distributeStatPoint(localPlayer, "defense");
        } else if (inputManager->isKeyJustPressed(InputManager::Key::F3)) {
            distributeStatPoint(localPlayer, "stamina");
        } else if (inputManager->isKeyJustPressed(InputManager::Key::F4)) {
            distributeStatPoint(localPlayer, "agility");
        }
    }
    // Handle PvP input
    else if (currentState == GameState::PvPMode) {
        if (localPlayer) {
            // Movement input (WASD)
            Vector3 moveDir = Vector3::zero();
            if (inputManager->isKeyPressed(InputManager::Key::W)) {
                moveDir = moveDir + camera->getForward();
            }
            if (inputManager->isKeyPressed(InputManager::Key::S)) {
                moveDir = moveDir - camera->getForward();
            }
            if (inputManager->isKeyPressed(InputManager::Key::A)) {
                moveDir = moveDir - camera->getRight();
            }
            if (inputManager->isKeyPressed(InputManager::Key::D)) {
                moveDir = moveDir + camera->getRight();
            }
            
            if (moveDir.magnitude() > 0) {
                moveDir = moveDir.normalized() * 10.0f;  // Movement speed
                localPlayer->setVelocity(moveDir);
            } else {
                localPlayer->setVelocity(Vector3::zero());
            }
            
            // Look direction from mouse
            Vector2 lookInput = inputManager->getCameraLookInput();
            if (lookInput.x != 0.0f || lookInput.y != 0.0f) {
                camera->handleMouseInput(lookInput.x, lookInput.y);
                localPlayer->setLookDirection(camera->getForward());
            }
        }
    }
    // Handle racing input
    else if (currentState == GameState::Playing) {
        // Handle car input
        if (playerCar) {
            playerCar->setThrottle(inputManager->getAccelerateInput());
            playerCar->setBrake(inputManager->getBrakeInput());
            playerCar->setSteer(inputManager->getSteerInput());
            playerCar->setBoost(inputManager->getBoostInput());
            playerCar->setHandbrake(inputManager->getHandbrakeInput());
        }
    }
    
    // Handle camera input (common for all modes)
    if (camera) {
        Vector2 lookInput = inputManager->getCameraLookInput();
        if (lookInput.x != 0.0f || lookInput.y != 0.0f) {
            camera->handleMouseInput(lookInput.x, lookInput.y);
        }
        
        float zoomInput = inputManager->getCameraZoomInput();
        if (zoomInput != 0.0f) {
            camera->handleScrollInput(zoomInput);
        }
    }
}

void Game::setState(GameState state) {
    currentState = state;
}

void Game::pause() {
    isPaused = true;
    setState(GameState::Paused);
}

void Game::resume() {
    isPaused = false;
    setState(GameState::Playing);
}

void Game::restart() {
    resetGame();
    setState(GameState::Playing);
}

void Game::quit() {
    isRunning = false;
}

void Game::addCar(std::unique_ptr<Car> car) {
    if (car) {
        cars.push_back(std::move(car));
        if (!playerCar) {
            playerCar = cars.back().get();
        }
    }
}

void Game::removeCar(Car* car) {
    if (!car) return;
    
    for (auto it = cars.begin(); it != cars.end(); ++it) {
        if (it->get() == car) {
            if (playerCar == car) {
                playerCar = nullptr;
            }
            cars.erase(it);
            break;
        }
    }
}

void Game::setPlayerCar(Car* car) {
    playerCar = car;
}

void Game::setTrack(std::unique_ptr<Track> newTrack) {
    track = std::move(newTrack);
}

void Game::setCamera(std::unique_ptr<Camera> newCamera) {
    camera = std::move(newCamera);
}

void Game::updateCamera(float dt) {
    if (!camera) return;
    
    // Handle touch camera controls for mobile
#if PLATFORM_MOBILE
    if (touchInputManager && camera->getMode() == Camera::CameraMode::ThirdPerson) {
        Vector2 cameraDrag = touchInputManager->getCameraDragDelta();
        if (cameraDrag.magnitude() > 0.01f) {
            camera->handleTouchDrag(cameraDrag.x, cameraDrag.y);
        }
    }
#endif
    
    // Update camera based on target
    if (currentState == GameState::PvPMode && localPlayer) {
        if (camera->getMode() == Camera::CameraMode::ThirdPerson) {
            camera->updateThirdPerson(localPlayer->getPosition(), localPlayer->getLookDirection(), dt);
        } else {
            camera->update(dt);
        }
    } else if (playerCar) {
        if (camera->getMode() == Camera::CameraMode::ThirdPerson) {
            camera->updateThirdPerson(playerCar->getPosition(), playerCar->getForward(), dt);
        } else {
            camera->update(dt);
        }
    }
    
    // Update renderer camera
    renderer->setViewMatrix(camera->getViewMatrix());
    renderer->setProjectionMatrix(camera->getProjectionMatrix());
    renderer->setCameraPosition(camera->getPosition());
}

void Game::setupInputCallbacks() {
    if (!inputManager) return;
    
    inputManager->setAccelerateCallback([this](float input) { onAccelerate(input); });
    inputManager->setBrakeCallback([this](float input) { onBrake(input); });
    inputManager->setSteerCallback([this](float input) { onSteer(input); });
    inputManager->setBoostCallback([this](bool input) { onBoost(input); });
    inputManager->setHandbrakeCallback([this](bool input) { onHandbrake(input); });
    inputManager->setCameraLookCallback([this](float x, float y) { onCameraLook(x, y); });
    inputManager->setCameraZoomCallback([this](float delta) { onCameraZoom(delta); });
    inputManager->setPauseCallback([this]() { onPause(); });
    inputManager->setResetCallback([this]() { onReset(); });
    
    // Combat callbacks
    inputManager->setAttack1Callback([this]() { onAttack1(); });
    inputManager->setAttack2Callback([this]() { onAttack2(); });
    inputManager->setShieldCallback([this]() { onShield(); });
    inputManager->setTeleportCallback([this]() { onTeleport(); });
    inputManager->setInteractCallback([this]() { onInteract(); });
    inputManager->setStatMenuCallback([this]() { onStatMenu(); });
}

void Game::onAccelerate(float input) {
    // Handled in handleInput()
}

void Game::onBrake(float input) {
    // Handled in handleInput()
}

void Game::onSteer(float input) {
    // Handled in handleInput()
}

void Game::onBoost(bool input) {
    // Handled in handleInput()
}

void Game::onHandbrake(bool input) {
    // Handled in handleInput()
}

void Game::onCameraLook(float deltaX, float deltaY) {
    // Handled in handleInput()
}

void Game::onCameraZoom(float delta) {
    // Handled in handleInput()
}

void Game::onPause() {
    if (currentState == GameState::Playing) {
        pause();
    } else if (currentState == GameState::Paused) {
        resume();
    }
}

void Game::onReset() {
    restart();
}

void Game::renderGame() {
    if (!renderer) return;
    
    // Render skybox
    renderer->renderSkybox();
    
    // Render track
    if (track) {
        renderTrack();
    }
    
    // Render cars
    renderCars();
    
    // Render environment
    renderEnvironment();
    
    // Render particles and effects
    renderParticles();
    renderTrails();
}

void Game::renderUI() {
    // UI rendering
}

void Game::renderHUD() {
    // HUD rendering
}

void Game::renderDebugInfo() {
    if (!renderer) return;
    
    // Debug info rendering
    renderer->renderDebugInfo();
}

void Game::renderMenu() {
    if (!renderer) return;
    
    // Render menu background
    renderer->renderQuad(Vector3::zero(), screenWidth, screenHeight, Vector3(0.1f, 0.1f, 0.2f), 1.0f);
    
    // Render menu options
    float centerX = screenWidth * 0.5f;
    float centerY = screenHeight * 0.4f;
    
    renderer->renderText("RACING GAME", centerX, centerY - 100, 3.0f, Vector3(1, 1, 1));
    renderer->renderText("[1] Racing Mode", centerX, centerY, 1.5f, Vector3(0.8f, 0.8f, 0.8f));
    renderer->renderText("[2] PvP Combat Mode", centerX, centerY + 40, 1.5f, Vector3(0.8f, 0.8f, 0.8f));
    renderer->renderText("[3] Settings", centerX, centerY + 80, 1.5f, Vector3(0.8f, 0.8f, 0.8f));
    renderer->renderText("[ESC] Quit", centerX, centerY + 120, 1.5f, Vector3(0.8f, 0.8f, 0.8f));
}

void Game::renderPauseMenu() {
    // Pause menu rendering
}

void Game::updateGameplay(float dt) {
    updateLapProgress();
    updateTiming();
    checkWinCondition();
    updateAI(dt);
}

void Game::updateLapProgress() {
    if (!playerCar || !track) return;
    
    // Check checkpoint progress
    if (track->checkCheckpoint(playerCar->getPosition(), 5.0f)) {
        // Checkpoint passed
    }
    
    currentLap = track->getCurrentLap();
}

void Game::updateTiming() {
    if (currentState == GameState::Playing) {
        currentLapTime += deltaTime;
    }
}

void Game::checkWinCondition() {
    if (currentLap >= totalLaps) {
        setState(GameState::GameOver);
    }
}

void Game::spawnCars() {
    // Spawn AI cars
}

void Game::updateAI(float dt) {
    // Update AI car behavior
}

void Game::setScreenSize(int width, int height) {
    screenWidth = width;
    screenHeight = height;
    if (renderer) {
        renderer->setViewport(0, 0, width, height);
    }
}

void Game::setFullscreen(bool fullscreen) {
    this->fullscreen = fullscreen;
}

void Game::setVSync(bool vsync) {
    this->vsync = vsync;
}

void Game::setMasterVolume(float volume) {
    masterVolume = std::clamp(volume, 0.0f, 1.0f);
}

void Game::setSFXVolume(float volume) {
    sfxVolume = std::clamp(volume, 0.0f, 1.0f);
}

void Game::setMusicVolume(float volume) {
    musicVolume = std::clamp(volume, 0.0f, 1.0f);
}

void Game::setCameraSensitivity(float sensitivity) {
    cameraSensitivity = std::max(0.1f, sensitivity);
    if (camera) {
        camera->setMouseSensitivity(sensitivity);
    }
}

void Game::setCameraInverted(bool inverted) {
    cameraInverted = inverted;
}

void Game::setDifficulty(float difficulty) {
    this->difficulty = std::clamp(difficulty, 0.1f, 2.0f);
}

void Game::setAutoBrake(bool enabled) {
    autoBrake = enabled;
}

void Game::setAutoSteer(bool enabled) {
    autoSteer = enabled;
}

void Game::setShowHUD(bool show) {
    showHUD = show;
}

void Game::setShowDebugInfo(bool show) {
    showDebugInfo = show;
}

void Game::resetGame() {
    currentLap = 0;
    currentLapTime = 0.0f;
    gameTime = 0.0f;
    
    if (track) {
        track->resetLapProgress();
    }
    
    for (auto& car : cars) {
        car->reset();
    }
}

void Game::loadSettings() {
    // Load settings from file
}

void Game::saveSettings() {
    // Save settings to file
}

void Game::showMessage(const std::string& message) {
    std::cout << message << std::endl;
}

void Game::initializeGame() {
    initializeCars();
    initializeTrack();
    initializeCamera();
    initializeInput();
}

void Game::initializeCars() {
    // Create player car
    auto playerCarPtr = std::make_unique<Car>(Vector3(0.0f, 0.0f, 0.0f));
    playerCar = playerCarPtr.get();
    addCar(std::move(playerCarPtr));
    
    // Add car to physics engine
    if (physicsEngine) {
        physicsEngine->addCar(playerCar);
    }
}

void Game::initializeTrack() {
    track = std::make_unique<Track>(50.0f, 10.0f, 100);
}

void Game::initializeCamera() {
    camera = std::make_unique<Camera>();
    camera->setMode(Camera::CameraMode::ThirdPerson);
    camera->setFollowTarget(Vector3::zero());
    camera->setFollowDistance(cameraDistance);
    camera->setFollowHeight(cameraHeight);
    camera->setMouseSensitivity(cameraSensitivity);
}

void Game::initializeInput() {
    setupInputCallbacks();
}

void Game::updatePhysics(float dt) {
    if (physicsEngine) {
        physicsEngine->update(dt);
    }
}

void Game::updateAudio(float dt) {
    // Audio update
}

void Game::updateUI(float dt) {
    // UI update
}

void Game::renderCars() {
    if (!renderer) return;
    
    for (const auto& car : cars) {
        if (car) {
            Matrix4 transform = car->getTransformMatrix();
            Vector3 color = (car.get() == playerCar) ? Vector3(1.0f, 0.0f, 0.0f) : Vector3(0.0f, 0.0f, 1.0f);
            renderer->renderCar(transform, color);
        }
    }
}

void Game::renderTrack() {
    if (!renderer || !track) return;
    
    const auto& vertices = track->getTrackVertices();
    const auto& indices = track->getTrackIndices();
    renderer->renderTrack(vertices, indices);
}

void Game::renderEnvironment() {
    if (!renderer) return;
    
    // Render ground plane
    renderer->renderPlane(Vector3::zero(), Vector3::up(), 200.0f, Vector3(0.2f, 0.3f, 0.2f));
    
    // Render some basic environment objects
    for (int i = 0; i < 10; i++) {
        float angle = (float)i / 10.0f * 2.0f * M_PI;
        float radius = 80.0f;
        Vector3 position(
            std::cos(angle) * radius,
            0.0f,
            std::sin(angle) * radius
        );
        renderer->renderCube(position, Vector3(2.0f, 5.0f, 2.0f), Vector3(0.5f, 0.5f, 0.5f));
    }
}

void Game::renderParticles() {
    // Particle rendering
}

void Game::renderTrails() {
    // Trail rendering
}

void Game::updateParticles(float dt) {
    // Particle update
}

void Game::updateTrails(float dt) {
    // Trail update
}

// PvP Mode Implementation
void Game::initializePvPMode() {
    // Switch to PvP mode
    setState(GameState::PvPMode);
    
    // Create local player
    localPlayer = combatManager->addPlayer("Player1");
    if (localPlayer) {
        localPlayer->setLocalPlayer(true);
        pvpPlayers.push_back(localPlayer);
    }
    
    // Add AI opponents for testing
    for (int i = 0; i < 3; i++) {
        Player* aiPlayer = combatManager->addPlayer("AI_" + std::to_string(i + 1));
        if (aiPlayer) {
            pvpPlayers.push_back(aiPlayer);
        }
    }
    
    // Setup PvP camera
    if (camera && localPlayer) {
        camera->setMode(Camera::CameraMode::ThirdPerson);
        camera->setFollowTarget(localPlayer->getPosition());
    }
    
    // Start the match
    combatManager->startMatch();
    
    std::cout << "PvP Mode initialized with " << pvpPlayers.size() << " players!" << std::endl;
}

void Game::updatePvPMode(float dt) {
    if (!combatManager) return;
    
    // Handle player movement with camera-relative controls
    if (localPlayer && camera) {
        Vector3 inputDirection(0, 0, 0);
        
#if PLATFORM_MOBILE
        // Get joystick input for movement
        if (touchInputManager) {
            Vector2 joystickDir = touchInputManager->getJoystickDirection();
            float joystickMag = touchInputManager->getJoystickMagnitude();
            
            if (joystickMag > 0.01f) {
                // Convert joystick input to movement direction
                // Note: Y is inverted because joystick Y goes down while forward is negative Z
                inputDirection.x = joystickDir.x;
                inputDirection.z = -joystickDir.y;  // Forward/backward
                inputDirection = inputDirection.normalized() * joystickMag;
            }
        }
#else
        // Desktop keyboard input
        if (inputManager) {
            Vector2 moveInput = inputManager->getMoveInput();
            inputDirection.x = moveInput.x;
            inputDirection.z = moveInput.y;
        }
#endif
        
        // Update player movement with camera-relative controls
        localPlayer->updateMovement(dt, inputDirection, 
                                    camera->getForward(), camera->getRight());
    }
    
    // Update combat system
    combatManager->update(dt);
    
    // Update camera to follow local player
    if (camera && localPlayer) {
        camera->setFollowTarget(localPlayer->getPosition());
        camera->updateThirdPerson(localPlayer->getPosition(), localPlayer->getLookDirection(), dt);
    }
    
    // Check for level ups
    for (auto& player : pvpPlayers) {
        if (player->getStats().statPoints > 0) {
            handlePlayerLevelUp(player);
        }
    }
    
    // Check match end conditions
    if (!combatManager->isMatchActive()) {
        endPvPMatch();
    }
}

void Game::startPvPMatch() {
    initializePvPMode();
}

void Game::endPvPMatch() {
    if (combatManager) {
        combatManager->endMatch();
    }
    
    // Show results and return to menu
    setState(GameState::Menu);
}

void Game::addPvPPlayer(const std::string& name) {
    if (!combatManager) return;
    
    Player* newPlayer = combatManager->addPlayer(name);
    if (newPlayer) {
        pvpPlayers.push_back(newPlayer);
    }
}

void Game::handlePlayerLevelUp(Player* player) {
    if (!player) return;
    
    // For AI players, auto-distribute stat points
    if (!player->getLocalPlayer()) {
        // Simple AI stat distribution
        int choice = rand() % 4;
        switch (choice) {
            case 0:
                player->getStats().applyStatPoint("strength");
                break;
            case 1:
                player->getStats().applyStatPoint("defense");
                break;
            case 2:
                player->getStats().applyStatPoint("stamina");
                break;
            case 3:
                player->getStats().applyStatPoint("agility");
                break;
        }
    }
    // For local player, show stats menu
    else if (currentState != GameState::StatsMenu) {
        setState(GameState::StatsMenu);
    }
}

void Game::distributeStatPoint(Player* player, const std::string& stat) {
    if (!player) return;
    
    player->getStats().applyStatPoint(stat);
    
    // Return to PvP mode if no more points
    if (player->getStats().statPoints <= 0 && currentState == GameState::StatsMenu) {
        setState(GameState::PvPMode);
    }
}

// Combat input callbacks
void Game::onAttack1() {
    if (currentState != GameState::PvPMode || !localPlayer) return;
    
    combatManager->handleFistAttack(localPlayer);
}

void Game::onAttack2() {
    if (currentState != GameState::PvPMode || !localPlayer || !camera) return;
    
    // Fire laser in camera direction
    Vector3 direction = camera->getForward();
    combatManager->handleLaserAttack(localPlayer, direction);
}

void Game::onShield() {
    if (currentState != GameState::PvPMode || !localPlayer) return;
    
    if (localPlayer->isShieldActive()) {
        localPlayer->deactivateShield();
    } else {
        localPlayer->activateShield();
    }
}

void Game::onTeleport() {
    if (currentState != GameState::PvPMode || !localPlayer || !camera) return;
    
    // Teleport forward
    Vector3 teleportTarget = localPlayer->getPosition() + camera->getForward() * 15.0f;
    combatManager->executeTeleport(localPlayer, teleportTarget);
}

void Game::onInteract() {
    if (currentState != GameState::PvPMode || !localPlayer) return;
    
    // Handle interaction with power-ups or other objects
    std::cout << "Interact pressed" << std::endl;
}

void Game::onStatMenu() {
    if (currentState == GameState::PvPMode && localPlayer && localPlayer->getStats().statPoints > 0) {
        setState(GameState::StatsMenu);
    } else if (currentState == GameState::StatsMenu) {
        setState(GameState::PvPMode);
    }
}

// Rendering functions for PvP
void Game::renderPvPMode() {
    if (!renderer) return;
    
    // Render arena/environment
    renderer->renderSkybox();
    
    // Render ground
    renderer->renderPlane(Vector3::zero(), Vector3::up(), 100.0f, Vector3(0.3f, 0.3f, 0.3f));
    
    // Render all players
    for (auto& player : pvpPlayers) {
        if (!player) continue;
        
        Matrix4 transform = player->getTransformMatrix();
        Vector3 color = player->getColor();
        
        // Render player model (using cube for now)
        renderer->renderCube(player->getPosition(), Vector3(1.0f, 2.0f, 1.0f), color);
        
        // Render shield if active
        if (player->isShieldActive()) {
            // Render translucent sphere around player
            renderer->renderSphere(player->getPosition(), 2.0f, Vector3(0.3f, 0.6f, 1.0f), 0.5f);
        }
        
        // Render health bar above player
        Vector3 healthBarPos = player->getPosition() + Vector3(0, 3.0f, 0);
        float healthPercent = player->getHealth() / player->getMaxHealth();
        renderer->renderHealthBar(healthBarPos, healthPercent, 2.0f, 0.3f);
    }
    
    // Render projectiles (would need access to projectiles from players)
    
    // Render combat HUD
    renderCombatHUD();
}

void Game::renderStatsMenu() {
    if (!renderer || !localPlayer) return;
    
    // Render background
    renderer->renderQuad(Vector3::zero(), screenWidth, screenHeight, Vector3(0.1f, 0.1f, 0.1f), 0.9f);
    
    // Render stats information
    Player::Stats stats = localPlayer->getStats();
    
    std::string title = "LEVEL UP - Distribute Stat Points";
    std::string level = "Level: " + std::to_string(stats.level);
    std::string points = "Available Points: " + std::to_string(stats.statPoints);
    std::string strength = "[1] Strength: " + std::to_string((int)stats.strength);
    std::string defense = "[2] Defense: " + std::to_string((int)stats.defense);
    std::string stamina = "[3] Stamina: " + std::to_string((int)stats.stamina);
    std::string agility = "[4] Agility: " + std::to_string((int)stats.agility);
    
    // Render text (placeholder - renderer would need text rendering)
    float y = screenHeight * 0.3f;
    renderer->renderText(title, screenWidth * 0.5f, y, 2.0f, Vector3(1, 1, 1));
    y += 50;
    renderer->renderText(level, screenWidth * 0.5f, y, 1.5f, Vector3(0.8f, 0.8f, 0.8f));
    y += 30;
    renderer->renderText(points, screenWidth * 0.5f, y, 1.5f, Vector3(1, 1, 0));
    y += 50;
    renderer->renderText(strength, screenWidth * 0.5f, y, 1.2f, Vector3(1, 0.5f, 0.5f));
    y += 30;
    renderer->renderText(defense, screenWidth * 0.5f, y, 1.2f, Vector3(0.5f, 0.5f, 1));
    y += 30;
    renderer->renderText(stamina, screenWidth * 0.5f, y, 1.2f, Vector3(0.5f, 1, 0.5f));
    y += 30;
    renderer->renderText(agility, screenWidth * 0.5f, y, 1.2f, Vector3(1, 1, 0.5f));
}

void Game::renderCombatHUD() {
    if (!renderer || !localPlayer) return;
    
    // Render player stats
    float hudX = 50;
    float hudY = screenHeight - 150;
    
    // Health bar
    float healthPercent = localPlayer->getHealth() / localPlayer->getMaxHealth();
    renderer->renderBar(hudX, hudY, 300, 30, healthPercent, Vector3(1, 0, 0), Vector3(0.3f, 0, 0));
    
    // Stamina bar
    float staminaPercent = localPlayer->getStamina() / localPlayer->getMaxStamina();
    renderer->renderBar(hudX, hudY + 40, 300, 20, staminaPercent, Vector3(0, 1, 0), Vector3(0, 0.3f, 0));
    
    // Shield bar
    if (localPlayer->isShieldActive()) {
        float shieldPercent = localPlayer->getShieldStrength() / 50.0f;  // Max shield strength
        renderer->renderBar(hudX, hudY + 70, 300, 15, shieldPercent, Vector3(0, 0.5f, 1), Vector3(0, 0, 0.3f));
    }
    
    // Level and experience
    std::string levelText = "Level " + std::to_string(localPlayer->getLevel());
    std::string expText = "EXP: " + std::to_string(localPlayer->getExperience()) + "/" + 
                         std::to_string(localPlayer->getLevel() * 100);
    
    renderer->renderText(levelText, hudX, hudY - 30, 1.5f, Vector3(1, 1, 1));
    renderer->renderText(expText, hudX, hudY - 10, 1.0f, Vector3(0.8f, 0.8f, 0.8f));
    
    // Ability cooldowns
    float abilityX = screenWidth / 2 - 150;
    float abilityY = screenHeight - 100;
    
    // Attack 1 (Q)
    renderer->renderAbilityIcon(abilityX, abilityY, 50, 50, "Q", localPlayer->canAttack());
    
    // Attack 2 (E)
    renderer->renderAbilityIcon(abilityX + 60, abilityY, 50, 50, "E", localPlayer->canAttack());
    
    // Shield (F)
    renderer->renderAbilityIcon(abilityX + 120, abilityY, 50, 50, "F", !localPlayer->isShieldActive());
    
    // Teleport (R)
    renderer->renderAbilityIcon(abilityX + 180, abilityY, 50, 50, "R", localPlayer->canTeleport());
    
    // Kill feed (top right)
    if (combatManager) {
        auto stats = combatManager->getPlayerStats(localPlayer->getPlayerId());
        std::string kda = "K: " + std::to_string(stats.kills) + 
                         " D: " + std::to_string(stats.deaths) + 
                         " A: " + std::to_string(stats.assists);
        renderer->renderText(kda, screenWidth - 200, 50, 1.2f, Vector3(1, 1, 1));
    }
}

#if PLATFORM_MOBILE
// Mobile-specific implementations
void Game::handleTouchInput(int touchId, float x, float y, int phase, float pressure) {
    if (inputManager) {
        inputManager->processTouchInput(touchId, x, y, phase, pressure);
    }
}

void Game::handleDeviceOrientation(float width, float height) {
    screenWidth = static_cast<int>(width);
    screenHeight = static_cast<int>(height);
    
    if (renderer) {
        renderer->setViewport(0, 0, screenWidth, screenHeight);
    }
    
    if (touchInputManager) {
        touchInputManager->setScreenSize(width, height);
    }
    
    if (mobileUI) {
        mobileUI->setScreenSize(width, height);
    }
    
    if (camera) {
        camera->setAspectRatio(width / height);
    }
}

void Game::setLowPowerMode(bool enabled) {
    // Reduce rendering quality, physics update rate, etc. for battery saving
    if (enabled) {
        // Lower graphics quality
        // Reduce physics timestep
        // Disable some effects
        std::cout << "Low power mode enabled" << std::endl;
    } else {
        std::cout << "Low power mode disabled" << std::endl;
    }
}
#endif