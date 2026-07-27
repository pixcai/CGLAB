#include "material.h"

#include "shader_manager.h"
#include "ubo_pool.h"

GLAB_NAMESPACE_BEGIN()

Material::Material(MaterialType type)
    : IResource(ResourceType::Material), shader_key((std::uint64_t)type) {
    auto& shader = ShaderManager::instance().get(shader_key);

    shader_block = const_cast<ShaderBlock*>(&shader.blocks().at("Material"));
    m_storage.reserve(shader_block->size);
    m_storage.clear();

    UBOPool::instance().initUBO(UBOBinding::Material, shader_block->size * kMaterialLimits);

    set("albedo", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
}

GLAB_NAMESPACE_END()
