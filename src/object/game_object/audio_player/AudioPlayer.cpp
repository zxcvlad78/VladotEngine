#include "AudioPlayer.hpp"
#include <iostream>
#include <vector>

ALenum get_al_format(int channels, int bits_per_sample) {
    bool stereo = (channels > 1);
    if (bits_per_sample == 16) {
        return stereo ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
    } else if (bits_per_sample == 8) {
        return stereo ? AL_FORMAT_STEREO8 : AL_FORMAT_MONO8;
    }
    return 0;
}

AudioPlayer::AudioPlayer() {
    alGenSources(1, &m_source);
    alGenBuffers(1, &m_buffer);

    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        std::cerr << "OpenAL error during AudioPlayer init: " << error << std::endl;
    }
}

AudioPlayer::~AudioPlayer() {
    stop();

    if (m_buffer) {
        alDeleteBuffers(1, &m_buffer);
        m_buffer = 0;
    }

    if (m_source) {
        alDeleteSources(1, &m_source);
        m_source = 0;
    }
}

void AudioPlayer::set_stream(Ref<AudioStreamResource> stream) {
    if (m_playing) {
        stop();
    }

    m_stream = stream;

    if (!m_stream) {
        return;
    }

    
    auto data = m_stream->get_data();
    auto sample_rate = m_stream->get_sample_rate();
    ALenum format = m_stream->get_openal_format(); // Use the format determined by the resource
    if (format == 0) {
        std::cerr << "Unsupported audio format!" << std::endl;
        return;
    }
    
    alBufferData(m_buffer, format, data.data(), static_cast<ALsizei>(data.size()), static_cast<ALsizei>(sample_rate));
    
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        std::cerr << "OpenAL error when loading buffer: " << error << std::endl;
        return;
    }
    
    alSourcei(m_source, AL_BUFFER, static_cast<ALint>(m_buffer));
}

void AudioPlayer::set_bus_name(const std::string& bus_name) {
    m_bus_name = bus_name;
}

std::string AudioPlayer::get_bus_name() const {
    return m_bus_name;
}

void AudioPlayer::set_volume_db(float p_volume_db) {
    this->m_volume_db = p_volume_db;
    alSourcef(m_source, AL_GAIN, std::pow(10.0f, m_volume_db / 20.0f));
}
float AudioPlayer::get_volume_db() const {
    return m_volume_db;
}

void AudioPlayer::play() {
    if (!m_stream) {
        std::cerr << "AudioPlayer::play(): No stream set!" << std::endl;
        return;
    }

    stop();

    alSourcePlay(m_source);
    m_playing = true;

    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        std::cerr << "OpenAL error during play: " << error << std::endl;
        m_playing = false;
    }
}

void AudioPlayer::stop() {
    if (m_playing && m_source) {
        alSourceStop(m_source);
        m_playing = false;
    }
}

bool AudioPlayer::is_playing() const {
    if (!m_source || !m_playing) return false;

    ALint state;
    alGetSourcei(m_source, AL_SOURCE_STATE, &state);
    return state == AL_PLAYING;
}