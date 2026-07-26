layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;

uniform Frame {
    float time;
} ubo_frame;

uniform Camera {
    mat4 view_matrix;
    mat4 projection_matrix;
    mat4 view_projection_matrix;

    vec4 position;
    vec4 viewport;
} ubo_camera;

struct MaterialData {
    vec4 base_color;
};

uniform Material {
    MaterialData data[1];
} ubo_material;

void main() {
    gl_Position = ubo_camera.view_projection_matrix * vec4(in_position, 1.0);
}
