#pragma once
#include "Renderer.h"
#include "../Platform/PlatformBase.h"
#include "../Input/TouchInputManager.h"

class MobileRenderer : public Renderer {
public:
    struct MobileOptimizations {
        bool useLowPolyModels;
        bool useSimpleShaders;
        bool reducedParticles;
        bool lowResolutionShadows;
        bool disablePostProcessing;
        int maxDrawDistance;
        int textureQuality; // 0 = low, 1 = medium, 2 = high
        float renderScale;   // 0.5 to 1.0
    };
    
    struct TouchVisuals {
        bool showVirtualControls;
        bool showTouchIndicators;
        float controlOpacity;
        Vector3 controlColor;
        Vector3 controlActiveColor;
    };

private:
    MobileOptimizations optimizations;
    TouchVisuals touchVisuals;
    TouchInputManager* touchInput;
    PlatformBase* platform;
    
    // Mobile-specific shaders
    std::unique_ptr<Shader> mobileBasicShader;
    std::unique_ptr<Shader> mobileUIShader;
    
    // Render targets for different quality levels
    unsigned int lowResFramebuffer;
    unsigned int lowResTexture;
    unsigned int lowResDepthBuffer;
    
    // UI textures
    unsigned int virtualButtonTexture;
    unsigned int virtualJoystickTexture;
    unsigned int touchIndicatorTexture;
    
    // Performance metrics
    float averageFPS;
    float targetFPS;
    bool autoAdjustQuality;
    
public:
    MobileRenderer();
    ~MobileRenderer() override;
    
    // Initialization with mobile features
    bool initialize(int width, int height) override;
    bool initializeMobile(PlatformBase* platform, int width, int height);
    void shutdown() override;
    
    // Mobile rendering setup
    void beginFrameMobile();
    void endFrameMobile();
    void setRenderScale(float scale);
    
    // Touch visualization
    void renderVirtualControls(TouchInputManager* touchManager);
    void renderVirtualJoystick(const Vector2& center, float radius, const Vector2& thumbPos, bool isActive);
    void renderVirtualButton(const Vector2& position, float radius, bool isPressed, const std::string& label);
    void renderTouchIndicator(const Vector2& position, float size);
    
    // Mobile-optimized rendering
    void renderCarMobile(const Matrix4& modelMatrix, const Vector3& color);
    void renderTrackMobile(const std::vector<Vector3>& vertices, const std::vector<unsigned int>& indices);
    void renderEnvironmentMobile();
    
    // UI rendering for mobile
    void renderMobileHUD(float speed, int lap, float lapTime, int position);
    void renderMobileMenu(const std::vector<std::string>& options, int selected);
    void renderMobilePauseMenu();
    
    // Touch-specific UI elements
    void renderSpeedometer(float x, float y, float radius, float speed, float maxSpeed);
    void renderMinimap(float x, float y, float width, float height);
    void renderPositionIndicator(int position, int totalRacers);
    
    // Optimization settings
    void setOptimizations(const MobileOptimizations& opts);
    void autoOptimizeForPerformance(float currentFPS);
    void setTargetFPS(float fps) { targetFPS = fps; }
    
    // Touch visual settings
    void setTouchVisualsEnabled(bool enabled);
    void setControlOpacity(float opacity);
    void setControlColors(const Vector3& normal, const Vector3& active);
    
    // Mobile shader management
    void loadMobileShaders();
    std::string getMobileVertexShaderSource();
    std::string getMobileFragmentShaderSource();
    std::string getMobileUIVertexShaderSource();
    std::string getMobileUIFragmentShaderSource();
    
    // Texture management for mobile
    void loadCompressedTexture(const std::string& path, int& textureId);
    void generateMipmaps(unsigned int textureId);
    
    // Performance helpers
    float getAverageFPS() const { return averageFPS; }
    bool isAutoAdjustEnabled() const { return autoAdjustQuality; }
    const MobileOptimizations& getOptimizations() const { return optimizations; }
    
private:
    void createLowResFramebuffer();
    void destroyLowResFramebuffer();
    void renderToLowResFramebuffer();
    void renderLowResToScreen();
    
    void updatePerformanceMetrics(float deltaTime);
    void adjustQualitySettings(float currentFPS);
    
    void createUITextures();
    void destroyUITextures();
    
    // Helper functions for mobile rendering
    void setupMobileMatrices(Shader* shader, const Matrix4& modelMatrix);
    void bindMobileTextures(Shader* shader);
    void setMobileRenderState();
};