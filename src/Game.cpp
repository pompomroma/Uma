#include "Game.h"
#include <iostream>
#include <chrono>
#include <GLFW/glfw3.h>

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
    , playerCar(nullptr) {
}

Game::~Game() {
    shutdown();
}

bool Game::initialize(int width, int height, const std::string& title) {
    screenWidth = width;
    screenHeight = height;
    
    // Initialize GLFW
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
    
    // Initialize renderer
    renderer = std::make_unique<Renderer>();
    if (!renderer->initialize(width, height)) {
        std::cerr << "Failed to initialize renderer" << std::endl;
        return false;
    }
    
    // Initialize input manager
    inputManager = std::make_unique<InputManager>();
    inputManager->initialize();
    
    // Initialize physics engine
    physicsEngine = std::make_unique<PhysicsEngine>();
    
    // Initialize game systems
    initializeGame();
    
    isRunning = true;
    return true;
}

void Game::shutdown() {
    isRunning = false;
    
    if (renderer) {
        renderer->shutdown();
        renderer.reset();
    }
    
    if (inputManager) {
        inputManager->shutdown();
        inputManager.reset();
    }
    
    physicsEngine.reset();
    camera.reset();
    track.reset();
    cars.clear();
    playerCar = nullptr;
    
    glfwTerminate();
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
        updatePhysics(dt);
        updateGameplay(dt);
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
    }
    
    if (showHUD) {
        renderHUD();
    }
    
    if (showDebugInfo) {
        renderDebugInfo();
    }
    
    renderer->endFrame();
}

void Game::handleInput() {
    if (!inputManager) return;
    
    inputManager->update(deltaTime);
    
    // Handle car input
    if (playerCar) {
        playerCar->setThrottle(inputManager->getAccelerateInput());
        playerCar->setBrake(inputManager->getBrakeInput());
        playerCar->setSteer(inputManager->getSteerInput());
        playerCar->setBoost(inputManager->getBoostInput());
        playerCar->setHandbrake(inputManager->getHandbrakeInput());
    }
    
    // Handle camera input
    Vector2 lookInput = inputManager->getCameraLookInput();
    if (camera && (lookInput.x != 0.0f || lookInput.y != 0.0f)) {
        camera->handleMouseInput(lookInput.x, lookInput.y);
    }
    
    float zoomInput = inputManager->getCameraZoomInput();
    if (camera && zoomInput != 0.0f) {
        camera->handleScrollInput(zoomInput);
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
    if (!camera || !playerCar) return;
    
    if (camera->getMode() == Camera::CameraMode::ThirdPerson) {
        camera->updateThirdPerson(playerCar->getPosition(), playerCar->getForward());
    } else {
        camera->update(dt);
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
    // Menu rendering
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
            Vector3 color = (car == playerCar) ? Vector3(1.0f, 0.0f, 0.0f) : Vector3(0.0f, 0.0f, 1.0f);
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