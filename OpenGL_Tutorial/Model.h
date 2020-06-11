#pragma once
#include <vector>
#include <fstream>
#include <iostream>

#ifdef _DEBUG
void _GLGetError(const char* file, int line, const char* call);
#define GLCALL(call) call; _GLGetError(__FILE__, __LINE__, #call)
#else
#define GLCALL(call) call
#endif

#include "libs/glm/glm.hpp"
#include "shader.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

struct Material {
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 emissive;
    float shininess;
};

class Mesh {
public:
    Mesh(std::vector<Vertex>& vertices, uint64 numVertices, std::vector<uint32>& indices, uint64 numIndices, Material material, Shader* shader);

    ~Mesh();

    glm::vec3 getDimension();

    inline void render();

private:

    glm::vec3 calculateDimension(std::vector<Vertex>& vertices);

    VertexBuffer* vertexBuffer;
    IndexBuffer* indexBuffer;
    Shader* shader;
    Material material;
    uint64 numIndices = 0;
    int diffuseLocation;
    int specularLocation;
    int emissiveLocation;
    int shininessLocation;
    int positionLocation;

    glm::vec3 dimension = glm::vec3(0.0f, 0.0f, 0.0f);
};

class Model {
public:
    void init(const char* filename, Shader* shader);

    void render();

    glm::vec3 getDimension();

    ~Model();

private:
    std::vector<Mesh*> meshes;
    glm::vec3 dimension = glm::vec3(0.0f, 0.0f, 0.0f);
};