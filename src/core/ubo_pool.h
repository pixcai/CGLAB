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
    glm::mat4 view_matrix;
    glm::mat4 projection_matrix;
    glm::mat4 view_projection_matrix;

    glm::vec4 position;
    glm::vec4 viewport;
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
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void updateCamera(const UniformCamera& data) {
        glBindBuffer(GL_UNIFORM_BUFFER, m_ubo_map[UBOBinding::Camera]);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, kUniformCameraSize, &data);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void updateMaterial(const Material& material) {
        if (!material.m_dirty) return;

        glBindBuffer(GL_UNIFORM_BUFFER, m_ubo_map[UBOBinding::Material]);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, kUniformCameraSize, material.m_storage.data());
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

private:
    void initUBO(UBOBinding binding, GLint size) {
        if (m_ubo_map.contains(binding)) return;

        glGenBuffers(1, &m_ubo_map[binding]);
        glBindBuffer(GL_UNIFORM_BUFFER, m_ubo_map[binding]);
        glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        glBindBufferBase(GL_UNIFORM_BUFFER, (GLuint)binding, m_ubo_map[binding]);
    }

    friend class Material;

private:
    std::unordered_map<UBOBinding, GLuint> m_ubo_map;
};

GLAB_NAMESPACE_END()
