#include <renderGraph/renderGraph.hpp>

gfx::UniformRingBuffer uniforms;

// GLOBAL DATA
utils::Handle<gfx::BindGroupLayout> globalsLayout;
utils::Handle<gfx::BindGroup> globalsGroup;
utils::Handle<gfx::Buffer> globalsBuffer;

//FRAMEBUFFERS AND RENDER TEXTURES
utils::Handle<gfx::Texture> mainColorTexture;
utils::Handle<gfx::Texture> mainDepthTexture;
utils::Handle<gfx::FrameBuffer> mainFrameBuffer;

//MAIN PASS
utils::Handle<gfx::BindGroupLayout> mainBindLayout;
utils::Handle<gfx::BindGroup> mainBindGroup;
utils::Handle<gfx::Buffer> mainLightBuffer;
utils::Handle<gfx::RenderPass> mainRenderPass;
utils::Handle<gfx::RenderPassLayout> mainRenderLayout;
utils::Handle<gfx::Shader> mainShader;


void renderGraph::init()
{ 
	uniforms = gfx::UniformRingBuffer(32 * 1024 * 1024);

	Vec3 eye = Vec3(0.0, 0.0, -1.0);
	Vec3 target = Vec3(0.0, 0.0, 0.0);
	Vec3 up = Vec3(0.0, 1.0, 0.0);
	Mat4 view = glm::lookAt(eye, target, up);
	Mat4 proj = glm::perspectiveZO(glm::radians(60.0f), 21.0f / 9.0f, 0.01f, 100.0f);

	Globals globals = {
		.viewProj = proj * view,
		.cameraPos = eye
	};

	globalsBuffer = gfx::ResourceManager::instance->Create(gfx::BufferDescriptor{
		.usage = gfx::BufferUsage::UNIFORM,
		.byteSize = sizeof(Globals),
		.initialData = utils::Span(reinterpret_cast<const uint8_t*>(&globals), sizeof(globals))
		});
	globalsLayout = gfx::ResourceManager::instance->Create(gfx::BindGroupLayoutDescriptor{
		.bufferBindings = {
			{.slot = 0}
		}
		});
	globalsGroup = gfx::ResourceManager::instance->Create(gfx::BindGroupDescriptor{
		.layout = globalsLayout,
		.buffers = {
			{.slot = 0, .buffer = globalsBuffer, .range = sizeof(Globals)}
		}
		}); 

	mainColorTexture = gfx::ResourceManager::instance->Create(gfx::TextureDescriptor{
		.format = gfx::TextureFormat::RGBA8_UNORM,
		.usage = gfx::TextureUsage::TEXTURE_BINDING | gfx::TextureUsage::RENDER_ATTACHMENT | gfx::TextureUsage::COPY_DST,
		.width = 3440,
		.height = 1440,
		});
	mainDepthTexture = gfx::ResourceManager::instance->Create(gfx::TextureDescriptor{
		.format = gfx::TextureFormat::D32_FLOAT,
		.usage = gfx::TextureUsage::TEXTURE_BINDING | gfx::TextureUsage::RENDER_ATTACHMENT | gfx::TextureUsage::COPY_DST,
		.width = 3440,
		.height = 1440,
		});
	mainFrameBuffer = gfx::ResourceManager::instance->Create(gfx::FrameBufferDescriptor{
		.depthTarget = mainDepthTexture,
		.colorTargets = {
			{mainColorTexture}
		}
		});
}

void renderGraph::render(Meshes& meshes)
{ 
	// OFFSCREEN RENDERING
	//{
	//	gfx::CommandBuffer* command = gfx::Renderer::instance->BeginCommandRecording(gfx::CommandBufferType::OFFSCREEN);
	//	offscreenStage::pass::render(command, meshes);
	//	command->Submit();
	//}

	// MAIN RENDERING
	{
		gfx::CommandBuffer* command = gfx::Renderer::instance->BeginCommandRecording(gfx::CommandBufferType::MAIN);
		mainStage::mainPass::render(command, meshes);
		mainStage::surfacePass::render(command);
		command->Submit();
	}
}

