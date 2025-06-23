#version 450

layout(location = 0) in vec3 in_position;
layout(location = 0) out vec3 out_position; 

layout(set = 0, binding = 0) uniform globals {
    mat4 viewProj;
    vec3 cameraPos;
} u_globals;

layout(set = 3, binding = 0) uniform draw {
    mat4 model;
    mat4 inverseModel;
} u_draw;

void main() {
    // NDC position
    gl_Position = u_globals.viewProj * u_draw.model * vec4(in_position, 1.0);
    // Map from NDC [-1,1] to UV [0,1]
    out_position = in_position;
}