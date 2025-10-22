#pragma once
#include <string>
#include <unordered_map>

class Shader {
private:
    unsigned int programID;
    std::unordered_map<std::string, int> uniformLocations;
    
    bool compileShader(unsigned int shader, const std::string& source);
    bool linkProgram();
    int getUniformLocation(const std::string& name);

public:
    Shader();
    ~Shader();
    
    bool loadFromFiles(const std::string& vertexPath, const std::string& fragmentPath);
    bool loadFromSource(const std::string& vertexSource, const std::string& fragmentSource);
    void use();
    void unuse();
    
    // Uniform setters
    void setBool(const std::string& name, bool value);
    void setInt(const std::string& name, int value);
    void setFloat(const std::string& name, float value);
    void setVec3(const std::string& name, float x, float y, float z);
    void setVec4(const std::string& name, float x, float y, float z, float w);
    void setMat4(const std::string& name, const float* matrix);
    
    // Utility
    unsigned int getProgramID() const { return programID; }
    bool isValid() const { return programID != 0; }
    void cleanup();
};