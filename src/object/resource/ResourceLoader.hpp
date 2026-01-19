#pragma once
#include <map>
#include <string>
#include <memory>
#include <typeindex>
#include <functional>
#include <iostream>
#include <vector>
#include "Resource.hpp"
#include "virtual_fs/VirtualFS.hpp"

template <typename T>
using Ref = std::shared_ptr<T>;

class ResourceLoader {
public:
    using ResourceFactory = std::function<Ref<Resource>(const std::string&)>;

    static void initialize(VirtualFS* p_vfs) { get_vfs_ptr() = p_vfs; }

    template <typename T>
    static void register_resource_type() {
        get_factories()[std::type_index(typeid(T))] = [](const std::string& path) {
            return std::make_shared<T>(path);
        };
    }

    template <typename T>
    static Ref<T> load(const std::string& p_path) {
        auto& cache = get_cache();
        auto it_cache = cache.find(p_path);
        if (it_cache != cache.end()) return std::static_pointer_cast<T>(it_cache->second);

        auto& factories = get_factories();
        auto it = factories.find(std::type_index(typeid(T)));
        if (it == factories.end()) return nullptr;

        VirtualFS* vfs = get_vfs_ptr();
        if (!vfs) return nullptr;

        std::vector<unsigned char> raw_data = vfs->read_file(p_path);
        if (raw_data.empty()) return nullptr;

        Ref<Resource> res = it->second(p_path);
        if (res && res->load_from_data(raw_data)) {
            cache[p_path] = res;
            return std::static_pointer_cast<T>(res);
        }
        return nullptr;
    }

private:
    static VirtualFS*& get_vfs_ptr() { static VirtualFS* v = nullptr; return v; }
    static std::map<std::string, Ref<Resource>>& get_cache() { static std::map<std::string, Ref<Resource>> c; return c; }
    static std::map<std::type_index, ResourceFactory>& get_factories() { static std::map<std::type_index, ResourceFactory> f; return f; }
};
