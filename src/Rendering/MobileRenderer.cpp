#include "MobileRenderer.h"
#include <algorithm>
#include <cmath>

#if PLATFORM_MOBILE
    #include <GLES3/gl3.h>
#else
    #include <GL/glew.h>
#endif

MobileRenderer::MobileRenderer() 
    : m_drawCalls(0)
    , m_trianglesRendered(0)
    , m_frameTime(0.0f)
    , m_textureMemory(0)
    , m_vertexMemory(0)
    , m_frustumCulling(true)
    , m_occlusionCulling(false)
    , m_lodEnabled(true)
    , m_lodDistance(100.0f)
    , m_maxLights(4)
    , m_shadowsEnabled(false)
    , m_particlesEnabled(true)
    , m_postProcessingEnabled(false)
    , m_uiEnabled(true)
    , m_uiScale(1.0f)
    , m_screenSize(1920, 1080)
    , m_safeArea(0, 0) {
    
    // Initialize mobile render settings
    m_settings.useInstancing = true;
    m_settings.useVAO = true;
    m_settings.useVBO = true;
    m_settings.useEBO = true;
    m_settings.useTextureCompression = true;
    m_settings.useMipmaps = true;
    m_settings.maxTextureSize = 2048;
    m_settings.maxVertexAttribs = 8;
    m_settings.textureQuality = 0.8f;
    m_settings.modelQuality = 0.7f;
    m_settings.enableFrustumCulling = true;
    m_settings.enableOcclusionCulling = false;
    m_settings.enableLOD = true;
    m_settings.maxLights = 4;
    m_settings.enableShadows = false;
    m_settings.enableParticles = true;
    m_settings.enablePostProcessing = false;
}

MobileRenderer::~MobileRenderer() {
    shutdown();
}

bool MobileRenderer::initialize(int width, int height) {
    if (!Renderer::initialize(width, height)) {
        return false;
    }
    
    m_screenSize = Vector2(width, height);
    
    // Initialize mobile-specific systems
    if (!initializeMobile()) {
        return false;
    }
    
    return true;
}

void MobileRenderer::shutdown() {
    // Cleanup mobile-specific resources
    m_uiElements.clear();
    m_instancedMeshes.clear();
    m_meshCache.clear();
    
    m_mobileBasicShader.reset();
    m_mobileUIShader.reset();
    m_mobileParticleShader.reset();
    m_mobileSkyboxShader.reset();
    
    Renderer::shutdown();
}

bool MobileRenderer::initializeMobile() {
    // Detect mobile capabilities
    detectCapabilities();
    
    // Setup mobile-specific systems
    setupMobileRenderState();
    setupMobileShaders();
    setupMobileMeshes();
    setupMobileTextures();
    setupMobileLighting();
    
    // Optimize for mobile
    optimizeForDevice();
    
    return true;
}

void MobileRenderer::detectCapabilities() {
    // Get platform info
    auto platformInfo = Platform::PlatformManager::getPlatformInfo();
    
    // Update settings based on platform capabilities
    m_settings.maxTextureSize = platformInfo.maxTextureSize;
    m_settings.maxVertexAttribs = platformInfo.maxVertexAttribs;
    m_settings.supportsInstancing = platformInfo.supportsInstancing;
    m_settings.supportsVAO = platformInfo.supportsVAO;
    
    // Adjust quality based on device capabilities
    if (platformInfo.maxTextureSize < 2048) {
        m_settings.textureQuality = 0.6f;
        m_settings.modelQuality = 0.5f;
    }
    
    // Disable features not supported on mobile
    if (!platformInfo.supportsInstancing) {
        m_settings.useInstancing = false;
    }
    
    if (!platformInfo.supportsVAO) {
        m_settings.useVAO = false;
    }
}

void MobileRenderer::optimizeForDevice() {
    // Reduce quality for better performance
    m_settings.textureQuality = std::min(m_settings.textureQuality, 0.8f);
    m_settings.modelQuality = std::min(m_settings.modelQuality, 0.7f);
    
    // Limit features for mobile
    m_settings.maxLights = std::min(m_settings.maxLights, 4);
    m_settings.enableShadows = false;
    m_settings.enablePostProcessing = false;
    
    // Enable mobile optimizations
    m_settings.enableFrustumCulling = true;
    m_settings.enableLOD = true;
    m_settings.useTextureCompression = true;
    m_settings.useMipmaps = true;
}

