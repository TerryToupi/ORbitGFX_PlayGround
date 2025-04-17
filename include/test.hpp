#include <window.hpp>
#include <device.hpp>

#include <resources/resourceManger.hpp>
#include <render/renderer.hpp>

#include <enums.hpp>
#include <handle.hpp>

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
utils::Handle<gfx::BindGroup> bindGroup;
utils::Handle<gfx::BindGroupLayout> bindGroupLayout;
gfx::TextureFormat surfaceFormat;

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

static const uint32_t indexData[] = {
    // First triangle
    0, 1, 2, // Bottom-left, Bottom-right, Top-left

    // Second triangle
    1, 3, 2  // Bottom-right, Top-right, Top-left
};

static const float uvData[] = {
    // First triangle
    0.0f, 0.0f, // Bottom-left
    1.0f, 0.0f, // Bottom-right
    0.0f, 1.0f, // Top-left

    // Second triangle
    1.0f, 0.0f, // Bottom-right
    1.0f, 1.0f, // Top-right
    0.0f, 1.0f  // Top-left
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