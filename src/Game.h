#pragma once
#include "Platform/PlatformDetect.h"
#include "Camera/Camera.h"
#include "Physics/Car.h"
#include "Physics/PhysicsEngine.h"
#include "Rendering/Renderer.h"
#include "Input/InputManager.h"
#include "World/Track.h"
#include "Combat/CombatManager.h"
#include "Combat/Player.h"
#include <memory>
#include <vector>

#if PLATFORM_MOBILE
#include "Input/TouchInputManager.h"
#include "UI/MobileUI.h"
#endif

class Game {
public:
    enum class GameState {
        Menu,
        Playing,
        Paused,
        GameOver,
        PvPMode,
        StatsMenu
    };

private:
    // Core systems
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<InputManager> inputManager;
    std::unique_ptr<PhysicsEngine> physicsEngine;
    std::unique_ptr<Camera> camera;
    std::unique_ptr<Track> track;
    std::unique_ptr<CombatManager> combatManager;
    
#if PLATFORM_MOBILE
    std::unique_ptr<TouchInputManager> touchInputManager;
    std::unique_ptr<MobileUI> mobileUI;
#endif
    
    // Game objects
    std::vector<std::unique_ptr<Car>> cars;
    Car* playerCar;
    
    // PvP objects
    Player* localPlayer;
    std::vector<Player*> pvpPlayers;
    
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
    void onAttack1();
    void onAttack2();
    void onShield();
    void onTeleport();
    void onInteract();
    void onStatMenu();
    
    // Rendering
    void renderGame();
    void renderUI();
    void renderHUD();
    void renderDebugInfo();
    void renderMenu();
    void renderPauseMenu();
    void renderPvPMode();
    void renderStatsMenu();
    void renderCombatHUD();
    
    // Gameplay
    void updateGameplay(float deltaTime);
    void updateLapProgress();
    void updateTiming();
    void checkWinCondition();
    void spawnCars();
    void updateAI(float deltaTime);
    
    // PvP gameplay
    void initializePvPMode();
    void updatePvPMode(float deltaTime);
    void startPvPMatch();
    void endPvPMatch();
    void addPvPPlayer(const std::string& name);
    void handlePlayerLevelUp(Player* player);
    void distributeStatPoint(Player* player, const std::string& stat);
    
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
    
#if PLATFORM_MOBILE
    // Mobile-specific
    void handleTouchInput(int touchId, float x, float y, int phase, float pressure = 1.0f);
    void handleDeviceOrientation(float width, float height);
    void setLowPowerMode(bool enabled);
#endif
    
private:
    void initializeGame();
    void initializeCars();
    void initializeTrack();
    void initializeCamera();
    void initializeInput();
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
};