out vec4 out_color;

struct MaterialData {
    vec4 base_color;
    float metallic;
    float roughness;
};

layout(std140) uniform Material {
    MaterialData materials[1];
};

void main() {
    out_color = materials[0].base_color;
}
