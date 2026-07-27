#include "geometry_helper.h"

#include "../gl.h"
#include "resource_manager.h"

GLAB_NAMESPACE_BEGIN()

ResourceHandle<Mesh> GeometryHelper::buildCube(float size) {
    auto cube = ResourceManager::instance().make<Mesh>();
    float half_size = size * 0.5f;

    cube->vertices.push_back({{half_size, half_size, half_size}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}});
    cube->vertices.push_back(
        {{-half_size, half_size, half_size}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}});
    cube->vertices.push_back(
        {{-half_size, -half_size, half_size}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}});
    cube->vertices.push_back(
        {{half_size, -half_size, half_size}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}});
    cube->vertices.push_back(
        {{half_size, half_size, -half_size}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}});
    cube->vertices.push_back(
        {{-half_size, half_size, -half_size}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}});
    cube->vertices.push_back(
        {{-half_size, -half_size, -half_size}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}});
    cube->vertices.push_back(
        {{half_size, -half_size, -half_size}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}});

    cube->indices.insert(cube->indices.end(),
                         {0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4, 4, 5, 1, 1, 0, 4,
                          7, 6, 2, 2, 3, 7, 4, 0, 3, 3, 7, 4, 5, 1, 2, 2, 6, 5});

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
