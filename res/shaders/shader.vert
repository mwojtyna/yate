#version 330 core

layout(location = 0) in vec4 i_Pos;
layout(location = 1) in vec4 i_Color;
layout(location = 2) in vec2 i_UV;

out vec4 v_Color;
out vec2 v_UV;

uniform mat4 u_MVP;

void main() {
    gl_Position = u_MVP * i_Pos;
    v_Color = i_Color;
    v_UV = i_UV;
}
