#pragma once
#include "Renderer.h"
#include "../Platform/Platform.h"
#include <vector>
#include <memory>

class MobileRenderer : public Renderer {
public:
    struct MobileRenderSettings {
        bool useInstancing;
        bool useVAO;
        bool useVBO;
        bool useEBO;
        bool useTextureCompression;
        bool useMipmaps;
        int maxTextureSize;
        int maxVertexAttribs;
        float textureQuality;
        float modelQuality;
        bool enableFrustumCulling;
        bool enableOcclusionCulling;
        bool enableLOD;
        int maxLights;
        bool enableShadows;
        bool enableParticles;
        bool enablePostProcessing;
    };

    struct MobileUIElement {
        enum Type {
            Button,
            Joystick,
            Slider,
            Label,
            Image,
            ProgressBar
        };
        
        Type type;
        Vector2 position;
        Vector2 size;
        Vector3 color;
        float opacity;
        bool isVisible;
        bool isInteractive;
        std::string text;
        int textureId;
    };

private:
    MobileRenderSettings m_settings;
    std::vector<MobileUIElement> m_uiElements;
    
    // Mobile-specific shaders
    std::unique_ptr<Shader> m_mobileBasicShader;
    std::unique_ptr<Shader> m_mobileUIShader;
    std::unique_ptr<Shader> m_mobileParticleShader;
    std::unique_ptr<Shader> m_mobileSkyboxShader;
    
    // Mobile-specific meshes
    std::vector<Mesh> m_instancedMeshes;
    std::unordered_map<std::string, Mesh> m_meshCache;
    
    // Performance tracking
    int m_drawCalls;
    int m_trianglesRendered;
    float m_frameTime;
    int m_textureMemory;
    int m_vertexMemory;
    
    // Mobile optimizations
    bool m_frustumCulling;
    bool m_occlusionCulling;
    bool m_lodEnabled;
    float m_lodDistance;
    int m_maxLights;
    bool m_shadowsEnabled;
    bool m_particlesEnabled;
    bool m_postProcessingEnabled;
    
    // UI rendering
    bool m_uiEnabled;
    float m_uiScale;
    Vector2 m_screenSize;
    Vector2 m_safeArea;
    
public:
    MobileRenderer();
    ~MobileRenderer();
    
    // Initialization
    bool initialize(int width, int height) override;
    void shutdown() override;
    
    // Mobile-specific initialization
    bool initializeMobile();
    void detectCapabilities();
    void optimizeForDevice();
    
    // Mobile rendering
    void beginFrame() override;
    void endFrame() override;
    void renderMobile();
    void renderUI();
    void renderMobileControls();
    
    // Mobile-optimized rendering
    void renderInstanced(const Mesh& mesh, const std::vector<Matrix4>& transforms, const Vector3& color);
    void renderLOD(const Mesh& mesh, const Matrix4& modelMatrix, const Vector3& color, float distance);
    void renderFrustumCulled(const Mesh& mesh, const Matrix4& modelMatrix, const Vector3& color);
    void renderOcclusionCulled(const Mesh& mesh, const Matrix4& modelMatrix, const Vector3& color);
    
    // Mobile UI
    void addUIElement(const MobileUIElement& element);
    void removeUIElement(int index);
    void updateUIElement(int index, const MobileUIElement& element);
    void clearUIElements();
    
    // Mobile controls rendering
    void renderVirtualJoystick(const Vector2& center, float radius, const Vector2& position, const Vector3& color);
    void renderVirtualButton(const Vector2& position, const Vector2& size, const std::string& label, const Vector3& color, bool isPressed);
    void renderVirtualSlider(const Vector2& position, const Vector2& size, float value, const Vector3& color);
    void renderHealthBar(const Vector2& position, const Vector2& size, float percentage, const Vector3& fillColor, const Vector3& bgColor);
    void renderProgressBar(const Vector2& position, const Vector2& size, float progress, const Vector3& fillColor, const Vector3& bgColor);
    
    // Mobile text rendering
    void renderText(const std::string& text, const Vector2& position, float scale, const Vector3& color);
    void renderText3D(const std::string& text, const Vector3& position, float scale, const Vector3& color);
    void renderDebugText(const std::string& text, const Vector2& position, const Vector3& color);
    
    // Mobile particle system
    void renderParticles(const std::vector<Particle>& particles);
    void updateParticles(std::vector<Particle>& particles, float deltaTime);
    
    // Mobile lighting
    void setupMobileLighting(Shader* shader);
    void addMobileLight(const Light& light);
    void removeMobileLight(int index);
    void clearMobileLights();
    
