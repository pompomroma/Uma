#pragma once
#include "Math.h"
#include <string>
#include <unordered_map>

#ifdef __APPLE__
#include <OpenGLES/ES3/gl.h>
#else
#include <GL/gl.h>
#endif

class Shader {
public:
    GLuint programId;
    std::unordered_map<std::string, GLint> uniformLocations;
    
    Shader() : programId(0) {}
    ~Shader();
    
    bool loadFromStrings(const std::string& vertexSource, const std::string& fragmentSource);
    void use() const;
    
    void setMat4(const std::string& name, const Mat4& matrix);
    void setVec3(const std::string& name, const Vec3& vector);
    void setFloat(const std::string& name, float value);
    void setInt(const std::string& name, int value);
    
private:
    GLint getUniformLocation(const std::string& name);
    GLuint compileShader(const std::string& source, GLenum type);
    bool linkProgram(GLuint vertexShader, GLuint fragmentShader);
};