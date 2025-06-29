#version 450

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec4 in_tangent;
layout(location = 2) in vec4 in_normal;
layout(location = 3) in vec4 in_color;
layout(location = 4) in vec2 in_uv;

layout(location = 0) out vec3 out_position; 
layout(location = 1) out vec3 out_normal;
layout(location = 2) out vec3 out_color;
layout(location = 3) out vec3 out_tangentLightPos;
layout(location = 4) out vec3 out_tangentViewPos;
layout(location = 5) out vec3 out_tangentFragPos;

layout(set = 0, binding = 0) uniform globals {
    mat4 viewProj;
    vec3 cameraPos;
} u_globals;

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
    out_color = in_color.xyz;

    mat3 normalMatrix = transpose(inverse(mat3(u_draw.model)));
    vec3 T = normalize(normalMatrix * in_tangent.xyz);
    vec3 N = normalize(normalMatrix * in_normal.xyz);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    mat3 TBN = transpose(mat3(T, B, N)); 
    out_tangentLightPos = TBN * u_light.lightPos;
    out_tangentViewPos  = TBN * u_globals.cameraPos;
    out_tangentFragPos  = TBN * out_position;

    out_normal = in_normal.xyz;
}