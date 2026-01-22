//TextureResource.hpp

#pragma once
#include "object/resource/Resource.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>

class TextureResource : public Resource {
public:
    TextureResource(std::string p_path);
    virtual ~TextureResource() override;

    virtual bool load_from_data(const std::vector<unsigned char>& data) override;

    void bind(uint32_t p_slot = 0) const;
    
    int get_width() const { return m_width; }
    int get_height() const { return m_height; }
    glm::vec2 get_size() const { return glm::vec2((float)m_width, (float)m_height); }

    virtual std::string get_class_name() const override { return "TextureResource"; }

private:
    uint32_t m_width = 0;
    uint32_t m_height = 0;
    uint32_t m_channels = 0;
};
