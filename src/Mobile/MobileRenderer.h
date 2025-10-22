#pragma once

#ifdef MOBILE_PLATFORM

#ifdef ANDROID_PLATFORM
    #include <GLES3/gl3.h>
    #include <EGL/egl.h>
#elif defined(IOS_PLATFORM)
    #include <OpenGLES/ES3/gl.h>
    #include <OpenGLES/ES3/glext.h>
#endif

#include "../Math/Vector3.h"
#include "../Math/Matrix4.h"
#include "../Utils/Shader.h"
#include <vector>
#include <memory>
#include <string>

class MobileRenderer {
public:
    struct MobileVertex {
        Vector3 position;
        Vector3 normal;
        Vector3 color;
        Vector2 texCoord;  // Changed from Vector3 to Vector2 for mobile optimization
        
        MobileVertex() {}
        MobileVertex(const Vector3& pos, const Vector3& norm, const Vector3& col, const Vector2& tex)
            : position(pos), normal(norm), color(col), texCoord(tex) {}
    };

    struct MobileMesh {
        std::vector<MobileVertex> vertices;
        std::vector<unsigned int> indices;
        GLuint VAO, VBO, EBO;
        bool isInitialized;
        
        MobileMesh() : VAO(0), VBO(0), EBO(0), isInitialized(false) {}
    };

    struct MobileLight {
        Vector3 position;
        Vector3 color;
        float intensity;
        float attenuation;
        
        MobileLight() : intensity(1.0f), attenuation(0.1f) {}
        MobileLight(const Vector3& pos, const Vector3& col, float intens = 1.0f, float atten = 0.1f)
            : position(pos), color(col), intensity(intens), attenuation(atten) {}
    };

    struct MobileRenderState {
        Vector3 clearColor;
        bool depthTest;
        bool cullFace;
        bool wireframe;
        float lineWidth;
        
        MobileRenderState() : clearColor(0.2f, 0.3f, 0.3f), depthTest(true), 
                             cullFace(true), wireframe(false), lineWidth(1.0f) {}
    };

    // Performance levels for mobile optimization
    enum class PerformanceLevel {
        Low,      // Minimal effects, low poly models
        Medium,   // Balanced quality/performance
        High,     // High quality for powerful devices
        Auto      // Automatically adjust based on device
    };

private:
    // Shaders optimized for mobile
    std::unique_ptr<Shader> mobileBasicShader;
    std::unique_ptr<Shader> mobileCarShader;
    std::unique_ptr<Shader> mobileTrackShader;
    std::unique_ptr<Shader> mobileUIShader;
    
    // Lighting system (simplified for mobile)
    std::vector<MobileLight> lights;
    Vector3 ambientLight;
    float ambientIntensity;
    
    // Render state
    MobileRenderState renderState;
    
    // Camera matrices
    Matrix4 viewMatrix;
    Matrix4 projectionMatrix;
    Vector3 cameraPosition;
    
    // Screen properties
    int screenWidth;
    int screenHeight;
    float aspectRatio;
    float fieldOfView;
    float nearPlane;
    float farPlane;
    float devicePixelRatio;  // For high-DPI screens
    
    // Performance settings
    PerformanceLevel performanceLevel;
    int maxLights;
    int maxDrawCalls;
    bool useInstancing;
    bool useLOD;  // Level of Detail
    
    // Performance tracking
    int currentDrawCalls;
    int currentTriangles;
    float frameTime;
    float averageFPS;
    
    // Mobile-specific optimizations
    bool useCompressedTextures;
    bool useHalfFloatTextures;
    int maxTextureSize;
    int shadowMapSize;
    
    // Batch rendering for mobile optimization
    struct BatchData {
        std::vector<Matrix4> modelMatrices;
        std::vector<Vector3> colors;
        MobileMesh* mesh;
        int instanceCount;
    };
    std::vector<BatchData> batches;

public:
    MobileRenderer();
    ~MobileRenderer();
    
    // Initialization
    bool initialize(int width, int height, float pixelRatio = 1.0f);
    void shutdown();
    bool isInitialized() const;
    
    // Performance management
    void setPerformanceLevel(PerformanceLevel level);
    PerformanceLevel getPerformanceLevel() const { return performanceLevel; }
    void autoDetectPerformanceLevel();
    void applyPerformanceSettings();
    
    // Screen management
    void setScreenSize(int width, int height, float pixelRatio = 1.0f);
    void setViewport(int x, int y, int width, int height);
    void handleScreenRotation();
    
    // Rendering pipeline
    void beginFrame();
    void endFrame();
    void clear();
    void present();
    
    // Camera
    void setViewMatrix(const Matrix4& view);
    void setProjectionMatrix(const Matrix4& projection);
    void setCameraPosition(const Vector3& position);
    void updateMatrices();
    
