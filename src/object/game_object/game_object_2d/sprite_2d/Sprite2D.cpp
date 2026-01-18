#include "object/game_object/game_object_2d/sprite_2d/Sprite2D.hpp"
#include "object/resource/ResourceLoader.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <cstdint>
#include "object/game_object/GameObject.hpp"
#include "object/game_object/game_object_2d/GameObject2D.hpp"
#include "object/Object.hpp"

// Инициализация статики
uint32_t Sprite2D::s_quad_vao = 0;
uint32_t Sprite2D::s_quad_vbo = 0;

Sprite2D::Sprite2D() : GameObject2D() {}

Sprite2D::~Sprite2D() {}

void Sprite2D::set_texture(Ref<TextureResource> p_texture) {
    m_texture = p_texture;
    if (m_texture) {
        // Используем m_position из GameObject2D (согласно вашей последней структуре)
        m_position = m_texture->get_size(); 
    }
}

void Sprite2D::set_shader(Ref<ShaderResource> p_shader) {
    m_shader = p_shader;
}

void Sprite2D::_process(float p_delta) {}

void Sprite2D::_draw() {
    if (!m_texture || !m_shader) return;

    m_shader->use();
    
    // Делаем (0,0) верхним левым углом окна 800x600
    glm::mat4 projection = glm::ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);
    m_shader->set_uniform("uProjection", projection);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(m_position, 0.0f));
    model = glm::rotate(model, glm::radians(m_rotation), glm::vec3(0, 0, 1));
    
    // Авто-масштаб под размер текстуры, если масштаб в коде не задан
    glm::vec2 final_scale = m_scale;
    if (final_scale == glm::vec2(1.0f) && m_texture) {
        final_scale = m_texture->get_size();
    }
    model = glm::scale(model, glm::vec3(final_scale, 1.0f));

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
        -0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  1.0f, 1.0f
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

REGISTER_LUA_TYPE(Sprite2D, ([](sol::state& lua) {
    // Явно указываем shared_ptr в качестве типа владения
    lua.new_usertype<Sprite2D>("Sprite2D",
        sol::call_constructor, sol::factories([]() { return std::make_shared<Sprite2D>(); }),
        "new", sol::factories([]() { return std::make_shared<Sprite2D>(); }),
        
        // КРИТИЧНО: Указываем полную цепочку наследования
        sol::base_classes, sol::bases<GameObject2D, GameObject, Object>(),

        "set_texture", [](Sprite2D& self, const std::string& path) {
            auto tex = ResourceLoader::load<TextureResource>(path);
            if (tex) self.set_texture(tex);
        },
        "set_shader", [](Sprite2D& self, const std::string& path) {
            auto shd = ResourceLoader::load<ShaderResource>(path);
            if (shd) self.set_shader(shd);
        }
    );
}))