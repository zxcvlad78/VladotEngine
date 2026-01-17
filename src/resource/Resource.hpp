#pragma once
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <iostream>
#include <sol/sol.hpp>

class Resource {
public:
    const std::string type;
    const std::string virtualPath;
    unsigned int id; 

    using ResourceFactory = std::function<std::unique_ptr<Resource>(const std::string& path)>;
    static std::map<std::string, ResourceFactory>& get_factory_registry();

    struct Registrar {
        Registrar(const std::string& type, ResourceFactory factory);
    };

    Resource(std::string t, std::string path) : type(std::move(t)), virtualPath(std::move(path)), id(0) {}
    virtual ~Resource() = default;
    virtual bool load_from_data(const std::vector<unsigned char>& data) = 0;

    static void bind_to_lua(sol::state& lua);
};