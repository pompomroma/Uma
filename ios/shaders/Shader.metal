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

// Basic vertex shader
vertex VertexOut vertex_main(VertexIn in [[stage_in]],
                           constant Uniforms &uniforms [[buffer(0)]]) {
    VertexOut out;
    
    // Transform position
    float4 worldPos = uniforms.modelMatrix * float4(in.position, 1.0);
    out.position = uniforms.projectionMatrix * uniforms.viewMatrix * worldPos;
    
    // Transform normal
    out.normal = normalize((uniforms.modelMatrix * float4(in.normal, 0.0)).xyz);
    
    // Pass through color and texture coordinates
    out.color = in.color;
    out.texCoord = in.texCoord;
    
    return out;
}

// Basic fragment shader
fragment float4 fragment_main(VertexOut in [[stage_in]]) {
    // Simple lighting calculation
    float3 lightDir = normalize(float3(1.0, 1.0, 1.0));
    float NdotL = max(dot(normalize(in.normal), lightDir), 0.0);
    
    // Combine base color with lighting
    float3 finalColor = in.color * (0.3 + 0.7 * NdotL);
    
    return float4(finalColor, 1.0);
}

// Car-specific vertex shader
vertex VertexOut car_vertex_main(VertexIn in [[stage_in]],
                                constant Uniforms &uniforms [[buffer(0)]]) {
    VertexOut out;
    
    // Transform position
    float4 worldPos = uniforms.modelMatrix * float4(in.position, 1.0);
    out.position = uniforms.projectionMatrix * uniforms.viewMatrix * worldPos;
    
    // Transform normal
    out.normal = normalize((uniforms.modelMatrix * float4(in.normal, 0.0)).xyz);
    
    // Pass through color and texture coordinates
    out.color = in.color;
    out.texCoord = in.texCoord;
    
    return out;
}

// Car-specific fragment shader with metallic look
fragment float4 car_fragment_main(VertexOut in [[stage_in]]) {
    // Metallic car material
    float3 lightDir = normalize(float3(1.0, 1.0, 1.0));
    float3 viewDir = normalize(in.position.xyz - in.position.xyz); // Simplified
    float3 normal = normalize(in.normal);
    
    // Basic PBR-like calculation
    float NdotL = max(dot(normal, lightDir), 0.0);
    float3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    
    // Metallic car color
    float3 baseColor = in.color;
    float3 metallic = mix(baseColor, float3(0.8, 0.8, 0.9), 0.7);
    float3 finalColor = metallic * (0.2 + 0.8 * NdotL) + spec * 0.5;
    
    return float4(finalColor, 1.0);
}

// Track vertex shader
vertex VertexOut track_vertex_main(VertexIn in [[stage_in]],
                                  constant Uniforms &uniforms [[buffer(0)]]) {
    VertexOut out;
    
    // Transform position
    float4 worldPos = uniforms.modelMatrix * float4(in.position, 1.0);
    out.position = uniforms.projectionMatrix * uniforms.viewMatrix * worldPos;
    
    // Transform normal
    out.normal = normalize((uniforms.modelMatrix * float4(in.normal, 0.0)).xyz);
    
    // Pass through color and texture coordinates
    out.color = in.color;
    out.texCoord = in.texCoord;
    
    return out;
}

// Track fragment shader
fragment float4 track_fragment_main(VertexOut in [[stage_in]]) {
    // Simple track material
    float3 lightDir = normalize(float3(1.0, 1.0, 1.0));
    float NdotL = max(dot(normalize(in.normal), lightDir), 0.0);
    
    // Track color with some variation
    float3 trackColor = in.color * (0.4 + 0.6 * NdotL);
    
    return float4(trackColor, 1.0);
}

// Skybox vertex shader
vertex VertexOut skybox_vertex_main(VertexIn in [[stage_in]],
                                   constant Uniforms &uniforms [[buffer(0)]]) {
    VertexOut out;
    
    // Skybox uses only projection matrix (no view/model)
    out.position = uniforms.projectionMatrix * float4(in.position, 1.0);
    out.position.z = out.position.w; // Ensure skybox is at far plane
    
    out.normal = in.normal;
    out.color = in.color;
    out.texCoord = in.texCoord;
    
    return out;
}

// Skybox fragment shader
fragment float4 skybox_fragment_main(VertexOut in [[stage_in]]) {
    // Simple sky gradient
    float3 skyColor = mix(float3(0.5, 0.7, 1.0), float3(0.8, 0.9, 1.0), in.texCoord.y);
    return float4(skyColor, 1.0);
}