#include "entity_factory.h"

#include "camera.h"
#include "light.h"
#include "mesh.h"
#include "transform.h"
#include "geometry_helper.h"

GLAB_NAMESPACE_BEGIN()

EntityFactory::EntityFactory(World& world) : m_world(world) {}

EntityObject EntityFactory::createCube() {
    EntityObject object{m_world};
    auto mesh_renderer = object.add<MeshRenderer>();
    mesh_renderer->mesh_handle = GeometryHelper::buildCube();
    mesh_renderer->material_handle = ResourceManager::instance().make<Material>();
    return object;
}

EntityObject EntityFactory::createCamera(glm::vec3 position, glm::vec3 target) {
    EntityObject object{m_world};
    auto camera = object.add<Camera>();
    camera->target = target;
    auto transform = object.add<Transform>();
    transform->position = position;
    transform->lookAt(target);
    transform->updateLocalMatrix();
    auto offset = position - target;
    camera->yaw = std::atan2(offset.x, offset.z);
    camera->pitch = std::asin(offset.y / glm::length(offset));
    camera->updateViewMatrix(transform->local_matrix);
    return object;
}

EntityObject EntityFactory::createDirectionalLight() {
    EntityObject object{m_world};
    auto light = object.add<Light>();
    light->type = LightType::Directional;
    return object;
}

GLAB_NAMESPACE_END()
