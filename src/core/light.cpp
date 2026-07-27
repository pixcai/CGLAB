#include "light.h"

#include <glm/glm.hpp>

#include "shader_manager.h"
#include "ubo_pool.h"

GLAB_NAMESPACE_BEGIN()

void Light::init() {
    auto& shader =
        ShaderManager::instance().get(Shader::key(ShaderType::Bundle, ShaderFeature_Lighting));
    shader_block = const_cast<ShaderBlock*>(&shader.blocks().at("Light"));
    m_storage.reserve(shader_block->size);
    m_storage.clear();

    UBOPool::instance().initUBO(UBOBinding::Light, shader_block->size * kLightLimits);

    set("color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    set("direction", glm::vec4(0.2f, 0.8f, 0.4f, 1.0f));
}

GLAB_NAMESPACE_END()
