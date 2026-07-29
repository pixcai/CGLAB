#pragma once

#include <cstddef>

#include <glm/glm.hpp>

#include "common.h"
#include "../gl.h"

GLAB_NAMESPACE_BEGIN()

enum class LightType {
    Directional,
    Point,
    Spot,
    Area,
};

struct alignas(16) UniformLight {
    glm::vec4 color{1.0f, 1.0f, 1.0f, 1.0f};
    glm::vec4 direction{-0.5f, -0.8f, -0.6f, 1.0f};
};

struct Light : IComponent {
    LightType type{LightType::Directional};
    GLuint index{0};
    UniformLight property{};
    bool dirty{true};
};

GLAB_NAMESPACE_END()
