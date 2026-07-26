#pragma once

#include <cstdint>

#include "shader.h"

GLAB_NAMESPACE_BEGIN()

class ShaderManager {
public:
    static ShaderManager& instance() noexcept {
        static ShaderManager singleton;
        return singleton;
    }

    void init();
    void destroy();
    Shader& get(std::uint64_t key);

private:
    void compile(ShaderType type, ShaderFeatureBits features);

private:
    std::unordered_map<std::uint64_t, Shader> m_shader_map;
};

GLAB_NAMESPACE_END()
