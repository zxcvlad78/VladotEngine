#pragma once
#include "object/game_object/game_object_2d/GameObject2D.hpp" 
#include "object/resource/texture_resource/TextureResource.hpp"
#include "object/resource/shader_resource/ShaderResource.hpp"
#include <glad/glad.h>
#include <cstdint>

class Sprite2D : public GameObject2D {
public:
    Sprite2D();
    virtual ~Sprite2D() override;

    void set_texture(Ref<TextureResource> p_texture);
    void set_shader(Ref<ShaderResource> p_shader);

    // Эти методы должны быть объявлены в GameObject.hpp как виртуальные
    virtual void _process(float p_delta) override;
    virtual void _draw() override;
    virtual std::string get_class_name() const override { return "Sprite2D"; }

private:
    Ref<TextureResource> m_texture;
    Ref<ShaderResource> m_shader;

    static uint32_t s_quad_vao;
    static uint32_t s_quad_vbo;
    static void _init_quad_data();
};
