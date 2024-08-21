#version 330 core

in vec4 v_Color;
in vec2 v_UV;

out vec4 o_Color;

uniform sampler2D u_Tex;

void main() {
    o_Color = vec4(v_Color.rgb, texture(u_Tex, v_UV).r);
}
