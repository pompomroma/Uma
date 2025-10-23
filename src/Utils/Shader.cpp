#include "Shader.h"
#include "../Platform/PlatformDetect.h"
#include <fstream>
#include <sstream>
#include <iostream>

#if GRAPHICS_OPENGL
#include <GL/glew.h>
#endif

Shader::Shader() : programID(0) {}

Shader::~Shader() {
    cleanup();
}

bool Shader::loadFromFiles(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexSource, fragmentSource;
    
    // Read vertex shader
    std::ifstream vertexFile(vertexPath);
    if (!vertexFile.is_open()) {
        std::cerr << "Failed to open vertex shader file: " << vertexPath << std::endl;
        return false;
    }
    
    std::stringstream vertexStream;
    vertexStream << vertexFile.rdbuf();
    vertexSource = vertexStream.str();
    vertexFile.close();
    
    // Read fragment shader
    std::ifstream fragmentFile(fragmentPath);
    if (!fragmentFile.is_open()) {
        std::cerr << "Failed to open fragment shader file: " << fragmentPath << std::endl;
        return false;
    }
    
    std::stringstream fragmentStream;
    fragmentStream << fragmentFile.rdbuf();
    fragmentSource = fragmentStream.str();
    fragmentFile.close();
    
    return loadFromSource(vertexSource, fragmentSource);
}

bool Shader::loadFromSource(const std::string& vertexSource, const std::string& fragmentSource) {
#if GRAPHICS_OPENGL
    // Create vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    if (!compileShader(vertexShader, vertexSource)) {
        glDeleteShader(vertexShader);
        return false;
    }

    // Create fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    if (!compileShader(fragmentShader, fragmentSource)) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }

    // Create shader program
    programID = glCreateProgram();
    glAttachShader(programID, vertexShader);
    glAttachShader(programID, fragmentShader);

    if (!linkProgram()) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(programID);
        programID = 0;
        return false;
    }

    // Clean up shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return true;
#else
    // Non-OpenGL platforms (e.g., iOS/Metal) use a stub implementation.
    // We pretend shader creation succeeded so higher-level code can run.
    programID = 1; // non-zero indicates "valid" in this stub
    (void)vertexSource;
    (void)fragmentSource;
    return true;
#endif
}

bool Shader::compileShader(unsigned int shader, const std::string& source) {
#if GRAPHICS_OPENGL
    const char* sourceCStr = source.c_str();
    glShaderSource(shader, 1, &sourceCStr, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        std::cerr << "Shader compilation failed: " << infoLog << std::endl;
        return false;
    }

    return true;
#else
    (void)shader;
    (void)source;
    return true;
#endif
}

bool Shader::linkProgram() {
#if GRAPHICS_OPENGL
    glLinkProgram(programID);

    int success;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetProgramInfoLog(programID, 1024, nullptr, infoLog);
        std::cerr << "Shader program linking failed: " << infoLog << std::endl;
        return false;
    }

    return true;
#else
    return true;
#endif
}

void Shader::use() {
#if GRAPHICS_OPENGL
    if (programID != 0) {
        glUseProgram(programID);
    }
#endif
}

void Shader::unuse() {
#if GRAPHICS_OPENGL
    glUseProgram(0);
#endif
}

void Shader::setBool(const std::string& name, bool value) {
#if GRAPHICS_OPENGL
    int location = getUniformLocation(name);
    if (location != -1) {
        glUniform1i(location, value ? 1 : 0);
    }
#else
    (void)name; (void)value;
#endif
}

void Shader::setInt(const std::string& name, int value) {
#if GRAPHICS_OPENGL
    int location = getUniformLocation(name);
    if (location != -1) {
        glUniform1i(location, value);
    }
#else
    (void)name; (void)value;
#endif
}

void Shader::setFloat(const std::string& name, float value) {
#if GRAPHICS_OPENGL
    int location = getUniformLocation(name);
    if (location != -1) {
        glUniform1f(location, value);
    }
#else
    (void)name; (void)value;
#endif
}

void Shader::setVec3(const std::string& name, float x, float y, float z) {
#if GRAPHICS_OPENGL
    int location = getUniformLocation(name);
    if (location != -1) {
        glUniform3f(location, x, y, z);
    }
#else
    (void)name; (void)x; (void)y; (void)z;
#endif
}

void Shader::setVec4(const std::string& name, float x, float y, float z, float w) {
#if GRAPHICS_OPENGL
    int location = getUniformLocation(name);
    if (location != -1) {
        glUniform4f(location, x, y, z, w);
    }
#else
    (void)name; (void)x; (void)y; (void)z; (void)w;
#endif
}

void Shader::setMat4(const std::string& name, const float* matrix) {
#if GRAPHICS_OPENGL
    int location = getUniformLocation(name);
    if (location != -1) {
        glUniformMatrix4fv(location, 1, GL_FALSE, matrix);
    }
#else
    (void)name; (void)matrix;
#endif
}

int Shader::getUniformLocation(const std::string& name) {
#if GRAPHICS_OPENGL
    auto it = uniformLocations.find(name);
    if (it != uniformLocations.end()) {
        return it->second;
    }

    int location = glGetUniformLocation(programID, name.c_str());
    if (location != -1) {
        uniformLocations[name] = location;
    }

    return location;
#else
    (void)name;
    return -1;
#endif
}

void Shader::cleanup() {
#if GRAPHICS_OPENGL
    if (programID != 0) {
        glDeleteProgram(programID);
        programID = 0;
    }
#else
    programID = 0;
#endif
    uniformLocations.clear();
}