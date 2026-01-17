#include "ResourceAPI.hpp"

ResourceAPI::ResourceAPI(VirtualFS* fs) : vfs(fs) {}

bool ResourceAPI::is_extension_in_list(const std::string& ext, const std::vector<std::string>& list) const {
    return std::find(list.begin(), list.end(), ext) != list.end();
}

Resource* ResourceAPI::load(const std::string& virtualPath, const std::string& desiredType) {
    if (loadedResources.count(virtualPath)) {
        return loadedResources[virtualPath].get();
    }
        
    auto& registry = Resource::get_factory_registry();
    if (registry.count(desiredType) == 0) {
        std::cerr << "ResourceAPI Error: Unknown type '" << desiredType << "' in registry." << std::endl;
        return nullptr;
    }
        
    std::unique_ptr<Resource> newResource = registry[desiredType](virtualPath);
    std::vector<unsigned char> rawData = vfs->read_file(virtualPath);
    if (rawData.empty()) return nullptr;

    newResource->id = nextResourceID++;
    if (newResource->load_from_data(rawData)) {
        loadedResources[virtualPath] = std::move(newResource);
        return loadedResources[virtualPath].get();
    }
    return nullptr;
}
    
Resource* ResourceAPI::load_auto(const std::string& virtualPath) {
    std::string ext = fs::path(virtualPath).extension().string();
    if (!ext.empty()) { ext = ext.substr(1); std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower); }
        
    if (is_extension_in_list(ext, EX_IMAGE)) return load(virtualPath, "texture");
    if (is_extension_in_list(ext, EX_AUDIO)) return load(virtualPath, "audio");
        
    std::cerr << "ResourceAPI Error: Could not auto-detect type for file: " << virtualPath << std::endl;
    return nullptr;
}
