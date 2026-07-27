layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;

layout(std140) uniform Frame {
    float time;
};

layout(std140) uniform Camera {
    mat4 view_projection_matrix;
};

void main() {
    gl_Position = view_projection_matrix * vec4(in_position, 1.0);
}
