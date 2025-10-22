#include "MobileRenderer.h"
#include <iostream>
#include <cmath>

#ifdef __ANDROID__
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#elif defined(__APPLE__)
#include "TargetConditionals.h"
#if TARGET_OS_IPHONE
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#endif
#else
#include <GL/glew.h>
#endif

MobileRenderer::MobileRenderer() 
    : Renderer()
    , touchInput(nullptr)
    , platform(nullptr)
    , lowResFramebuffer(0)
    , lowResTexture(0)
    , lowResDepthBuffer(0)
    , virtualButtonTexture(0)
    , virtualJoystickTexture(0)
    , touchIndicatorTexture(0)
    , averageFPS(60.0f)
    , targetFPS(60.0f)
    , autoAdjustQuality(true) {
    
    // Default mobile optimizations
    optimizations.useLowPolyModels = false;
    optimizations.useSimpleShaders = false;
    optimizations.reducedParticles = false;
    optimizations.lowResolutionShadows = true;
    optimizations.disablePostProcessing = false;
    optimizations.maxDrawDistance = 200;
    optimizations.textureQuality = 1;
    optimizations.renderScale = 1.0f;
    
    // Touch visual settings
    touchVisuals.showVirtualControls = true;
    touchVisuals.showTouchIndicators = false;
    touchVisuals.controlOpacity = 0.7f;
    touchVisuals.controlColor = Vector3(1.0f, 1.0f, 1.0f);
    touchVisuals.controlActiveColor = Vector3(0.0f, 1.0f, 0.0f);
}

MobileRenderer::~MobileRenderer() {
    shutdown();
}

bool MobileRenderer::initialize(int width, int height) {
    // Call base initialization
    if (!Renderer::initialize(width, height)) {
        return false;
    }
    
    // Load mobile-specific shaders
    loadMobileShaders();
    
    // Create UI textures
    createUITextures();
    
    // Setup render scale framebuffer if needed
    if (optimizations.renderScale < 1.0f) {
        createLowResFramebuffer();
    }
    
    return true;
}

bool MobileRenderer::initializeMobile(PlatformBase* platformInstance, int width, int height) {
    platform = platformInstance;
    
    // Mobile-specific GL settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    // Disable features not needed on mobile
    glDisable(GL_MULTISAMPLE);
    
    return initialize(width, height);
}

void MobileRenderer::shutdown() {
    destroyLowResFramebuffer();
    destroyUITextures();
    
    mobileBasicShader.reset();
    mobileUIShader.reset();
    
    Renderer::shutdown();
}

void MobileRenderer::beginFrameMobile() {
    if (optimizations.renderScale < 1.0f && lowResFramebuffer != 0) {
        // Render to lower resolution framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, lowResFramebuffer);
        int scaledWidth = screenWidth * optimizations.renderScale;
        int scaledHeight = screenHeight * optimizations.renderScale;
        glViewport(0, 0, scaledWidth, scaledHeight);
    }
    
    beginFrame();
}

void MobileRenderer::endFrameMobile() {
    if (optimizations.renderScale < 1.0f && lowResFramebuffer != 0) {
        // Render low res framebuffer to screen
        renderLowResToScreen();
    }
    
    endFrame();
}

