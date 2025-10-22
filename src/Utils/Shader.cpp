#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glad/glad.h>

Shader::Shader() : programID(0) {
}

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
}

bool Shader::compileShader(unsigned int shader, const std::string& source) {
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
}

bool Shader::linkProgram() {
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
}

void Shader::use() {
    if (programID != 0) {
        glUseProgram(programID);
    }
}

void Shader::unuse() {
    glUseProgram(0);
}

void Shader::setBool(const std::string& name, bool value) {
    int location = getUniformLocation(name);
    if (location != -1) {
        glUniform1i(location, value ? 1 : 0);
    }
}

void Shader::setInt(const std::string& name, int value) {
    int location = getUniformLocation(name);
    if (location != -1) {
        glUniform1i(location, value);
    }
}

void Shader::setFloat(const std::string& name, float value) {
    int location = getUniformLocation(name);
    if (location != -1) {
        glUniform1f(location, value);
    }
}

void Shader::setVec3(const std::string& name, float x, float y, float z) {
    int location = getUniformLocation(name);
    if (location != -1) {
        glUniform3f(location, x, y, z);
    }
}

void Shader::setVec4(const std::string& name, float x, float y, float z, float w) {
    int location = getUniformLocation(name);
    if (location != -1) {
        glUniform4f(location, x, y, z, w);
    }
}

void Shader::setMat4(const std::string& name, const float* matrix) {
    int location = getUniformLocation(name);
    if (location != -1) {
        glUniformMatrix4fv(location, 1, GL_FALSE, matrix);
    }
}

int Shader::getUniformLocation(const std::string& name) {
    auto it = uniformLocations.find(name);
    if (it != uniformLocations.end()) {
        return it->second;
    }
    
    int location = glGetUniformLocation(programID, name.c_str());
    if (location != -1) {
        uniformLocations[name] = location;
    }
    
    return location;
}

void Shader::cleanup() {
    if (programID != 0) {
        glDeleteProgram(programID);
        programID = 0;
    }
    uniformLocations.clear();
}