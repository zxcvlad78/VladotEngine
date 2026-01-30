#include "WAVStreamResource.hpp"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <alext.h>


struct Guid {
    uint32_t Data1; uint16_t Data2; uint16_t Data3; uint8_t Data4[8];
    bool operator==(const Guid& other) const { return std::memcmp(this, &other, sizeof(Guid)) == 0; }
};

static const Guid KSDATAFORMAT_SUBTYPE_PCM = { 0x00000001, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 } };
static const Guid KSDATAFORMAT_SUBTYPE_IEEE_FLOAT = { 0x00000003, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 } };

bool WAVStreamResource::load_from_data(const std::vector<unsigned char>& data) {
    if (data.size() < 44) return false;
    if (std::memcmp(&data[0], "RIFF", 4) != 0 || std::memcmp(&data[8], "WAVE", 4) != 0) return false;

    size_t offset = 12;
    uint16_t audio_format = 0;
    bool fmt_found = false;

    while (offset + 8 <= data.size()) {
        char chunk_id[5] = {0};
        std::memcpy(chunk_id, &data[offset], 4);
        uint32_t chunk_size = *reinterpret_cast<const uint32_t*>(&data[offset + 4]);
        size_t chunk_data_ptr = offset + 8;

        if (std::string(chunk_id) == "fmt ") {
            audio_format = *reinterpret_cast<const uint16_t*>(&data[chunk_data_ptr]);
            m_channels = *reinterpret_cast<const uint16_t*>(&data[chunk_data_ptr + 2]);
            m_sample_rate = *reinterpret_cast<const uint32_t*>(&data[chunk_data_ptr + 4]);
            m_bits_per_sample = *reinterpret_cast<const uint16_t*>(&data[chunk_data_ptr + 14]);

            if (audio_format == 0xFFFE && chunk_size >= 40) { 
                Guid sub_format = *reinterpret_cast<const Guid*>(&data[chunk_data_ptr + 24]);
                if (sub_format == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT) audio_format = 3;
                else audio_format = 1;
            }

            if (audio_format == 1) { // PCM
                if (m_bits_per_sample == 8) {
                    if (m_channels == 1) m_openal_format = AL_FORMAT_MONO8;
                    else if (m_channels == 2) m_openal_format = AL_FORMAT_STEREO8;
                    else if (m_channels == 4) m_openal_format = AL_FORMAT_QUAD8;
                    else if (m_channels == 6) m_openal_format = AL_FORMAT_51CHN8;
                } else if (m_bits_per_sample == 16) {
                    if (m_channels == 1) m_openal_format = AL_FORMAT_MONO16;
                    else if (m_channels == 2) m_openal_format = AL_FORMAT_STEREO16;
                    else if (m_channels == 4) m_openal_format = AL_FORMAT_QUAD16;
                    else if (m_channels == 6) m_openal_format = AL_FORMAT_51CHN16;
                } /*else if (m_bits_per_sample == 32) {
                    if (m_channels == 1) m_openal_format = AL_FORMAT_MONO_FLOAT32;
                    else if (m_channels == 2) m_openal_format = AL_FORMAT_STEREO_FLOAT32;
                }*/
                
            } else if (audio_format == 3) { // FLOAT
                if (m_channels == 1) m_openal_format = AL_FORMAT_MONO_FLOAT32;
                else if (m_channels == 2) m_openal_format = AL_FORMAT_STEREO_FLOAT32;
                else if (m_channels == 6) m_openal_format = AL_FORMAT_51CHN32;
            }
            fmt_found = true;
        } 
        else if (std::string(chunk_id) == "data" && fmt_found) {
            m_data.assign(data.begin() + chunk_data_ptr, data.begin() + std::min(chunk_data_ptr + chunk_size, data.size()));
            return true;
        }
        offset += 8 + ((chunk_size + 1) & ~1);
    }
    return false;
}