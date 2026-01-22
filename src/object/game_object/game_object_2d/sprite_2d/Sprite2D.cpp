// Sprite2D.cpp
#include "object/game_object/game_object_2d/sprite_2d/Sprite2D.hpp"
#include "object/resource/ResourceLoader.hpp"
#include "object/scene_tree/SceneTree.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

uint32_t Sprite2D::s_quad_vao = 0;
uint32_t Sprite2D::s_quad_vbo = 0;

Sprite2D::Sprite2D() : GameObject2D() {
    m_shader = ResourceLoader::load<ShaderResource>("res/shaders/sprite.glsl");
}

Sprite2D::~Sprite2D() {
}

void Sprite2D::set_texture(Ref<TextureResource> p_texture) {
    m_texture = p_texture;
}

void Sprite2D::set_shader(Ref<ShaderResource> p_shader) {
    if (p_shader) {
        m_shader = p_shader;
    }
}

void Sprite2D::_process(float p_delta) {
}

void Sprite2D::_draw() {
    if (!m_texture || !m_shader) return;

    m_shader->use();

    m_shader->set_uniform("uProjection", SceneTree::current_projection);

    glm::mat4 model = glm::mat4(1.0f);
    
    model = glm::translate(model, glm::vec3(m_position, 0.0f));
    
    model = glm::rotate(model, glm::radians(m_rotation), glm::vec3(0.0f, 0.0f, 1.0f));

    glm::vec2 size_in_pixels = m_texture->get_size();
    model = glm::scale(model, glm::vec3(size_in_pixels * m_scale, 1.0f));

    m_shader->set_uniform("uModel", model);
    
    m_texture->bind(0);
    m_shader->set_uniform("uTexture", 0); 

    if (s_quad_vao == 0) {
        _init_quad_data();
    }

    glBindVertexArray(s_quad_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Sprite2D::_init_quad_data() {
    float vertices[] = { 
        // pos      // tex
        0.0f, 1.0f,  0.0f, 1.0f,
        0.0f, 0.0f,  0.0f, 0.0f,
        1.0f, 0.0f,  1.0f, 0.0f,

        0.0f, 1.0f,  0.0f, 1.0f,
        1.0f, 0.0f,  1.0f, 0.0f,
        1.0f, 1.0f,  1.0f, 1.0f 
    };

    glGenVertexArrays(1, &s_quad_vao);
    glGenBuffers(1, &s_quad_vbo);

    glBindVertexArray(s_quad_vao);
    glBindBuffer(GL_ARRAY_BUFFER, s_quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
}
