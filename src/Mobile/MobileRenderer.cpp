#ifdef MOBILE_PLATFORM

#include "MobileRenderer.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstring>

MobileRenderer::MobileRenderer() 
    : screenWidth(0), screenHeight(0), aspectRatio(1.0f), fieldOfView(45.0f),
      nearPlane(0.1f), farPlane(1000.0f), devicePixelRatio(1.0f),
      performanceLevel(PerformanceLevel::Auto), maxLights(4), maxDrawCalls(1000),
      useInstancing(true), useLOD(true), currentDrawCalls(0), currentTriangles(0),
      frameTime(0.0f), averageFPS(60.0f), useCompressedTextures(true),
      useHalfFloatTextures(true), maxTextureSize(1024), shadowMapSize(512),
      ambientIntensity(0.3f) {
    
    ambientLight = Vector3(0.4f, 0.4f, 0.4f);
}

MobileRenderer::~MobileRenderer() {
    shutdown();
}

bool MobileRenderer::initialize(int width, int height, float pixelRatio) {
    screenWidth = width;
    screenHeight = height;
    devicePixelRatio = pixelRatio;
    aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    
    std::cout << "Initializing Mobile Renderer..." << std::endl;
    std::cout << "Screen: " << width << "x" << height << " (ratio: " << pixelRatio << ")" << std::endl;
    
    // Log device info
    logDeviceInfo();
    
    // Auto-detect performance level
    autoDetectPerformanceLevel();
    
    // Apply performance settings
    applyPerformanceSettings();
    
    // Initialize OpenGL ES state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    
    // Set viewport
    setViewport(0, 0, width, height);
    
    // Load mobile-optimized shaders
    loadMobileShaders();
    
    // Set default clear color
    setClearColor(Vector3(0.2f, 0.3f, 0.3f));
    
    checkGLError("MobileRenderer initialization");
    
    std::cout << "Mobile Renderer initialized successfully!" << std::endl;
    return true;
}

void MobileRenderer::shutdown() {
    // Clear batches
    clearBatches();
    
    // Clean up shaders
    mobileBasicShader.reset();
    mobileCarShader.reset();
    mobileTrackShader.reset();
    mobileUIShader.reset();
    
    // Clear lights
    lights.clear();
    
    std::cout << "Mobile Renderer shutdown complete." << std::endl;
}

bool MobileRenderer::isInitialized() const {
    return mobileBasicShader != nullptr;
}

void MobileRenderer::setPerformanceLevel(PerformanceLevel level) {
    performanceLevel = level;
    applyPerformanceSettings();
}

void MobileRenderer::autoDetectPerformanceLevel() {
    // Get GPU info to determine performance level
    std::string renderer = getGLRenderer();
    std::string version = getGLVersion();
    
    std::cout << "Auto-detecting performance level..." << std::endl;
    std::cout << "GPU: " << renderer << std::endl;
    std::cout << "OpenGL ES: " << version << std::endl;
    
    // Simple heuristic based on common mobile GPUs
    if (renderer.find("Adreno 6") != std::string::npos ||
        renderer.find("Mali-G7") != std::string::npos ||
        renderer.find("Apple A1") != std::string::npos ||
        renderer.find("Apple M") != std::string::npos) {
        performanceLevel = PerformanceLevel::High;
        std::cout << "High performance device detected" << std::endl;
    } else if (renderer.find("Adreno 5") != std::string::npos ||
               renderer.find("Mali-G5") != std::string::npos ||
               renderer.find("Apple A") != std::string::npos) {
        performanceLevel = PerformanceLevel::Medium;
        std::cout << "Medium performance device detected" << std::endl;
    } else {
        performanceLevel = PerformanceLevel::Low;
        std::cout << "Low performance device detected" << std::endl;
    }
}

