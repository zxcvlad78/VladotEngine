#pragma once
#include "object/resource/Resource.hpp"
#include <vector>
#include <glad/glad.h>


struct Vertex {
    float position[3];
    float normal[3];
    float texCoords[2];
};

class Mesh : public Resource {
public:
    Mesh() = default;
    virtual ~Mesh() override;

private:
    unsigned int VAO = 0, VBO = 0, EBO = 0;
    size_t indexCount = 0;
};