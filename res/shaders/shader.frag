#version 330 core

in vec4 v_Color;
in vec2 v_UV;

out vec4 o_Color;

uniform sampler2D u_Tex;

const float fontPxRange = 2.0f;

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

float screenPxRange(float pxRange) {
    vec2 unitRange = vec2(pxRange) / vec2(textureSize(u_Tex, 0));
    vec2 screenTexSize = vec2(1.0) / fwidth(v_UV);
    return max(0.5 * dot(unitRange, screenTexSize), 1.0);
}

void main() {
    vec3 msd = texture(u_Tex, v_UV).rgb;
    float sd = median(msd.r, msd.g, msd.b);
    float screenPxDistance = screenPxRange(fontPxRange) * (sd - 0.5);
    float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);
    o_Color = mix(vec4(0.0), v_Color, opacity);
}
