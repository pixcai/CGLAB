#pragma once

#include <glm/glm.hpp>

#include "common.h"

GLAB_NAMESPACE_BEGIN()

enum class LightType {
    Directional,
    Point,
    Spot,
    Area,
};

struct Light : IComponent {
    LightType type{LightType::Directional};

    glm::vec4 color{1.0f, 1.0f, 1.0f, 1.0f};
    float intensity{1000.0f};
};

GLAB_NAMESPACE_END()
