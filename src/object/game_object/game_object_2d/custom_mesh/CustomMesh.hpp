#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "object/game_object/game_object_2d/GameObject2D.hpp"
#include "object/resource/shader_resource/ShaderResource.hpp"

class CustomMesh : public GameObject2D {
public:
    CustomMesh();
    ~CustomMesh() override;

    void add_vertex(glm::vec3 position, glm::vec4 color);
    void add_index(uint32_t index);
    void update_buffers();
    void set_draw_mode_points();
    void set_draw_mode_triangles();

    void set_shader(Ref<ShaderResource> p_shader);
    
    void _draw() override;
    std::string get_class_name() const override { return "CustomMesh"; }

private:
    struct Vertex {
        glm::vec3 position;
        glm::vec4 color;
    };

    std::vector<Vertex> m_vertices;
    std::vector<uint32_t> m_indices;
    GLuint m_vao, m_vbo, m_ebo;
    GLenum m_draw_mode = GL_POINTS;
    Ref<ShaderResource> m_shader;
};
