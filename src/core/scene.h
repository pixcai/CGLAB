#pragma once

#include <vector>
#include <unordered_map>

#include <glm/glm.hpp>

#include "common.h"
#include "entity_factory.h"
#include "rendering.h"
#include "world.h"

GLAB_NAMESPACE_BEGIN()

class Scene {
public:
    void destroyObject(std::uint32_t id);
    void addObject(EntityObject object);
    EntityObject* getObject(std::uint32_t id) noexcept;

    std::vector<RenderItem>& collectRenderItems();

public:
    glm::vec4 clear_color{0.1f, 0.1f, 0.1f, 1.0f};
    EntityFactory factory{m_world};

private:
    World m_world{};
    std::vector<RenderItem> m_render_items;
    std::unordered_map<std::uint32_t, EntityObject> m_object_map;

    friend class EntityObject;
};

GLAB_NAMESPACE_END()
