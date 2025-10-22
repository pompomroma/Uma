#include "Mesh.h"
#include <cmath>

Mesh::Mesh() : VAO(0), VBO(0), EBO(0), isSetup(false) {}

Mesh::~Mesh() {
    if (isSetup) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
}

void Mesh::setupMesh() {
    if (isSetup) return;
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    
    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    
    // Normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    
    // Texture coordinate attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    
    glBindVertexArray(0);
    isSetup = true;
}

void Mesh::render() const {
    if (!isSetup) return;
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

Mesh Mesh::createCube() {
    Mesh mesh;
    
    // Cube vertices with normals and texture coordinates
    mesh.vertices = {
        // Front face
        Vertex(Vec3(-1, -1,  1), Vec3( 0,  0,  1), 0, 0),
        Vertex(Vec3( 1, -1,  1), Vec3( 0,  0,  1), 1, 0),
        Vertex(Vec3( 1,  1,  1), Vec3( 0,  0,  1), 1, 1),
        Vertex(Vec3(-1,  1,  1), Vec3( 0,  0,  1), 0, 1),
        
        // Back face
        Vertex(Vec3(-1, -1, -1), Vec3( 0,  0, -1), 1, 0),
        Vertex(Vec3(-1,  1, -1), Vec3( 0,  0, -1), 1, 1),
        Vertex(Vec3( 1,  1, -1), Vec3( 0,  0, -1), 0, 1),
        Vertex(Vec3( 1, -1, -1), Vec3( 0,  0, -1), 0, 0),
        
        // Top face
        Vertex(Vec3(-1,  1, -1), Vec3( 0,  1,  0), 0, 1),
        Vertex(Vec3(-1,  1,  1), Vec3( 0,  1,  0), 0, 0),
        Vertex(Vec3( 1,  1,  1), Vec3( 0,  1,  0), 1, 0),
        Vertex(Vec3( 1,  1, -1), Vec3( 0,  1,  0), 1, 1),
        
        // Bottom face
        Vertex(Vec3(-1, -1, -1), Vec3( 0, -1,  0), 1, 1),
        Vertex(Vec3( 1, -1, -1), Vec3( 0, -1,  0), 0, 1),
        Vertex(Vec3( 1, -1,  1), Vec3( 0, -1,  0), 0, 0),
        Vertex(Vec3(-1, -1,  1), Vec3( 0, -1,  0), 1, 0),
        
        // Right face
        Vertex(Vec3( 1, -1, -1), Vec3( 1,  0,  0), 1, 0),
        Vertex(Vec3( 1,  1, -1), Vec3( 1,  0,  0), 1, 1),
        Vertex(Vec3( 1,  1,  1), Vec3( 1,  0,  0), 0, 1),
        Vertex(Vec3( 1, -1,  1), Vec3( 1,  0,  0), 0, 0),
        
        // Left face
        Vertex(Vec3(-1, -1, -1), Vec3(-1,  0,  0), 0, 0),
        Vertex(Vec3(-1, -1,  1), Vec3(-1,  0,  0), 1, 0),
        Vertex(Vec3(-1,  1,  1), Vec3(-1,  0,  0), 1, 1),
        Vertex(Vec3(-1,  1, -1), Vec3(-1,  0,  0), 0, 1)
    };
    
    mesh.indices = {
        0,  1,  2,   2,  3,  0,   // Front
        4,  5,  6,   6,  7,  4,   // Back
        8,  9, 10,  10, 11,  8,   // Top
        12, 13, 14,  14, 15, 12,   // Bottom
        16, 17, 18,  18, 19, 16,   // Right
        20, 21, 22,  22, 23, 20    // Left
    };
    
    return mesh;
}

Mesh Mesh::createSphere(int segments) {
    Mesh mesh;
    
    for (int i = 0; i <= segments; i++) {
        float phi = M_PI * i / segments;
        for (int j = 0; j <= segments; j++) {
            float theta = 2.0f * M_PI * j / segments;
            
            float x = std::sin(phi) * std::cos(theta);
            float y = std::cos(phi);
            float z = std::sin(phi) * std::sin(theta);
            
            Vec3 pos(x, y, z);
            Vec3 normal = pos.normalize();
            float u = (float)j / segments;
            float v = (float)i / segments;
            
            mesh.vertices.push_back(Vertex(pos, normal, u, v));
        }
    }
    
    for (int i = 0; i < segments; i++) {
        for (int j = 0; j < segments; j++) {
            int first = i * (segments + 1) + j;
            int second = first + segments + 1;
            
            mesh.indices.push_back(first);
            mesh.indices.push_back(second);
            mesh.indices.push_back(first + 1);
            
            mesh.indices.push_back(second);
            mesh.indices.push_back(second + 1);
            mesh.indices.push_back(first + 1);
        }
    }
    
    return mesh;
}

Mesh Mesh::createPlane(float width, float height) {
    Mesh mesh;
    
    float halfWidth = width * 0.5f;
    float halfHeight = height * 0.5f;
    
    mesh.vertices = {
        Vertex(Vec3(-halfWidth, 0, -halfHeight), Vec3(0, 1, 0), 0, 0),
        Vertex(Vec3( halfWidth, 0, -halfHeight), Vec3(0, 1, 0), 1, 0),
        Vertex(Vec3( halfWidth, 0,  halfHeight), Vec3(0, 1, 0), 1, 1),
        Vertex(Vec3(-halfWidth, 0,  halfHeight), Vec3(0, 1, 0), 0, 1)
    };
    
    mesh.indices = {
        0, 1, 2,
        2, 3, 0
    };
    
    return mesh;
}

Mesh Mesh::createCylinder(float radius, float height, int segments) {
    Mesh mesh;
    
    float halfHeight = height * 0.5f;
    
    // Center vertices for top and bottom
    mesh.vertices.push_back(Vertex(Vec3(0, halfHeight, 0), Vec3(0, 1, 0), 0.5f, 0.5f));
    mesh.vertices.push_back(Vertex(Vec3(0, -halfHeight, 0), Vec3(0, -1, 0), 0.5f, 0.5f));
    
    // Side vertices
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * M_PI * i / segments;
        float x = radius * std::cos(angle);
        float z = radius * std::sin(angle);
        
        Vec3 normal(x / radius, 0, z / radius);
        float u = (float)i / segments;
        
        // Top ring
        mesh.vertices.push_back(Vertex(Vec3(x, halfHeight, z), normal, u, 1));
        // Bottom ring
        mesh.vertices.push_back(Vertex(Vec3(x, -halfHeight, z), normal, u, 0));
    }
    
    // Top cap indices
    for (int i = 0; i < segments; i++) {
        mesh.indices.push_back(0);
        mesh.indices.push_back(2 + i * 2);
        mesh.indices.push_back(2 + ((i + 1) % segments) * 2);
    }
    
    // Bottom cap indices
    for (int i = 0; i < segments; i++) {
        mesh.indices.push_back(1);
        mesh.indices.push_back(3 + ((i + 1) % segments) * 2);
        mesh.indices.push_back(3 + i * 2);
    }
    
    // Side indices
    for (int i = 0; i < segments; i++) {
        int curr = 2 + i * 2;
        int next = 2 + ((i + 1) % segments) * 2;
        
        // First triangle
        mesh.indices.push_back(curr);
        mesh.indices.push_back(curr + 1);
        mesh.indices.push_back(next);
        
        // Second triangle
        mesh.indices.push_back(next);
        mesh.indices.push_back(curr + 1);
        mesh.indices.push_back(next + 1);
    }
    
    return mesh;
}