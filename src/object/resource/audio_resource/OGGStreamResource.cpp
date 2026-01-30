#include "OGGStreamResource.hpp"
#include <iostream>
#include <vector>

bool OGGStreamResource::decode_ogg(const std::vector<unsigned char>& ogg_data) {
    return false;
}

bool OGGStreamResource::load_from_data(const std::vector<unsigned char>& data) {
    if (data.size() < 4 || std::string((char*)&data[0], 4) != "OggS") {
        std::cerr << "Invalid or unsupported OGG file (no OggS header)." << std::endl;
        return false;
    }
    return true;
}