    // Mesh rendering
    void renderMesh(const MobileMesh& mesh, const Matrix4& modelMatrix, 
                   const Vector3& color = Vector3(1.0f, 1.0f, 1.0f));
    void renderBatchedMeshes();  // Render all batched meshes at once
    void addToBatch(const MobileMesh& mesh, const Matrix4& modelMatrix, const Vector3& color);
    void clearBatches();
    
    // Primitive rendering (optimized for mobile)
    void renderCar(const Matrix4& modelMatrix, const Vector3& color = Vector3(1.0f, 0.0f, 0.0f));
    void renderTrack(const std::vector<Vector3>& vertices, const std::vector<unsigned int>& indices);
    void renderCube(const Vector3& position, const Vector3& scale, const Vector3& color);
    void renderSphere(const Vector3& position, float radius, const Vector3& color, int segments = 16);
    void renderPlane(const Vector3& position, const Vector3& normal, float size, const Vector3& color);
    void renderLine(const Vector3& start, const Vector3& end, const Vector3& color, float width = 1.0f);
    
    // UI rendering (2D overlay)
    void renderUI();
    void renderQuad2D(float x, float y, float width, float height, const Vector3& color, float alpha = 1.0f);
    void renderText(const std::string& text, float x, float y, float scale, const Vector3& color);
    void renderProgressBar(float x, float y, float width, float height, float progress, 
                          const Vector3& fillColor, const Vector3& bgColor);
    
    // Lighting (simplified for mobile)
    void addLight(const MobileLight& light);
    void removeLight(int index);
    void clearLights();
    void setAmbientLight(const Vector3& color, float intensity);
    void setupLighting(Shader* shader);
    
    // Render state
    void setClearColor(const Vector3& color);
    void setDepthTest(bool enable);
    void setCullFace(bool enable);
    void setWireframe(bool enable);
    void setLineWidth(float width);
    
    // Shader management
    void loadMobileShaders();
    void reloadShaders();
    Shader* getBasicShader() const { return mobileBasicShader.get(); }
    Shader* getCarShader() const { return mobileCarShader.get(); }
    Shader* getTrackShader() const { return mobileTrackShader.get(); }
    Shader* getUIShader() const { return mobileUIShader.get(); }
    
    // Mesh creation (optimized for mobile)
    MobileMesh createCubeMesh(float size = 1.0f);
    MobileMesh createSphereMesh(float radius = 1.0f, int segments = 16);  // Lower default segments
    MobileMesh createPlaneMesh(float width = 1.0f, float height = 1.0f);
    MobileMesh createCarMesh();  // Simplified car model for mobile
    MobileMesh createTrackMesh(const std::vector<Vector3>& vertices, const std::vector<unsigned int>& indices);
    
    // Level of Detail (LOD) system
    MobileMesh createLODMesh(const MobileMesh& highPoly, float reductionFactor);
    void selectLOD(const Vector3& objectPosition, const Vector3& cameraPosition, 
                   MobileMesh& highLOD, MobileMesh& mediumLOD, MobileMesh& lowLOD, MobileMesh*& selectedMesh);
    
    // Performance monitoring
    int getDrawCalls() const { return currentDrawCalls; }
    int getTrianglesRendered() const { return currentTriangles; }
    float getFrameTime() const { return frameTime; }
    float getAverageFPS() const { return averageFPS; }
    void resetStats();
    
    // Debug rendering
    void renderDebugInfo();
    void renderPerformanceInfo();
    void renderBoundingBox(const Vector3& min, const Vector3& max, const Vector3& color);
    
    // Mobile-specific utilities
    bool isExtensionSupported(const std::string& extension);
    void checkGLError(const std::string& operation);
    std::string getGLVersion();
    std::string getGLRenderer();
    void logDeviceInfo();
    
private:
    // Internal mesh management
    void setupMesh(MobileMesh& mesh);
    void cleanupMesh(MobileMesh& mesh);
    void renderMeshInternal(const MobileMesh& mesh);
    
    // Shader source generation for mobile
    std::string getMobileVertexShaderSource();
    std::string getMobileFragmentShaderSource();
    std::string getMobileCarVertexShaderSource();
    std::string getMobileCarFragmentShaderSource();
    std::string getMobileTrackVertexShaderSource();
    std::string getMobileTrackFragmentShaderSource();
    std::string getMobileUIVertexShaderSource();
    std::string getMobileUIFragmentShaderSource();
    
    // Performance optimization helpers
    void optimizeForDevice();
    void updatePerformanceMetrics();
    void adjustQualitySettings();
    
    // Batch rendering helpers
    void flushBatches();
    void sortBatchesByMaterial();
    
    // Memory management
    void freeUnusedResources();
    void compactMemory();
};

#endif // MOBILE_PLATFORM