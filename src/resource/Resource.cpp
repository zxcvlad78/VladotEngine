#include "resource/Resource.hpp"
#include "lua_binder/LuaBinder.hpp"

std::map<std::string, Resource::ResourceFactory>& Resource::get_factory_registry() {
    static std::map<std::string, ResourceFactory> registry;
    return registry;
}

Resource::Registrar::Registrar(const std::string& type, ResourceFactory factory) {
    get_factory_registry()[type] = std::move(factory);
}

void Resource::bind_to_lua(sol::state& lua) {
    lua.new_usertype<Resource>("Resource",
        "type", sol::property(&Resource::type),
        "id", sol::property(&Resource::id),
        "path", sol::property(&Resource::virtualPath)
    );
}

static LuaBinder::Registrar binder_base_resource([](sol::state& lua) {
    Resource::bind_to_lua(lua);
});