void MobileRenderer::renderVirtualControls(TouchInputManager* touchManager) {
    if (!touchManager || !touchVisuals.showVirtualControls) return;
    
    touchInput = touchManager;
    
    // Setup orthographic projection for UI
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Render based on current control layout
    // This would typically use the UI shader and render textured quads
    // For now, using basic shapes
    
    // Example: Render accelerate button
    Vector2 accelPos(screenWidth - 150, screenHeight - 150);
    renderVirtualButton(accelPos, 60, touchManager->isButtonPressed("Accelerate"), "A");
    
    // Example: Render brake button
    Vector2 brakePos(screenWidth - 300, screenHeight - 150);
    renderVirtualButton(brakePos, 60, touchManager->isButtonPressed("Brake"), "B");
    
    // Render virtual joysticks if active
    Vector2 leftStick = touchManager->getMovementInput();
    if (leftStick.magnitude() > 0) {
        Vector2 center(150, screenHeight - 150);
        renderVirtualJoystick(center, 80, center + leftStick * 60, true);
    }
    
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void MobileRenderer::renderVirtualJoystick(const Vector2& center, float radius, 
                                          const Vector2& thumbPos, bool isActive) {
    // Render joystick base
    Vector3 baseColor = isActive ? touchVisuals.controlActiveColor : touchVisuals.controlColor;
    baseColor = baseColor * touchVisuals.controlOpacity;
    
    // Draw outer circle (simplified - would use textured quad in production)
    renderCircle(center, radius, baseColor, 0.3f);
    
    // Draw thumb position
    renderCircle(thumbPos, radius * 0.4f, baseColor, 0.6f);
}

void MobileRenderer::renderVirtualButton(const Vector2& position, float radius, 
                                        bool isPressed, const std::string& label) {
    Vector3 color = isPressed ? touchVisuals.controlActiveColor : touchVisuals.controlColor;
    float opacity = isPressed ? touchVisuals.controlOpacity : touchVisuals.controlOpacity * 0.7f;
    
    // Draw button circle
    renderCircle(position, radius, color, opacity);
    
    // Draw label
    if (!label.empty()) {
        renderText(label, position.x, position.y, 1.5f, Vector3(1, 1, 1));
    }
}

void MobileRenderer::renderCircle(const Vector2& center, float radius, const Vector3& color, float opacity) {
    // Simplified circle rendering using triangle fan
    const int segments = 32;
    
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(color.x, color.y, color.z, opacity);
    glVertex2f(center.x, center.y);
    
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * M_PI * i / segments;
        float x = center.x + radius * cos(angle);
        float y = center.y + radius * sin(angle);
        glVertex2f(x, y);
    }
    glEnd();
}

