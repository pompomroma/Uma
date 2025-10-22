#include "Game.h"
#include <iostream>
#include <fstream>
#include <sstream>

#ifdef __APPLE__
#include <OpenGLES/ES3/gl.h>
#else
#include <GL/gl.h>
#endif

Game::Game() : 
    isInitialized(false),
    gameTime(0),
    deltaTime(0),
    lastFrameTime(0),
    currentCheckpoint(0),
    raceTime(0),
    raceStarted(false),
    lightPosition(10, 10, 10),
    lightColor(1, 1, 1),
    screenWidth(0),
    screenHeight(0),
    inputDirection(0, 0, 0),
    dashPressed(false),
    jumpPressed(false),
    touchStartX(0),
    touchStartY(0),
    isTouching(false) {}

Game::~Game() {
    cleanup();
}

bool Game::initialize(int width, int height) {
    screenWidth = width;
    screenHeight = height;
    
    // Initialize OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glClearColor(0.3f, 0.6f, 1.0f, 1.0f); // Sky blue background
    
    // Create game objects
    player = std::make_unique<Player>();
    camera = std::make_unique<Camera>();
    basicShader = std::make_unique<Shader>();
    
    // Load shader
    std::string vertexShaderSource = R"(
#version 300 es
precision highp float;

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat4 uNormalMatrix;

out vec3 vPosition;
out vec3 vNormal;
out vec2 vTexCoord;
out vec3 vWorldPos;

void main() {
    vec4 worldPos = uModel * vec4(aPosition, 1.0);
    vWorldPos = worldPos.xyz;
    vPosition = aPosition;
    vNormal = normalize((uNormalMatrix * vec4(aNormal, 0.0)).xyz);
    vTexCoord = aTexCoord;
    
    gl_Position = uProjection * uView * worldPos;
}
)";

    std::string fragmentShaderSource = R"(
#version 300 es
precision highp float;

in vec3 vPosition;
in vec3 vNormal;
in vec2 vTexCoord;
in vec3 vWorldPos;

uniform vec3 uLightPos;
uniform vec3 uLightColor;
uniform vec3 uViewPos;
uniform vec3 uObjectColor;
uniform float uSpecularStrength;
uniform float uShininess;
uniform float uSpeed;
uniform float uDashIntensity;
uniform vec3 uDashColor;
uniform float uTime;

out vec4 FragColor;

void main() {
    vec3 ambient = 0.1 * uLightColor;
    
    vec3 norm = normalize(vNormal);
    vec3 lightDir = normalize(uLightPos - vWorldPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor;
    
    vec3 viewDir = normalize(uViewPos - vWorldPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), uShininess);
    vec3 specular = uSpecularStrength * spec * uLightColor;
    
    vec3 baseColor = uObjectColor;
    
    float speedEffect = uSpeed / 25.0;
    vec3 speedGlow = vec3(0.1, 0.3, 1.0) * speedEffect * 0.3;
    
    vec3 dashEffect = uDashColor * uDashIntensity;
    
    vec3 result = (ambient + diffuse + specular) * baseColor + speedGlow + dashEffect;
    
    FragColor = vec4(result, 1.0);
}
)";

    if (!basicShader->loadFromStrings(vertexShaderSource, fragmentShaderSource)) {
        std::cerr << "Failed to load shaders" << std::endl;
        return false;
    }
    
    // Create meshes
    playerMesh = std::make_unique<Mesh>(Mesh::createCube());
    playerMesh->setupMesh();
    
    groundMesh = std::make_unique<Mesh>(Mesh::createPlane(100, 100));
    groundMesh->setupMesh();
    
    trackMesh = std::make_unique<Mesh>(Mesh::createPlane(200, 20));
    trackMesh->setupMesh();
    
    // Setup camera
    camera->aspectRatio = (float)width / height;
    camera->transform.position = Vec3(0, 5, 10);
    
    // Setup scene
    setupScene();
    
    isInitialized = true;
    return true;
}

void Game::setupScene() {
    // Position player at start
    player->transform.position = Vec3(0, 1, 0);
    
    // Create checkpoints for racing
    checkpoints = {
        Vec3(0, 0, -20),
        Vec3(30, 0, -40),
        Vec3(60, 0, -20),
        Vec3(80, 0, 20),
        Vec3(50, 0, 40),
        Vec3(0, 0, 20),
        Vec3(-30, 0, 0)
    };
    
    currentCheckpoint = 0;
}

void Game::update(float currentTime) {
    if (!isInitialized) return;
    
    deltaTime = currentTime - lastFrameTime;
    lastFrameTime = currentTime;
    gameTime = currentTime;
    
    // Limit delta time to prevent large jumps
    if (deltaTime > 0.033f) deltaTime = 0.033f;
    
    updateInput();
    
    // Update player
    player->handleInput(inputDirection, dashPressed, jumpPressed);
    player->update(deltaTime);
    
    // Update camera to follow player
    camera->followTarget(player->transform.position, player->transform.forward(), deltaTime);
    camera->update(deltaTime);
    
    // Add camera shake when dashing
    if (player->isDashing()) {
        camera->addShake(0.5f, 0.1f);
    }
    
    // Update race logic
    if (raceStarted) {
        raceTime += deltaTime;
        checkCheckpoints();
    }
}

