#include <window.hpp>
#include <device.hpp>

#include <resources/resourceManger.hpp>
#include <render/renderer.hpp>
#include <render/uniforms.hpp>

#include <loaders/imageLoader.hpp>
#include <loaders/shaderModuleLoader.hpp>

#include <enums.hpp>
#include <handle.hpp>

#include <iostream>

utils::Handle<gfx::RenderPassLayout> renderPassLayout;
utils::Handle<gfx::RenderPass> renderPass;
utils::Handle<gfx::Shader> shader;
utils::Handle<gfx::Shader> shader2;
utils::Handle<gfx::Buffer> vertexBuffer1;
utils::Handle<gfx::Buffer> vertexBuffer2;
utils::Handle<gfx::Buffer> indexBuffer;
utils::Handle<gfx::Buffer> uvBuffer;
utils::Handle<gfx::Buffer> normalBuffer;
utils::Handle<gfx::Texture> mainTexture;
utils::Handle<gfx::Sampler> mainSampler;
utils::Handle<gfx::FrameBuffer> mainFrame;
utils::Handle<gfx::Texture> frameTexture;
utils::Handle<gfx::BindGroup> bindGroup;
utils::Handle<gfx::BindGroupLayout> bindGroupLayout;
gfx::TextureFormat surfaceFormat; 

gfx::UniformRingBuffer* uniformBuffer; 

struct Color
{
    float r = 0;
    float g = 0;
    float b = 0; 
    float a = 0;
};

static const float topLeftQuad[] = {
    // Unique vertices for the top-left quad
    -0.9f,  0.0f, 0.0f, // 0: Bottom-left
     0.0f,  0.0f, 0.0f, // 1: Bottom-right
    -0.9f,  0.9f, 0.0f, // 2: Top-left
     0.0f,  0.9f, 0.0f  // 3: Top-right
};

static const float bottomRightQuad[] = {
    // Unique vertices for the bottom-right quad
     0.0f, -0.9f, 0.0f, // 0: Bottom-left
     0.9f, -0.9f, 0.0f, // 1: Bottom-right
     0.0f,  0.0f, 0.0f, // 2: Top-left
     0.9f,  0.0f, 0.0f  // 3: Top-right
}; 

static const float fsQuad[] = {
    -0.9f,  0.9f, 0.0f, // Top-left
     0.9f,  0.9f, 0.0f, // Top-right
     0.9f, -0.9f, 0.0f, // Bottom-right
    -0.9f, -0.9f, 0.0f  // Bottom-left
};

static const uint32_t indexData[] = {
    0, 1, 2, // First triangle (Top-left, Top-right, Bottom-right)
    2, 3, 0  // Second triangle (Bottom-right, Bottom-left, Top-left)
};

static const float uvData[] = {
    0.0f, 0.0f, // Top-left
    1.0f, 0.0f, // Top-right
    1.0f, 1.0f, // Bottom-right
    0.0f, 1.0f  // Bottom-left
};

static const float normalData[] = {
    // First triangle
    0.0f, 0.0f, 1.0f, // Normal for Bottom-left
    0.0f, 0.0f, 1.0f, // Normal for Bottom-right
    0.0f, 0.0f, 1.0f, // Normal for Top-left

    // Second triangle
    0.0f, 0.0f, 1.0f, // Normal for Bottom-right
    0.0f, 0.0f, 1.0f, // Normal for Top-right
    0.0f, 0.0f, 1.0f  // Normal for Top-left
};