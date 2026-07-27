#include "geometry_helper.h"

#include "../gl.h"
#include "resource_manager.h"

GLAB_NAMESPACE_BEGIN()

ResourceHandle<Mesh> GeometryHelper::buildCube(float size) {
    auto cube = ResourceManager::instance().make<Mesh>();
    float half_size = size * 0.5f;

    // front
    cube->vertices.push_back({{half_size, half_size, half_size}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}});
    cube->vertices.push_back(
        {{-half_size, half_size, half_size}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}});
    cube->vertices.push_back(
        {{-half_size, -half_size, half_size}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}});
    cube->vertices.push_back(
        {{half_size, -half_size, half_size}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}});
    // back
    cube->vertices.push_back(
        {{half_size, half_size, -half_size}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}});
    cube->vertices.push_back(
        {{-half_size, half_size, -half_size}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}});
    cube->vertices.push_back(
        {{-half_size, -half_size, -half_size}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}});
    cube->vertices.push_back(
        {{half_size, -half_size, -half_size}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}});
    // top
    cube->vertices.push_back(
        {{half_size, half_size, -half_size}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}});
    cube->vertices.push_back(
        {{-half_size, half_size, -half_size}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}});
    cube->vertices.push_back(
        {{-half_size, half_size, half_size}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}});
    cube->vertices.push_back({{half_size, half_size, half_size}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}});
    // bottom
    cube->vertices.push_back(
        {{half_size, -half_size, -half_size}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}});
    cube->vertices.push_back(
        {{-half_size, -half_size, -half_size}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}});
    cube->vertices.push_back(
        {{-half_size, -half_size, half_size}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}});
    cube->vertices.push_back(
        {{half_size, -half_size, half_size}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}});
    // left
    cube->vertices.push_back(
        {{-half_size, half_size, -half_size}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}});
    cube->vertices.push_back(
        {{-half_size, half_size, half_size}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}});
    cube->vertices.push_back(
        {{-half_size, -half_size, half_size}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}});
    cube->vertices.push_back(
        {{-half_size, -half_size, -half_size}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}});
    // right
    cube->vertices.push_back(
        {{half_size, half_size, -half_size}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}});
    cube->vertices.push_back({{half_size, half_size, half_size}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}});
    cube->vertices.push_back(
        {{half_size, -half_size, half_size}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}});
    cube->vertices.push_back(
        {{half_size, -half_size, -half_size}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}});

    // clang-format off
    cube->indices.insert(cube->indices.end(), {
                         // front
                         0, 1, 2, 2, 3, 0,
                         // back
                         4, 5, 6, 6, 7, 4,
                         // top
                         8, 9, 10, 10, 11, 8,
                         // bottom
                         12, 13, 14, 14, 15, 12,
                         // left
                         16, 17, 18, 18, 19, 16,
                         // right
                         20, 21, 22, 22, 23, 20,
                     });
    // clang-format on

    glBindVertexArray(cube->vao);

    glBindBuffer(GL_ARRAY_BUFFER, cube->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * cube->vertices.size(), cube->vertices.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * cube->indices.size(),
                 cube->indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
    return cube;
}

GLAB_NAMESPACE_END()
