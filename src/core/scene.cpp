#include "scene.h"

GLAB_NAMESPACE_BEGIN()

EntityObject Scene::createObject() { return EntityObject(this); }

void Scene::destroyObject(std::uint32_t id) {
    if (auto object = getObject(id); object != nullptr) {
        object->destroy();
    }
}

void Scene::addObject(EntityObject object) {
    if (object.id() == Entity::kInvalidID) return;
    m_object_map.try_emplace(object.id(), std::move(object));
}

EntityObject* Scene::getObject(std::uint32_t id) noexcept {
    if (m_object_map.contains(id)) {
        return &m_object_map.at(id);
    }
    return nullptr;
}

std::vector<RenderItem>& Scene::collectRenderItems() {
    m_render_items.clear();
    for (auto& [_, object] : m_object_map) {
        auto mesh_renderer = object.get<MeshRenderer>();
        if (!mesh_renderer) {
            continue;
        }
        RenderItem render_item{};
        render_item.mesh_handle = mesh_renderer->mesh_handle;
        render_item.material_handle = mesh_renderer->material_handle;
        render_item.model_matrix = glm::mat4(1.0f);
        m_render_items.push_back(render_item);
    }
    return m_render_items;
}

GLAB_NAMESPACE_END()
