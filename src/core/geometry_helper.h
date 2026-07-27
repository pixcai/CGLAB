#pragma once

#include "mesh.h"

GLAB_NAMESPACE_BEGIN()

struct GeometryHelper {
    static ResourceHandle<Mesh> buildCube(float size = 2.0f);
};

GLAB_NAMESPACE_END()
