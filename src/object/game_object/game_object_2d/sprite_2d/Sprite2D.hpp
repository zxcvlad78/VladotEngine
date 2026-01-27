//Sprite2D.hpp

#pragma once
#include <cstdint>

#include "object/game_object/game_object_2d/GameObject2D.hpp" 
#include "object/resource/texture_resource/TextureResource.hpp"

class Sprite2D : public GameObject2D {
public:
    Sprite2D();
    virtual ~Sprite2D() override;

    virtual void set_material(Ref<Material> p_material) override;
    void set_texture(Ref<TextureResource> p_texture);

    Ref<TextureResource> get_texture() const { return m_texture; }

    virtual void _process(float p_delta) override;
    virtual void _draw() override;
    virtual std::string get_class_name() const override { return "Sprite2D"; }
    static void bind_lua(lua_State* L);

private:
    Ref<TextureResource> m_texture;

    static uint32_t s_quad_vao;
    static uint32_t s_quad_vbo;
    static void _init_quad_data();
};
