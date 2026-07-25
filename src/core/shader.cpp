#include "shader.h"

#include "../logger.h"

GLAB_NAMESPACE_BEGIN()

Shader::Shader(GLuint program) : m_program(program) {
    m_reflector.reflect(program);

    auto& uniform_blocks = m_reflector.getUniformBlocks();

    for (auto& uniform_block : uniform_blocks) {
        m_total_block_size += uniform_block.size;
        glUniformBlockBinding(program, uniform_block.index, uniform_block.binding);
        LOG_DEBUG("Uniform block: name={}, index={}, size={}", uniform_block.name,
                  uniform_block.index, uniform_block.size);
        for (auto& field : uniform_block.fields) {
            LOG_DEBUG("Uniform block field: name={}, offset={}", field.name, field.offset);
        }
    }
    if (!uniform_blocks.empty()) {
        glGenBuffers(1, &m_ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, m_ubo);
        glBufferData(GL_UNIFORM_BUFFER, m_total_block_size, nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
}

Shader::Shader(Shader&& other) noexcept {
    m_reflector = std::move(other.m_reflector);
    m_program = other.m_program;
    m_ubo = other.m_ubo;
}

Shader& Shader::operator=(Shader&& other) noexcept {
    destroy();
    m_reflector = std::move(other.m_reflector);
    m_program = other.m_program;
    m_ubo = other.m_ubo;
    return *this;
}

void Shader::setUniform(const std::string& name, const void* data) {
    auto uniform = m_reflector.findUniform(name);
    if (!uniform) return;

    auto location = uniform->location;
    auto count = uniform->array_size;

    switch (uniform->type) {
        case UniformType::Float:
            glUniform1fv(location, count, (GLfloat*)data);
            break;
        case UniformType::Int:
            glUniform1iv(location, count, (GLint*)data);
            break;
        case UniformType::Vec2:
            glUniform2fv(location, count, (GLfloat*)data);
            break;
        case UniformType::Vec3:
            glUniform3fv(location, count, (GLfloat*)data);
            break;
        case UniformType::Vec4:
            glUniform4fv(location, count, (GLfloat*)data);
            break;
        case UniformType::Mat4:
            glUniformMatrix4fv(location, count, GL_FALSE, (GLfloat*)data);
            break;
        case UniformType::Sampler2D:
            glUniform1iv(location, count, (GLint*)data);
            break;
    }
}

void Shader::setUniformBlock(const std::string& name, const void* data) {
    auto block = m_reflector.findUniformBlock(name);
    if (!block) return;

    glBindBuffer(GL_UNIFORM_BUFFER, m_ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, block->offset, block->size, data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Shader::destroy() {
    if (m_program) {
        glDeleteProgram(m_program);
    }
    if (m_ubo) {
        glDeleteBuffers(1, &m_ubo);
    }
    m_program = 0;
    m_ubo = 0;
}

GLAB_NAMESPACE_END()
