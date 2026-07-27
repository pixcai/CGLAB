#include "shader.h"

#include "ubo_pool.h"

GLAB_NAMESPACE_BEGIN()

Shader::Shader(GLuint program) : m_program(program) {
    reflect();

    for (auto& [_, block] : m_block_map) {
        glUniformBlockBinding(m_program, block.index, block.binding);
        LOG_DEBUG("Uniform block: name={}, binding={}, index={}, size={}", block.name,
                  block.binding, block.index, block.size);
        for (auto& [_, field] : block.uniform_map) {
            LOG_DEBUG("Uniform block field: name={}, basename={}, offset={}", field.name,
                      field.basename, field.offset);
        }
    }
}

Shader::Shader(Shader&& other) noexcept {
    m_program = other.m_program;
    m_uniform_map = std::move(other.m_uniform_map);
    m_block_map = std::move(other.m_block_map);
}

static std::string stripArraySuffix(const std::string& name) {
    std::size_t bracket = name.find('[');

    if (bracket != std::string::npos) {
        return name.substr(0, bracket);
    }
    return name;
}

static std::string stripBlockName(const std::string& name) {
    std::size_t dot = name.find('.');

    if (dot != std::string::npos) {
        return name.substr(dot + 1);
    }
    return name;
}

static std::unordered_map<std::string, UBOBinding> g_block_bindings{
    {"Frame", UBOBinding::Frame},
    {"Object", UBOBinding::Object},
    {"Camera", UBOBinding::Camera},
    {"Material", UBOBinding::Material},
    {"Light", UBOBinding::Light}};

void Shader::reflect() {
    GLint uniform_count;
    glGetProgramiv(m_program, GL_ACTIVE_UNIFORMS, &uniform_count);

    char name_buffer[32];
    GLint size;

    for (GLint i = 0; i < uniform_count; ++i) {
        GLsizei name_length;
        GLenum type;
        glGetActiveUniform(m_program, i, sizeof(name_buffer), &name_length, &size, &type,
                           name_buffer);

        std::string name(name_buffer, name_length);
        std::string basename = stripArraySuffix(name);
        GLint location = glGetUniformLocation(m_program, name.c_str());
        if (location == -1) {
            continue;
        }

        ShaderUniform uniform{};
        uniform.type = type;
        uniform.name = name;
        uniform.basename = basename;
        uniform.location = location;
        uniform.array_size = size;

        m_uniform_map.try_emplace(name, uniform);
        m_uniform_map.try_emplace(basename, uniform);
    }

    GLint block_count;
    glGetProgramiv(m_program, GL_ACTIVE_UNIFORM_BLOCKS, &block_count);

    for (GLint i = 0; i < block_count; ++i) {
        GLsizei name_length;
        glGetActiveUniformBlockName(m_program, i, sizeof(name_buffer), &name_length, name_buffer);

        ShaderBlock block{};
        block.name = std::string(name_buffer, name_length);
        block.index = i;

        glGetActiveUniformBlockiv(m_program, i, GL_UNIFORM_BLOCK_DATA_SIZE, &block.size);
        if (g_block_bindings.contains(block.name)) {
            block.binding = (GLint)g_block_bindings.at(block.name);
        }

        GLint field_count;
        glGetActiveUniformBlockiv(m_program, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &field_count);

        std::vector<GLuint> field_indices(field_count);
        glad_glGetActiveUniformBlockiv(m_program, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES,
                                       (GLint*)field_indices.data());

        for (auto field_index : field_indices) {
            GLenum type;
            glGetActiveUniform(m_program, field_index, sizeof(name_buffer), &name_length, &size,
                               &type, name_buffer);

            std::string name(name_buffer, name_length);
            std::string basename = stripArraySuffix(stripBlockName(name));

            GLint offset;
            glGetActiveUniformsiv(m_program, 1, &field_index, GL_UNIFORM_OFFSET, &offset);

            ShaderBlockUniform block_field{};
            block_field.type = type;
            block_field.name = name;
            block_field.basename = basename;
            block_field.offset = offset;
            switch (type) {
                case GL_FLOAT:
                case GL_INT:
                    block_field.size = 4;
                    break;
                case GL_FLOAT_VEC2:
                    block_field.size = 8;
                    break;
                case GL_FLOAT_VEC3:
                case GL_FLOAT_VEC4:
                    block_field.size = 16;
                    break;
                case GL_FLOAT_MAT4:
                    block_field.size = 64;
                    break;
                default:
                    block_field.size = 0;
                    break;
            }
            block_field.array_size = size;
            block.uniform_map.try_emplace(name, std::move(block_field));
        }
        m_block_map.try_emplace(block.name, std::move(block));
    }
}

void Shader::destroy() {
    if (m_program) {
        glDeleteProgram(m_program);
    }
    m_program = 0;
    m_uniform_map.clear();
    m_block_map.clear();
}

GLAB_NAMESPACE_END()
