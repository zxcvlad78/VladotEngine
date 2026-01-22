//TextureResource.cpp

#include "object/resource/texture_resource/TextureResource.hpp"
#include "object/resource/ResourceLoader.hpp"
#include <iostream>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include "stb_image.h"

REGISTER_RESOURCE_TYPE(TextureResource)

TextureResource::TextureResource(std::string p_path) : Resource(std::move(p_path)) {
    m_width = 0;
    m_height = 0;
    m_channels = 0;
}

TextureResource::~TextureResource() {
    if (rid != 0) {
        glDeleteTextures(1, &rid);
    }
}

bool TextureResource::load_from_data(const std::vector<unsigned char>& data) {
    if (data.empty()) return false;

    stbi_set_flip_vertically_on_load(true);
    
    int w, h, ch;
    unsigned char* pixels = stbi_load_from_memory(data.data(), (int)data.size(), &w, &h, &ch, 0);

    if (!pixels) {
        std::cerr << "[TextureResource] Failed to load: " << virtualPath << std::endl;
        return false;
    }

    m_width = (uint32_t)w;
    m_height = (uint32_t)h;
    m_channels = (uint32_t)ch;

    glGenTextures(1, &rid);
    glBindTexture(GL_TEXTURE_2D, rid);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format = GL_RGB;
    if (m_channels == 4) format = GL_RGBA;
    else if (m_channels == 1) format = GL_RED;

    glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
    return true;
}

void TextureResource::bind(uint32_t p_slot) const {
    glActiveTexture(GL_TEXTURE0 + p_slot);
    glBindTexture(GL_TEXTURE_2D, rid);
}
