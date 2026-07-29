#pragma once

#include <cstddef>
#include <cstdint>
#include <format>
#include <string>
#include <vector>

#include "../gl.h"
#include "resource.h"
#include "shader.h"

GLAB_NAMESPACE_BEGIN()

enum class MaterialType : std::uint64_t {
    Standard = Shader::key(ShaderType::Bundle, ShaderFeature_Lighting),
};

struct Material : IResource {
    static constexpr GLuint kMaterialLimits = 1;

    Material(MaterialType type = MaterialType::Standard);

    void destroy() override {}

    template <typename ValueType>
    void set(const std::string& name, const ValueType& value) {
        auto fullname = std::format("materials[{}].{}", index, name);
        if (!shader_block->uniform_map.contains(fullname)) return;

        auto& field = shader_block->uniform_map.at(fullname);
        std::memcpy(m_storage.data() + field.offset, &value, field.array_size * field.size);
        dirty = true;
    }

    template <typename ValueType>
    ValueType get(const std::string& name) {
        auto fullname = std::format("materials[{}].{}", index, name);
        if (!shader_block->uniform_map.contains(fullname)) return ValueType();

        auto& field = shader_block->uniform_map.at(fullname);
        return *reinterpret_cast<ValueType*>(m_storage.data() + field.offset);
    }

public:
    GLuint index{0};
    bool dirty{true};
    std::uint64_t shader_key;
    ShaderBlock* shader_block{nullptr};

private:
    std::vector<std::byte> m_storage;

    friend class UBOPool;
};

GLAB_NAMESPACE_END()
