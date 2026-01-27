#include "GameObject2D.hpp"
#include "object/resource/material/Material.hpp"

GameObject2D::GameObject2D() {
    set_material(nullptr);

}

void GameObject2D::set_material(Ref<Material> p_material) {
    if (!p_material) {
        m_material = Material::get_default_material();
        return;
    }
    m_material = p_material;
}