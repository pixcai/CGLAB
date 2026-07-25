add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", {outputdir = "$(builddir)"})

set_languages("cxx23")

add_requires("imgui v1.92.7-docking", {configs = {opengl3 = true, glfw = true}})
add_requires("glad", {configs = {profile = "core"}})
add_requires("glfw", "glm", "stb", "spdlog", "tracy v0.13.0")

target("GLAB")
    set_kind("binary")
    set_rundir("$(projectdir)")
    add_files("src/**.cpp")
    -- Use GLAD instead of system OpenGL header
    add_defines("GLFW_INCLUDE_NONE")
    if is_mode("debug") then
        add_defines("TRACY_ENABLE")
    end
    add_packages("imgui","glad", "glfw", "glm", "stb", "spdlog", "tracy")
