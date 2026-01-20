#include "VirtualFS.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <set>

namespace fs = std::filesystem;

std::string VirtualFS::get_absolute_path(const std::string& virtualPath) {
    std::string normPath = normalize_path(virtualPath);

    for (int i = (int)sources.size() - 1; i >= 0; --i) {
        const auto& source = sources[i];

        if (source.type == Type::FOLDER) {
            fs::path physicalPath = fs::path(source.physicalPath) / normPath;

            if (fs::exists(physicalPath)) {
                return fs::absolute(physicalPath).string();
            }
        } 
        else if (source.type == Type::ZIP) {
            mz_zip_archive* zip = const_cast<mz_zip_archive*>(&source.zipArchive);
            int file_index = mz_zip_reader_locate_file(zip, normPath.c_str(), nullptr, 0);
            
            if (file_index >= 0) {
                return fs::absolute(source.physicalPath).string();
            }
        }
    }

    return "";
}

std::string VirtualFS::normalize_path(std::string path) {
    std::replace(path.begin(), path.end(), '\\', '/');
    return path;
}

std::vector<std::string> VirtualFS::list_directory(const std::string& virtualPath) {
    std::string normPath = normalize_path(virtualPath);
    std::set<std::string> entries;

    for (int i = (int)sources.size() - 1; i >= 0; --i) {
        const auto& source = sources[i];

        if (source.type == Type::FOLDER) {
            fs::path physicalSearchPath = fs::path(source.physicalPath) / normPath;
            if (fs::exists(physicalSearchPath) && fs::is_directory(physicalSearchPath)) {
                for (const auto& entry : fs::directory_iterator(physicalSearchPath)) {
                    entries.insert(entry.path().filename().string());
                }
            }
        } 
        else if (source.type == Type::ZIP) {
            mz_zip_archive* zip = const_cast<mz_zip_archive*>(&source.zipArchive);
            mz_uint num_files = mz_zip_reader_get_num_files(zip);
            
            for (mz_uint i = 0; i < num_files; i++) {
                mz_zip_archive_file_stat file_stat;
                if (mz_zip_reader_file_stat(zip, i, &file_stat)) {
                    std::string entryPath = normalize_path(file_stat.m_filename);
                    
                    if (entryPath.find(normPath) == 0 && entryPath != normPath) {
                        std::string relative = entryPath.substr(normPath.length());
                        if (!relative.empty() && relative[0] == '/') relative.erase(0, 1);
                        
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

std::string VirtualFS::read_file_string(const std::string& virtualPath) {
    std::vector<unsigned char> data = read_file(virtualPath);
    if (!data.empty()) {
        return std::string(reinterpret_cast<char*>(data.data()), data.size());
    }
    return "";
}


void VirtualFS::mount(const std::string& physicalPath, Type type) {
    DataSource ds;
    ds.type = type;
    ds.physicalPath = physicalPath;
    if (type == ZIP) {
        mz_zip_zero_struct(&ds.zipArchive);
        if (!mz_zip_reader_init_file(&ds.zipArchive, physicalPath.c_str(), 0)) {
            std::cerr << "VFS Error: Cannot open ZIP " << physicalPath << std::endl;
            return;
        }
    }
    sources.push_back(std::move(ds));
    std::cout << "VFS Mounted: " << physicalPath << std::endl;
}

std::vector<unsigned char> VirtualFS::read_file(const std::string& virtualPath) {
    std::string normPath = normalize_path(virtualPath);
    for (int i = (int)sources.size() - 1; i >= 0; --i) {
        if (sources[i].type == Type::FOLDER) {
            fs::path fullPath = fs::path(sources[i].physicalPath) / normPath;
            
            std::ifstream file(fullPath, std::ios::binary | std::ios::ate);
            if (file.is_open()) {
                std::streamsize size = file.tellg();
                file.seekg(0, std::ios::beg);
                std::vector<unsigned char> buffer(size);
                file.read(reinterpret_cast<char*>(buffer.data()), size);
                return buffer;
            }
            std::cout << "[VFS] Cant open file at path: " << virtualPath << std::endl;
        } else if (sources[i].type == Type::ZIP) {
            mz_zip_archive* zip = &sources[i].zipArchive;
            size_t uncompressed_size;
            void* p = mz_zip_reader_extract_file_to_heap(zip, normPath.c_str(), &uncompressed_size, 0);
            if (p) {
                std::vector<unsigned char> data((unsigned char*)p, (unsigned char*)p + uncompressed_size);
                mz_free(p);
                return data;
            }
        }
    }
    return {};
}
