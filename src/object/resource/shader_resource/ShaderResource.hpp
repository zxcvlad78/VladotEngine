#pragma once
#include "object/resource/Resource.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class ShaderResource : public Resource {
public:
    ShaderResource(std::string p_path);
    virtual ~ShaderResource() override;

    virtual bool load_from_data(const std::vector<unsigned char>& data) override;

    void use() const;

    void set_uniform(const std::string& name, const glm::mat4& value) const;
    void set_uniform(const std::string& name, const glm::vec4& value) const;
    void set_uniform(const std::string& name, float value) const;
    void set_uniform(const std::string& name, int value) const;

    virtual std::string get_class_name() const override { return "ShaderResource"; }

private:
    uint32_t _compile_shader(unsigned int type, const std::string& source);
};