void MobileRenderer::applyPerformanceSettings() {
    switch (performanceLevel) {
        case PerformanceLevel::Low:
            maxLights = 2;
            maxDrawCalls = 500;
            maxTextureSize = 512;
            shadowMapSize = 256;
            useInstancing = false;
            useLOD = true;
            break;
            
        case PerformanceLevel::Medium:
            maxLights = 4;
            maxDrawCalls = 750;
            maxTextureSize = 1024;
            shadowMapSize = 512;
            useInstancing = true;
            useLOD = true;
            break;
            
        case PerformanceLevel::High:
            maxLights = 6;
            maxDrawCalls = 1000;
            maxTextureSize = 2048;
            shadowMapSize = 1024;
            useInstancing = true;
            useLOD = false;
            break;
            
        case PerformanceLevel::Auto:
            autoDetectPerformanceLevel();
            return;
    }
    
    std::cout << "Performance settings applied: " << std::endl;
    std::cout << "  Max Lights: " << maxLights << std::endl;
    std::cout << "  Max Draw Calls: " << maxDrawCalls << std::endl;
    std::cout << "  Max Texture Size: " << maxTextureSize << std::endl;
    std::cout << "  Instancing: " << (useInstancing ? "enabled" : "disabled") << std::endl;
    std::cout << "  LOD: " << (useLOD ? "enabled" : "disabled") << std::endl;
}

void MobileRenderer::setScreenSize(int width, int height, float pixelRatio) {
    screenWidth = width;
    screenHeight = height;
    devicePixelRatio = pixelRatio;
    aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    
    setViewport(0, 0, width, height);
    
    // Update projection matrix for new aspect ratio
    updateMatrices();
}

void MobileRenderer::setViewport(int x, int y, int width, int height) {
    glViewport(x, y, width, height);
    checkGLError("setViewport");
}

void MobileRenderer::handleScreenRotation() {
    // Swap width and height
    std::swap(screenWidth, screenHeight);
    aspectRatio = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);
    
    setViewport(0, 0, screenWidth, screenHeight);
    updateMatrices();
    
    std::cout << "Screen rotation handled: " << screenWidth << "x" << screenHeight << std::endl;
}

void MobileRenderer::beginFrame() {
    currentDrawCalls = 0;
    currentTriangles = 0;
    
    // Clear batches from previous frame
    clearBatches();
    
    // Update performance metrics
    updatePerformanceMetrics();
}

void MobileRenderer::endFrame() {
    // Render all batched meshes
    renderBatchedMeshes();
    
    // Update average FPS
    static float fpsAccumulator = 0.0f;
    static int frameCount = 0;
    
    fpsAccumulator += 1.0f / (frameTime + 0.001f);  // Avoid division by zero
    frameCount++;
    
    if (frameCount >= 30) {  // Update every 30 frames
        averageFPS = fpsAccumulator / frameCount;
        fpsAccumulator = 0.0f;
        frameCount = 0;
    }
}

void MobileRenderer::clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    checkGLError("clear");
}

void MobileRenderer::present() {
    // Platform-specific present implementation will be in Android/iOS specific files
    // This is just a placeholder for the base implementation
}

void MobileRenderer::setViewMatrix(const Matrix4& view) {
    viewMatrix = view;
}

void MobileRenderer::setProjectionMatrix(const Matrix4& projection) {
    projectionMatrix = projection;
}

void MobileRenderer::setCameraPosition(const Vector3& position) {
    cameraPosition = position;
}

void MobileRenderer::updateMatrices() {
    // Update projection matrix based on current screen size
    float fovRadians = fieldOfView * 3.14159f / 180.0f;
    projectionMatrix = Matrix4::perspective(fovRadians, aspectRatio, nearPlane, farPlane);
}

void MobileRenderer::renderMesh(const MobileMesh& mesh, const Matrix4& modelMatrix, const Vector3& color) {
    if (!mesh.isInitialized || currentDrawCalls >= maxDrawCalls) {
        return;
    }
    
    // For mobile optimization, add to batch instead of immediate rendering
    if (useInstancing) {
        addToBatch(mesh, modelMatrix, color);
    } else {
        // Immediate rendering for low-end devices
        renderMeshInternal(mesh);
        currentDrawCalls++;
        currentTriangles += mesh.indices.size() / 3;
    }
}

