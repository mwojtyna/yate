#version 330 core

in vec4 v_Color;
in vec2 v_UV;
flat in int v_Bg; // bool

out vec4 o_Color;

uniform sampler2D u_Tex;

void main() {
    if (v_Bg == 1) {
        o_Color = v_Color;
    } else {
        o_Color = vec4(v_Color.rgb, texture(u_Tex, v_UV).r * v_Color.a);
    }
}