void MobileRenderer::beginFrame() {
    Renderer::beginFrame();
    
    // Mobile-specific frame setup
    m_drawCalls = 0;
    m_trianglesRendered = 0;
    
    // Setup mobile render state
    setupMobileRenderState();
}

void MobileRenderer::endFrame() {
    // Render mobile UI
    if (m_uiEnabled) {
        renderUI();
    }
    
    // Mobile-specific frame end
    updateMobilePerformance();
    
    Renderer::endFrame();
}

void MobileRenderer::renderMobile() {
    // Render mobile-optimized scene
    renderMobileSkybox();
    renderMobileEnvironment();
    renderMobileCars();
    renderMobileTrack();
    
    if (m_particlesEnabled) {
        renderMobileParticles();
        renderMobileTrails();
    }
}

void MobileRenderer::renderUI() {
    if (!m_uiEnabled) return;
    
    // Switch to UI shader
    if (m_mobileUIShader) {
        m_mobileUIShader->use();
    }
    
    // Render UI elements
    for (const auto& element : m_uiElements) {
        if (element.isVisible) {
            renderMobileUIElement(element);
        }
    }
    
    // Render mobile controls
    renderMobileControls();
}

void MobileRenderer::renderMobileControls() {
    // This would render virtual controls
    // Implementation depends on the specific control system
}

void MobileRenderer::renderInstanced(const Mesh& mesh, const std::vector<Matrix4>& transforms, const Vector3& color) {
    if (!m_settings.useInstancing || transforms.empty()) {
        // Fallback to individual rendering
        for (const auto& transform : transforms) {
            renderMobileMesh(mesh, transform, color);
        }
        return;
    }
    
    // Instanced rendering implementation
    if (m_mobileBasicShader) {
        m_mobileBasicShader->use();
        setupMatrices(m_mobileBasicShader.get(), Matrix4::identity());
        m_mobileBasicShader->setVec3("color", color.x, color.y, color.z);
        
        // Setup instanced rendering
        // This would use glDrawElementsInstanced
        m_drawCalls++;
    }
}

void MobileRenderer::renderLOD(const Mesh& mesh, const Matrix4& modelMatrix, const Vector3& color, float distance) {
    if (!m_settings.enableLOD) {
        renderMobileMesh(mesh, modelMatrix, color);
        return;
    }
    
    // LOD implementation
    float lodLevel = distance / m_lodDistance;
    if (lodLevel > 1.0f) {
        // Use lower detail mesh
        // Implementation would select appropriate LOD mesh
    }
    
    renderMobileMesh(mesh, modelMatrix, color);
}

void MobileRenderer::renderFrustumCulled(const Mesh& mesh, const Matrix4& modelMatrix, const Vector3& color) {
    if (!m_settings.enableFrustumCulling) {
        renderMobileMesh(mesh, modelMatrix, color);
        return;
    }
    
    // Frustum culling implementation
    // This would check if the mesh is within the camera frustum
    renderMobileMesh(mesh, modelMatrix, color);
}

void MobileRenderer::renderOcclusionCulled(const Mesh& mesh, const Matrix4& modelMatrix, const Vector3& color) {
    if (!m_settings.enableOcclusionCulling) {
        renderMobileMesh(mesh, modelMatrix, color);
        return;
    }
    
    // Occlusion culling implementation
    // This would check if the mesh is occluded by other objects
    renderMobileMesh(mesh, modelMatrix, color);
}

void MobileRenderer::addUIElement(const MobileUIElement& element) {
    m_uiElements.push_back(element);
}

void MobileRenderer::removeUIElement(int index) {
    if (index >= 0 && index < m_uiElements.size()) {
        m_uiElements.erase(m_uiElements.begin() + index);
    }
}

void MobileRenderer::updateUIElement(int index, const MobileUIElement& element) {
    if (index >= 0 && index < m_uiElements.size()) {
        m_uiElements[index] = element;
    }
}

void MobileRenderer::clearUIElements() {
    m_uiElements.clear();
}