void MobileRenderer::addToBatch(const MobileMesh& mesh, const Matrix4& modelMatrix, const Vector3& color) {
    // Find existing batch for this mesh
    for (auto& batch : batches) {
        if (batch.mesh == &mesh) {
            batch.modelMatrices.push_back(modelMatrix);
            batch.colors.push_back(color);
            batch.instanceCount++;
            return;
        }
    }
    
    // Create new batch
    BatchData newBatch;
    newBatch.mesh = const_cast<MobileMesh*>(&mesh);
    newBatch.modelMatrices.push_back(modelMatrix);
    newBatch.colors.push_back(color);
    newBatch.instanceCount = 1;
    batches.push_back(newBatch);
}

void MobileRenderer::renderBatchedMeshes() {
    if (!useInstancing) {
        return;
    }
    
    for (const auto& batch : batches) {
        if (batch.mesh && batch.mesh->isInitialized && batch.instanceCount > 0) {
            // Render all instances of this mesh at once
            // This is a simplified version - full instancing would use instance buffers
            for (int i = 0; i < batch.instanceCount; ++i) {
                renderMeshInternal(*batch.mesh);
                currentDrawCalls++;
                currentTriangles += batch.mesh->indices.size() / 3;
            }
        }
    }
}

void MobileRenderer::clearBatches() {
    batches.clear();
}

void MobileRenderer::renderCar(const Matrix4& modelMatrix, const Vector3& color) {
    static MobileMesh carMesh;
    static bool carMeshCreated = false;
    
    if (!carMeshCreated) {
        carMesh = createCarMesh();
        carMeshCreated = true;
    }
    
    renderMesh(carMesh, modelMatrix, color);
}

void MobileRenderer::renderCube(const Vector3& position, const Vector3& scale, const Vector3& color) {
    static MobileMesh cubeMesh;
    static bool cubeMeshCreated = false;
    
    if (!cubeMeshCreated) {
        cubeMesh = createCubeMesh();
        cubeMeshCreated = true;
    }
    
    Matrix4 modelMatrix = Matrix4::translation(position) * Matrix4::scale(scale);
    renderMesh(cubeMesh, modelMatrix, color);
}

void MobileRenderer::renderSphere(const Vector3& position, float radius, const Vector3& color, int segments) {
    static std::map<int, MobileMesh> sphereMeshes;
    
    if (sphereMeshes.find(segments) == sphereMeshes.end()) {
        sphereMeshes[segments] = createSphereMesh(radius, segments);
    }
    
    Matrix4 modelMatrix = Matrix4::translation(position) * Matrix4::scale(Vector3(radius, radius, radius));
    renderMesh(sphereMeshes[segments], modelMatrix, color);
}

void MobileRenderer::setClearColor(const Vector3& color) {
    renderState.clearColor = color;
    glClearColor(color.x, color.y, color.z, 1.0f);
    checkGLError("setClearColor");
}

void MobileRenderer::setDepthTest(bool enable) {
    renderState.depthTest = enable;
    if (enable) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
    checkGLError("setDepthTest");
}

void MobileRenderer::setCullFace(bool enable) {
    renderState.cullFace = enable;
    if (enable) {
        glEnable(GL_CULL_FACE);
    } else {
        glDisable(GL_CULL_FACE);
    }
    checkGLError("setCullFace");
}

void MobileRenderer::loadMobileShaders() {
    std::cout << "Loading mobile-optimized shaders..." << std::endl;
    
    // Load basic shader
    mobileBasicShader = std::make_unique<Shader>();
    if (!mobileBasicShader->loadFromSource(getMobileVertexShaderSource(), getMobileFragmentShaderSource())) {
        std::cerr << "Failed to load mobile basic shader" << std::endl;
    }
    
    // Load car shader
    mobileCarShader = std::make_unique<Shader>();
    if (!mobileCarShader->loadFromSource(getMobileCarVertexShaderSource(), getMobileCarFragmentShaderSource())) {
        std::cerr << "Failed to load mobile car shader" << std::endl;
    }
    
    // Load track shader
    mobileTrackShader = std::make_unique<Shader>();
    if (!mobileTrackShader->loadFromSource(getMobileTrackVertexShaderSource(), getMobileTrackFragmentShaderSource())) {
        std::cerr << "Failed to load mobile track shader" << std::endl;
    }
    
    // Load UI shader
    mobileUIShader = std::make_unique<Shader>();
    if (!mobileUIShader->loadFromSource(getMobileUIVertexShaderSource(), getMobileUIFragmentShaderSource())) {
        std::cerr << "Failed to load mobile UI shader" << std::endl;
    }
    
    std::cout << "Mobile shaders loaded successfully!" << std::endl;
}

