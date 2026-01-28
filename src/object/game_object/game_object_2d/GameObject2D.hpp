//GameObject2D.hpp

#pragma once
#include "object/ref_counted/RefCounted.hpp"
#include "object/game_object/GameObject.hpp"
#include <glm/glm.hpp>
#include <cstdint>
#include <nlohmann/json.hpp>

class Material;

class GameObject2D : public GameObject {
public:
    GameObject2D();
    virtual ~GameObject2D() = default;

    virtual void set_material(Ref<Material> p_material);
    Ref<Material> get_material() const { return m_material; }

    void SetPosition(glm::vec2 p_pos) { m_position = p_pos; }
    glm::vec2 GetPosition() const { return m_position; }

    void SetScale(glm::vec2 p_scale) { m_scale = p_scale; }
    glm::vec2 GetScale() const { return m_scale; }

    void SetRotation(float p_degrees) { m_rotation = p_degrees; }
    float GetRotation() const { return m_rotation; }

    void SetZIndex(int32_t p_z) { m_zIndex = p_z; }
    int32_t GetZIndex() const { return m_zIndex; }

    static void bind_lua(lua_State* L);

    virtual void handle_rpc(const std::string& func_name, const nlohmann::json& args) override;

protected:
    Ref<Material> m_material;

    glm::vec2 m_position{0.0f, 0.0f};
    glm::vec2 m_scale{1.0f, 1.0f};
    float m_rotation{0.0f};
    int32_t m_zIndex{0};
};