void MobileRenderer::renderVirtualJoystick(const Vector2& center, float radius, const Vector2& position, const Vector3& color) {
    // Render joystick base
    renderMobileCircle(center, radius, Vector3(0.3f, 0.3f, 0.3f), 0.8f);
    
    // Render joystick handle
    renderMobileCircle(position, radius * 0.3f, color, 1.0f);
}

void MobileRenderer::renderVirtualButton(const Vector2& position, const Vector2& size, const std::string& label, const Vector3& color, bool isPressed) {
    Vector3 buttonColor = isPressed ? color * 0.7f : color;
    float scale = isPressed ? 0.9f : 1.0f;
    
    // Render button background
    renderMobileQuad(position, size * scale, buttonColor, 0.8f);
    
    // Render button label
    Vector2 textPos = position + size * 0.5f;
    renderText(label, textPos, 1.0f, Vector3(1, 1, 1));
}

void MobileRenderer::renderVirtualSlider(const Vector2& position, const Vector2& size, float value, const Vector3& color) {
    // Render slider background
    renderMobileQuad(position, size, Vector3(0.2f, 0.2f, 0.2f), 0.8f);
    
    // Render slider fill
    Vector2 fillSize = Vector2(size.x * value, size.y);
    renderMobileQuad(position, fillSize, color, 1.0f);
}

void MobileRenderer::renderHealthBar(const Vector2& position, const Vector2& size, float percentage, const Vector3& fillColor, const Vector3& bgColor) {
    // Render background
    renderMobileQuad(position, size, bgColor, 0.8f);
    
    // Render fill
    Vector2 fillSize = Vector2(size.x * percentage, size.y);
    renderMobileQuad(position, fillSize, fillColor, 1.0f);
}

void MobileRenderer::renderProgressBar(const Vector2& position, const Vector2& size, float progress, const Vector3& fillColor, const Vector3& bgColor) {
    renderHealthBar(position, size, progress, fillColor, bgColor);
}

void MobileRenderer::renderText(const std::string& text, const Vector2& position, float scale, const Vector3& color) {
    // Mobile text rendering implementation
    // This would use a text rendering system optimized for mobile
}

void MobileRenderer::renderText3D(const std::string& text, const Vector3& position, float scale, const Vector3& color) {
    // 3D text rendering implementation
    // This would render text in 3D space
}

void MobileRenderer::renderDebugText(const std::string& text, const Vector2& position, const Vector3& color) {
    // Debug text rendering implementation
    renderText(text, position, 0.8f, color);
}

void MobileRenderer::renderParticles(const std::vector<Particle>& particles) {
    if (!m_particlesEnabled || particles.empty()) return;
    
    // Mobile particle rendering implementation
    // This would use instanced rendering for particles
}

void MobileRenderer::updateParticles(std::vector<Particle>& particles, float deltaTime) {
    // Mobile particle update implementation
    // This would update particle positions, velocities, etc.
}

void MobileRenderer::setupMobileLighting(Shader* shader) {
    if (!shader) return;
    
    // Setup mobile-optimized lighting
    shader->setInt("numLights", std::min((int)lights.size(), m_maxLights));
    
    for (size_t i = 0; i < lights.size() && i < m_maxLights; i++) {
        std::string prefix = "lights[" + std::to_string(i) + "].";
        shader->setVec3(prefix + "position", lights[i].position.x, lights[i].position.y, lights[i].position.z);
        shader->setVec3(prefix + "color", lights[i].color.x, lights[i].color.y, lights[i].color.z);
        shader->setFloat(prefix + "intensity", lights[i].intensity);
        shader->setFloat(prefix + "attenuation", lights[i].attenuation);
    }
}

void MobileRenderer::addMobileLight(const Light& light) {
    if (lights.size() < m_maxLights) {
        addLight(light);
    }
}

void MobileRenderer::removeMobileLight(int index) {
    removeLight(index);
}

void MobileRenderer::clearMobileLights() {
    clearLights();
}

void MobileRenderer::renderShadowMap() {
    // Mobile shadow mapping implementation
    // This would render shadows optimized for mobile
}

