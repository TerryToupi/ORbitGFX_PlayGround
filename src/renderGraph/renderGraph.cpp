#include <renderGraph/renderGraph.hpp>
#include <resources/resourceManger.hpp>
#include <render/uniforms.hpp>

utils::Handle<gfx::BindGroupLayout> globalsLayout;
utils::Handle<gfx::BindGroup> globalsGroup;
utils::Handle<gfx::Buffer> globalsBuffer;
gfx::UniformRingBuffer* uniforms;

void renderGraph::init()
{
	uniforms = new gfx::UniformRingBuffer(32 * 1024 * 1024);

	globalsBuffer = gfx::ResourceManager::instance->Create(gfx::BufferDescriptor{
		.usage = gfx::BufferUsage::UNIFORM,
		.byteSize = sizeof(renderGlobals::data)
		});

	globalsLayout = gfx::ResourceManager::instance->Create(gfx::BindGroupLayoutDescriptor{
		.bufferBindings = {
			{.slot = 0}
		}
		});

	globalsGroup = gfx::ResourceManager::instance->Create(gfx::BindGroupDescriptor{
		.layout = globalsLayout,
		.buffers = {
			{.slot = 0, .buffer = globalsBuffer, .range = sizeof(renderGlobals::data)}
		}
		});
}

void renderGraph::render(Meshes& meshes)
{
}

void renderGraph::destroy()
{
	uniforms->Destroy();
	delete uniforms;

	gfx::ResourceManager::instance->Remove(globalsLayout);
	gfx::ResourceManager::instance->Remove(globalsBuffer);
	gfx::ResourceManager::instance->Remove(globalsGroup);
}

