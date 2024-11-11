#version 330 core

layout(location = 0) in vec3 i_Pos;
layout(location = 1) in vec4 i_Color;
layout(location = 2) in vec2 i_UV;
layout(location = 3) in int i_Bg; // bool

out vec4 v_Color;
out vec2 v_UV;
flat out int v_Bg; // bool

uniform mat4 u_MVP;

void main() {
    gl_Position = u_MVP * vec4(i_Pos, 1.0);
    v_Color = i_Color;
    v_UV = i_UV;
    v_Bg = i_Bg;
}
