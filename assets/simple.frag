#version 450

// Fragment Shader
layout(location = 0) in vec2 uv; // Input color with location 0

layout(set = 0, binding = 0) uniform texture2D meTexture;
layout(set = 0, binding = 1) uniform sampler meSampler;
layout(set = 3, binding = 0) uniform DynamicBuffer { 
	vec4 color;
} u_Color;

layout(location = 0) out vec4 color;    // Output color with location 0

void main() {

	color = texture(sampler2D(meTexture, meSampler), uv) * u_Color.color; 
}