#include "Renderer.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <string>

Renderer::Renderer() 
    : screenWidth(1920)
    , screenHeight(1080)
    , aspectRatio(16.0f / 9.0f)
    , fieldOfView(45.0f)
    , nearPlane(0.1f)
    , farPlane(1000.0f)
    , drawCalls(0)
    , trianglesRendered(0)
    , frameTime(0.0f) {
    
    renderState.clearColor = Vector3(0.1f, 0.1f, 0.2f);
    renderState.depthTest = true;
    renderState.cullFace = true;
    renderState.wireframe = false;
    renderState.lineWidth = 1.0f;
    
    viewMatrix = Matrix4::identity();
    projectionMatrix = Matrix4::identity();
    cameraPosition = Vector3::zero();
}

Renderer::~Renderer() {
    shutdown();
}

bool Renderer::initialize(int width, int height) {
    screenWidth = width;
    screenHeight = height;
    aspectRatio = (float)width / (float)height;
    
    // Initialize OpenGL
    if (!gladLoadGL()) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }
    
    // Set OpenGL settings
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    
    // Load shaders
    loadShaders();
    
    // Set up projection matrix
    projectionMatrix = Matrix4::perspective(fieldOfView * M_PI / 180.0f, aspectRatio, nearPlane, farPlane);
    
    // Add default light
    Light defaultLight;
    defaultLight.position = Vector3(0.0f, 10.0f, 0.0f);
    defaultLight.color = Vector3(1.0f, 1.0f, 1.0f);
    defaultLight.intensity = 1.0f;
    defaultLight.attenuation = 0.1f;
    addLight(defaultLight);
    
    return true;
}

void Renderer::shutdown() {
    basicShader.reset();
    carShader.reset();
    trackShader.reset();
    skyboxShader.reset();
    lights.clear();
}

