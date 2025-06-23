#version 450
precision highp float;

layout(location = 0) in vec2 in_uv;
layout(location = 0) out vec4 out_color;

layout(set = 0, binding = 0) uniform texture2D u_texture;
layout(set = 0, binding = 1) uniform sampler u_sampler;

void main() {
    out_color = texture(sampler2D(u_texture, u_sampler),  1.0 - in_uv);
}