void renderGraph::destroy()
{  
	uniforms.Destroy();

	gfx::ResourceManager::instance->Remove(globalsLayout);
	gfx::ResourceManager::instance->Remove(globalsBuffer);
	gfx::ResourceManager::instance->Remove(globalsGroup); 

	gfx::ResourceManager::instance->Remove(mainColorTexture);
	gfx::ResourceManager::instance->Remove(mainDepthTexture);
	gfx::ResourceManager::instance->Remove(mainFrameBuffer);
}



void offscreenStage::pass::init()
{
}

void offscreenStage::pass::render(gfx::CommandBuffer* cmdBuf, Meshes& meshes)
{
}

void offscreenStage::pass::destroy()
{
}



void mainStage::mainPass::init()
{ 
	Light light{
		.color = Vec3(1.0f, 0.5f, 0.0f),
		.intensity = 1.0f,
		.pos = Vec3(30.0f, 30.0f, 30.0f)
	};

	mainLightBuffer = gfx::ResourceManager::instance->Create(gfx::BufferDescriptor{
		.usage = gfx::BufferUsage::UNIFORM,
		.byteSize = sizeof(Light),
		.initialData = utils::Span(reinterpret_cast<const uint8_t*>(&light), sizeof(Light))
		});
	mainBindLayout = gfx::ResourceManager::instance->Create(gfx::BindGroupLayoutDescriptor{
		.bufferBindings = {
			{.slot = 0}
		}
		});
	mainBindGroup = gfx::ResourceManager::instance->Create(gfx::BindGroupDescriptor{
		.layout = mainBindLayout,
		.buffers = {
			{.slot = 0, .buffer = mainLightBuffer, .range = sizeof(Light)}
		}
		});

	mainRenderLayout = gfx::ResourceManager::instance->Create(gfx::RenderPassLayoutDescriptor{
	.depth = {
		.depthTarget = true,
		.depthTargetFormat = gfx::TextureFormat::D32_FLOAT
		},
	.colorTargets = {
		{.enabled = true, .format = gfx::TextureFormat::RGBA8_UNORM}
		}
		});
	mainRenderPass = gfx::ResourceManager::instance->Create(gfx::RenderPassDescriptor{
		.depthTarget = {
			.loadOp = gfx::LoadOperation::CLEAR,
			.storeOp = gfx::StoreOperation::STORE,
			.stencilLoadOp = gfx::LoadOperation::DONT_CARE,
			.stencilStoreOp = gfx::StoreOperation::DONT_CARE,
			.clearZ = 1.0
		},
		.colorTargets = {
			{.loadOp = gfx::LoadOperation::CLEAR, .storeOp = gfx::StoreOperation::STORE}
		},
		.layout = mainRenderLayout,
		});

}

void mainStage::mainPass::render(gfx::CommandBuffer* cmdBuf, Meshes& meshes)
{ 
	std::vector<gfx::Draw> stream;
	for (const auto& mesh : meshes)
	{
		auto alloc = uniforms.BumpAllocate<CallData>();
		alloc.ptr->model = mesh.model;
		alloc.ptr->inverseModel = mesh.inverseModel;

		stream.push_back({
			.shader = mainShader,
			.bindGroups = { globalsGroup, utils::Handle<gfx::BindGroup>(), mainBindGroup },
			.dynamicBuffer = uniforms.GetBuffer(),
			.indexBuffer = mesh.index,
			.vertexBuffers = { mesh.vertex1, mesh.vertex2 },
			.dynamicBufferOffset = alloc.offset,
			.triangleCount = mesh.triangles
			});
	} 

	uniforms.Upload();
	cmdBuf->BeginRenderPass(mainRenderPass, mainFrameBuffer, utils::Span<gfx::Draw>(stream.data(), stream.size()));
}

void mainStage::mainPass::destroy()
{
	gfx::ResourceManager::instance->Remove(mainLightBuffer);
	gfx::ResourceManager::instance->Remove(mainBindLayout);
	gfx::ResourceManager::instance->Remove(mainBindGroup);
	gfx::ResourceManager::instance->Remove(mainRenderLayout);
	gfx::ResourceManager::instance->Remove(mainRenderPass);
}



void mainStage::surfacePass::init()
{
}

void mainStage::surfacePass::render(gfx::CommandBuffer* cmdBuf)
{
}

void mainStage::surfacePass::destroy()
{
}
