#version 330 core

layout(location = 0) in vec3 i_Pos;
layout(location = 1) in vec4 i_Color;

out vec4 v_FragColor;

uniform mat4 u_Transform;

void main() {
    gl_Position = u_Transform * vec4(i_Pos, 1.0);
    v_FragColor = i_Color;
}