MobileRenderer::MobileMesh MobileRenderer::createCarMesh() {
    MobileMesh mesh;
    
    // Simplified car mesh for mobile (box-like car)
    std::vector<MobileVertex> vertices = {
        // Front face
        {{-1.0f, -0.5f,  1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{ 1.0f, -0.5f,  1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{ 1.0f,  0.5f,  1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
        {{-1.0f,  0.5f,  1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
        
        // Back face
        {{-1.0f, -0.5f, -1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{ 1.0f, -0.5f, -1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{ 1.0f,  0.5f, -1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
        {{-1.0f,  0.5f, -1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}
    };
    
    std::vector<unsigned int> indices = {
        // Front face
        0, 1, 2, 2, 3, 0,
        // Back face
        4, 5, 6, 6, 7, 4,
        // Left face
        7, 3, 0, 0, 4, 7,
        // Right face
        1, 5, 6, 6, 2, 1,
        // Top face
        3, 2, 6, 6, 7, 3,
        // Bottom face
        0, 1, 5, 5, 4, 0
    };
    
    mesh.vertices = vertices;
    mesh.indices = indices;
    setupMesh(mesh);
    
    return mesh;
}

void MobileRenderer::setupMesh(MobileMesh& mesh) {
    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);
    glGenBuffers(1, &mesh.EBO);
    
    glBindVertexArray(mesh.VAO);
    
    // Vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(MobileVertex), 
                 mesh.vertices.data(), GL_STATIC_DRAW);
    
    // Element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int),
                 mesh.indices.data(), GL_STATIC_DRAW);
    
    // Vertex attributes
    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MobileVertex), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MobileVertex), 
                         (void*)offsetof(MobileVertex, normal));
    glEnableVertexAttribArray(1);
    
    // Color
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(MobileVertex), 
                         (void*)offsetof(MobileVertex, color));
    glEnableVertexAttribArray(2);
    
    // Texture coordinates
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(MobileVertex), 
                         (void*)offsetof(MobileVertex, texCoord));
    glEnableVertexAttribArray(3);
    
    glBindVertexArray(0);
    
    mesh.isInitialized = true;
    checkGLError("setupMesh");
}

void MobileRenderer::renderMeshInternal(const MobileMesh& mesh) {
    if (!mesh.isInitialized) return;
    
    glBindVertexArray(mesh.VAO);
    glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
    checkGLError("renderMeshInternal");
}

void MobileRenderer::checkGLError(const std::string& operation) {
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error in " << operation << ": " << error << std::endl;
    }
}

std::string MobileRenderer::getGLVersion() {
    const char* version = (const char*)glGetString(GL_VERSION);
    return version ? std::string(version) : "Unknown";
}

std::string MobileRenderer::getGLRenderer() {
    const char* renderer = (const char*)glGetString(GL_RENDERER);
    return renderer ? std::string(renderer) : "Unknown";
}

void MobileRenderer::logDeviceInfo() {
    std::cout << "=== Mobile Device Info ===" << std::endl;
    std::cout << "OpenGL ES Version: " << getGLVersion() << std::endl;
    std::cout << "Renderer: " << getGLRenderer() << std::endl;
    std::cout << "Vendor: " << (const char*)glGetString(GL_VENDOR) << std::endl;
    
    GLint maxTextureSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    std::cout << "Max Texture Size: " << maxTextureSize << std::endl;
    
    GLint maxVertexAttribs;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs);
    std::cout << "Max Vertex Attributes: " << maxVertexAttribs << std::endl;
    
    std::cout << "=========================" << std::endl;
}

void MobileRenderer::updatePerformanceMetrics() {
    // Simple performance tracking
    static auto lastTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    
    frameTime = std::chrono::duration<float>(currentTime - lastTime).count();
    lastTime = currentTime;
    
    // Adjust quality if performance is poor
    if (averageFPS < 30.0f && performanceLevel != PerformanceLevel::Low) {
        std::cout << "Performance below 30 FPS, reducing quality..." << std::endl;
        adjustQualitySettings();
    }
}

void MobileRenderer::adjustQualitySettings() {
    // Automatically reduce quality settings if performance is poor
    if (performanceLevel == PerformanceLevel::High) {
        performanceLevel = PerformanceLevel::Medium;
    } else if (performanceLevel == PerformanceLevel::Medium) {
        performanceLevel = PerformanceLevel::Low;
    }
    
    applyPerformanceSettings();
}

// Simplified shader sources for mobile
std::string MobileRenderer::getMobileVertexShaderSource() {
    return R"(
        #version 300 es
        precision mediump float;
        
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aNormal;
        layout (location = 2) in vec3 aColor;
        layout (location = 3) in vec2 aTexCoord;
        
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        
        out vec3 FragPos;
        out vec3 Normal;
        out vec3 Color;
        out vec2 TexCoord;
        
        void main() {
            FragPos = vec3(model * vec4(aPos, 1.0));
            Normal = mat3(transpose(inverse(model))) * aNormal;
            Color = aColor;
            TexCoord = aTexCoord;
            
            gl_Position = projection * view * vec4(FragPos, 1.0);
        }
    )";
}

