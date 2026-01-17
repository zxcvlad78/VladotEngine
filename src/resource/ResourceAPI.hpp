#pragma once
#include <string>
#include <map>
#include <memory>
#include <vector>
#include <filesystem>
#include "virtual_fs/VirtualFS.hpp"
#include "resource/Resource.hpp"
#include <algorithm>
#include <cctype>
#include <iostream>

namespace fs = std::filesystem;

class ResourceAPI {
    VirtualFS* vfs;
    std::map<std::string, std::unique_ptr<Resource>> loadedResources;
    unsigned int nextResourceID = 1;

    bool is_extension_in_list(const std::string& ext, const std::vector<std::string>& list) const;
    
public:
    const std::vector<std::string> EX_AUDIO = {"mp3", "wav", "ogg"};
    const std::vector<std::string> EX_IMAGE = {"png", "jpg", "jpeg", "tga"};

    ResourceAPI(VirtualFS* fs);

    Resource* load(const std::string& virtualPath, const std::string& desiredType);
    Resource* load_auto(const std::string& virtualPath);
};
