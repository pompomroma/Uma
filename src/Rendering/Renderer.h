#pragma once
#include "../Platform/PlatformDetect.h"
#include "../Math/Vector3.h"
#include "../Math/Matrix4.h"
#include "../Utils/Shader.h"
#include <vector>
#include <memory>

#if PLATFORM_IOS
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#endif

class Renderer {
public:
    struct Vertex {
        Vector3 position;
        Vector3 normal;
        Vector3 color;
        Vector3 texCoord;
        
        Vertex() {}
        Vertex(const Vector3& pos, const Vector3& norm, const Vector3& col, const Vector3& tex)
            : position(pos), normal(norm), color(col), texCoord(tex) {}
    };

    struct Mesh {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
#if GRAPHICS_OPENGL
        unsigned int VAO, VBO, EBO;
#elif GRAPHICS_METAL
        id<MTLBuffer> vertexBuffer;
        id<MTLBuffer> indexBuffer;
#endif
        bool isInitialized;
    };

    struct Light {
        Vector3 position;
        Vector3 color;
        float intensity;
        float attenuation;
    };

    struct RenderState {
        Vector3 clearColor;
        bool depthTest;
        bool cullFace;
        bool wireframe;
        float lineWidth;
    };

private:
#if GRAPHICS_OPENGL
    std::unique_ptr<Shader> basicShader;
    std::unique_ptr<Shader> carShader;
    std::unique_ptr<Shader> trackShader;
    std::unique_ptr<Shader> skyboxShader;
#elif GRAPHICS_METAL
    id<MTLDevice> metalDevice;
    id<MTLCommandQueue> metalCommandQueue;
    id<MTLRenderPipelineState> basicPipelineState;
    id<MTLRenderPipelineState> carPipelineState;
    id<MTLRenderPipelineState> trackPipelineState;
    id<MTLRenderPipelineState> skyboxPipelineState;
    id<MTLDepthStencilState> depthStencilState;
    MTKView* metalView;
#endif
    
    std::vector<Light> lights;
    RenderState renderState;
    
    // Camera matrices
    Matrix4 viewMatrix;
    Matrix4 projectionMatrix;
    Vector3 cameraPosition;
    
    // Rendering settings
    int screenWidth;
    int screenHeight;
    float aspectRatio;
    float fieldOfView;
    float nearPlane;
    float farPlane;
    
    // Performance
    int drawCalls;
    int trianglesRendered;
    float frameTime;

public:
    Renderer();
    ~Renderer();
    
    // Initialization
    bool initialize(int width, int height);
    void shutdown();
    
    // Rendering setup
    void beginFrame();
    void endFrame();
    void clear();
    void setViewport(int x, int y, int width, int height);
    
    // Camera
    void setViewMatrix(const Matrix4& view);
    void setProjectionMatrix(const Matrix4& projection);
    void setCameraPosition(const Vector3& position);
    void updateMatrices();
    
    // Rendering
    void renderMesh(const Mesh& mesh, const Matrix4& modelMatrix, const Vector3& color = Vector3(1.0f, 1.0f, 1.0f));
    void renderCar(const Matrix4& modelMatrix, const Vector3& color = Vector3(1.0f, 0.0f, 0.0f));
    void renderTrack(const std::vector<Vector3>& vertices, const std::vector<unsigned int>& indices);
    void renderSkybox();
    void renderUI();
    
    // Primitive rendering
    void renderCube(const Vector3& position, const Vector3& scale, const Vector3& color);
    void renderSphere(const Vector3& position, float radius, const Vector3& color);
    void renderPlane(const Vector3& position, const Vector3& normal, float size, const Vector3& color);
    void renderLine(const Vector3& start, const Vector3& end, const Vector3& color);
    void renderGrid(int size, float spacing, const Vector3& color);
    
    // Lighting
    void addLight(const Light& light);
    void removeLight(int index);
    void clearLights();
    void setAmbientLight(const Vector3& color, float intensity);
    
    // Render state
    void setClearColor(const Vector3& color);
    void setDepthTest(bool enable);
    void setCullFace(bool enable);
    void setWireframe(bool enable);
    void setLineWidth(float width);
    
    // Shaders
#if GRAPHICS_OPENGL
    void loadShaders();
    void reloadShaders();
#elif GRAPHICS_METAL
    bool loadMetalShaders();
    void setMetalDevice(id<MTLDevice> device);
    void setMetalCommandQueue(id<MTLCommandQueue> commandQueue);
    void setMetalView(MTKView* view);
#endif
    Shader* getBasicShader() const { return basicShader.get(); }
    Shader* getCarShader() const { return carShader.get(); }
    Shader* getTrackShader() const { return trackShader.get(); }
    Shader* getSkyboxShader() const { return skyboxShader.get(); }
    
    // Utility
    Mesh createCubeMesh(float size = 1.0f);
    Mesh createSphereMesh(float radius = 1.0f, int segments = 32);
    Mesh createPlaneMesh(float width = 1.0f, float height = 1.0f);
    Mesh createCylinderMesh(float radius = 1.0f, float height = 1.0f, int segments = 32);
    Mesh createCarMesh();
    Mesh createTrackMesh(const std::vector<Vector3>& vertices, const std::vector<unsigned int>& indices);
    
    // Performance
    int getDrawCalls() const { return drawCalls; }
    int getTrianglesRendered() const { return trianglesRendered; }
    float getFrameTime() const { return frameTime; }
    void resetStats();
    
    // Debug
    void renderDebugInfo();
    void renderBoundingBox(const Vector3& min, const Vector3& max, const Vector3& color);
    
    // Combat/PvP rendering
    void renderSphere(const Vector3& position, float radius, const Vector3& color, float opacity = 1.0f);
    void renderHealthBar(const Vector3& position, float percentage, float width, float height);
    void renderBar(float x, float y, float width, float height, float percentage, const Vector3& fillColor, const Vector3& bgColor);
    void renderQuad(const Vector3& position, float width, float height, const Vector3& color, float opacity = 1.0f);
    void renderText(const std::string& text, float x, float y, float scale, const Vector3& color);
    void renderAbilityIcon(float x, float y, float width, float height, const std::string& key, bool isReady);
    
private:
    void setupMesh(Mesh& mesh);
    void cleanupMesh(Mesh& mesh);
    void renderMeshInternal(const Mesh& mesh);
    void setupLighting(Shader* shader);
    void setupMatrices(Shader* shader, const Matrix4& modelMatrix);
    std::string getVertexShaderSource();
    std::string getFragmentShaderSource();
    std::string getCarVertexShaderSource();
    std::string getCarFragmentShaderSource();
    std::string getTrackVertexShaderSource();
    std::string getTrackFragmentShaderSource();
    std::string getSkyboxVertexShaderSource();
    std::string getSkyboxFragmentShaderSource();
};