#version 450

layout(location = 0) in vec3 in_position; 
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec3 in_color;
layout(location = 3) in vec3 in_tangentLightPos;
layout(location = 4) in vec3 in_tangentViewPos;
layout(location = 5) in vec3 in_tangentFragPos;

layout(location = 0) out vec4 out_color;

layout(set = 0, binding = 0) uniform globals {
    mat4 viewProj;
    vec3 cameraPos;
} u_globals;

layout(set = 2, binding = 0) uniform Light {
    vec3 lightColor;
    float lightIntensity;
    vec3 lightPos;
} u_light;

void main() {
    vec3 ambient = 0.05 * in_color;

    vec3 normal = normalize(in_normal);

    vec3 lightDir = normalize(in_tangentLightPos - in_tangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * in_color;

    vec3 viewDir = normalize(in_tangentViewPos - in_tangentFragPos);
    vec3 reflectDir = reflect(-lightDir, normal);

    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    vec3 specular = u_light.lightColor * spec; // assuming bright white light color
    out_color = vec4(ambient + diffuse + specular, 1.0);
}