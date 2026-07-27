layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;

layout(std140) uniform Frame {
    float time;
} frame;

layout(std140) uniform Object {
    mat4 model_matrix;
} object;

layout(std140) uniform Camera {
    vec4 position;
    mat4 view_projection_matrix;
} camera;

out vec3 world_normal;
out vec3 view_direction;

void main() {
    vec3 world_position = vec3(object.model_matrix * vec4(in_position, 1.0));
    world_normal = mat3(transpose(inverse(object.model_matrix))) * in_normal;
    view_direction = vec3(camera.position) - world_position;
    gl_Position = camera.view_projection_matrix * vec4(in_position, 1.0);
}
