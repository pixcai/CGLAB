#pragma once

#include <GLFW/glfw3.h>

#include "namespace.h"

GLAB_NAMESPACE_BEGIN()

class Editor final {
public:
    Editor(GLFWwindow* window);
    ~Editor();

    void render();

private:
    void renderMainMenuBar();
    void renderStatusBar();
    void initLayout();
    void renderWorkspace();
    void renderOutliner();
    void renderProperties();
};

GLAB_NAMESPACE_END()
