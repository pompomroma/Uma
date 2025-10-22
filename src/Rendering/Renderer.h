#pragma once
#include "../Math/Vector3.h"
#include "../Math/Matrix4.h"
#include "../Utils/Shader.h"
#include <vector>
#include <memory>

class Renderer {
public:
    struct Vertex {
        Vector3 position;
        Vector3 normal;
        Vector3 color;
        Vector3 texCoord;
    };

    struct Mesh {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        unsigned int VAO, VBO, EBO;
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
    std::unique_ptr<Shader> basicShader;
    std::unique_ptr<Shader> carShader;
    std::unique_ptr<Shader> trackShader;
    std::unique_ptr<Shader> skyboxShader;
    
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
    void renderBillboardText(const Vector3& position, const std::string& text, const Vector3& color);
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
    void loadShaders();
    void reloadShaders();
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