void MobileRenderer::setupShadowMapping(Shader* shader) {
    // Mobile shadow mapping setup
    // This would setup shadow mapping for mobile
}

void MobileRenderer::renderPostProcessing() {
    // Mobile post-processing implementation
    // This would apply post-processing effects optimized for mobile
}

void MobileRenderer::setupPostProcessing() {
    // Mobile post-processing setup
    // This would setup post-processing for mobile
}

void MobileRenderer::setTargetFPS(int fps) {
    // Mobile FPS limiting implementation
    // This would limit the frame rate to save battery
}

void MobileRenderer::setVSync(bool enabled) {
    // Mobile VSync implementation
    // This would enable/disable VSync for mobile
}

void MobileRenderer::setTextureQuality(float quality) {
    m_settings.textureQuality = std::max(0.1f, std::min(1.0f, quality));
}

void MobileRenderer::setModelQuality(float quality) {
    m_settings.modelQuality = std::max(0.1f, std::min(1.0f, quality));
}

void MobileRenderer::setLODDistance(float distance) {
    m_lodDistance = std::max(10.0f, distance);
}

void MobileRenderer::setMaxLights(int maxLights) {
    m_maxLights = std::max(1, std::min(8, maxLights));
}

void MobileRenderer::setShadowsEnabled(bool enabled) {
    m_shadowsEnabled = enabled;
    m_settings.enableShadows = enabled;
}

void MobileRenderer::setParticlesEnabled(bool enabled) {
    m_particlesEnabled = enabled;
    m_settings.enableParticles = enabled;
}

void MobileRenderer::setPostProcessingEnabled(bool enabled) {
    m_postProcessingEnabled = enabled;
    m_settings.enablePostProcessing = enabled;
}

void MobileRenderer::setUIScale(float scale) {
    m_uiScale = std::max(0.5f, std::min(2.0f, scale));
}

void MobileRenderer::setSafeArea(const Vector2& safeArea) {
    m_safeArea = safeArea;
}

void MobileRenderer::setUIEnabled(bool enabled) {
    m_uiEnabled = enabled;
}

void MobileRenderer::preloadAssets() {
    // Preload mobile assets
    // This would preload commonly used assets for better performance
}

void MobileRenderer::unloadUnusedAssets() {
    // Unload unused mobile assets
    // This would free memory by unloading unused assets
}

void MobileRenderer::optimizeMemory() {
    // Optimize memory usage for mobile
    // This would optimize memory usage for mobile devices
}

int MobileRenderer::getTextureMemoryUsage() const {
    return m_textureMemory;
}

int MobileRenderer::getVertexMemoryUsage() const {
    return m_vertexMemory;
}

void MobileRenderer::renderDebugInfo() {
    Renderer::renderDebugInfo();
    
    // Mobile-specific debug info
    renderPerformanceOverlay();
    renderMemoryUsage();
}

void MobileRenderer::renderPerformanceOverlay() {
    // Performance overlay rendering
    // This would show FPS, draw calls, etc.
}

void MobileRenderer::renderTouchDebug() {
    // Touch debug rendering
    // This would show touch points and gestures
}

void MobileRenderer::renderMemoryUsage() {
    // Memory usage rendering
    // This would show memory usage information
}

Vector2 MobileRenderer::screenToWorld(const Vector2& screenPos) const {
    // Screen to world conversion
    // This would convert screen coordinates to world coordinates
    return screenPos;
}

Vector2 MobileRenderer::worldToScreen(const Vector3& worldPos) const {
    // World to screen conversion
    // This would convert world coordinates to screen coordinates
    return Vector2(worldPos.x, worldPos.y);
}

bool MobileRenderer::isPointInRect(const Vector2& point, const Vector2& rectPos, const Vector2& rectSize) const {
    return point.x >= rectPos.x && point.x <= rectPos.x + rectSize.x &&
           point.y >= rectPos.y && point.y <= rectPos.y + rectSize.y;
}

Vector2 MobileRenderer::getScreenSize() const {
    return m_screenSize;
}

Vector2 MobileRenderer::getSafeArea() const {
    return m_safeArea;
}