void Renderer::beginFrame() {
    resetStats();
    glClearColor(renderState.clearColor.x, renderState.clearColor.y, renderState.clearColor.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::endFrame() {
    // Swap buffers would be called here
}

void Renderer::clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::setViewport(int x, int y, int width, int height) {
    glViewport(x, y, width, height);
    screenWidth = width;
    screenHeight = height;
    aspectRatio = (float)width / (float)height;
    projectionMatrix = Matrix4::perspective(fieldOfView * M_PI / 180.0f, aspectRatio, nearPlane, farPlane);
}

void Renderer::setViewMatrix(const Matrix4& view) {
    viewMatrix = view;
}

void Renderer::setProjectionMatrix(const Matrix4& projection) {
    projectionMatrix = projection;
}

void Renderer::setCameraPosition(const Vector3& position) {
    cameraPosition = position;
}

void Renderer::updateMatrices() {
    // Matrices are updated when needed
}

void Renderer::renderMesh(const Mesh& mesh, const Matrix4& modelMatrix, const Vector3& color) {
    if (!mesh.isInitialized || !basicShader) return;
    
    basicShader->use();
    setupMatrices(basicShader.get(), modelMatrix);
    basicShader->setVec3("color", color.x, color.y, color.z);
    setupLighting(basicShader.get());
    
    renderMeshInternal(mesh);
    drawCalls++;
}

void Renderer::renderCar(const Matrix4& modelMatrix, const Vector3& color) {
    if (!carShader) return;
    
    carShader->use();
    setupMatrices(carShader.get(), modelMatrix);
    carShader->setVec3("color", color.x, color.y, color.z);
    setupLighting(carShader.get());
    
    // Render car mesh (simplified cube for now)
    Mesh carMesh = createCarMesh();
    renderMeshInternal(carMesh);
    drawCalls++;
}

void Renderer::renderTrack(const std::vector<Vector3>& vertices, const std::vector<unsigned int>& indices) {
    if (!trackShader || vertices.empty()) return;
    
    trackShader->use();
    setupMatrices(trackShader.get(), Matrix4::identity());
    trackShader->setVec3("color", 0.3f, 0.3f, 0.3f);
    setupLighting(trackShader.get());
    
    // Create and render track mesh
    Mesh trackMesh = createTrackMesh(vertices, indices);
    renderMeshInternal(trackMesh);
    drawCalls++;
}

void Renderer::renderSkybox() {
    if (!skyboxShader) return;
    
    skyboxShader->use();
    setupMatrices(skyboxShader.get(), Matrix4::identity());
    
    // Render skybox (simplified for now)
    Mesh skyboxMesh = createSphereMesh(100.0f, 32);
    renderMeshInternal(skyboxMesh);
    drawCalls++;
}

void Renderer::renderUI() {
    // UI rendering would be implemented here
}

void Renderer::renderCube(const Vector3& position, const Vector3& scale, const Vector3& color) {
    Matrix4 modelMatrix = Matrix4::translation(position) * Matrix4::scale(scale);
    Mesh cubeMesh = createCubeMesh();
    renderMesh(cubeMesh, modelMatrix, color);
}

void Renderer::renderSphere(const Vector3& position, float radius, const Vector3& color) {
    Matrix4 modelMatrix = Matrix4::translation(position) * Matrix4::scale(Vector3(radius, radius, radius));
    Mesh sphereMesh = createSphereMesh();
    renderMesh(sphereMesh, modelMatrix, color);
}

void Renderer::renderPlane(const Vector3& position, const Vector3& normal, float size, const Vector3& color) {
    Matrix4 modelMatrix = Matrix4::translation(position);
    Mesh planeMesh = createPlaneMesh(size, size);
    renderMesh(planeMesh, modelMatrix, color);
}

void Renderer::renderLine(const Vector3& start, const Vector3& end, const Vector3& color) {
    // Line rendering implementation
    glLineWidth(renderState.lineWidth);
    // ... line rendering code
}

void Renderer::renderBillboardText(const Vector3& /*position*/, const std::string& /*text*/, const Vector3& /*color*/) {
    // Placeholder: in this minimal engine we don't have text rendering.
}

void Renderer::renderGrid(int size, float spacing, const Vector3& color) {
    // Grid rendering implementation
    glLineWidth(renderState.lineWidth);
    // ... grid rendering code
}

void Renderer::addLight(const Light& light) {
    lights.push_back(light);
}

void Renderer::removeLight(int index) {
    if (index >= 0 && index < lights.size()) {
        lights.erase(lights.begin() + index);
    }
}

void Renderer::clearLights() {
    lights.clear();
}

void Renderer::setAmbientLight(const Vector3& color, float intensity) {
    // Set ambient light
}

void Renderer::setClearColor(const Vector3& color) {
    renderState.clearColor = color;
}

void Renderer::setDepthTest(bool enable) {
    renderState.depthTest = enable;
    if (enable) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
}

void Renderer::setCullFace(bool enable) {
    renderState.cullFace = enable;
    if (enable) {
        glEnable(GL_CULL_FACE);
    } else {
        glDisable(GL_CULL_FACE);
    }
}

void Renderer::setWireframe(bool enable) {
    renderState.wireframe = enable;
    if (enable) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void Renderer::setLineWidth(float width) {
    renderState.lineWidth = width;
}

void Renderer::loadShaders() {
    // Load basic shader
    basicShader = std::make_unique<Shader>();
    basicShader->loadFromSource(getVertexShaderSource(), getFragmentShaderSource());
    
    // Load car shader
    carShader = std::make_unique<Shader>();
    carShader->loadFromSource(getCarVertexShaderSource(), getCarFragmentShaderSource());
    
    // Load track shader
    trackShader = std::make_unique<Shader>();
    trackShader->loadFromSource(getTrackVertexShaderSource(), getTrackFragmentShaderSource());
    
    // Load skybox shader
    skyboxShader = std::make_unique<Shader>();
    skyboxShader->loadFromSource(getSkyboxVertexShaderSource(), getSkyboxFragmentShaderSource());
}

void Renderer::reloadShaders() {
    loadShaders();
}

Renderer::Mesh Renderer::createCubeMesh(float size) {
    Mesh mesh;
    
    float halfSize = size * 0.5f;
    
    // Define cube vertices
    std::vector<Vertex> vertices = {
        // Front face
        {{-halfSize, -halfSize,  halfSize}, {0, 0, 1}, {1, 0, 0}, {0, 0}},
        {{ halfSize, -halfSize,  halfSize}, {0, 0, 1}, {0, 1, 0}, {1, 0}},
        {{ halfSize,  halfSize,  halfSize}, {0, 0, 1}, {0, 0, 1}, {1, 1}},
        {{-halfSize,  halfSize,  halfSize}, {0, 0, 1}, {1, 1, 0}, {0, 1}},
        
        // Back face
        {{-halfSize, -halfSize, -halfSize}, {0, 0, -1}, {1, 0, 1}, {0, 0}},
        {{ halfSize, -halfSize, -halfSize}, {0, 0, -1}, {0, 1, 1}, {1, 0}},
        {{ halfSize,  halfSize, -halfSize}, {0, 0, -1}, {1, 1, 1}, {1, 1}},
        {{-halfSize,  halfSize, -halfSize}, {0, 0, -1}, {0, 0, 0}, {0, 1}},
    };
    
    // Define cube indices
    std::vector<unsigned int> indices = {
        0, 1, 2, 2, 3, 0, // Front
        4, 5, 6, 6, 7, 4, // Back
        0, 1, 5, 5, 4, 0, // Bottom
        2, 3, 7, 7, 6, 2, // Top
        0, 3, 7, 7, 4, 0, // Left
        1, 2, 6, 6, 5, 1  // Right
    };
    
    mesh.vertices = vertices;
    mesh.indices = indices;
    setupMesh(mesh);
    
    return mesh;
}

Renderer::Mesh Renderer::createSphereMesh(float radius, int segments) {
    Mesh mesh;
    
    // Generate sphere vertices
    for (int i = 0; i <= segments; i++) {
        float lat = M_PI * i / segments;
        for (int j = 0; j <= segments; j++) {
            float lon = 2.0f * M_PI * j / segments;
            
            Vertex vertex;
            vertex.position = Vector3(
                radius * std::sin(lat) * std::cos(lon),
                radius * std::cos(lat),
                radius * std::sin(lat) * std::sin(lon)
            );
            vertex.normal = vertex.position.normalized();
            vertex.color = Vector3(1.0f, 1.0f, 1.0f);
            vertex.texCoord = Vector3((float)j / segments, (float)i / segments, 0.0f);
            
            mesh.vertices.push_back(vertex);
        }
    }
    
    // Generate sphere indices
    for (int i = 0; i < segments; i++) {
        for (int j = 0; j < segments; j++) {
            int first = i * (segments + 1) + j;
            int second = first + segments + 1;
            
            mesh.indices.push_back(first);
            mesh.indices.push_back(second);
            mesh.indices.push_back(first + 1);
            
            mesh.indices.push_back(first + 1);
            mesh.indices.push_back(second);
            mesh.indices.push_back(second + 1);
        }
    }
    
    setupMesh(mesh);
    return mesh;
}

Renderer::Mesh Renderer::createPlaneMesh(float width, float height) {
    Mesh mesh;
    
    float halfWidth = width * 0.5f;
    float halfHeight = height * 0.5f;
    
    std::vector<Vertex> vertices = {
        {{-halfWidth, 0, -halfHeight}, {0, 1, 0}, {1, 1, 1}, {0, 0}},
        {{ halfWidth, 0, -halfHeight}, {0, 1, 0}, {1, 1, 1}, {1, 0}},
        {{ halfWidth, 0,  halfHeight}, {0, 1, 0}, {1, 1, 1}, {1, 1}},
        {{-halfWidth, 0,  halfHeight}, {0, 1, 0}, {1, 1, 1}, {0, 1}}
    };
    
    std::vector<unsigned int> indices = {
        0, 1, 2, 2, 3, 0
    };
    
    mesh.vertices = vertices;
    mesh.indices = indices;
    setupMesh(mesh);
    
    return mesh;
}

Renderer::Mesh Renderer::createCylinderMesh(float radius, float height, int segments) {
    Mesh mesh;
    
    // Generate cylinder vertices
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * M_PI * i / segments;
        float x = radius * std::cos(angle);
        float z = radius * std::sin(angle);
        
        // Top circle
        Vertex topVertex;
        topVertex.position = Vector3(x, height * 0.5f, z);
        topVertex.normal = Vector3(0, 1, 0);
        topVertex.color = Vector3(1, 1, 1);
        topVertex.texCoord = Vector3((float)i / segments, 1, 0);
        mesh.vertices.push_back(topVertex);
        
        // Bottom circle
        Vertex bottomVertex;
        bottomVertex.position = Vector3(x, -height * 0.5f, z);
        bottomVertex.normal = Vector3(0, -1, 0);
        bottomVertex.color = Vector3(1, 1, 1);
        bottomVertex.texCoord = Vector3((float)i / segments, 0, 0);
        mesh.vertices.push_back(bottomVertex);
    }
    
    // Generate cylinder indices
    for (int i = 0; i < segments; i++) {
        int top1 = i * 2;
        int top2 = ((i + 1) % segments) * 2;
        int bottom1 = i * 2 + 1;
        int bottom2 = ((i + 1) % segments) * 2 + 1;
        
        // Top face
        mesh.indices.push_back(0);
        mesh.indices.push_back(top2);
        mesh.indices.push_back(top1);
        
        // Bottom face
        mesh.indices.push_back(1);
        mesh.indices.push_back(bottom1);
        mesh.indices.push_back(bottom2);
        
        // Side faces
        mesh.indices.push_back(top1);
        mesh.indices.push_back(top2);
        mesh.indices.push_back(bottom1);
        
        mesh.indices.push_back(bottom1);
        mesh.indices.push_back(top2);
        mesh.indices.push_back(bottom2);
    }
    
    setupMesh(mesh);
    return mesh;
}

Renderer::Mesh Renderer::createCarMesh() {
    // Simple car mesh (elongated cube)
    return createCubeMesh(2.0f);
}

Renderer::Mesh Renderer::createTrackMesh(const std::vector<Vector3>& vertices, const std::vector<unsigned int>& indices) {
    Mesh mesh;
    
    // Convert track vertices to mesh vertices
    for (const Vector3& pos : vertices) {
        Vertex vertex;
        vertex.position = pos;
        vertex.normal = Vector3(0, 1, 0);
        vertex.color = Vector3(0.3f, 0.3f, 0.3f);
        vertex.texCoord = Vector3(0, 0, 0);
        mesh.vertices.push_back(vertex);
    }
    
    mesh.indices = indices;
    setupMesh(mesh);
    
    return mesh;
}

void Renderer::renderDebugInfo() {
    // Debug info rendering
}

void Renderer::renderBoundingBox(const Vector3& min, const Vector3& max, const Vector3& color) {
    // Bounding box rendering
}

void Renderer::setupMesh(Mesh& mesh) {
    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);
    glGenBuffers(1, &mesh.EBO);
    
    glBindVertexArray(mesh.VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), mesh.vertices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    
    // Color attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);
    
    // Texture coordinate attribute
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
    glEnableVertexAttribArray(3);
    
    glBindVertexArray(0);
    mesh.isInitialized = true;
}

void Renderer::cleanupMesh(Mesh& mesh) {
    if (mesh.isInitialized) {
        glDeleteVertexArrays(1, &mesh.VAO);
        glDeleteBuffers(1, &mesh.VBO);
        glDeleteBuffers(1, &mesh.EBO);
        mesh.isInitialized = false;
    }
}

void Renderer::renderMeshInternal(const Mesh& mesh) {
    if (!mesh.isInitialized) return;
    
    glBindVertexArray(mesh.VAO);
    glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
    trianglesRendered += mesh.indices.size() / 3;
}

void Renderer::setupLighting(Shader* shader) {
    if (!shader) return;
    
    shader->setInt("numLights", lights.size());
    for (size_t i = 0; i < lights.size() && i < 8; i++) {
        std::string prefix = "lights[" + std::to_string(i) + "].";
        shader->setVec3(prefix + "position", lights[i].position.x, lights[i].position.y, lights[i].position.z);
        shader->setVec3(prefix + "color", lights[i].color.x, lights[i].color.y, lights[i].color.z);
        shader->setFloat(prefix + "intensity", lights[i].intensity);
        shader->setFloat(prefix + "attenuation", lights[i].attenuation);
    }
}

void Renderer::setupMatrices(Shader* shader, const Matrix4& modelMatrix) {
    if (!shader) return;
    
    shader->setMat4("model", modelMatrix.m);
    shader->setMat4("view", viewMatrix.m);
    shader->setMat4("projection", projectionMatrix.m);
    shader->setVec3("cameraPosition", cameraPosition.x, cameraPosition.y, cameraPosition.z);
}

void Renderer::resetStats() {
    drawCalls = 0;
    trianglesRendered = 0;
}

std::string Renderer::getVertexShaderSource() {
    return R"(
        #version 330 core
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

std::string Renderer::getFragmentShaderSource() {
    return R"(
        #version 330 core
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
        uniform Light lights[8];
        
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

std::string Renderer::getCarVertexShaderSource() {
    return getVertexShaderSource(); // Same as basic for now
}

std::string Renderer::getCarFragmentShaderSource() {
    return getFragmentShaderSource(); // Same as basic for now
}

std::string Renderer::getTrackVertexShaderSource() {
    return getVertexShaderSource(); // Same as basic for now
}

std::string Renderer::getTrackFragmentShaderSource() {
    return getFragmentShaderSource(); // Same as basic for now
}

std::string Renderer::getSkyboxVertexShaderSource() {
    return R"(
        #version 330 core
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

std::string Renderer::getSkyboxFragmentShaderSource() {
    return R"(
        #version 330 core
        in vec3 TexCoord;
        out vec4 FragColor;
        
        void main() {
            FragColor = vec4(0.5, 0.7, 1.0, 1.0); // Simple sky color
        }
    )";
}