#version 330 core

in vec4 v_Color;
in vec2 v_UV;

out vec4 o_Color;

uniform sampler2D u_Tex;

void main() {
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(u_Tex, v_UV).r);
    o_Color = v_Color * sampled;
}
