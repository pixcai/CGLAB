#pragma once

#include "shader.h"

GLAB_NAMESPACE_BEGIN()

class ShaderManager {
public:
    static ShaderManager& instance() noexcept {
        static ShaderManager singleton;
        return singleton;
    }

    ~ShaderManager();

    void init();
    Shader& get(ShaderType type, ShaderFeatureBits features);

private:
    void compile(ShaderType type, ShaderFeatureBits features);

private:
    std::unordered_map<std::uint64_t, Shader> m_shader_map;
};

GLAB_NAMESPACE_END()
