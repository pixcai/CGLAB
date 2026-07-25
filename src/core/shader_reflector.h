#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "../gl.h"
#include "../namespace.h"

GLAB_NAMESPACE_BEGIN()

enum class UniformType {
    Float,
    Int,
    Vec2,
    Vec3,
    Vec4,
    Mat4,
    Sampler2D,
};

struct Uniform {
    UniformType type;
    std::string name;
    std::string basename;
    GLint location;
    GLint array_size;
};

struct UniformBlockField {
    UniformType type;
    std::string name;
    std::string basename;
    GLint offset;
    GLint array_size;
};

struct UniformBlock {
    std::string name;
    GLuint index;
    GLint binding;
    GLint size;
    GLint offset;
    std::vector<UniformBlockField> fields;
    std::unordered_map<std::string, UniformBlockField*> field_map;
};

class ShaderReflector {
public:
    void reflect(GLuint program);
    void clear();

    const Uniform* findUniform(const std::string& name) const noexcept;
    const std::vector<Uniform>& getUniforms() const noexcept { return m_uniforms; }

    const UniformBlock* findUniformBlock(const std::string& name) const noexcept;
    const std::vector<UniformBlock>& getUniformBlocks() const noexcept { return m_blocks; }

private:
    std::vector<Uniform> m_uniforms;
    std::unordered_map<std::string, Uniform*> m_uniform_map;
    std::vector<UniformBlock> m_blocks;
    std::unordered_map<std::string, UniformBlock*> m_block_map;
};

GLAB_NAMESPACE_END()
