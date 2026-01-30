#pragma once
#include "AudioResource.hpp"
#include <vector>
#include <string>

class MP3StreamResource : public AudioStreamResource {
private:
    bool decode_mp3(const std::vector<unsigned char>& mp3_data);

public:
    MP3StreamResource(std::string p_path) : AudioStreamResource(std::move(p_path)) {}

    bool load_from_data(const std::vector<unsigned char>& data) override;
};