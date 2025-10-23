#include "Renderer.h"

#if GRAPHICS_METAL

#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#include <iostream>
#include <cmath>

// Metal-specific implementation
bool Renderer::initialize(int width, int height) {
    screenWidth = width;
    screenHeight = height;
    aspectRatio = (float)width / (float)height;
    
    std::cout << "Initializing Metal renderer..." << std::endl;
    
    // Metal device should be set externally (from GameViewController)
    if (!metalDevice) {
        metalDevice = MTLCreateSystemDefaultDevice();
        if (!metalDevice) {
            std::cerr << "Failed to create Metal device" << std::endl;
            return false;
        }
    }
    
    // Create command queue
    if (!metalCommandQueue) {
        metalCommandQueue = [metalDevice newCommandQueue];
        if (!metalCommandQueue) {
            std::cerr << "Failed to create Metal command queue" << std::endl;
            return false;
        }
    }
    
    // Create depth stencil state
    MTLDepthStencilDescriptor* depthStencilDesc = [[MTLDepthStencilDescriptor alloc] init];
    depthStencilDesc.depthCompareFunction = MTLCompareFunctionLess;
    depthStencilDesc.depthWriteEnabled = YES;
    depthStencilState = [metalDevice newDepthStencilStateWithDescriptor:depthStencilDesc];
    
    // Load shaders and create pipeline states
    if (!loadMetalShaders()) {
        std::cerr << "Failed to load Metal shaders" << std::endl;
        return false;
    }
    
    // Set up projection matrix
    projectionMatrix = Matrix4::perspective(fieldOfView * M_PI / 180.0f, aspectRatio, nearPlane, farPlane);
    
    // Add default light
    Light defaultLight;
    defaultLight.position = Vector3(0.0f, 10.0f, 0.0f);
    defaultLight.color = Vector3(1.0f, 1.0f, 1.0f);
    defaultLight.intensity = 1.0f;
    defaultLight.attenuation = 0.1f;
    addLight(defaultLight);
    
    std::cout << "Metal renderer initialized successfully" << std::endl;
    return true;
}

void Renderer::shutdown() {
    metalDevice = nil;
    metalCommandQueue = nil;
    basicPipelineState = nil;
    carPipelineState = nil;
    trackPipelineState = nil;
    skyboxPipelineState = nil;
    depthStencilState = nil;
    metalView = nil;
    lights.clear();
}

void Renderer::beginFrame() {
    resetStats();
    // Metal frame setup is handled in the MTKViewDelegate
}

void Renderer::endFrame() {
    // Metal frame presentation is handled in the MTKViewDelegate
}

void Renderer::clear() {
    // Metal clearing is handled in the render pass descriptor
}

void Renderer::setViewport(int x, int y, int width, int height) {
    screenWidth = width;
    screenHeight = height;
    aspectRatio = (float)width / (float)height;
    projectionMatrix = Matrix4::perspective(fieldOfView * M_PI / 180.0f, aspectRatio, nearPlane, farPlane);
}

void Renderer::setMetalDevice(id<MTLDevice> device) {
    metalDevice = device;
}

void Renderer::setMetalCommandQueue(id<MTLCommandQueue> commandQueue) {
    metalCommandQueue = commandQueue;
}

void Renderer::setMetalView(MTKView* view) {
    metalView = view;
}