void MobileRenderer::renderMobileHUD(float speed, int lap, float lapTime, int position) {
    // Setup 2D rendering
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    float scale = touchInput ? 1.5f : 1.0f; // Larger UI elements for touch
    
    // Render speedometer
    renderSpeedometer(screenWidth - 200, screenHeight - 200, 80, speed, 300);
    
    // Render position indicator
    renderPositionIndicator(position, 8);
    
    // Render lap info
    std::string lapText = "Lap " + std::to_string(lap) + "/3";
    renderText(lapText, 50, 50, scale, Vector3(1, 1, 1));
    
    // Render lap time
    int minutes = (int)lapTime / 60;
    int seconds = (int)lapTime % 60;
    int milliseconds = (int)((lapTime - (int)lapTime) * 1000);
    char timeStr[32];
    sprintf(timeStr, "%02d:%02d.%03d", minutes, seconds, milliseconds);
    renderText(timeStr, 50, 80, scale, Vector3(1, 1, 1));
    
    // Render minimap
    renderMinimap(50, screenHeight - 200, 150, 150);
    
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void MobileRenderer::renderSpeedometer(float x, float y, float radius, float speed, float maxSpeed) {
    // Draw speedometer background
    renderCircle(Vector2(x, y), radius, Vector3(0.2f, 0.2f, 0.2f), 0.8f);
    
    // Draw speed arc
    float speedRatio = speed / maxSpeed;
    float startAngle = M_PI * 0.75f;
    float endAngle = startAngle + (M_PI * 1.5f * speedRatio);
    
    glLineWidth(5.0f);
    glBegin(GL_LINE_STRIP);
    glColor4f(1.0f, speedRatio > 0.8f ? 0.0f : 1.0f, 0.0f, 1.0f);
    
    for (float angle = startAngle; angle <= endAngle; angle += 0.1f) {
        float px = x + radius * 0.8f * cos(angle);
        float py = y + radius * 0.8f * sin(angle);
        glVertex2f(px, py);
    }
    glEnd();
    glLineWidth(1.0f);
    
    // Draw speed text
    char speedStr[32];
    sprintf(speedStr, "%.0f", speed);
    renderText(speedStr, x, y, 2.0f, Vector3(1, 1, 1));
    renderText("km/h", x, y + 20, 0.8f, Vector3(0.8f, 0.8f, 0.8f));
}

void MobileRenderer::renderMinimap(float x, float y, float width, float height) {
    // Draw minimap background
    glColor4f(0.1f, 0.1f, 0.1f, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
    
    // Draw minimap border
    glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
    glLineWidth(1.0f);
    
    // TODO: Draw track and car positions on minimap
}

void MobileRenderer::renderPositionIndicator(int position, int totalRacers) {
    float x = screenWidth / 2;
    float y = 100;
    
    // Position background
    glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
    glBegin(GL_QUADS);
    glVertex2f(x - 50, y - 30);
    glVertex2f(x + 50, y - 30);
    glVertex2f(x + 50, y + 30);
    glVertex2f(x - 50, y + 30);
    glEnd();
    
    // Position text
    std::string posText = std::to_string(position);
    std::string suffix = "th";
    if (position == 1) suffix = "st";
    else if (position == 2) suffix = "nd";
    else if (position == 3) suffix = "rd";
    
    renderText(posText + suffix, x - 20, y - 10, 2.5f, Vector3(1, 1, 1));
    renderText("/" + std::to_string(totalRacers), x + 20, y + 10, 1.2f, Vector3(0.7f, 0.7f, 0.7f));
}

void MobileRenderer::renderMobileMenu(const std::vector<std::string>& options, int selected) {
    // Render menu background with transparency
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Background
    glColor4f(0.1f, 0.1f, 0.2f, 0.95f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(screenWidth, 0);
    glVertex2f(screenWidth, screenHeight);
    glVertex2f(0, screenHeight);
    glEnd();
    
    // Title
    renderText("RACING GAME 3D", screenWidth / 2, screenHeight * 0.2f, 3.0f, Vector3(1, 1, 1));
    renderText("MOBILE EDITION", screenWidth / 2, screenHeight * 0.25f, 1.5f, Vector3(0.7f, 0.7f, 1.0f));
    
    // Menu options with touch areas
    float startY = screenHeight * 0.4f;
    float spacing = 80.0f;
    
    for (size_t i = 0; i < options.size(); i++) {
        float y = startY + i * spacing;
        Vector3 color = (i == selected) ? Vector3(1, 1, 0) : Vector3(0.8f, 0.8f, 0.8f);
        
        // Touch area background
        if (i == selected) {
            glColor4f(0.3f, 0.3f, 0.5f, 0.5f);
            glBegin(GL_QUADS);
            glVertex2f(screenWidth * 0.3f, y - 30);
            glVertex2f(screenWidth * 0.7f, y - 30);
            glVertex2f(screenWidth * 0.7f, y + 30);
            glVertex2f(screenWidth * 0.3f, y + 30);
            glEnd();
        }
        
        renderText(options[i], screenWidth / 2, y, 2.0f, color);
    }
    
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void MobileRenderer::loadMobileShaders() {
    // Create mobile-optimized shaders
    mobileBasicShader = std::make_unique<Shader>();
    mobileBasicShader->loadFromString(getMobileVertexShaderSource(), getMobileFragmentShaderSource());
    
    mobileUIShader = std::make_unique<Shader>();
    mobileUIShader->loadFromString(getMobileUIVertexShaderSource(), getMobileUIFragmentShaderSource());
}

std::string MobileRenderer::getMobileVertexShaderSource() {
    return R"(
#version 300 es
precision mediump float;

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

void main() {
    FragPos = vec3(uModel * vec4(aPosition, 1.0));
    Normal = mat3(transpose(inverse(uModel))) * aNormal;
    TexCoord = aTexCoord;
    gl_Position = uProjection * uView * vec4(FragPos, 1.0);
}
)";
}

std::string MobileRenderer::getMobileFragmentShaderSource() {
    return R"(
#version 300 es
precision mediump float;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 uColor;
uniform vec3 uLightPos;
uniform vec3 uLightColor;
uniform vec3 uViewPos;

out vec4 FragColor;

void main() {
    // Simple Phong lighting optimized for mobile
    vec3 ambient = 0.3 * uColor;
    
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(uLightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor;
    
    vec3 result = ambient + diffuse * uColor;
    FragColor = vec4(result, 1.0);
}
)";
}

std::string MobileRenderer::getMobileUIVertexShaderSource() {
    return R"(
#version 300 es
precision mediump float;

layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec2 aTexCoord;

uniform mat4 uProjection;

out vec2 TexCoord;

void main() {
    TexCoord = aTexCoord;
    gl_Position = uProjection * vec4(aPosition, 0.0, 1.0);
}
)";
}

std::string MobileRenderer::getMobileUIFragmentShaderSource() {
    return R"(
#version 300 es
precision mediump float;

in vec2 TexCoord;

uniform sampler2D uTexture;
uniform vec4 uColor;
uniform bool uUseTexture;

out vec4 FragColor;

void main() {
    if (uUseTexture) {
        FragColor = texture(uTexture, TexCoord) * uColor;
    } else {
        FragColor = uColor;
    }
}
)";
}

void MobileRenderer::setOptimizations(const MobileOptimizations& opts) {
    optimizations = opts;
    
    // Update render scale framebuffer if needed
    if (optimizations.renderScale < 1.0f) {
        if (lowResFramebuffer == 0) {
            createLowResFramebuffer();
        }
    } else {
        destroyLowResFramebuffer();
    }
}

void MobileRenderer::autoOptimizeForPerformance(float currentFPS) {
    if (!autoAdjustQuality) return;
    
    // Update average FPS
    averageFPS = averageFPS * 0.9f + currentFPS * 0.1f;
    
    // Adjust quality if FPS is consistently low or high
    if (averageFPS < targetFPS * 0.9f) {
        // Reduce quality
        adjustQualitySettings(averageFPS);
    } else if (averageFPS > targetFPS * 1.1f) {
        // Can potentially increase quality
        // But be conservative to avoid oscillation
    }
}

void MobileRenderer::adjustQualitySettings(float currentFPS) {
    float fpsRatio = currentFPS / targetFPS;
    
    if (fpsRatio < 0.5f) {
        // Very low FPS - aggressive optimization
        optimizations.renderScale = 0.5f;
        optimizations.reducedParticles = true;
        optimizations.disablePostProcessing = true;
        optimizations.textureQuality = 0;
        optimizations.maxDrawDistance = 100;
    } else if (fpsRatio < 0.75f) {
        // Low FPS - moderate optimization
        optimizations.renderScale = 0.75f;
        optimizations.reducedParticles = true;
        optimizations.textureQuality = std::max(0, optimizations.textureQuality - 1);
        optimizations.maxDrawDistance = 150;
    } else if (fpsRatio < 0.9f) {
        // Slightly low FPS - minor optimization
        optimizations.renderScale = 0.85f;
    }
    
    // Apply new optimizations
    setOptimizations(optimizations);
}

void MobileRenderer::createLowResFramebuffer() {
    destroyLowResFramebuffer();
    
    int scaledWidth = screenWidth * optimizations.renderScale;
    int scaledHeight = screenHeight * optimizations.renderScale;
    
    // Create framebuffer
    glGenFramebuffers(1, &lowResFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, lowResFramebuffer);
    
    // Create color texture
    glGenTextures(1, &lowResTexture);
    glBindTexture(GL_TEXTURE_2D, lowResTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, scaledWidth, scaledHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lowResTexture, 0);
    
    // Create depth buffer
    glGenRenderbuffers(1, &lowResDepthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, lowResDepthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, scaledWidth, scaledHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, lowResDepthBuffer);
    
    // Check completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Failed to create low-res framebuffer" << std::endl;
        destroyLowResFramebuffer();
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MobileRenderer::destroyLowResFramebuffer() {
    if (lowResFramebuffer != 0) {
        glDeleteFramebuffers(1, &lowResFramebuffer);
        lowResFramebuffer = 0;
    }
    if (lowResTexture != 0) {
        glDeleteTextures(1, &lowResTexture);
        lowResTexture = 0;
    }
    if (lowResDepthBuffer != 0) {
        glDeleteRenderbuffers(1, &lowResDepthBuffer);
        lowResDepthBuffer = 0;
    }
}

void MobileRenderer::renderLowResToScreen() {
    // Bind default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, screenWidth, screenHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Render fullscreen quad with low-res texture
    glDisable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_2D, lowResTexture);
    
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(-1, -1);
    glTexCoord2f(1, 0); glVertex2f(1, -1);
    glTexCoord2f(1, 1); glVertex2f(1, 1);
    glTexCoord2f(0, 1); glVertex2f(-1, 1);
    glEnd();
    
    glEnable(GL_DEPTH_TEST);
}

void MobileRenderer::createUITextures() {
    // TODO: Load actual textures for UI elements
    // For now, create placeholder textures
}

void MobileRenderer::destroyUITextures() {
    if (virtualButtonTexture != 0) {
        glDeleteTextures(1, &virtualButtonTexture);
        virtualButtonTexture = 0;
    }
    if (virtualJoystickTexture != 0) {
        glDeleteTextures(1, &virtualJoystickTexture);
        virtualJoystickTexture = 0;
    }
    if (touchIndicatorTexture != 0) {
        glDeleteTextures(1, &touchIndicatorTexture);
        touchIndicatorTexture = 0;
    }
}