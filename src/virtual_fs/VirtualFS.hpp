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
private:
    struct DataSource {
        Type type;
        std::string physicalPath;
        mz_zip_archive zipArchive; 
    };

    std::vector<DataSource> sources;

public:
    void mount(const std::string& physicalPath, Type type);
    void mount_recursive(const std::string& rootPath);
    std::vector<unsigned char> read_file(const std::string& virtualPath);
    std::string read_file_string(const std::string& virtualPath);
};
