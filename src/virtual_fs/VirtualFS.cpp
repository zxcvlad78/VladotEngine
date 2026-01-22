// VirtualFS.cpp
#include "VirtualFS.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <set>

namespace fs = std::filesystem;

void VirtualFS::mount(const std::string& physicalPath, Type type) {
    auto ds = std::make_unique<DataSource>();
    ds->type = type;
    ds->physicalPath = physicalPath;

    if (type == ZIP) {
        if (!mz_zip_reader_init_file(&ds->zipArchive, physicalPath.c_str(), 0)) {
            std::cerr << "[VFS] Error: Cannot initialize ZIP archive: " << physicalPath << std::endl;
            return;
        }
    }

    sources.push_back(std::move(ds));
    std::cout << "[VFS] Mounted " << (type == ZIP ? "ZIP: " : "Folder: ") << physicalPath << std::endl;
}

std::string VirtualFS::normalize_path(std::string path) {
    std::replace(path.begin(), path.end(), '\\', '/');
    
    if (!path.empty() && path[0] == '/') {
        path.erase(0, 1);
    }
    return path;
}

std::vector<unsigned char> VirtualFS::read_file(const std::string& virtualPath) {
    std::string normPath = normalize_path(virtualPath);

    for (int i = (int)sources.size() - 1; i >= 0; --i) {
        auto& source = *sources[i];

        if (source.type == Type::FOLDER) {
            fs::path fullPath = fs::path(source.physicalPath) / normPath;
            if (fs::exists(fullPath) && !fs::is_directory(fullPath)) {
                std::ifstream file(fullPath, std::ios::binary | std::ios::ate);
                if (file.is_open()) {
                    std::streamsize size = file.tellg();
                    file.seekg(0, std::ios::beg);
                    std::vector<unsigned char> buffer(size);
                    if (file.read(reinterpret_cast<char*>(buffer.data()), size)) {
                        return buffer;
                    }
                }
            }
        } 
        else if (source.type == Type::ZIP) {
            size_t uncompressed_size;
            void* p = mz_zip_reader_extract_file_to_heap(&source.zipArchive, normPath.c_str(), &uncompressed_size, 0);
            if (p) {
                std::vector<unsigned char> data(reinterpret_cast<unsigned char*>(p), 
                                               reinterpret_cast<unsigned char*>(p) + uncompressed_size);
                mz_free(p);
                return data;
            }
        }
    }

    std::cerr << "[VFS] Warning: File not found: " << virtualPath << std::endl;
    return {};
}

std::string VirtualFS::read_file_string(const std::string& virtualPath) {
    std::vector<unsigned char> data = read_file(virtualPath);
    if (!data.empty()) {
        return std::string(reinterpret_cast<char*>(data.data()), data.size());
    }
    return "";
}

std::vector<std::string> VirtualFS::list_directory(const std::string& virtualPath) {
    std::string normPath = normalize_path(virtualPath);
    if (!normPath.empty() && normPath.back() != '/') normPath += '/';

    std::set<std::string> entries;

    for (int i = (int)sources.size() - 1; i >= 0; --i) {
        auto& source = *sources[i];

        if (source.type == Type::FOLDER) {
            fs::path searchPath = fs::path(source.physicalPath) / normPath;
            if (fs::exists(searchPath) && fs::is_directory(searchPath)) {
                for (const auto& entry : fs::directory_iterator(searchPath)) {
                    entries.insert(entry.path().filename().string());
                }
            }
        } 
        else if (source.type == Type::ZIP) {
            mz_uint num_files = mz_zip_reader_get_num_files(&source.zipArchive);
            for (mz_uint j = 0; j < num_files; j++) {
                mz_zip_archive_file_stat stat;
                if (mz_zip_reader_file_stat(&source.zipArchive, j, &stat)) {
                    std::string fileName = normalize_path(stat.m_filename);
                    
                    if (fileName.find(normPath) == 0 && fileName != normPath) {
                        std::string relative = fileName.substr(normPath.length());
                        size_t slashPos = relative.find('/');
                        if (slashPos != std::string::npos) {
                            entries.insert(relative.substr(0, slashPos));
                        } else {
                            entries.insert(relative);
                        }
                    }
                }
            }
        }
    }

    return std::vector<std::string>(entries.begin(), entries.end());
}

std::string VirtualFS::get_absolute_path(const std::string& virtualPath) {
    std::string normPath = normalize_path(virtualPath);

    for (int i = (int)sources.size() - 1; i >= 0; --i) {
        if (sources[i]->type == Type::FOLDER) {
            fs::path physical = fs::path(sources[i]->physicalPath) / normPath;
            if (fs::exists(physical)) {
                return fs::absolute(physical).string();
            }
        }
        else if (sources[i]->type == Type::ZIP) {
            if (mz_zip_reader_locate_file(&sources[i]->zipArchive, normPath.c_str(), nullptr, 0) != -1) {
                return fs::absolute(sources[i]->physicalPath).string();
            }
        }
    }
    return "";
}
