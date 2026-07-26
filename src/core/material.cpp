#include "material.h"

#include "shader_manager.h"
#include "ubo_pool.h"

GLAB_NAMESPACE_BEGIN()

Material::Material(MaterialType type) : IResource(ResourceType::Material) {
    auto& shader = ShaderManager::instance().get((std::uint64_t)type);

    m_block = shader.blocks().at("Material");
    m_storage.reserve(m_block->size);
    m_storage.clear();

    UBOPool::instance().initUBO(UBOBinding::Material, m_block->size * kMaterialLimits);
}

GLAB_NAMESPACE_END()
