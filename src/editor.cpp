#include "editor.h"

#include <cstdio>

#include <imgui.h>
#include <imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>
#include <tracy/Tracy.hpp>

#include "core/camera.h"
#include "core/geometry_helper.h"
#include "core/material.h"
#include "core/renderer.h"
#include "core/resource_manager.h"
#include "core/scene.h"
#include "core/shader_manager.h"
#include "core/transform.h"
#include "core/ubo_pool.h"
#include "editor_context.h"

GLAB_NAMESPACE_BEGIN()

static EditorContext g_editor_context;

EditorContext& EditorContext::instance() noexcept { return g_editor_context; }

static Camera g_camera;
static Transform g_transform;

static float yaw, pitch;

static Material* cube_material;
static glm::vec4 cube_color;

Editor::Editor(GLFWwindow* window) {
    ShaderManager::instance().init();
    UBOPool::instance().init();

    auto renderer = new Renderer();
    auto scene = new Scene();

    auto object = scene->createObject();
    auto mesh_renderer = object.add<MeshRenderer>();
    mesh_renderer->mesh_handle = GeometryHelper::buildCube();
    mesh_renderer->material_handle = ResourceManager::instance().make<Material>();
    cube_material = mesh_renderer->material_handle.get();
    scene->addObject(object);

    g_editor_context.renderer = renderer;
    g_editor_context.scene = scene;

    g_transform.position = glm::vec3(0.0f, 2.0f, 4.0f);
    g_transform.lookAt(glm::vec3(0.0f, 0.0f, 0.0f));
    g_transform.updateLocalMatrix();

    g_camera.updateViewMatrix(g_transform.local_matrix);
    cube_color = cube_material->get<glm::vec4>("albedo");
}

Editor::~Editor() {
    ShaderManager::instance().destroy();
    UBOPool::instance().destroy();
    delete g_editor_context.renderer;
    delete g_editor_context.scene;
}

void Editor::render() {
    FrameMark;

    auto renderer = g_editor_context.renderer;
    auto scene = g_editor_context.scene;
    auto render_items = scene->collectRenderItems();
    {
        ZoneScopedN("Render");
        renderer->setClearColor(scene->clear_color);
        renderer->render(render_items, g_camera);
        ResourceManager::instance().flushDestroyQueue();
    }

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGuiID dockspace = ImGui::GetID("DockSpace");

    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Root", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
                     ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_MenuBar);
    renderMainMenuBar();
    ImGui::DockSpace(dockspace);
    ImGui::End();
    ImGui::PopStyleVar();
    renderStatusBar();

    renderWorkspace();
    renderInspector();

    static bool initialized{false};

    if (!initialized) {
        initLayout();
        initialized = true;
    }
}

void Editor::renderMainMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void Editor::renderStatusBar() {
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + viewport->Size.y - 32.0f));
    ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, 32.0f));

    char info[32];
    snprintf(info, sizeof(info), "(%d, %d) | FPS %d", (int)io.MousePos.x, (int)io.MousePos.y,
             (int)io.Framerate);

    float info_width = ImGui::CalcTextSize(info).x;

    ImGui::Begin("StatusBar", nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar);
    ImGui::SetCursorPosX(viewport->Pos.x + viewport->Size.x - style.WindowPadding.x - info_width);
    ImGui::Text("%s", info);
    ImGui::End();
}

void Editor::initLayout() {
    ImGuiID dockspace = ImGui::GetID("DockSpace");
    ImGuiID workspace, inspector;

    ImGui::DockBuilderRemoveNode(dockspace);
    ImGui::DockBuilderAddNode(dockspace, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(dockspace, ImGui::GetMainViewport()->Size);

    ImGui::DockBuilderSplitNode(dockspace, ImGuiDir_Left, 0.75f, &workspace, &inspector);
    ImGui::DockBuilderDockWindow("Workspace", workspace);
    ImGui::DockBuilderDockWindow("Inspector", inspector);
    ImGui::DockBuilderFinish(dockspace);
}

void Editor::renderWorkspace() {
    auto renderer = g_editor_context.renderer;
    ImGuiIO& io = ImGui::GetIO();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Workspace", nullptr, ImGuiWindowFlags_MenuBar);

    if (ImGui::IsWindowHovered()) {
        if (io.MouseWheelH != 0.0f || io.MouseWheel != 0.0f) {
            yaw += io.MouseWheelH * 0.01f;
            pitch += io.MouseWheel * 0.01f;
            g_transform.rotation = glm::quat(glm::vec3(pitch, yaw, 0.0f));
            g_transform.updateLocalMatrix();
            g_camera.updateViewMatrix(g_transform.local_matrix);
        }
    }

    ImGui::BeginMenuBar();
    ImGui::EndMenuBar();

    auto size = ImGui::GetContentRegionAvail();
    renderer->resize((int)size.x, (int)size.y);
    ImGui::Image((ImTextureID)(std::intptr_t)renderer->texture(), size, ImVec2(0.0f, 1.0f),
                 ImVec2(1.0f, 0.0f));

    ImGui::End();
    ImGui::PopStyleVar();
}

void Editor::renderInspector() {
    auto scene = g_editor_context.scene;

    ImGui::Begin("Inspector");
    ImGui::ColorEdit4("Scene color", glm::value_ptr(scene->clear_color));
    if (ImGui::ColorEdit4("Cube color", glm::value_ptr(cube_color))) {
        cube_material->set("albedo", cube_color);
        UBOPool::instance().updateMaterial(*cube_material);
    }
    ImGui::End();
}

GLAB_NAMESPACE_END()
