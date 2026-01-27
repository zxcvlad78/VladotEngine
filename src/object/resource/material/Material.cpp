#include "Material.hpp"
#include "object/resource/ResourceLoader.hpp"

REGISTER_RESOURCE_TYPE(Material)

Material::Material(std::string p_path) : Resource(std::move(p_path)) {}

Material::~Material() {
}

Ref<Material> Material::get_default_material() {
    static Ref<Material> def_mat = [] {
        auto m = create_ref<Material>("default_material");
        m->set_shader(ShaderResource::get_default_shader());
        return m;
    }();
    return def_mat;
}
void Material::set_shader(Ref<ShaderResource> p_shader) {
    if (!p_shader) {
        m_shader = ShaderResource::get_default_shader();
        return;
    }
    m_shader = p_shader;
}