#include "renderer.h"

#include "../gl.h"
#include "shader_manager.h"

GLAB_NAMESPACE_BEGIN()

Renderer::Renderer(int width, int height) : m_framebuffer(width, height) {}

void Renderer::resize(int width, int height) { m_framebuffer.resize(width, height); }

void Renderer::setClearColor(glm::vec4 clear_color) { m_clear_color = clear_color; }

GLuint Renderer::texture() const noexcept { return m_framebuffer.texture(); }

void Renderer::beginFrame() {
    m_framebuffer.bind();
    glClearColor(m_clear_color.r, m_clear_color.g, m_clear_color.b, m_clear_color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::endFrame() {
    m_framebuffer.unbind();
    m_framebuffer.resolve();
}

void Renderer::render(const std::vector<RenderItem>& render_items, Camera& camera,
                      const Transform& transform) {
    if (!(camera.width == m_framebuffer.width() && camera.height == m_framebuffer.height())) {
        camera.width = m_framebuffer.width();
        camera.height = m_framebuffer.height();
        camera.updateProjectionMatrix();
    }
    m_uniform_camera.position = glm::vec4(transform.position, 1.0f);
    m_uniform_camera.view_projection_matrix = camera.view_projection_matrix;
    UBOPool::instance().updateFrame(m_uniform_frame);
    UBOPool::instance().updateCamera(m_uniform_camera);
    buildCommandQueue(render_items);
    glEnable(GL_DEPTH_TEST);
    beginFrame();
    executeCommands(m_opaque_queue, camera);
    executeCommands(m_transparent_queue, camera);
    endFrame();
}

void Renderer::buildCommandQueue(const std::vector<RenderItem>& render_items) {
    m_opaque_queue.clear();
    m_transparent_queue.clear();

    for (auto& render_item : render_items) {
        if (render_item.render_queue < 48) {
            m_opaque_queue.push_back(createCommand(render_item));
        } else {
            m_transparent_queue.push_back(createCommand(render_item));
        }
    }

    sortOpaque();
    sortTransparent();
}

void Renderer::sortOpaque() {}

void Renderer::sortTransparent() {}

DrawCommand Renderer::createCommand(const RenderItem& render_item) {
    DrawCommand command{};
    command.render_queue = render_item.render_queue;
    command.model_matrix = render_item.model_matrix;
    command.mesh_handle = render_item.mesh_handle;
    command.material_handle = render_item.material_handle;
    return command;
}

void Renderer::executeCommands(const std::vector<DrawCommand>& commands, const Camera& camera) {
    for (auto& command : commands) {
        auto mesh = command.mesh_handle.get();
        auto material = command.material_handle.get();
        auto& shader = ShaderManager::instance().get(material->shader_key);

        m_uniform_object.model_matrix = command.model_matrix;
        glBindVertexArray(mesh->vao);
        glUseProgram(shader.program());
        UBOPool::instance().updateObject(m_uniform_object);
        UBOPool::instance().updateMaterial(*material);
        glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, nullptr);
    }
    glBindVertexArray(0);
}

GLAB_NAMESPACE_END()
