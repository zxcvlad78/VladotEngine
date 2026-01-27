#pragma once
#include "object/resource/Resource.hpp"
#include "object/resource/shader_resource/ShaderResource.hpp"

class ShaderResource;

class Material : public Resource {
public:
    Material(std::string p_path);
    ~Material();

    static Ref<Material> get_default_material();

    void set_shader(Ref<ShaderResource> p_shader);
    Ref<ShaderResource> get_shader() const { return m_shader; }
    bool load_from_data(const std::vector<unsigned char>& data) { return true; }
    void set_shader_parameter(const std::string &p_param, const auto &p_value);

private:
    Ref<ShaderResource> m_shader;
};