std::string MobileRenderer::getMobileFragmentShaderSource() {
    return R"(
        #version 300 es
        precision mediump float;
        
        in vec3 FragPos;
        in vec3 Normal;
        in vec3 Color;
        in vec2 TexCoord;
        
        out vec4 FragColor;
        
        uniform vec3 lightPos;
        uniform vec3 lightColor;
        uniform vec3 viewPos;
        uniform vec3 ambientLight;
        
        void main() {
            // Ambient
            vec3 ambient = ambientLight * Color;
            
            // Diffuse
            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(lightPos - FragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * lightColor * Color;
            
            // Simple specular
            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 reflectDir = reflect(-lightDir, norm);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
            vec3 specular = spec * lightColor;
            
            vec3 result = ambient + diffuse + specular;
            FragColor = vec4(result, 1.0);
        }
    )";
}

std::string MobileRenderer::getMobileCarVertexShaderSource() {
    return getMobileVertexShaderSource(); // Same as basic for now
}

std::string MobileRenderer::getMobileCarFragmentShaderSource() {
    return getMobileFragmentShaderSource(); // Same as basic for now
}

std::string MobileRenderer::getMobileTrackVertexShaderSource() {
    return getMobileVertexShaderSource(); // Same as basic for now
}

std::string MobileRenderer::getMobileTrackFragmentShaderSource() {
    return getMobileFragmentShaderSource(); // Same as basic for now
}

std::string MobileRenderer::getMobileUIVertexShaderSource() {
    return R"(
        #version 300 es
        precision mediump float;
        
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoord;
        
        uniform mat4 projection;
        
        out vec2 TexCoord;
        
        void main() {
            gl_Position = projection * vec4(aPos, 0.0, 1.0);
            TexCoord = aTexCoord;
        }
    )";
}

std::string MobileRenderer::getMobileUIFragmentShaderSource() {
    return R"(
        #version 300 es
        precision mediump float;
        
        in vec2 TexCoord;
        
        out vec4 FragColor;
        
        uniform vec4 color;
        uniform sampler2D texture1;
        uniform bool useTexture;
        
        void main() {
            if (useTexture) {
                FragColor = texture(texture1, TexCoord) * color;
            } else {
                FragColor = color;
            }
        }
    )";
}

#endif // MOBILE_PLATFORM