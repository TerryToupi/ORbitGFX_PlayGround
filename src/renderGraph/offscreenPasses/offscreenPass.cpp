#include <renderGraph/offscreenPasses/offscreenPass.hpp>

utils::Handle<gfx::RenderPass> offRenderPass;
utils::Handle<gfx::RenderPassLayout> offRenderLayout;
utils::Handle<gfx::BindGroup> offBindGroup;
utils::Handle<gfx::BindGroupLayout> offBindLayout;
utils::Handle<gfx::Shader> offShader;


void offscreen::init(utils::Handle<gfx::BindGroupLayout> globals)
{
}

void offscreen::render(Meshes& meshes, utils::Handle<gfx::BindGroup> globals, gfx::UniformRingBuffer* uniforms)
{
}

void offscreen::destroy()
{
}
