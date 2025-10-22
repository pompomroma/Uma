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
    , combatModeEnabled(false)
    , pvpMode(false)
    , maxPlayers(4)
    , matchTimeLimit(300.0f)
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
    
    // Initialize combat system
    combatSystem = std::make_unique<CombatSystem>(this);
    
    // Initialize combat HUD
    combatHUD = std::make_unique<CombatHUD>(renderer.get(), combatSystem.get());
    combatHUD->initialize(width, height);
    
    // Initialize particle system
    particleSystem = std::make_unique<ParticleSystem>(renderer.get());
    particleSystem->initialize();
    
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
    combatSystem.reset();
    combatHUD.reset();
    particleSystem.reset();
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
        
        if (currentState == GameState::Combat) {
            updateCombat(dt);
        } else {
            updateGameplay(dt);
        }
        
        updateCamera(dt);
        updateParticles(dt);
        updateTrails(dt);
        
        // Update particle system
        if (particleSystem) {
            particleSystem->update(dt);
        }
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
        case GameState::Combat:
            renderGame();
            renderProjectiles();
            renderCombatEffects();
            break;
        case GameState::CombatPaused:
            renderGame();
            renderProjectiles();
            renderCombatEffects();
            renderPauseMenu();
            break;
        case GameState::GameOver:
            renderGame();
            break;
    }
    
    if (showHUD) {
        if (currentState == GameState::Combat || currentState == GameState::CombatPaused) {
            renderCombatHUD();
        } else {
            renderHUD();
        }
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
    
    // Racing callbacks
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
    inputManager->setFireLaserCallback([this]() { onFireLaser(); });
    inputManager->setFirePlasmaCallback([this]() { onFirePlasma(); });
    inputManager->setFireMissileCallback([this]() { onFireMissile(); });
    inputManager->setFireEnergyBallCallback([this]() { onFireEnergyBall(); });
    inputManager->setFistAttackCallback([this]() { onFistAttack(); });
    inputManager->setActivateShieldCallback([this]() { onActivateShield(); });
    inputManager->setTeleportCallback([this](Vector2 pos) { onTeleport(pos); });
    inputManager->setDashCallback([this]() { onDash(); });
    inputManager->setEnergyBurstCallback([this]() { onEnergyBurst(); });
    inputManager->setHealCallback([this]() { onHeal(); });
    inputManager->setToggleCombatModeCallback([this](bool enabled) { onToggleCombatMode(enabled); });
    inputManager->setAimCallback([this](float x, float y) { onAim(x, y); });
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
    initializeCombat();
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

void Game::initializeCombat() {
    if (combatSystem) {
        combatSystem->initialize();
        setupCombatCallbacks();
    }
}

void Game::setupCombatCallbacks() {
    if (!combatSystem) return;
    
    // Set up combat system callbacks
    combatSystem->setPlayerDamagedCallback([this](Car* attacker, Car* victim, float damage) {
        // Handle player damage events (e.g., show damage numbers, screen effects)
        if (particleSystem && victim) {
            particleSystem->createDamageEffect(victim->getPosition(), damage);
        }
    });
    
    combatSystem->setPlayerKilledCallback([this](Car* attacker, Car* victim) {
        // Handle player death events (e.g., show kill feed, update scores)
        if (particleSystem && victim) {
            particleSystem->createExplosion(victim->getPosition(), 5.0f, Projectile::ProjectileType::Missile);
        }
    });
    
    combatSystem->setPlayerRespawnedCallback([this](Car* player) {
        // Handle player respawn events (e.g., spawn effects)
        if (particleSystem && player) {
            particleSystem->createTeleportEffect(player->getPosition(), player->getPosition());
        }
    });
    
    combatSystem->setProjectileCreatedCallback([this](Projectile* projectile) {
        // Handle projectile creation (e.g., muzzle flash, launch effects)
        if (particleSystem && projectile) {
            Vector3 pos = projectile->getPosition();
            Vector3 dir = projectile->getDirection();
            particleSystem->createEffect(ParticleSystem::EffectType::Sparks, pos, dir);
        }
    });
    
    combatSystem->setExplosionCallback([this](const Vector3& position, float radius, Projectile::ProjectileType type) {
        // Handle explosions
        if (particleSystem) {
            particleSystem->createExplosion(position, radius, type);
        }
    });
    
    combatSystem->setMatchEndCallback([this]() {
        // Handle match end
        setState(GameState::GameOver);
    });
}

void Game::updateCombat(float deltaTime) {
    if (combatSystem) {
        combatSystem->update(deltaTime);
    }
}

void Game::startCombatMatch() {
    if (combatSystem) {
        combatSystem->startMatch();
        setState(GameState::Combat);
        
        // Add all cars to combat
        for (auto& car : cars) {
            combatSystem->addPlayer(car.get());
        }
        
        // Set player car for HUD
        if (combatHUD && playerCar) {
            combatHUD->setPlayerCar(playerCar);
        }
    }
}

void Game::endCombatMatch() {
    if (combatSystem) {
        combatSystem->endMatch();
        setState(GameState::GameOver);
    }
}

void Game::addPlayerToCombat(Car* car) {
    if (combatSystem && car) {
        combatSystem->addPlayer(car);
    }
}

void Game::removePlayerFromCombat(Car* car) {
    if (combatSystem && car) {
        combatSystem->removePlayer(car);
    }
}

void Game::renderCombatHUD() {
    if (combatHUD) {
        combatHUD->update(deltaTime);
        combatHUD->render();
    }
}

void Game::renderProjectiles() {
    if (!renderer || !combatSystem) return;
    
    // Render all active projectiles
    // This would iterate through the combat system's projectiles and render them
    // For now, this is a placeholder
}

void Game::renderCombatEffects() {
    if (particleSystem) {
        particleSystem->render();
    }
}

Vector3 Game::screenToWorldPosition(Vector2 screenPos) {
    // Convert screen coordinates to world position
    // This is a simplified implementation
    if (!camera) return Vector3::zero();
    
    // Normalize screen coordinates (-1 to 1)
    float normalizedX = (screenPos.x / screenWidth) * 2.0f - 1.0f;
    float normalizedY = 1.0f - (screenPos.y / screenHeight) * 2.0f;
    
    // Project onto ground plane (y = 0)
    // This is a simplified projection - a full implementation would use proper ray casting
    Vector3 cameraPos = camera->getPosition();
    Vector3 forward = camera->getForward();
    
    // Calculate intersection with ground plane
    float t = -cameraPos.y / forward.y;
    Vector3 worldPos = cameraPos + forward * t;
    
    // Apply screen offset
    Vector3 right = camera->getRight();
    Vector3 up = camera->getUp();
    worldPos += right * normalizedX * 50.0f; // Scale factor
    worldPos += up * normalizedY * 50.0f;
    
    return worldPos;
}

// Combat input handlers
void Game::onFireLaser() {
    if (combatSystem && playerCar) {
        combatSystem->usePlayerAbility(playerCar, PlayerAbilities::AbilityType::LaserAttack);
    }
}

void Game::onFirePlasma() {
    if (combatSystem && playerCar) {
        combatSystem->usePlayerAbility(playerCar, PlayerAbilities::AbilityType::PlasmaBlast);
    }
}

void Game::onFireMissile() {
    if (combatSystem && playerCar) {
        combatSystem->usePlayerAbility(playerCar, PlayerAbilities::AbilityType::MissileStrike);
    }
}

void Game::onFireEnergyBall() {
    if (combatSystem && playerCar) {
        combatSystem->usePlayerAbility(playerCar, PlayerAbilities::AbilityType::EnergyBall);
    }
}

void Game::onFistAttack() {
    if (combatSystem && playerCar) {
        combatSystem->usePlayerAbility(playerCar, PlayerAbilities::AbilityType::FistAttack);
    }
}

void Game::onActivateShield() {
    if (combatSystem && playerCar) {
        combatSystem->usePlayerAbility(playerCar, PlayerAbilities::AbilityType::Shield);
    }
}

void Game::onTeleport(Vector2 mousePos) {
    if (combatSystem && playerCar) {
        Vector3 worldPos = screenToWorldPosition(mousePos);
        combatSystem->usePlayerAbilityWithTarget(playerCar, PlayerAbilities::AbilityType::Teleport, worldPos);
    }
}

void Game::onDash() {
    if (combatSystem && playerCar) {
        combatSystem->usePlayerAbility(playerCar, PlayerAbilities::AbilityType::Dash);
    }
}

void Game::onEnergyBurst() {
    if (combatSystem && playerCar) {
        combatSystem->usePlayerAbility(playerCar, PlayerAbilities::AbilityType::EnergyBurst);
    }
}

void Game::onHeal() {
    if (combatSystem && playerCar) {
        combatSystem->usePlayerAbility(playerCar, PlayerAbilities::AbilityType::Heal);
    }
}

void Game::onToggleCombatMode(bool enabled) {
    combatModeEnabled = enabled;
    if (playerCar) {
        playerCar->setCombatMode(enabled);
    }
    
    if (enabled && currentState == GameState::Playing) {
        startCombatMatch();
    }
}

void Game::onAim(float deltaX, float deltaY) {
    if (playerCar && combatModeEnabled) {
        // Update aim direction based on mouse/input delta
        Vector3 currentAim = playerCar->getAimDirection();
        
        // Apply aim sensitivity
        float sensitivity = cameraSensitivity * 0.1f;
        
        // Update aim direction (simplified - would need proper 3D rotation)
        Vector3 right = playerCar->getRight();
        Vector3 up = Vector3::up();
        
        Vector3 newAim = currentAim;
        newAim += right * deltaX * sensitivity;
        newAim += up * deltaY * sensitivity;
        
        playerCar->setAimDirection(newAim.normalized());
    }
}

// Settings
void Game::setCombatModeEnabled(bool enabled) {
    combatModeEnabled = enabled;
}

void Game::setPvpMode(bool enabled) {
    pvpMode = enabled;
}

void Game::setMaxPlayers(int max) {
    maxPlayers = std::max(1, max);
}

void Game::setMatchTimeLimit(float time) {
    matchTimeLimit = std::max(60.0f, time);
    if (combatSystem) {
        combatSystem->setMatchTimeLimit(time);
    }
}