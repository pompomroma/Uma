#version 300 es
precision highp float;

in vec3 vPosition;
in vec3 vNormal;
in vec2 vTexCoord;
in vec3 vWorldPos;

uniform vec3 uLightPos;
uniform vec3 uLightColor;
uniform vec3 uViewPos;
uniform vec3 uObjectColor;
uniform float uSpecularStrength;
uniform float uShininess;

// Racing game specific uniforms
uniform float uSpeed;
uniform float uDashIntensity;
uniform vec3 uDashColor;
uniform float uTime;

out vec4 FragColor;

void main() {
    // Basic Phong lighting
    vec3 ambient = 0.1 * uLightColor;
    
    vec3 norm = normalize(vNormal);
    vec3 lightDir = normalize(uLightPos - vWorldPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor;
    
    vec3 viewDir = normalize(uViewPos - vWorldPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), uShininess);
    vec3 specular = uSpecularStrength * spec * uLightColor;
    
    vec3 baseColor = uObjectColor;
    
    // Add speed-based effects
    float speedEffect = uSpeed / 25.0; // Normalize to dash speed
    vec3 speedGlow = vec3(0.1, 0.3, 1.0) * speedEffect * 0.3;
    
    // Add dash trail effect
    vec3 dashEffect = uDashColor * uDashIntensity;
    
    // Combine lighting with effects
    vec3 result = (ambient + diffuse + specular) * baseColor + speedGlow + dashEffect;
    
    FragColor = vec4(result, 1.0);
}