void Game::updateInput() {
    // Reset input each frame
    dashPressed = false;
    jumpPressed = false;
    
    // Touch input is processed in handleTouch methods
}

void Game::render() {
    if (!isInitialized) return;
    
    glViewport(0, 0, screenWidth, screenHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    basicShader->use();
    
    // Set camera matrices
    Mat4 view = camera->getViewMatrix();
    Mat4 projection = camera->getProjectionMatrix();
    basicShader->setMat4("uView", view);
    basicShader->setMat4("uProjection", projection);
    
    // Set lighting uniforms
    basicShader->setVec3("uLightPos", lightPosition);
    basicShader->setVec3("uLightColor", lightColor);
    basicShader->setVec3("uViewPos", camera->transform.position);
    basicShader->setFloat("uSpecularStrength", 0.5f);
    basicShader->setFloat("uShininess", 32.0f);
    basicShader->setFloat("uTime", gameTime);
    
    // Render ground
    Transform groundTransform;
    groundTransform.position = Vec3(0, -0.5f, 0);
    renderObject(*groundMesh, groundTransform, Vec3(0.2f, 0.8f, 0.2f));
    
    // Render track
    Transform trackTransform;
    trackTransform.position = Vec3(0, -0.4f, 0);
    renderObject(*trackMesh, trackTransform, Vec3(0.4f, 0.4f, 0.4f));
    
    // Render player with special effects
    Vec3 playerColor = Vec3(1.0f, 0.3f, 0.1f);
    basicShader->setFloat("uSpeed", player->getCurrentSpeed());
    basicShader->setFloat("uDashIntensity", player->showDashTrail ? player->dashTrailIntensity : 0.0f);
    basicShader->setVec3("uDashColor", player->dashTrailColor);
    
    renderObject(*playerMesh, player->transform, playerColor);
    
    // Render checkpoints
    for (size_t i = 0; i < checkpoints.size(); i++) {
        Transform checkpointTransform;
        checkpointTransform.position = checkpoints[i] + Vec3(0, 2, 0);
        checkpointTransform.scale = Vec3(0.5f, 3.0f, 0.5f);
        
        Vec3 checkpointColor = (i == currentCheckpoint) ? Vec3(1, 1, 0) : Vec3(0, 1, 1);
        renderObject(*playerMesh, checkpointTransform, checkpointColor);
    }
}

void Game::renderObject(const Mesh& mesh, const Transform& transform, const Vec3& color) {
    Mat4 model = transform.getMatrix();
    Mat4 normalMatrix = model; // Simplified - should be inverse transpose
    
    basicShader->setMat4("uModel", model);
    basicShader->setMat4("uNormalMatrix", normalMatrix);
    basicShader->setVec3("uObjectColor", color);
    
    mesh.render();
}

void Game::handleTouch(float x, float y, bool pressed) {
    if (pressed) {
        touchStartX = x;
        touchStartY = y;
        isTouching = true;
        
        // Start race on first touch
        if (!raceStarted) {
            startRace();
        }
    }
}

void Game::handleTouchMove(float x, float y) {
    if (!isTouching) return;
    
    float deltaX = x - touchStartX;
    float deltaY = y - touchStartY;
    
    // Convert screen coordinates to movement direction
    float sensitivity = 0.01f;
    inputDirection.x = deltaX * sensitivity;
    inputDirection.z = -deltaY * sensitivity; // Invert Y for forward/backward
    
    // Normalize if too large
    if (inputDirection.length() > 1.0f) {
        inputDirection = inputDirection.normalize();
    }
    
    // Dash on long swipe
    if (std::abs(deltaX) > 100 || std::abs(deltaY) > 100) {
        dashPressed = true;
    }
}

void Game::handleTouchEnd() {
    isTouching = false;
    inputDirection = Vec3(0, 0, 0);
}

void Game::startRace() {
    raceStarted = true;
    raceTime = 0;
    currentCheckpoint = 0;
}

void Game::checkCheckpoints() {
    if (currentCheckpoint >= checkpoints.size()) return;
    
    Vec3 checkpointPos = checkpoints[currentCheckpoint];
    float distance = (player->transform.position - checkpointPos).length();
    
    if (distance < 5.0f) {
        currentCheckpoint++;
        
        if (currentCheckpoint >= checkpoints.size()) {
            // Race completed!
            std::cout << "Race completed in " << raceTime << " seconds!" << std::endl;
            resetRace();
        }
    }
}

void Game::resetRace() {
    raceStarted = false;
    raceTime = 0;
    currentCheckpoint = 0;
    player->transform.position = Vec3(0, 1, 0);
}

void Game::cleanup() {
    // Cleanup is handled by smart pointers
    isInitialized = false;
}