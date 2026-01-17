#include "VirtualFS.hpp"
namespace fs = std::filesystem;

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

void VirtualFS::mount_recursive(const std::string& rootPath) {
    if (!fs::exists(rootPath)) return;

    for (const auto& entry : fs::recursive_directory_iterator(rootPath)) {
        if (entry.is_directory()) {
            // Монтируем каждую найденную папку как потенциальный источник
            this->mount(entry.path().string(), Type::FOLDER);
        } 
        else if (entry.path().extension() == ".zip") {
            // Монтируем ZIP-архивы
            this->mount(entry.path().string(), Type::ZIP);
        }
    }
}

std::vector<unsigned char> VirtualFS::read_file(const std::string& virtualPath) {
    for (int i = sources.size() - 1; i >= 0; --i) {
        if (sources[i].type == Type::FOLDER) {
            std::string fullPath = sources[i].physicalPath + "/" + virtualPath;
            std::ifstream file(fullPath, std::ios::binary | std::ios::ate);
            if (file.is_open()) {
                std::streamsize size = file.tellg();
                file.seekg(0, std::ios::beg);
                std::vector<unsigned char> buffer(size);
                file.read(reinterpret_cast<char*>(buffer.data()), size);
                return buffer;
            }
        } else if (sources[i].type == Type::ZIP) {
            mz_zip_archive* zip = &sources[i].zipArchive;
            size_t uncompressed_size;
            void* p = mz_zip_reader_extract_file_to_heap(zip, virtualPath.c_str(), &uncompressed_size, 0);
            if (p) {
                std::vector<unsigned char> data(static_cast<unsigned char*>(p), static_cast<unsigned char*>(p) + uncompressed_size);
                mz_free(p);
                return data;
            }
        }
    }
    std::cerr << "VFS Error: File not found: " << virtualPath << std::endl;
    return {};
}

std::string VirtualFS::read_file_string(const std::string& virtualPath) {
    std::vector<unsigned char> data = read_file(virtualPath);
    if (!data.empty()) {
        return std::string(reinterpret_cast<char*>(data.data()), data.size());
    }
    return "";
}
