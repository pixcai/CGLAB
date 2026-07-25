#include "editor.h"

#include <cstdio>

#include <imgui.h>
#include <imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>
#include <tracy/Tracy.hpp>

#include "core/geometry_helper.h"
#include "core/material.h"
#include "core/renderer.h"
#include "core/resource_manager.h"
#include "core/scene.h"
#include "core/shader_manager.h"
#include "editor_context.h"

GLAB_NAMESPACE_BEGIN()

static EditorContext g_editor_context;

EditorContext& EditorContext::instance() noexcept { return g_editor_context; }

Editor::Editor(GLFWwindow* window) {
    ShaderManager::instance().init();

    auto renderer = new Renderer();
    auto scene = new Scene();

    auto object = scene->createObject();
    auto mesh_renderer = object.add<MeshRenderer>();
    mesh_renderer->mesh_handle = GeometryHelper::buildCube();
    mesh_renderer->material_handle = ResourceManager::instance().make<Material>();
    scene->addObject(object);

    g_editor_context.renderer = renderer;
    g_editor_context.scene = scene;
}

Editor::~Editor() {
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
        renderer->render(render_items);
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

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Workspace", nullptr, ImGuiWindowFlags_MenuBar);

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
    ImGui::End();
}

GLAB_NAMESPACE_END()
