#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "common.h"
#include "../gl.h"
#include "shader.h"

GLAB_NAMESPACE_BEGIN()

enum class LightType {
    Directional,
    Point,
    Spot,
    Area,
};

struct Light : IComponent {
    static constexpr GLuint kLightLimits = 1;

    LightType type{LightType::Directional};

    void init() override;

    template <typename ValueType>
    void set(const std::string& name, const ValueType& value) {
        auto fullname = std::format("lights[{}].{}", index, name);
        if (!shader_block->uniform_map.contains(fullname)) return;

        auto& field = shader_block->uniform_map.at(fullname);
        std::memcpy(m_storage.data() + field.offset, &value, field.array_size * field.size);
        m_dirty = true;
    }

    template <typename ValueType>
    ValueType get(const std::string& name) {
        auto fullname = std::format("lights[{}].{}", index, name);
        if (!shader_block->uniform_map.contains(fullname)) return ValueType();

        auto& field = shader_block->uniform_map.at(fullname);
        return *reinterpret_cast<ValueType*>(m_storage.data() + field.offset);
    }

public:
    GLuint index{0};
    ShaderBlock* shader_block{nullptr};

private:
    mutable bool m_dirty{false};
    std::vector<std::byte> m_storage;

    friend class UBOPool;
};

GLAB_NAMESPACE_END()
