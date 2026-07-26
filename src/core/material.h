#pragma once

#include <cstddef>
#include <cstdint>
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
        if (!m_block->uniform_map.contains(name)) return;

        auto field = m_block->uniform_map.at(name);
        std::memcpy(m_storage.data() + field->offset, &value, field->array_size * field->size);
        m_dirty = true;
    }

    GLuint index() const noexcept { return m_index; }

    void setIndex(GLuint index) {
        m_index = index < kMaterialLimits ? index : (kMaterialLimits - 1);
    }

private:
    bool m_dirty{false};
    GLuint m_index{0};
    ShaderBlock* m_block{nullptr};
    std::vector<std::byte> m_storage;

    friend class UBOPool;
};

GLAB_NAMESPACE_END()