void MobileRenderer::loadMobileShaders() {
    // Load mobile-specific shaders
    m_mobileBasicShader = std::make_unique<Shader>();
    m_mobileBasicShader->loadFromSource(getMobileBasicVertexShader(), getMobileBasicFragmentShader());
    
    m_mobileUIShader = std::make_unique<Shader>();
    m_mobileUIShader->loadFromSource(getMobileUIVertexShader(), getMobileUIFragmentShader());
    
    m_mobileParticleShader = std::make_unique<Shader>();
    m_mobileParticleShader->loadFromSource(getMobileParticleVertexShader(), getMobileParticleFragmentShader());
    
    m_mobileSkyboxShader = std::make_unique<Shader>();
    m_mobileSkyboxShader->loadFromSource(getMobileSkyboxVertexShader(), getMobileSkyboxFragmentShader());
}

void MobileRenderer::reloadMobileShaders() {
    loadMobileShaders();
}

Shader* MobileRenderer::getMobileBasicShader() const {
    return m_mobileBasicShader.get();
}

Shader* MobileRenderer::getMobileUIShader() const {
    return m_mobileUIShader.get();
}

Shader* MobileRenderer::getMobileParticleShader() const {
    return m_mobileParticleShader.get();
}

Shader* MobileRenderer::getMobileSkyboxShader() const {
    return m_mobileSkyboxShader.get();
}

Renderer::Mesh MobileRenderer::createMobileMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    Mesh mesh;
    mesh.vertices = vertices;
    mesh.indices = indices;
    setupMesh(mesh);
    return mesh;
}

Renderer::Mesh MobileRenderer::createMobileQuad(float width, float height) {
    return createPlaneMesh(width, height);
}

Renderer::Mesh MobileRenderer::createMobileCircle(float radius, int segments) {
    return createSphereMesh(radius, segments);
}

Renderer::Mesh MobileRenderer::createMobileButton(float width, float height) {
    return createMobileQuad(width, height);
}

Renderer::Mesh MobileRenderer::createMobileJoystick(float radius, int segments) {
    return createMobileCircle(radius, segments);
}

int MobileRenderer::loadMobileTexture(const std::string& filename) {
    // Mobile texture loading implementation
    // This would load textures optimized for mobile
    return 0;
}

void MobileRenderer::unloadMobileTexture(int textureId) {
    // Mobile texture unloading implementation
    // This would unload textures
}

void MobileRenderer::setMobileTexture(int textureId, int slot) {
    // Mobile texture binding implementation
    // This would bind textures for mobile
}

void MobileRenderer::playMobileSound(const std::string& filename) {
    Platform::PlatformManager::playSound(filename);
}

void MobileRenderer::stopMobileSound(const std::string& filename) {
    // Stop mobile sound implementation
}

void MobileRenderer::setMobileVolume(float volume) {
    Platform::PlatformManager::setVolume(volume);
}

void MobileRenderer::handleTouchInput(const Vector2& position, bool isPressed) {
    // Handle touch input for mobile
    // This would handle touch input for mobile controls
}

void MobileRenderer::handleGestureInput(const Vector2& delta, float scale, float rotation) {
    // Handle gesture input for mobile
    // This would handle gesture input for mobile controls
}

void MobileRenderer::handleOrientationChange(int orientation) {
    // Handle orientation change for mobile
    // This would handle screen orientation changes
}

void MobileRenderer::handleScreenSizeChange(int width, int height) {
    m_screenSize = Vector2(width, height);
    setViewport(0, 0, width, height);
}

void MobileRenderer::handleMemoryWarning() {
    // Handle memory warning for mobile
    // This would free up memory when the system is low on memory
    unloadUnusedAssets();
    optimizeMemory();
}

void MobileRenderer::handleAppStateChange(int state) {
    // Handle app state change for mobile
    // This would handle app state changes (foreground/background)
}

void MobileRenderer::renderMobileSkybox() {
    // Mobile skybox rendering
    renderSkybox();
}

void MobileRenderer::renderMobileEnvironment() {
    // Mobile environment rendering
    renderEnvironment();
}

void MobileRenderer::renderMobileCars() {
    // Mobile car rendering
    renderCars();
}

void MobileRenderer::renderMobileTrack() {
    // Mobile track rendering
    renderTrack();
}

