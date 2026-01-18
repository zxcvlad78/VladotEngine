#include "ShaderResource.hpp"
#include "object/resource/ResourceLoader.hpp"
#include <iostream>
#include <sstream>

REGISTER_RESOURCE_TYPE(ShaderResource)

ShaderResource::ShaderResource(std::string p_path) : Resource(std::move(p_path)) {}

ShaderResource::~ShaderResource() {
    if (rid != 0) {
        glDeleteProgram(rid);
        std::cout << "[ShaderResource] Program deleted: " << virtualPath << std::endl;
    }
}

bool ShaderResource::load_from_data(const std::vector<unsigned char>& data) {
    if (data.empty()) return false;

    std::string source(reinterpret_cast<const char*>(data.data()), data.size());
    
    std::string vertexSource, fragmentSource;
    const std::string vTag = "#type vertex";
    const std::string fTag = "#type fragment";

    size_t vPos = source.find(vTag);
    size_t fPos = source.find(fTag);

    if (vPos == std::string::npos || fPos == std::string::npos) {
        std::cerr << "[ShaderResource] Missing #type tags in: " << virtualPath << std::endl;
        return false;
    }

    // Извлечение исходного кода
    if (vPos < fPos) {
        vertexSource = source.substr(vPos + vTag.length(), fPos - (vPos + vTag.length()));
        fragmentSource = source.substr(fPos + fTag.length());
    } else {
        fragmentSource = source.substr(fPos + fTag.length(), vPos - (fPos + fTag.length()));
        vertexSource = source.substr(vPos + vTag.length());
    }

    // Компиляция
    uint32_t vs = _compile_shader(GL_VERTEX_SHADER, vertexSource);
    uint32_t fs = _compile_shader(GL_FRAGMENT_SHADER, fragmentSource);

    if (vs == 0 || fs == 0) return false;

    // Линковка программы
    rid = glCreateProgram();
    glAttachShader(rid, vs);
    glAttachShader(rid, fs);
    glLinkProgram(rid);

    int success;
    glGetProgramiv(rid, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(rid, 512, NULL, infoLog);
        std::cerr << "[ShaderResource] Linking Error (" << virtualPath << "):\n" << infoLog << std::endl;
        return false;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
    return true;
}

uint32_t ShaderResource::_compile_shader(unsigned int type, const std::string& source) {
    uint32_t id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(id, 512, NULL, infoLog);
        std::cerr << "[ShaderResource] Compile Error (" << (type == GL_VERTEX_SHADER ? "Vert" : "Frag") 
                  << ") in " << virtualPath << ":\n" << infoLog << std::endl;
        glDeleteShader(id);
        return 0;
    }
    return id;
}

void ShaderResource::use() const {
    if (rid != 0) glUseProgram(rid);
}

void ShaderResource::set_uniform(const std::string& name, const glm::mat4& value) const {
    glUniformMatrix4fv(glGetUniformLocation(rid, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void ShaderResource::set_uniform(const std::string& name, const glm::vec4& value) const {
    glUniform4fv(glGetUniformLocation(rid, name.c_str()), 1, glm::value_ptr(value));
}

void ShaderResource::set_uniform(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(rid, name.c_str()), value);
}

void ShaderResource::set_uniform(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(rid, name.c_str()), value);
}
