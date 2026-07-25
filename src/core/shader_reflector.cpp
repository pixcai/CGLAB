#include "shader_reflector.h"

#include <stdexcept>

GLAB_NAMESPACE_BEGIN()

static std::unordered_map<GLint, UniformType> g_uniform_type_map{
    {GL_FLOAT, UniformType::Float},          {GL_INT, UniformType::Int},
    {GL_FLOAT_VEC2, UniformType::Vec2},      {GL_FLOAT_VEC3, UniformType::Vec3},
    {GL_FLOAT_VEC4, UniformType::Vec4},      {GL_FLOAT_MAT4, UniformType::Mat4},
    {GL_SAMPLER_2D, UniformType::Sampler2D},
};

static UniformType toUniformType(GLenum type) {
    for (auto [gl_type, uniform_type] : g_uniform_type_map) {
        if (gl_type == type) {
            return uniform_type;
        }
    }

    throw std::runtime_error("Unsupported uniform type");
}

static std::string stripArraySuffix(const std::string& name) {
    std::size_t bracket = name.find('[');

    if (bracket != std::string::npos) {
        return name.substr(0, bracket);
    }
    return name;
}

void ShaderReflector::reflect(GLuint program) {
    clear();
    if (program == 0) return;

    GLint uniform_count;
    glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &uniform_count);

    char name_buffer[32];
    GLint size;

    for (GLint i = 0; i < uniform_count; ++i) {
        GLsizei name_length;
        GLenum type;
        glGetActiveUniform(program, i, sizeof(name_buffer), &name_length, &size, &type,
                           name_buffer);

        std::string name(name_buffer, name_length);
        std::string basename = stripArraySuffix(name);
        GLint location = glGetUniformLocation(program, name.c_str());
        if (location == -1) {
            continue;
        }

        Uniform uniform{};
        uniform.type = toUniformType(type);
        uniform.name = name;
        uniform.basename = basename;
        uniform.location = location;
        uniform.array_size = size;

        m_uniforms.push_back(uniform);
        m_uniform_map[name] = &m_uniforms.back();
        m_uniform_map[basename] = &m_uniforms.back();
    }

    GLint block_count;
    glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &block_count);

    GLint block_offset{0};

    for (GLint i = 0; i < block_count; ++i) {
        GLsizei name_length;
        glGetActiveUniformBlockName(program, i, sizeof(name_buffer), &name_length, name_buffer);

        UniformBlock block{};
        block.name = std::string(name_buffer, name_length);
        block.index = i;

        glGetActiveUniformBlockiv(program, i, GL_UNIFORM_BLOCK_BINDING, &block.binding);
        glGetActiveUniformBlockiv(program, i, GL_UNIFORM_BLOCK_DATA_SIZE, &size);
        block.size = size;
        block.offset = block_offset;
        block_offset += size;

        GLint field_count;
        glGetActiveUniformBlockiv(program, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &field_count);

        std::vector<GLuint> field_indices(field_count);
        glad_glGetActiveUniformBlockiv(program, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES,
                                       (GLint*)field_indices.data());

        for (auto field_index : field_indices) {
            GLenum type;
            glGetActiveUniform(program, field_index, sizeof(name_buffer), &name_length, &size,
                               &type, name_buffer);

            std::string name(name_buffer, name_length);
            std::string basename = stripArraySuffix(name);

            GLint offset;
            glGetActiveUniformsiv(program, 1, &field_index, GL_UNIFORM_OFFSET, &offset);

            UniformBlockField block_field{};
            block_field.type = toUniformType(type);
            block_field.name = name;
            block_field.basename = basename;
            block_field.offset = offset;
            block_field.array_size = size;

            block.fields.push_back(block_field);
            block.field_map[name] = &block.fields.back();
            block.field_map[basename] = &block.fields.back();
        }

        m_blocks.push_back(block);
        m_block_map[block.name] = &m_blocks.back();
    }
}

void ShaderReflector::clear() {
    m_uniforms.clear();
    m_uniform_map.clear();
    m_blocks.clear();
    m_block_map.clear();
}

const Uniform* ShaderReflector::findUniform(const std::string& name) const noexcept {
    if (m_uniform_map.contains(name)) {
        return m_uniform_map.at(name);
    }
    return nullptr;
}

const UniformBlock* ShaderReflector::findUniformBlock(const std::string& name) const noexcept {
    if (m_block_map.contains(name)) {
        return m_block_map.at(name);
    }
    return nullptr;
}

GLAB_NAMESPACE_END()
