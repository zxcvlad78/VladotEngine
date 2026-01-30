#pragma once
#include <vector>
#include <memory>
#include <string>
#include <AL/al.h>
#include "object/resource/Resource.hpp"

class AudioStreamResource : public Resource {
protected:
    std::vector<unsigned char> m_data;
    std::string m_path;
    uint32_t m_sample_rate = 44100;
    uint16_t m_channels = 1;
    uint16_t m_bits_per_sample = 16;
    ALenum m_openal_format = 0;

public:
    AudioStreamResource(std::string p_path) : Resource(std::move(p_path)) {}
    virtual ~AudioStreamResource() = default;
    virtual bool load_from_data(const std::vector<unsigned char>& data) = 0;

    const std::vector<unsigned char>& get_data() const { return m_data; }
    uint32_t get_sample_rate() const { return m_sample_rate; }
    uint16_t get_channels() const { return m_channels; }
    uint16_t get_bits_per_sample() const { return m_bits_per_sample; }
    ALenum get_openal_format() const { return m_openal_format; }

    const std::string& get_path() const { return m_path; }
    void set_path(const std::string& path) { m_path = path; }
    std::string get_class_name() const override { return "AudioStreamResource"; }
};

template<typename T>
class AudioResource : public T {
public:
    AudioResource(const std::string& path) : T(path) {}
};