void MobileRenderer::renderMobileParticles() {
    // Mobile particle rendering
    renderParticles();
}

void MobileRenderer::renderMobileTrails() {
    // Mobile trail rendering
    renderTrails();
}

void MobileRenderer::renderMobileHUD() {
    // Mobile HUD rendering
    renderHUD();
}

void MobileRenderer::renderMobileCombatHUD() {
    // Mobile combat HUD rendering
    renderCombatHUD();
}

void MobileRenderer::renderMobileRacingHUD() {
    // Mobile racing HUD rendering
    renderHUD();
}

void MobileRenderer::renderMobileMenuHUD() {
    // Mobile menu HUD rendering
    renderMenu();
}

void MobileRenderer::startPerformanceTimer() {
    // Start performance timer
    // This would start timing for performance measurement
}

void MobileRenderer::endPerformanceTimer() {
    // End performance timer
    // This would end timing for performance measurement
}

float MobileRenderer::getAverageFrameTime() const {
    return m_frameTime;
}

int MobileRenderer::getAverageDrawCalls() const {
    return m_drawCalls;
}

int MobileRenderer::getAverageTriangles() const {
    return m_trianglesRendered;
}

void MobileRenderer::setupMobileRenderState() {
    // Setup mobile render state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    
    // Mobile-specific render state
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void MobileRenderer::setupMobileShaders() {
    loadMobileShaders();
}

void MobileRenderer::setupMobileMeshes() {
    // Setup mobile meshes
    // This would setup commonly used meshes for mobile
}

void MobileRenderer::setupMobileTextures() {
    // Setup mobile textures
    // This would setup commonly used textures for mobile
}

void MobileRenderer::setupMobileLighting() {
    // Setup mobile lighting
    // This would setup lighting optimized for mobile
}

void MobileRenderer::setupMobileShadows() {
    // Setup mobile shadows
    // This would setup shadow mapping for mobile
}

void MobileRenderer::setupMobilePostProcessing() {
    // Setup mobile post-processing
    // This would setup post-processing for mobile
}

void MobileRenderer::renderMobileMesh(const Mesh& mesh, const Matrix4& modelMatrix, const Vector3& color) {
    if (!mesh.isInitialized || !m_mobileBasicShader) return;
    
    m_mobileBasicShader->use();
    setupMatrices(m_mobileBasicShader.get(), modelMatrix);
    m_mobileBasicShader->setVec3("color", color.x, color.y, color.z);
    setupMobileLighting(m_mobileBasicShader.get());
    
    renderMeshInternal(mesh);
    m_drawCalls++;
}

void MobileRenderer::renderMobileUIElement(const MobileUIElement& element) {
    // Render mobile UI element
    // This would render individual UI elements
}

void MobileRenderer::renderMobileText(const std::string& text, const Vector2& position, float scale, const Vector3& color) {
    // Render mobile text
    // This would render text optimized for mobile
}

void MobileRenderer::updateMobilePerformance() {
    // Update mobile performance metrics
    // This would update performance metrics for mobile
}

void MobileRenderer::optimizeMobileRendering() {
    // Optimize mobile rendering
    // This would optimize rendering for mobile
}

void MobileRenderer::cullMobileObjects() {
    // Cull mobile objects
    // This would cull objects not visible on mobile
}

void MobileRenderer::sortMobileObjects() {
    // Sort mobile objects
    // This would sort objects for optimal rendering on mobile
}

std::string MobileRenderer::getMobileBasicVertexShader() {
    return R"(
        #version 300 es
        precision mediump float;
        
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aNormal;
        layout (location = 2) in vec3 aColor;
        layout (location = 3) in vec3 aTexCoord;
        
        out vec3 FragPos;
        out vec3 Normal;
        out vec3 Color;
        out vec3 TexCoord;
        
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        
        void main() {
            FragPos = vec3(model * vec4(aPos, 1.0));
            Normal = mat3(transpose(inverse(model))) * aNormal;
            Color = aColor;
            TexCoord = aTexCoord;
            
            gl_Position = projection * view * vec4(FragPos, 1.0);
        }
    )";
}

