#pragma once
#include "Camera/Camera.h"
#include "Physics/Car.h"
#include "Physics/PhysicsEngine.h"
#include "Rendering/Renderer.h"
#include "Input/InputManager.h"
#include "World/Track.h"
#include "Combat/CombatSystem.h"
#include "UI/CombatHUD.h"
#include "Effects/ParticleSystem.h"
#include <memory>
#include <vector>

class Game {
public:
    enum class GameState {
        Menu,
        Playing,
        Paused,
        GameOver,
        Combat,         // PvP combat mode
        CombatPaused    // Combat mode paused
    };

private:
    // Core systems
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<InputManager> inputManager;
    std::unique_ptr<PhysicsEngine> physicsEngine;
    std::unique_ptr<Camera> camera;
    std::unique_ptr<Track> track;
    std::unique_ptr<CombatSystem> combatSystem;
    std::unique_ptr<CombatHUD> combatHUD;
    std::unique_ptr<ParticleSystem> particleSystem;
    
    // Game objects
    std::vector<std::unique_ptr<Car>> cars;
    Car* playerCar;
    
    // Game state
    GameState currentState;
    bool isRunning;
    bool isPaused;
    float gameTime;
    float deltaTime;
    int currentLap;
    int totalLaps;
    float bestTime;
    float currentLapTime;
    
    // Performance
    int frameCount;
    float fps;
    float lastFpsUpdate;
    
    // Settings
    int screenWidth;
    int screenHeight;
    bool fullscreen;
    bool vsync;
    float masterVolume;
    float sfxVolume;
    float musicVolume;
    
    // Camera settings
    float cameraDistance;
    float cameraHeight;
    float cameraSensitivity;
    bool cameraInverted;
    
    // Gameplay settings
    float difficulty;
    bool autoBrake;
    bool autoSteer;
    bool showHUD;
    bool showDebugInfo;
    
    // Combat settings
    bool combatModeEnabled;
    bool pvpMode;
    int maxPlayers;
    float matchTimeLimit;

public:
    Game();
    ~Game();
    
    // Initialization
    bool initialize(int width, int height, const std::string& title);
    void shutdown();
    
    // Main game loop
    void run();
    void update(float deltaTime);
    void render();
    void handleInput();
    
    // Game state management
    void setState(GameState state);
    GameState getState() const { return currentState; }
    void pause();
    void resume();
    void restart();
    void quit();
    
    // Car management
    void addCar(std::unique_ptr<Car> car);
    void removeCar(Car* car);
    void setPlayerCar(Car* car);
    Car* getPlayerCar() const { return playerCar; }
    
    // Track management
    void setTrack(std::unique_ptr<Track> newTrack);
    Track* getTrack() const { return track.get(); }
    
    // Camera management
    void setCamera(std::unique_ptr<Camera> newCamera);
    Camera* getCamera() const { return camera.get(); }
    void updateCamera(float deltaTime);
    
    // Input handling
    void setupInputCallbacks();
    void onAccelerate(float input);
    void onBrake(float input);
    void onSteer(float input);
    void onBoost(bool input);
    void onHandbrake(bool input);
    void onCameraLook(float deltaX, float deltaY);
    void onCameraZoom(float delta);
    void onPause();
    void onReset();
    
    // Combat input handling
    void onFireLaser();
    void onFirePlasma();
    void onFireMissile();
    void onFireEnergyBall();
    void onFistAttack();
    void onActivateShield();
    void onTeleport(Vector2 mousePos);
    void onDash();
    void onEnergyBurst();
    void onHeal();
    void onToggleCombatMode(bool enabled);
    void onAim(float deltaX, float deltaY);
    
    // Rendering
    void renderGame();
    void renderUI();
    void renderHUD();
    void renderDebugInfo();
    void renderMenu();
    void renderPauseMenu();
    void renderCombatHUD();
    void renderProjectiles();
    void renderCombatEffects();
    
    // Gameplay
    void updateGameplay(float deltaTime);
    void updateLapProgress();
    void updateTiming();
    void checkWinCondition();
    void spawnCars();
    void updateAI(float deltaTime);
    
    // Combat gameplay
    void updateCombat(float deltaTime);
    void startCombatMatch();
    void endCombatMatch();
    void addPlayerToCombat(Car* car);
    void removePlayerFromCombat(Car* car);
    
    // Settings
    void setScreenSize(int width, int height);
    void setFullscreen(bool fullscreen);
    void setVSync(bool vsync);
    void setMasterVolume(float volume);
    void setSFXVolume(float volume);
    void setMusicVolume(float volume);
    void setCameraSensitivity(float sensitivity);
    void setCameraInverted(bool inverted);
    void setDifficulty(float difficulty);
    void setAutoBrake(bool enabled);
    void setAutoSteer(bool enabled);
    void setShowHUD(bool show);
    void setShowDebugInfo(bool show);
    void setCombatModeEnabled(bool enabled);
    void setPvpMode(bool enabled);
    void setMaxPlayers(int max);
    void setMatchTimeLimit(float time);
    
    // Getters
    bool getIsRunning() const { return isRunning; }
    bool getIsPaused() const { return isPaused; }
    float getGameTime() const { return gameTime; }
    int getCurrentLap() const { return currentLap; }
    int getTotalLaps() const { return totalLaps; }
    float getBestTime() const { return bestTime; }
    float getCurrentLapTime() const { return currentLapTime; }
    float getFPS() const { return fps; }
    
    // Utility
    void resetGame();
    void loadSettings();
    void saveSettings();
    void showMessage(const std::string& message);
    
private:
    void initializeGame();
    void initializeCars();
    void initializeTrack();
    void initializeCamera();
    void initializeInput();
    void initializeCombat();
    void updatePhysics(float deltaTime);
    void updateAudio(float deltaTime);
    void updateUI(float deltaTime);
    void renderCars();
    void renderTrack();
    void renderEnvironment();
    void renderParticles();
    void renderTrails();
    void updateParticles(float deltaTime);
    void updateTrails(float deltaTime);
    void setupCombatCallbacks();
    Vector3 screenToWorldPosition(Vector2 screenPos);
};