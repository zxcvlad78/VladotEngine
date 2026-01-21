#include <glad/glad.h>
#include <GLFW/glfw3.h> 
#include <glm/gtc/matrix_transform.hpp>
#include <cstdint>
#include "object/game_object/game_object_2d/sprite_2d/Sprite2D.hpp"
#include "object/resource/ResourceLoader.hpp"
#include "object/game_object/GameObject.hpp"

uint32_t Sprite2D::s_quad_vao = 0;
uint32_t Sprite2D::s_quad_vbo = 0;

Sprite2D::Sprite2D() : GameObject2D() {
    if (!m_shader) {
        set_shader(ResourceLoader::load<ShaderResource>("res/shaders/sprite.glsl"));
    }
}

Sprite2D::~Sprite2D() {}

void Sprite2D::set_texture(Ref<TextureResource> p_texture) {
    m_texture = p_texture;
    if (m_texture) {
        m_position = m_texture->get_size(); 
    }
}

void Sprite2D::set_shader(Ref<ShaderResource> p_shader) {
    if (p_shader) {
        m_shader = p_shader;
    } else {
        m_shader = ResourceLoader::load<ShaderResource>("res/shaders/sprite.glsl");
        std::cerr << "Warning: Cannot set null shader." << std::endl;
    }
}

void Sprite2D::_process(float p_delta) {}

void Sprite2D::_draw() {
    if (!m_texture || !m_shader) return;

    m_shader->use();

    int window_width, window_height;
    glfwGetWindowSize(glfwGetCurrentContext(), &window_width, &window_height);

    glm::mat4 projection = glm::ortho(0.0f, (float)window_width, (float)window_height, 0.0f, -1.0f, 1.0f);
    m_shader->set_uniform("uProjection", projection);

    glm::mat4 model = glm::mat4(1.0f);
    
    model = glm::translate(model, glm::vec3(m_position, 0.0f));
    
    model = glm::rotate(model, glm::radians(m_rotation), glm::vec3(0, 0, 1));

    glm::vec2 size_in_pixels = glm::vec2(m_texture->get_width(), m_texture->get_height());
    model = glm::scale(model, glm::vec3(size_in_pixels * m_scale, 1.0f));

    m_shader->set_uniform("uModel", model);
    
    m_texture->bind(0);
    m_shader->set_uniform("uTexture", 0); 

    if (s_quad_vao == 0) _init_quad_data();

    glBindVertexArray(s_quad_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Sprite2D::_init_quad_data() {
    float vertices[] = { 
        0.0f, 1.0f,  0.0f, 0.0f, // (x, y), (u, v)
        0.0f, 0.0f,  0.0f, 1.0f, // (x, y), (u, v)
        1.0f, 0.0f,  1.0f, 1.0f, // (x, y), (u, v)
        0.0f, 1.0f,  0.0f, 0.0f, // (x, y), (u, v)
        1.0f, 0.0f,  1.0f, 1.0f, // (x, y), (u, v)
        1.0f, 1.0f,  1.0f, 0.0f  // (x, y), (u, v)
    };
    glGenVertexArrays(1, &s_quad_vao);
    glGenBuffers(1, &s_quad_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, s_quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindVertexArray(s_quad_vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}