std::string MobileRenderer::getMobileBasicFragmentShader() {
    return R"(
        #version 300 es
        precision mediump float;
        
        in vec3 FragPos;
        in vec3 Normal;
        in vec3 Color;
        in vec3 TexCoord;
        
        out vec4 FragColor;
        
        uniform vec3 color;
        uniform vec3 cameraPosition;
        
        struct Light {
            vec3 position;
            vec3 color;
            float intensity;
            float attenuation;
        };
        
        uniform int numLights;
        uniform Light lights[4];
        
        void main() {
            vec3 norm = normalize(Normal);
            vec3 viewDir = normalize(cameraPosition - FragPos);
            
            vec3 result = vec3(0.1, 0.1, 0.1); // Ambient
            
            for (int i = 0; i < numLights; i++) {
                vec3 lightDir = normalize(lights[i].position - FragPos);
                float distance = length(lights[i].position - FragPos);
                float attenuation = 1.0 / (1.0 + lights[i].attenuation * distance * distance);
                
                float diff = max(dot(norm, lightDir), 0.0);
                vec3 diffuse = lights[i].color * lights[i].intensity * diff * attenuation;
                
                result += diffuse;
            }
            
            FragColor = vec4(result * color, 1.0);
        }
    )";
}

std::string MobileRenderer::getMobileUIVertexShader() {
    return R"(
        #version 300 es
        precision mediump float;
        
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoord;
        
        out vec2 TexCoord;
        
        uniform mat4 projection;
        uniform mat4 model;
        
        void main() {
            TexCoord = aTexCoord;
            gl_Position = projection * model * vec4(aPos, 0.0, 1.0);
        }
    )";
}

std::string MobileRenderer::getMobileUIFragmentShader() {
    return R"(
        #version 300 es
        precision mediump float;
        
        in vec2 TexCoord;
        out vec4 FragColor;
        
        uniform vec3 color;
        uniform float opacity;
        uniform sampler2D texture1;
        
        void main() {
            vec4 texColor = texture(texture1, TexCoord);
            FragColor = vec4(color * texColor.rgb, opacity * texColor.a);
        }
    )";
}

std::string MobileRenderer::getMobileParticleVertexShader() {
    return R"(
        #version 300 es
        precision mediump float;
        
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aVelocity;
        layout (location = 2) in float aLife;
        
        out float Life;
        
        uniform mat4 projection;
        uniform mat4 view;
        uniform float time;
        
        void main() {
            Life = aLife;
            vec3 pos = aPos + aVelocity * time;
            gl_Position = projection * view * vec4(pos, 1.0);
        }
    )";
}

std::string MobileRenderer::getMobileParticleFragmentShader() {
    return R"(
        #version 300 es
        precision mediump float;
        
        in float Life;
        out vec4 FragColor;
        
        uniform vec3 color;
        
        void main() {
            float alpha = Life;
            FragColor = vec4(color, alpha);
        }
    )";
}

std::string MobileRenderer::getMobileSkyboxVertexShader() {
    return R"(
        #version 300 es
        precision mediump float;
        
        layout (location = 0) in vec3 aPos;
        
        out vec3 TexCoord;
        
        uniform mat4 view;
        uniform mat4 projection;
        
        void main() {
            TexCoord = aPos;
            vec4 pos = projection * view * vec4(aPos, 1.0);
            gl_Position = pos.xyww;
        }
    )";
}

std::string MobileRenderer::getMobileSkyboxFragmentShader() {
    return R"(
        #version 300 es
        precision mediump float;
        
        in vec3 TexCoord;
        out vec4 FragColor;
        
        void main() {
            FragColor = vec4(0.5, 0.7, 1.0, 1.0); // Simple sky color
        }
    )";
}

bool MobileRenderer::isMobileCapable() {
    return Platform::PlatformManager::getCurrentPlatform() == Platform::PlatformType::Android ||
           Platform::PlatformManager::getCurrentPlatform() == Platform::PlatformType::iOS;
}

void MobileRenderer::logMobileInfo() {
    // Log mobile device information
    // This would log information about the mobile device
}

void MobileRenderer::optimizeForMobile() {
    // Optimize for mobile
    // This would apply mobile-specific optimizations
}

void MobileRenderer::handleMobileErrors() {
    // Handle mobile errors
    // This would handle mobile-specific errors
}