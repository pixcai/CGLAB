#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "camera.h"
#include "framebuffer.h"
#include "rendering.h"
#include "transform.h"
#include "ubo_pool.h"

GLAB_NAMESPACE_BEGIN()

class Renderer {
public:
    Renderer() = default;
    Renderer(int width, int height);

    void resize(int width, int height);

    void render(const std::vector<RenderItem>& render_items, Camera& camera,
                const Transform& transform);

    void setClearColor(glm::vec4 clear_color);
    GLuint texture() const noexcept;

private:
    void beginFrame();
    void endFrame();
    void buildCommandQueue(const std::vector<RenderItem>& render_items);
    void sortOpaque();
    void sortTransparent();
    DrawCommand createCommand(const RenderItem& render_item);
    void executeCommands(const std::vector<DrawCommand>& commands, const Camera& camera);

private:
    glm::vec4 m_clear_color{0.0f, 0.0f, 0.0f, 1.0f};
    UniformFrame m_uniform_frame{};
    UniformObject m_uniform_object{};
    UniformCamera m_uniform_camera{};
    Framebuffer m_framebuffer{};
    std::vector<DrawCommand> m_opaque_queue;
    std::vector<DrawCommand> m_transparent_queue;
};

GLAB_NAMESPACE_END()
