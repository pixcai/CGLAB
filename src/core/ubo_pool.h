#pragma once

#include <unordered_map>
#include <glm/glm.hpp>

#include "../gl.h"
#include "../logger.h"
#include "material.h"

GLAB_NAMESPACE_BEGIN()

enum class UBOBinding : std::uint32_t {
    Frame = 0,
    Camera = 1,
    Material = 2,
};

struct alignas(16) UniformFrame {
    float time;
};

struct alignas(16) UniformCamera {
    glm::mat4 view_projection_matrix;
};

class UBOPool {
public:
    static UBOPool& instance() noexcept {
        static UBOPool singleton;
        return singleton;
    }

    static constexpr std::size_t kUniformFrameSize{sizeof(UniformFrame)};
    static constexpr std::size_t kUniformCameraSize{sizeof(UniformCamera)};

    void init() {
        LOG_DEBUG("UBOPool::kUniformFrameSize={}", kUniformFrameSize);
        LOG_DEBUG("UBOPool::kUniformCameraSize={}", kUniformCameraSize);
        initUBO(UBOBinding::Frame, kUniformFrameSize);
        initUBO(UBOBinding::Camera, kUniformCameraSize);
    }

    void destroy() {
        for (auto& [_, ubo] : m_ubo_map) {
            glDeleteBuffers(1, &ubo);
        }
        m_ubo_map.clear();
    }

    void updateFrame(const UniformFrame& data) {
        glBindBuffer(GL_UNIFORM_BUFFER, m_ubo_map[UBOBinding::Frame]);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, kUniformFrameSize, &data);
    }

    void updateCamera(const UniformCamera& data) {
        glBindBuffer(GL_UNIFORM_BUFFER, m_ubo_map[UBOBinding::Camera]);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, kUniformCameraSize, &data);
    }

    void updateMaterial(const Material& material) {
        if (!material.m_dirty) return;

        auto size = material.shader_block->size;
        glBindBuffer(GL_UNIFORM_BUFFER, m_ubo_map[UBOBinding::Material]);
        glBufferSubData(GL_UNIFORM_BUFFER, material.index * size, size, material.m_storage.data());
        material.m_dirty = false;
    }

private:
    void initUBO(UBOBinding binding, GLint size) {
        if (m_ubo_map.contains(binding)) return;

        GLuint ubo;
        glGenBuffers(1, &ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, (GLuint)binding, ubo);
        m_ubo_map[binding] = ubo;
    }

    friend class Material;

private:
    std::unordered_map<UBOBinding, GLuint> m_ubo_map;
};

GLAB_NAMESPACE_END()
