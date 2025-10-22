#pragma once
#include "Player.h"
#include "Camera.h"
#include "Shader.h"
#include "Mesh.h"
#include "Math.h"
#include <memory>
#include <vector>

class Game {
public:
    // Core components
    std::unique_ptr<Player> player;
    std::unique_ptr<Camera> camera;
    std::unique_ptr<Shader> basicShader;
    
    // Meshes
    std::unique_ptr<Mesh> playerMesh;
    std::unique_ptr<Mesh> groundMesh;
    std::unique_ptr<Mesh> trackMesh;
    
    // Game state
    bool isInitialized;
    float gameTime;
    float deltaTime;
    float lastFrameTime;
    
    // Racing elements
    std::vector<Vec3> checkpoints;
    int currentCheckpoint;
    float raceTime;
    bool raceStarted;
    
    // Lighting
    Vec3 lightPosition;
    Vec3 lightColor;
    
    // Screen dimensions
    int screenWidth;
    int screenHeight;
    
    Game();
    ~Game();
    
    bool initialize(int width, int height);
    void update(float currentTime);
    void render();
    void cleanup();
    
    // Input handling
    void handleTouch(float x, float y, bool pressed);
    void handleTouchMove(float x, float y);
    void handleTouchEnd();
    
    // Racing game specific
    void startRace();
    void checkCheckpoints();
    void resetRace();
    
private:
    // Input state
    Vec3 inputDirection;
    bool dashPressed;
    bool jumpPressed;
    
    // Touch input processing
    float touchStartX, touchStartY;
    bool isTouching;
    
    void setupScene();
    void updateInput();
    void renderObject(const Mesh& mesh, const Transform& transform, const Vec3& color);
};