#include "MP3StreamResource.hpp"
#include <iostream>
#include <miniz/miniz.h>

bool MP3StreamResource::decode_mp3(const std::vector<unsigned char>& mp3_data) {
    return false; 
}

bool MP3StreamResource::load_from_data(const std::vector<unsigned char>& data) {
    if (data.size() < 10 || std::string((char*)&data[0], 3) != "ID3") {
        std::cerr << "Invalid or unsupported MP3 file (no ID3 tag or too small)." << std::endl;
        return false;
    }
    return true;
}