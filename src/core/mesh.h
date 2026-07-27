#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "../gl.h"
#include "material.h"
#include "resource.h"

GLAB_NAMESPACE_BEGIN()

struct Vertex {
    glm::vec3 position{0.0f, 0.0f, 0.0f};
    glm::vec3 normal{0.0f, 0.0f, 0.0f};
    glm::vec2 uv{0.0f, 0.0f};
};

struct Mesh : IResource {
    GLuint vao{0}, vbo{0}, ebo{0};

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    Mesh() : IResource(ResourceType::Mesh) {}

    void init() override {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void*)offsetof(Vertex, position));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void*)offsetof(Vertex, uv));
        glEnableVertexAttribArray(2);

        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);

        glBindVertexArray(0);
    }

    void destroy() override;
};

struct MeshRenderer : IComponent {
    ResourceHandle<Mesh> mesh_handle;
    ResourceHandle<Material> material_handle;
};

GLAB_NAMESPACE_END()