bool Renderer::loadMetalShaders() {
    // Create a simple vertex shader source
    NSString* vertexShaderSource = @R"(
#include <metal_stdlib>
using namespace metal;

struct VertexIn {
    float3 position [[attribute(0)]];
    float3 normal [[attribute(1)]];
    float3 color [[attribute(2)]];
    float2 texCoord [[attribute(3)]];
};

struct VertexOut {
    float4 position [[position]];
    float3 worldPosition;
    float3 normal;
    float3 color;
    float2 texCoord;
};

struct Uniforms {
    float4x4 modelMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
    float3 cameraPosition;
};

vertex VertexOut vertex_main(VertexIn in [[stage_in]],
                            constant Uniforms& uniforms [[buffer(1)]]) {
    VertexOut out;
    
    float4 worldPosition = uniforms.modelMatrix * float4(in.position, 1.0);
    out.worldPosition = worldPosition.xyz;
    out.position = uniforms.projectionMatrix * uniforms.viewMatrix * worldPosition;
    out.normal = normalize((uniforms.modelMatrix * float4(in.normal, 0.0)).xyz);
    out.color = in.color;
    out.texCoord = in.texCoord;
    
    return out;
}

fragment float4 fragment_main(VertexOut in [[stage_in]]) {
    // Simple lighting calculation
    float3 lightDir = normalize(float3(0.0, 1.0, 0.5));
    float3 normal = normalize(in.normal);
    float diffuse = max(dot(normal, lightDir), 0.1);
    
    float3 finalColor = in.color * diffuse;
    return float4(finalColor, 1.0);
}
)";

    NSError* error = nil;
    id<MTLLibrary> library = [metalDevice newLibraryWithSource:vertexShaderSource options:nil error:&error];
    
    if (!library) {
        NSLog(@"Failed to create Metal library: %@", error.localizedDescription);
        return false;
    }
    
    id<MTLFunction> vertexFunction = [library newFunctionWithName:@"vertex_main"];
    id<MTLFunction> fragmentFunction = [library newFunctionWithName:@"fragment_main"];
    
    if (!vertexFunction || !fragmentFunction) {
        NSLog(@"Failed to find Metal shader functions");
        return false;
    }
    
    // Create render pipeline descriptor
    MTLRenderPipelineDescriptor* pipelineDesc = [[MTLRenderPipelineDescriptor alloc] init];
    pipelineDesc.vertexFunction = vertexFunction;
    pipelineDesc.fragmentFunction = fragmentFunction;
    pipelineDesc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
    pipelineDesc.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float;
    
    // Create vertex descriptor
    MTLVertexDescriptor* vertexDesc = [[MTLVertexDescriptor alloc] init];
    vertexDesc.attributes[0].format = MTLVertexFormatFloat3;
    vertexDesc.attributes[0].offset = 0;
    vertexDesc.attributes[0].bufferIndex = 0;
    
    vertexDesc.attributes[1].format = MTLVertexFormatFloat3;
    vertexDesc.attributes[1].offset = 12;
    vertexDesc.attributes[1].bufferIndex = 0;
    
    vertexDesc.attributes[2].format = MTLVertexFormatFloat3;
    vertexDesc.attributes[2].offset = 24;
    vertexDesc.attributes[2].bufferIndex = 0;
    
    vertexDesc.attributes[3].format = MTLVertexFormatFloat2;
    vertexDesc.attributes[3].offset = 36;
    vertexDesc.attributes[3].bufferIndex = 0;
    
    vertexDesc.layouts[0].stride = 44; // sizeof(Vertex)
    vertexDesc.layouts[0].stepRate = 1;
    vertexDesc.layouts[0].stepFunction = MTLVertexStepFunctionPerVertex;
    
    pipelineDesc.vertexDescriptor = vertexDesc;
    
    // Create pipeline state
    basicPipelineState = [metalDevice newRenderPipelineStateWithDescriptor:pipelineDesc error:&error];
    if (!basicPipelineState) {
        NSLog(@"Failed to create Metal pipeline state: %@", error.localizedDescription);
        return false;
    }
    
    // For now, use the same pipeline for all rendering types
    carPipelineState = basicPipelineState;
    trackPipelineState = basicPipelineState;
    skyboxPipelineState = basicPipelineState;
    
    return true;
}

// Stub implementations for Metal-specific rendering
void Renderer::renderMesh(const Mesh& mesh, const Matrix4& modelMatrix, const Vector3& color) {
    // Metal mesh rendering implementation would go here
    // This is a complex implementation that would require Metal command buffers
}

void Renderer::renderCar(const Matrix4& modelMatrix, const Vector3& color) {
    // Create a simple cube mesh for the car
    Mesh carMesh = createCubeMesh(2.0f);
    renderMesh(carMesh, modelMatrix, color);
}

void Renderer::renderTrack(const std::vector<Vector3>& vertices, const std::vector<unsigned int>& indices) {
    // Metal track rendering implementation
}

void Renderer::renderSkybox() {
    // Metal skybox rendering implementation
}

void Renderer::renderCube(const Vector3& position, const Vector3& scale, const Vector3& color) {
    Matrix4 transform = Matrix4::translation(position) * Matrix4::scale(scale);
    Mesh cubeMesh = createCubeMesh();
    renderMesh(cubeMesh, transform, color);
}

void Renderer::renderSphere(const Vector3& position, float radius, const Vector3& color, float opacity) {
    Matrix4 transform = Matrix4::translation(position) * Matrix4::scale(Vector3(radius, radius, radius));
    Mesh sphereMesh = createSphereMesh();
    renderMesh(sphereMesh, transform, color);
}

void Renderer::renderPlane(const Vector3& position, const Vector3& normal, float size, const Vector3& color) {
    Matrix4 transform = Matrix4::translation(position) * Matrix4::scale(Vector3(size, 1.0f, size));
    Mesh planeMesh = createPlaneMesh();
    renderMesh(planeMesh, transform, color);
}

// Placeholder implementations for other rendering functions
void Renderer::renderHealthBar(const Vector3& position, float percentage, float width, float height) {
    // Metal health bar rendering
}

void Renderer::renderBar(float x, float y, float width, float height, float percentage, const Vector3& fillColor, const Vector3& bgColor) {
    // Metal bar rendering
}

void Renderer::renderQuad(const Vector3& position, float width, float height, const Vector3& color, float opacity) {
    // Metal quad rendering
}

void Renderer::renderText(const std::string& text, float x, float y, float scale, const Vector3& color) {
    // Metal text rendering (would need a text rendering system)
}

void Renderer::renderAbilityIcon(float x, float y, float width, float height, const std::string& key, bool isReady) {
    // Metal ability icon rendering
}

void Renderer::renderDebugInfo() {
    // Metal debug info rendering
}

#endif // GRAPHICS_METAL