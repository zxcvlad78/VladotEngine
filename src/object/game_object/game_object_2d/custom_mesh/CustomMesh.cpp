#include <glad/glad.h> 
#include <GLFW/glfw3.h> 
#include "object/game_object/game_object_2d/custom_mesh/CustomMesh.hpp"
#include "object/resource/shader_resource/ShaderResource.hpp"
#include "object/resource/ResourceLoader.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

CustomMesh::CustomMesh() {
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);
    if (m_vao == 0 || m_vbo == 0 || m_ebo == 0) {
        std::cerr << "[CustomMesh] CRITICAL: Failed to generate GL buffers." << std::endl;
    }
    if (!m_shader) set_shader(ResourceLoader::load<ShaderResource>("res/shaders/basic.glsl"));
}

CustomMesh::~CustomMesh() {
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
    if (m_vao == 0 || m_vbo == 0 || m_ebo == 0) {
        std::cerr << "[CustomMesh] CRITICAL: Failed to delete GL buffers." << std::endl;
    }
}
void CustomMesh::add_vertex(glm::vec3 position, glm::vec4 color) {
    m_vertices.push_back({position, color});
}
void CustomMesh::add_index(uint32_t index) {
    m_indices.push_back(index);
}
void CustomMesh::set_draw_mode_points() { m_draw_mode = GL_POINTS; }
void CustomMesh::set_draw_mode_lines() { m_draw_mode = GL_LINES; }
void CustomMesh::set_draw_mode_triangles() { m_draw_mode = GL_TRIANGLES; }
void CustomMesh::set_shader(Ref<ShaderResource> p_shader) { m_shader = p_shader; }

void CustomMesh::update_buffers() {
    if (m_vertices.empty()) {
        std::cerr << "[CustomMesh] Warning: Updating buffers with empty vertex data." << std::endl;
        return;
    }
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), m_vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(uint32_t), m_indices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glBindVertexArray(0);
}

void CustomMesh::_draw() { 
    if (!m_shader || m_vertices.empty()) return;
    m_shader->use();

    int window_width, window_height;
    glfwGetWindowSize(glfwGetCurrentContext(), &window_width, &window_height);
    glm::mat4 projection = glm::ortho(0.0f, (float)window_width, (float)window_height, 0.0f, -1.0f, 1.0f);
    m_shader->set_uniform("uProjection", projection);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(m_position, 0.0f));
    model = glm::rotate(model, glm::radians(m_rotation), glm::vec3(0, 0, 1));
    model = glm::scale(model, glm::vec3(m_scale, 1.0f));
    m_shader->set_uniform("uModel", model);

    glBindVertexArray(m_vao);
    if (!m_indices.empty()) {
        glDrawElements(m_draw_mode, m_indices.size(), GL_UNSIGNED_INT, 0);
    } else {
        glDrawArrays(m_draw_mode, 0, m_vertices.size());
    }
    glBindVertexArray(0);
}
