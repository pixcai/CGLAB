#pragma once

#include <unordered_map>

#include <glm/glm.hpp>

#include "../gl.h"
#include "../logger.h"
#include "material.h"
#include "light.h"

GLAB_NAMESPACE_BEGIN()

enum class UBOBinding : std::uint32_t {
    Frame = 0,
    Object = 1,
    Camera = 2,
    Material = 3,
    Light = 4,
};

struct alignas(16) UniformFrame {
    float time;
};

struct alignas(16) UniformObject {
    glm::mat4 model_matrix;
};

struct alignas(16) UniformCamera {
    glm::vec4 position;
    glm::mat4 view_projection_matrix;
};

class UBOPool {
public:
    static UBOPool& instance() noexcept {
        static UBOPool singleton;
        return singleton;
    }

    static constexpr std::size_t kUniformFrameSize{sizeof(UniformFrame)};
    static constexpr std::size_t kUniformObjectSize{sizeof(UniformObject)};
    static constexpr std::size_t kUniformCameraSize{sizeof(UniformCamera)};
    static constexpr std::size_t kUniformLightSize{sizeof(UniformLight)};

    void init() {
        LOG_DEBUG("UBOPool::kUniformFrameSize={}", kUniformFrameSize);
        LOG_DEBUG("UBOPool::kUniformCameraSize={}", kUniformCameraSize);
        initUBO(UBOBinding::Frame, kUniformFrameSize);
        initUBO(UBOBinding::Object, kUniformObjectSize);
        initUBO(UBOBinding::Camera, kUniformCameraSize);
        initUBO(UBOBinding::Light, kUniformLightSize);
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

    void updateObject(const UniformObject& data) {
        glBindBuffer(GL_UNIFORM_BUFFER, m_ubo_map[UBOBinding::Object]);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, kUniformObjectSize, &data);
    }

    void updateCamera(const UniformCamera& data) {
        glBindBuffer(GL_UNIFORM_BUFFER, m_ubo_map[UBOBinding::Camera]);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, kUniformCameraSize, &data);
    }

    void updateMaterial(Material& material) {
        if (!material.dirty) return;

        auto size = material.shader_block->size;
        glBindBuffer(GL_UNIFORM_BUFFER, m_ubo_map[UBOBinding::Material]);
        glBufferSubData(GL_UNIFORM_BUFFER, material.index * size, size, material.m_storage.data());
        material.dirty = false;
    }

    void updateLight(Light& light) {
        if (!light.dirty) return;

        auto size = kUniformLightSize;
        glBindBuffer(GL_UNIFORM_BUFFER, m_ubo_map[UBOBinding::Light]);
        glBufferSubData(GL_UNIFORM_BUFFER, light.index * size, size, &light.property);
        light.dirty = false;
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
