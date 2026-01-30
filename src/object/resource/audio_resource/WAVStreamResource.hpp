#pragma once
#include "AudioResource.hpp"
#include <vector>
#include <string>

class WAVStreamResource : public AudioStreamResource {
public:
    WAVStreamResource(std::string p_path) : AudioStreamResource(std::move(p_path)) {}

    bool load_from_data(const std::vector<unsigned char>& data) override;
};