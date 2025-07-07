#version 450

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec4 in_tangent;
layout(location = 2) in vec4 in_normal;
layout(location = 3) in vec4 in_color;
layout(location = 4) in vec2 in_uv;

layout(location = 0) out vec3 out_position; 
layout(location = 1) out vec3 out_normal;
layout(location = 2) out vec3 out_color;

layout(set = 0, binding = 0) uniform globals {
    mat4 viewProj;
    vec3 cameraPos;
} u_globals;

layout(set = 1, binding = 0) uniform texture2D u_diffuse;
layout(set = 1, binding = 1) uniform sampler u_sampler;

layout(set = 2, binding = 0) uniform Light {
    vec3 lightColor;
    float lightIntensity;
    vec3 lightPos;
} u_light;

layout(set = 3, binding = 0) uniform draw {
    mat4 model;
    mat4 inverseModel;
} u_draw;

void main() 
{
    gl_Position = u_globals.viewProj * u_draw.model * vec4(in_position, 1.0);

    out_position = vec3(u_draw.model * vec4(in_position, 1.0));
    out_color = texture(sampler2D(u_diffuse, u_sampler), in_uv).xyz;
    out_normal = in_normal.xyz;
}