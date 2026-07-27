#pragma once

#include <glm/glm.hpp>

#include "material.h"
#include "mesh.h"
#include "resource.h"

GLAB_NAMESPACE_BEGIN()

using RenderQueueType = std::uint8_t;

constexpr RenderQueueType RenderQueue_Opaque = 32;
constexpr RenderQueueType RenderQueue_Transparent = 64;

struct RenderItem {
    RenderQueueType render_queue{RenderQueue_Opaque};
    glm::mat4 model_matrix{1.0f};
    ResourceHandle<Mesh> mesh_handle;
    ResourceHandle<Material> material_handle;
};

struct DrawCommand {
    RenderQueueType render_queue{RenderQueue_Opaque};
    glm::mat4 model_matrix{1.0f};
    ResourceHandle<Mesh> mesh_handle;
    ResourceHandle<Material> material_handle;
};

GLAB_NAMESPACE_END()
