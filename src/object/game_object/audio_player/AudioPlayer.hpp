#pragma once
#include "object/resource/audio_resource/AudioResource.hpp"
#include <AL/al.h>
#include <AL/alc.h>

class AudioPlayer {
private:
    Ref<AudioStreamResource> m_stream;
    bool m_playing = false;

    ALuint m_source = 0;
    ALuint m_buffer = 0;

public:
    AudioPlayer();
    ~AudioPlayer();

    void set_stream(Ref<AudioStreamResource> stream);
    Ref<AudioStreamResource> get_stream() const;

    void play();
    void stop();
    bool is_playing() const;
};