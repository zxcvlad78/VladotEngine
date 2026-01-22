#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <algorithm>
#include <cstring>
#include "miniz.h" 

class VirtualFS {
public:
    enum Type { FOLDER, ZIP };

    struct DataSource {
        Type type;
        std::string physicalPath;
        mz_zip_archive zipArchive;

        DataSource() { std::memset(&zipArchive, 0, sizeof(zipArchive)); }
        ~DataSource() {
            if (type == ZIP && zipArchive.m_archive_size > 0) {
                mz_zip_reader_end(&zipArchive);
            }
        }
    };

    VirtualFS() = default;
    ~VirtualFS() = default;

    void mount(const std::string& physicalPath, Type type) {
        auto ds = std::make_unique<DataSource>();
        ds->type = type;
        ds->physicalPath = physicalPath;

        if (type == ZIP) {
            if (!mz_zip_reader_init_file(&ds->zipArchive, physicalPath.c_str(), 0)) {
                std::cerr << "[VFS] Failed to init ZIP: " << physicalPath << std::endl;
                return;
            }
        }
        sources.push_back(std::move(ds));
        std::cout << "[VFS] Mounted: " << physicalPath << std::endl;
    }

    std::vector<unsigned char> read_file(const std::string& virtualPath) {
        std::string normPath = normalize_path(virtualPath);
        for (int i = (int)sources.size() - 1; i >= 0; --i) {
            auto& source = *sources[i];
            if (source.type == FOLDER) {
                std::filesystem::path fullPath = std::filesystem::path(source.physicalPath) / normPath;
                if (std::filesystem::exists(fullPath)) {
                    std::ifstream file(fullPath, std::ios::binary | std::ios::ate);
                    if (file.is_open()) {
                        size_t size = file.tellg();
                        file.seekg(0, std::ios::beg);
                        std::vector<unsigned char> buffer(size);
                        file.read((char*)buffer.data(), size);
                        return buffer;
                    }
                }
            } else {
                size_t size;
                void* p = mz_zip_reader_extract_file_to_heap(&source.zipArchive, normPath.c_str(), &size, 0);
                if (p) {
                    std::vector<unsigned char> data((unsigned char*)p, (unsigned char*)p + size);
                    mz_free(p);
                    return data;
                }
            }
        }
        return {};
    }

    std::string normalize_path(std::string path) {
        std::replace(path.begin(), path.end(), '\\', '/');
        if (!path.empty() && path[0] == '/') path.erase(0, 1);
        return path;
    }

private:
    std::vector<std::unique_ptr<DataSource>> sources;
};
