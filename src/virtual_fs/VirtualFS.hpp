#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <iostream>
#include "miniz.h" 


class VirtualFS {
public:
    enum Type { FOLDER, ZIP };
    struct DataSource {
        Type type;
        std::string physicalPath;
        mz_zip_archive zipArchive; 
    };

        std::vector<DataSource> sources;
        std::vector<DataSource> get_sources() const { return sources; }

        std::string get_absolute_path(const std::string& virtualPath);
        std::string normalize_path(std::string path);
        std::vector<std::string> list_directory(const std::string& virtualPath);
        void mount(const std::string& physicalPath, Type type);
        void mount_recursive(const std::string& rootPath);
        std::vector<unsigned char> read_file(const std::string& virtualPath);
        std::string read_file_string(const std::string& virtualPath);
};
