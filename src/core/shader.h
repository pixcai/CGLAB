#pragma once

#include <string>

#include "../gl.h"
#include "../namespace.h"
#include "shader_reflector.h"

GLAB_NAMESPACE_BEGIN()

using ShaderFeatureBits = std::uint32_t;

constexpr ShaderFeatureBits ShaderFeature_None = 0;
constexpr ShaderFeatureBits ShaderFeature_Lighting = 1;

enum class ShaderType {
    Bundle,
    Wireframe,
};

class Shader {
public:
    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    const ShaderReflector& reflector() const noexcept { return m_reflector; }
    GLuint program() const noexcept { return m_program; }

    void setUniform(const std::string& name, const void* data);
    void setUniformBlock(const std::string& name, const void* data);

    GLint size() const noexcept { return m_total_block_size; }

private:
    Shader(GLuint program);
    void destroy();

    friend class ShaderManager;

private:
    ShaderReflector m_reflector{};
    GLint m_total_block_size{0};
    GLuint m_program{0};
    GLuint m_ubo{0};
};

GLAB_NAMESPACE_END()
