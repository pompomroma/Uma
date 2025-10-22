#!/usr/bin/env python3
import re

with open('src/Rendering/Renderer.cpp', 'r') as f:
    content = f.read()

# Fix the glad include
content = content.replace('#include <glad/glad.h>', '#include <GL/glew.h>')
content = content.replace('if (!gladLoadGL())', 'GLenum err = glewInit();\n    if (err != GLEW_OK)')
content = content.replace('std::cerr << "Failed to initialize GLAD"', 'std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(err)')

# Fix setMat4 calls with Matrix4 that need data()
content = re.sub(r'basicShader->setMat4\("projection", orthoProjection\)', 
                r'basicShader->setMat4("projection", orthoProjection.data())', content)
content = re.sub(r'basicShader->setMat4\("view", Matrix4::identity\(\)\)', 
                r'basicShader->setMat4("view", Matrix4::identity().data())', content)
content = re.sub(r'basicShader->setMat4\("model", bgTransform\)', 
                r'basicShader->setMat4("model", bgTransform.data())', content)
content = re.sub(r'basicShader->setMat4\("model", fillTransform\)', 
                r'basicShader->setMat4("model", fillTransform.data())', content)

with open('src/Rendering/Renderer.cpp', 'w') as f:
    f.write(content)