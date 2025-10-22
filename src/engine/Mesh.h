#pragma once
#include "Math.h"
#include <vector>

#ifdef __APPLE__
#include <OpenGLES/ES3/gl.h>
#else
#include <GL/gl.h>
#endif

struct Vertex {
    Vec3 position;
    Vec3 normal;
    float texCoords[2];
    
    Vertex(const Vec3& pos, const Vec3& norm, float u, float v) 
        : position(pos), normal(norm) {
        texCoords[0] = u;
        texCoords[1] = v;
    }
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    GLuint VAO, VBO, EBO;
    
    Mesh();
    ~Mesh();
    
    void setupMesh();
    void render() const;
    
    // Static mesh generators for racing game
    static Mesh createCube();
    static Mesh createSphere(int segments = 32);
    static Mesh createPlane(float width = 10.0f, float height = 10.0f);
    static Mesh createCylinder(float radius = 1.0f, float height = 2.0f, int segments = 32);
    
private:
    bool isSetup;
};