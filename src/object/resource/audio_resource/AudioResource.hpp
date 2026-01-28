#pragma once
#include "Resource.hpp"

class AudioResource : public Resource {
public:
AudioResource(std::string p_path) : Resource(std::move(p_path)) {}
virtual ~AudioResource() = default;

virtual bool load_from_data(const std::vector<unsigned char>& data) = 0;
std::string get_class_name() const override { return "AudioResource"; }
};

class AudioStreamResource : public AudioResource {
protected:
    std::vector<unsigned char> m_data;
    int m_sample_rate = 44100;
    int m_channels = 2;
    int m_bits_per_sample = 16;

public:
    AudioStreamResource(std::string p_path) : AudioResource(std::move(p_path)) {}

    virtual ~AudioStreamResource() = default;

    virtual bool load_from_data(const std::vector<unsigned char>& data) = 0;

    const std::vector<unsigned char>& get_data() const { return m_data; }
    int get_sample_rate() const { return m_sample_rate; }
    int get_channels() const { return m_channels; }
    int get_bits_per_sample() const { return m_bits_per_sample; }
};


class WAVStreamResource : public AudioStreamResource {
public:
    WAVStreamResource(std::string p_path) : AudioStreamResource(std::move(p_path)) {}

    bool load_from_data(const std::vector<unsigned char>& data) override {
        if (data.size() < 44) return false;

        if (std::string((char*)&data[0], 4) != "RIFF") return false;

        m_channels = data[22];
        m_sample_rate = *reinterpret_cast<const int*>(&data[24]);
        m_bits_per_sample = data[34];

        int header_size = 44;
        m_data.assign(data.begin() + header_size, data.end());

        return true;
    }
};