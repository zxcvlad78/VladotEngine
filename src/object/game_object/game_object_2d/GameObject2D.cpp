#include "GameObject2D.hpp"
#include "object/resource/material/Material.hpp"
#include <iostream>

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

void GameObject2D::handle_rpc(const std::string& func_name, const nlohmann::json& args) {
    if (func_name == "SetPosition" && args.contains("x") && args.contains("y")) {
        SetPosition({args["x"].get<float>(), args["y"].get<float>()});
    }
    else if (func_name == "SetRotation" && args.contains("degrees")) {
        SetRotation(args["degrees"].get<float>());
    }
    else if (func_name == "SetScale" && args.contains("x") && args.contains("y")) {
        SetScale({args["x"].get<float>(), args["y"].get<float>()});
    }
    else if (func_name == "SetZIndex" && args.contains("z")) {
        SetZIndex(args["z"].get<int32_t>());
    }
    else {
        std::cout << "[GameObject2D] Unknown RPC: " << func_name << std::endl;
        for (auto& [key, value] : args.items()) {
            std::cout << "  Arg: " << key << " = " << value << std::endl;
        }
    }
}