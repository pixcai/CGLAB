#pragma once

#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include "common.h"

GLAB_NAMESPACE_BEGIN()

enum class CameraType {
    Perspective,
    Orthographics,
};

struct Camera : IComponent {
    CameraType type{CameraType::Perspective};

    glm::mat4 view_matrix{1.0f};
    glm::mat4 projection_matrix{1.0f};
    glm::mat4 view_projection_matrix{1.0f};

    float near{0.1f};
    float far{1000.0f};

    // Perspective camera fields
    float fovy{45.0f};
    int width{1}, height{1};

    // Orthographics camera fields
    float left{-1.0f}, right{1.0f}, bottom{-1.0f}, top{1.0f};

    bool active{true};

    void updateViewMatrix(const glm::mat4& camera_world_matrix) {
        view_matrix = glm::inverse(camera_world_matrix);
        view_projection_matrix = projection_matrix * view_matrix;
    }

    void updateProjectionMatrix() {
        if (type == CameraType::Orthographics) {
            projection_matrix = glm::ortho(left, right, bottom, top);
        } else {
            projection_matrix =
                glm::perspective(glm::radians(fovy), (float)width / height, near, far);
        }
        view_projection_matrix = projection_matrix * view_matrix;
    }
};

GLAB_NAMESPACE_END()
