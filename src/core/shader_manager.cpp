#include "shader_manager.h"

#include <format>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "../logger.h"
#include "shader.h"

GLAB_NAMESPACE_BEGIN()

static std::unordered_map<ShaderFeatureBits, std::string> g_define_map{
    {ShaderFeature_Lighting, "USE_LIGHTING"},
};

void ShaderManager::init() {
    static bool initialized{false};

    if (initialized) return;

    LOG_INFO("Build shaders");

    get(Shader::key(ShaderType::Bundle, ShaderFeature_Lighting));
    get(Shader::key(ShaderType::Wireframe, ShaderFeature_None));

    initialized = true;
}

void ShaderManager::destroy() {
    LOG_INFO("Destroy all shaders");
    for (auto& [_, shader] : m_shader_map) {
        shader.destroy();
    }
    m_shader_map.clear();
}

Shader& ShaderManager::get(std::uint64_t key) {
    ShaderType type{(std::uint32_t)(key >> 32)};
    ShaderFeatureBits features{(std::uint32_t)key};

    if (!m_shader_map.contains(key)) {
        compile(type, features);
    }
    return m_shader_map.at(key);
}

static std::string loadShaderFile(const std::string& path,
                                  const std::vector<std::string>& defines) {
    std::fstream file{path};

    if (!file.is_open()) {
        LOG_ERROR("Failed to open shader file: {}", path);
        return {};
    }

    std::stringstream buffer;
    buffer << GLAB_GLSL_VERSION << "\n\n";
    for (auto& define : defines) {
        buffer << std::format("#define {} 1", define) << "\n";
    }
    buffer << file.rdbuf();

    return buffer.str();
}

void ShaderManager::compile(ShaderType type, ShaderFeatureBits features) {
    std::vector<std::string> defines;

    for (auto& [feature, define] : g_define_map) {
        if (features & feature) {
            defines.push_back(define);
        }
    }

    std::string vs_path, fs_path;

    switch (type) {
        case ShaderType::Bundle:
            vs_path = "assets/shaders/bundle.vs.glsl";
            fs_path = "assets/shaders/bundle.fs.glsl";
            break;
        case ShaderType::Wireframe:
            vs_path = "assets/shaders/wireframe.vs.glsl";
            fs_path = "assets/shaders/wireframe.fs.glsl";
            break;
        default:
            break;
    }

    GLint success;
    char info_log[512];

    auto source = loadShaderFile(vs_path, defines);
    auto code = source.c_str();
    LOG_DEBUG("Source code of {}:", vs_path);
    LOG_DEBUG("\n{}", code);

    auto vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &code, nullptr);
    glCompileShader(vs);
    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vs, sizeof(info_log), nullptr, info_log);
        throw std::runtime_error(std::format("Failed to compile shader {}: {}", vs_path, info_log));
    }

    source = loadShaderFile(fs_path, defines);
    code = source.c_str();
    LOG_DEBUG("Source code of {}:", fs_path);
    LOG_DEBUG("\n{}", code);

    auto fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &code, nullptr);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fs, sizeof(info_log), nullptr, info_log);
        throw std::runtime_error(std::format("Failed to compile shader {}: {}", fs_path, info_log));
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, sizeof(info_log), nullptr, info_log);
        throw std::runtime_error(std::format("Failed to link program: {}", info_log));
    }

    m_shader_map.try_emplace(Shader::key(type, features), Shader(program));
}

GLAB_NAMESPACE_END()
