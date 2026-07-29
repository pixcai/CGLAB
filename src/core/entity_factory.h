#pragma once

#include <glm/glm.hpp>

#include "common.h"
#include "world.h"

GLAB_NAMESPACE_BEGIN()

class EntityObject {
public:
    EntityObject() = delete;

    explicit EntityObject(World& world) : m_world(world) { m_entity = m_world.createEntity(); }

    std::uint32_t id() const noexcept { return m_entity.id; }

    template <ComponentLike Component, typename... Args>
    Component* add(Args&&... args) {
        return m_world.addComponent<Component>(m_entity, std::forward<Args>(args)...);
    };

    template <ComponentLike Component>
    void remove() {
        m_world.removeComponent<Component>(m_entity);
    }

    template <ComponentLike Component>
    Component* get() noexcept {
        return m_world.getComponent<Component>(m_entity);
    }

    template <ComponentLike Component>
    bool has() noexcept {
        return m_world.hasComponent<Component>(m_entity);
    }

    void destroy() {
        m_world.destroyEntity(m_entity);
        m_entity = {};
    }

private:
    World& m_world;
    Entity m_entity;
};

class EntityFactory {
public:
    EntityFactory(World& world);

    EntityObject createCube();
    EntityObject createCamera(glm::vec3 position, glm::vec3 target = glm::vec3(0.0f));
    EntityObject createDirectionalLight();

private:
    World& m_world;
};

GLAB_NAMESPACE_END()
