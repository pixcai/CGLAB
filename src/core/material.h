#pragma once

#include <vector>
#include <unordered_map>

#include "resource.h"
#include "shader.h"

GLAB_NAMESPACE_BEGIN()

struct Material : IResource {
    Material() : IResource(ResourceType::Material) {}

    void bind(Shader& shader) {}

    void unbind() {}

    void destroy() override {}

private:
    std::vector<std::byte> m_storage;
    std::unordered_map<std::string, int> m_properties;
};

GLAB_NAMESPACE_END()
