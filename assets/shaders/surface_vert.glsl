#version 450

layout(location = 0) in vec2 in_position;
layout(location = 0) out vec2 out_uv;

void main() {
    // NDC position
    gl_Position = vec4(in_position, 0.0, 1.0);
    // Map from NDC [-1,1] to UV [0,1]
    out_uv = in_position * 0.5 + 0.5;
}