    // Mobile shadows
    void renderShadowMap();
    void setupShadowMapping(Shader* shader);
    
    // Mobile post-processing
    void renderPostProcessing();
    void setupPostProcessing();
    
    // Mobile performance
    void setTargetFPS(int fps);
    void setVSync(bool enabled);
    void setTextureQuality(float quality);
    void setModelQuality(float quality);
    void setLODDistance(float distance);
    void setMaxLights(int maxLights);
    void setShadowsEnabled(bool enabled);
    void setParticlesEnabled(bool enabled);
    void setPostProcessingEnabled(bool enabled);
    
    // Mobile UI settings
    void setUIScale(float scale);
    void setSafeArea(const Vector2& safeArea);
    void setUIEnabled(bool enabled);
    
    // Mobile memory management
    void preloadAssets();
    void unloadUnusedAssets();
    void optimizeMemory();
    int getTextureMemoryUsage() const;
    int getVertexMemoryUsage() const;
    
    // Mobile debugging
    void renderDebugInfo() override;
    void renderPerformanceOverlay();
    void renderTouchDebug();
    void renderMemoryUsage();
    
    // Mobile-specific utilities
    Vector2 screenToWorld(const Vector2& screenPos) const;
    Vector2 worldToScreen(const Vector3& worldPos) const;
    bool isPointInRect(const Vector2& point, const Vector2& rectPos, const Vector2& rectSize) const;
    Vector2 getScreenSize() const;
    Vector2 getSafeArea() const;
    
    // Mobile shader management
    void loadMobileShaders();
    void reloadMobileShaders();
    Shader* getMobileBasicShader() const;
    Shader* getMobileUIShader() const;
    Shader* getMobileParticleShader() const;
    Shader* getMobileSkyboxShader() const;
    
    // Mobile mesh management
    Mesh createMobileMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    Mesh createMobileQuad(float width, float height);
    Mesh createMobileCircle(float radius, int segments);
    Mesh createMobileButton(float width, float height);
    Mesh createMobileJoystick(float radius, int segments);
    
    // Mobile texture management
    int loadMobileTexture(const std::string& filename);
    void unloadMobileTexture(int textureId);
    void setMobileTexture(int textureId, int slot);
    
    // Mobile audio integration
    void playMobileSound(const std::string& filename);
    void stopMobileSound(const std::string& filename);
    void setMobileVolume(float volume);
    
    // Mobile input integration
    void handleTouchInput(const Vector2& position, bool isPressed);
    void handleGestureInput(const Vector2& delta, float scale, float rotation);
    
    // Mobile platform integration
    void handleOrientationChange(int orientation);
    void handleScreenSizeChange(int width, int height);
    void handleMemoryWarning();
    void handleAppStateChange(int state);
    
    // Mobile-specific rendering
    void renderMobileSkybox();
    void renderMobileEnvironment();
    void renderMobileCars();
    void renderMobileTrack();
    void renderMobileParticles();
    void renderMobileTrails();
    
    // Mobile HUD
    void renderMobileHUD();
    void renderMobileCombatHUD();
    void renderMobileRacingHUD();
    void renderMobileMenuHUD();
    
    // Mobile performance monitoring
    void startPerformanceTimer();
    void endPerformanceTimer();
    float getAverageFrameTime() const;
    int getAverageDrawCalls() const;
    int getAverageTriangles() const;
    
private:
    void setupMobileRenderState();
    void setupMobileShaders();
    void setupMobileMeshes();
    void setupMobileTextures();
    void setupMobileLighting();
    void setupMobileShadows();
    void setupMobilePostProcessing();
    
    void renderMobileMesh(const Mesh& mesh, const Matrix4& modelMatrix, const Vector3& color);
    void renderMobileUIElement(const MobileUIElement& element);
    void renderMobileText(const std::string& text, const Vector2& position, float scale, const Vector3& color);
    
    void updateMobilePerformance();
    void optimizeMobileRendering();
    void cullMobileObjects();
    void sortMobileObjects();
    
    // Mobile shader sources
    std::string getMobileBasicVertexShader();
    std::string getMobileBasicFragmentShader();
    std::string getMobileUIVertexShader();
    std::string getMobileUIFragmentShader();
    std::string getMobileParticleVertexShader();
    std::string getMobileParticleFragmentShader();
    std::string getMobileSkyboxVertexShader();
    std::string getMobileSkyboxFragmentShader();
    
    // Mobile utilities
    bool isMobileCapable();
    void logMobileInfo();
    void optimizeForMobile();
    void handleMobileErrors();
};