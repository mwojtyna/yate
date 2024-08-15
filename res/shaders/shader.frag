#version 330 core

in vec4 v_Color;
in vec2 v_UV;
flat in uint v_TexId;

out vec4 o_Color;

uniform sampler2DArray u_Textures;

void main() {
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(u_Textures, vec3(v_UV.xy, v_TexId)).r);
    o_Color = /* v_Color * */ sampled;
}
