#pragma once

#include <vector>
#include <unordered_map>

#include <glm/glm.hpp>

#include "common.h"
#include "rendering.h"
#include "world.h"

GLAB_NAMESPACE_BEGIN()

class EntityObject;

class Scene {
public:
    EntityObject createObject();
    void destroyObject(std::uint32_t id);
    void addObject(EntityObject object);
    EntityObject* getObject(std::uint32_t id) noexcept;

    std::vector<RenderItem>& collectRenderItems();

public:
    glm::vec4 clear_color{0.1f, 0.1f, 0.1f, 1.0f};

private:
    World m_world;
    std::vector<RenderItem> m_render_items;
    std::unordered_map<std::uint32_t, EntityObject> m_object_map;

    friend class EntityObject;
};

class EntityObject {
public:
    EntityObject() = delete;

    explicit EntityObject(Scene* scene) : m_scene(scene) {
        m_entity = m_scene->m_world.createEntity();
    }

    std::uint32_t id() const noexcept { return m_entity.id; }

    template <ComponentLike Component, typename... Args>
    Component* add(Args&&... args) {
        return m_scene->m_world.addComponent<Component>(m_entity, std::forward<Args>(args)...);
    };

    template <ComponentLike Component>
    void remove() {
        m_scene->m_world.removeComponent<Component>(m_entity);
    }

    template <ComponentLike Component>
    Component* get() noexcept {
        return m_scene->m_world.getComponent<Component>(m_entity);
    }

    template <ComponentLike Component>
    bool has() noexcept {
        return m_scene->m_world.hasComponent<Component>(m_entity);
    }

    void destroy() {
        m_scene->m_world.destroyEntity(m_entity);
        m_scene->m_object_map.erase(m_entity.id);
        m_entity = {};
    }

private:
    Scene* m_scene;
    Entity m_entity;
};

GLAB_NAMESPACE_END()
