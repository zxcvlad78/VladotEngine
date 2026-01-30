#pragma once
#include "object/resource/audio_resource/AudioResource.hpp"
#include <AL/al.h>
#include <AL/alc.h>

class AudioPlayer {
private:
    Ref<AudioStreamResource> m_stream;
    bool m_playing = false;

    std::string m_bus_name = "Master";
    float m_volume_db = 0.0f;

    ALuint m_source = 0;
    ALuint m_buffer = 0;

public:
    AudioPlayer();
    ~AudioPlayer();

    void set_stream(Ref<AudioStreamResource> stream);
    Ref<AudioStreamResource> get_stream() const;

    void set_bus_name(const std::string& name);
    std::string get_bus_name() const;

    void set_volume_db(float volume);
    float get_volume_db() const;

    void play();
    void stop();
    bool is_playing() const;
};