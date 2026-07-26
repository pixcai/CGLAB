#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../gl.h"
#include "../logger.h"
#include "../namespace.h"

GLAB_NAMESPACE_BEGIN()

struct ShaderUniform {
    GLenum type;
    std::string name;
    std::string basename;
    GLint location;
    GLint array_size;
};

struct ShaderBlockUniform {
    GLenum type;
    std::string name;
    std::string basename;
    GLint offset;
    GLint size;
    GLint array_size;
};

struct ShaderBlock {
    std::string name;
    GLuint index;
    GLint binding;
    GLint size;
    std::vector<ShaderBlockUniform> uniforms;
    std::unordered_map<std::string, ShaderBlockUniform*> uniform_map;
};

using ShaderFeatureBits = std::uint32_t;

constexpr ShaderFeatureBits ShaderFeature_None = 0;
constexpr ShaderFeatureBits ShaderFeature_Lighting = 1;

enum class ShaderType : std::uint32_t {
    Bundle = 0,
    Wireframe = 1,
};

class Shader {
public:
    static constexpr std::uint64_t key(ShaderType type, ShaderFeatureBits features) noexcept {
        return (((std::uint64_t)type) << 32 | (std::uint32_t)(features));
    }

    Shader(const Shader& other) = delete;
    Shader& operator=(const Shader& other) = delete;

    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) = delete;

    GLuint program() const noexcept { return m_program; }

    template <typename... Args>
    void setUniform(const std::string& name, Args&&... args) {
        if (!m_uniform_map.contains(name)) {
            LOG_WARN("Invalid uniform name: {}", name);
            return;
        }

        auto uniform = m_uniform_map.at(name);
        auto location = uniform->location;

        switch (uniform->type) {
            case GL_FLOAT:
                glUniform1f(location, std::forward<Args>(args)...);
                break;
            case GL_INT:
            case GL_SAMPLER_2D:
                glUniform1i(location, std::forward<Args>(args)...);
                break;
            case GL_FLOAT_VEC2:
                glad_glUniform2f(location, std::forward<Args>(args)...);
                break;
            case GL_FLOAT_VEC3:
                glUniform3f(location, std::forward<Args>(args)...);
                break;
            case GL_FLOAT_VEC4:
                glUniform4f(location, std::forward<Args>(args)...);
                break;
            case GL_FLOAT_MAT4:
                glUniformMatrix4fv(location, 1, GL_TRUE, std::forward<Args>(args)...);
                break;
        }
    }

    const std::unordered_map<std::string, ShaderUniform*>& uniforms() const noexcept {
        return m_uniform_map;
    }

    const std::unordered_map<std::string, ShaderBlock*>& blocks() const noexcept {
        return m_block_map;
    }

private:
    Shader(GLuint program);

    void reflect();
    void destroy();

    friend class ShaderManager;

private:
    GLuint m_program{0};
    std::vector<ShaderUniform> m_uniforms;
    std::unordered_map<std::string, ShaderUniform*> m_uniform_map;
    std::vector<ShaderBlock> m_blocks;
    std::unordered_map<std::string, ShaderBlock*> m_block_map;
};

GLAB_NAMESPACE_END()
