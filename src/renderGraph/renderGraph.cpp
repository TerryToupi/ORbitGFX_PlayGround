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

//SURFACE PASS
utils::Handle<gfx::RenderPass> surfaceRenderPass;
utils::Handle<gfx::RenderPassLayout> surfaceRenderLayout;
utils::Handle<gfx::BindGroup> surfaceBindGroup;
utils::Handle<gfx::BindGroupLayout> surfaceBindGroupLayout;
utils::Handle<gfx::Shader> surfaceShader;
utils::Handle<gfx::Buffer> surfaceVertex;
utils::Handle<gfx::Buffer> surfaceIndex;


float surfaceQuad[] = {
  -1.0, -1.0, // bottom-left
   1.0, -1.0, // bottom-right
   1.0,  1.0, // top-right
  -1.0,  1.0, // top-left
};
uint32_t surfaceQuadIndices[] = {
	0, 1, 2,    // first triangle
	2, 3, 0     // second triangle
};


void renderGraph::init(utils::Handle<gfx::BindGroupLayout> material)
{ 
	uniforms = gfx::UniformRingBuffer(32 * 1024 * 1024);

	globalsBuffer = gfx::ResourceManager::instance->Create(gfx::BufferDescriptor{
		.usage = gfx::BufferUsage::UNIFORM,
		.byteSize = sizeof(Globals),
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

	// INITILIZE STAGES
	offscreenStage::pass::init(material);
	mainStage::mainPass::init(material);
	mainStage::UIPass::init();
	mainStage::surfacePass::init();
}

void renderGraph::render(Meshes& meshes, EditorCamera& cam)
{ 
	Globals globals = {
		.viewProj = cam.getViewProjMatrix(),
		.cameraPos = cam.getCameraPos()
	};
	gfx::ResourceManager::instance->SetBufferData(globalsBuffer, 0, (void*)(&globals), sizeof(Globals));

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
		mainStage::UIPass::render(command);
		command->Submit();
	}
}

void renderGraph::destroy()
{  
	//DESTROY STAGES
	offscreenStage::pass::destroy();
	mainStage::mainPass::destroy();
	mainStage::UIPass::destroy();
	mainStage::surfacePass::destroy();

	uniforms.Destroy();

	gfx::ResourceManager::instance->Remove(globalsLayout);
	gfx::ResourceManager::instance->Remove(globalsBuffer);
	gfx::ResourceManager::instance->Remove(globalsGroup); 

	gfx::ResourceManager::instance->Remove(mainColorTexture);
	gfx::ResourceManager::instance->Remove(mainDepthTexture);
	gfx::ResourceManager::instance->Remove(mainFrameBuffer);
}



void offscreenStage::pass::init(utils::Handle<gfx::BindGroupLayout> material)
{
}

void offscreenStage::pass::render(gfx::CommandBuffer* cmdBuf, Meshes& meshes)
{
}

void offscreenStage::pass::destroy()
{
}



void mainStage::mainPass::init(utils::Handle<gfx::BindGroupLayout> material)
{ 
	Light light{
		.color = Vec3(1.0f, 0.0f, 0.0f),
		.intensity = 1.0f,
		.pos = Vec3(0.0f, 50.0f, 0.0f)
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

	std::vector<uint8_t> vsModule = loadModule("shaders/main_vert.glsl");
	std::vector<uint8_t> fsModule = loadModule("shaders/main_frag.glsl");
	mainShader = gfx::ResourceManager::instance->Create(gfx::ShaderDescriptor{
		.type = gfx::ShaderPipelineType::GRAPHICS,
		.VS = {.enabled = true, .sourceCode = utils::Span<uint8_t>(vsModule.data(), vsModule.size())},
		.PS = {.enabled = true, .sourceCode = utils::Span<uint8_t>(fsModule.data(), fsModule.size())},
		.bindLayouts = {
			{ globalsLayout },
			{ material },
			{ mainBindLayout },
			{ uniforms.GetLayout() }
		},
		.graphicsState = {
			.depthTest = gfx::Compare::LESS,
			.vertexBufferBindings = {
				{
					.byteStride = 12,
					.attributes = {
						{.byteOffset = 0, .format = gfx::VertexFormat::F32x3}
					}
				},
				{
					.byteStride = 24,
					.attributes = {
						{.byteOffset = 0, .format = gfx::VertexFormat::F16x4},
						{.byteOffset = 8, .format = gfx::VertexFormat::F16x4},
						{.byteOffset = 16, .format = gfx::VertexFormat::UNORM8x4},
						{.byteOffset = 20, .format = gfx::VertexFormat::F16x2},
					}
				}
			},
			.renderPassLayout = mainRenderLayout
		}
		});

}

void mainStage::mainPass::render(gfx::CommandBuffer* cmdBuf, Meshes& meshes)
{ 
	gfx::DrawStreamEncoder stream;
	for (const auto& mesh : meshes)
	{
		auto alloc = uniforms.BumpAllocate<CallData>();
		alloc.ptr->model = mesh.model;
		alloc.ptr->inverseModel = mesh.inverseModel;

		stream.Encode({
			.shader = mainShader,
			.bindGroups = { globalsGroup, mesh.materialBG, mainBindGroup },
			.dynamicBuffer = uniforms.GetBuffer(),
			.indexBuffer = mesh.index,
			.vertexBuffers = { mesh.vertex1, mesh.vertex2 },
			.dynamicBufferOffset = alloc.offset,
			.triangleCount = mesh.triangles
			});
	} 

	uniforms.Upload();
	cmdBuf->BeginRenderPass(mainRenderPass, mainFrameBuffer, utils::Span<uint32_t>(stream.Get().data(), stream.Get().size()));
}

void mainStage::mainPass::destroy()
{
	gfx::ResourceManager::instance->Remove(mainLightBuffer);
	gfx::ResourceManager::instance->Remove(mainBindLayout);
	gfx::ResourceManager::instance->Remove(mainBindGroup);
	gfx::ResourceManager::instance->Remove(mainRenderLayout);
	gfx::ResourceManager::instance->Remove(mainRenderPass);
	gfx::ResourceManager::instance->Remove(mainShader);
}



void mainStage::surfacePass::init()
{
	surfaceRenderLayout = gfx::ResourceManager::instance->Create(gfx::RenderPassLayoutDescriptor{
		.colorTargets = {
			{.enabled = true, .format = gfx::Window::instance->GetSurfaceFormat()}
		}
		});
	surfaceRenderPass = gfx::ResourceManager::instance->Create(gfx::RenderPassDescriptor{
		.colorTargets = {
			{.loadOp = gfx::LoadOperation::CLEAR, .storeOp = gfx::StoreOperation::STORE}
		},
		.layout = surfaceRenderLayout,
		});

	surfaceBindGroupLayout = gfx::ResourceManager::instance->Create(gfx::BindGroupLayoutDescriptor{
		.textureBindings = {
			{.slot = 0}
		},
		.samplerBindings = {
			{.slot = 1}
		}
		});

	//utils::Handle<gfx::Texture> debugTexture = loadImage("textures/b.png");
	surfaceBindGroup = gfx::ResourceManager::instance->Create(gfx::BindGroupDescriptor{
		.layout = surfaceBindGroupLayout,
		.textures = {
			{.slot = 0, .texture = mainColorTexture}
		},
		.samplers = {
			{.slot = 1, .sampler = gfx::ResourceManager::instance->Create(gfx::SamplerDescriptor{})}
		}
		}); 

	surfaceVertex = gfx::ResourceManager::instance->Create(gfx::BufferDescriptor{
		.usage = gfx::BufferUsage::VERTEX,
		.byteSize = sizeof(surfaceQuad),
		.initialData = utils::Span<uint8_t>(reinterpret_cast<const uint8_t*>(surfaceQuad), 1)
		});
	surfaceIndex= gfx::ResourceManager::instance->Create(gfx::BufferDescriptor{
		.usage = gfx::BufferUsage::INDEX,
		.byteSize = sizeof(surfaceQuadIndices),
		.initialData = utils::Span<uint8_t>(reinterpret_cast<const uint8_t*>(surfaceQuadIndices), 1)
		});

	std::vector<uint8_t> vsModule = loadModule("shaders/surface_vert.glsl");
	std::vector<uint8_t> fsModule = loadModule("shaders/surface_frag.glsl");
	surfaceShader = gfx::ResourceManager::instance->Create(gfx::ShaderDescriptor{
		.type = gfx::ShaderPipelineType::GRAPHICS,
		.VS = {.enabled = true, .sourceCode = utils::Span<uint8_t>(vsModule.data(), vsModule.size())},
		.PS = {.enabled = true, .sourceCode = utils::Span<uint8_t>(fsModule.data(), fsModule.size())},
		.bindLayouts = {
			{ surfaceBindGroupLayout },
			{},
			{},
			{}
		},
		.graphicsState = {
			.vertexBufferBindings = {
				{
					.byteStride = 2 * sizeof(float), 
					.attributes = {
						{.byteOffset = 0, .format = gfx::VertexFormat::F32x2}
					}
				}
			},
			.renderPassLayout = surfaceRenderLayout
		}
		});
}

void mainStage::surfacePass::render(gfx::CommandBuffer* cmdBuf)
{ 
	gfx::DrawStreamEncoder encoder;
	encoder.Encode({
		.shader = surfaceShader,
		.bindGroups = { surfaceBindGroup },
		.indexBuffer = surfaceIndex,
		.vertexBuffers = { surfaceVertex },
		.triangleCount = 2
	});

	cmdBuf->BeginSurfacePass(surfaceRenderPass, utils::Span<uint32_t>(encoder.Get().data(), encoder.Get().size()));
}

void mainStage::surfacePass::destroy()
{
	gfx::ResourceManager::instance->Remove(surfaceRenderLayout);
	gfx::ResourceManager::instance->Remove(surfaceRenderPass);
	gfx::ResourceManager::instance->Remove(surfaceBindGroupLayout);
	gfx::ResourceManager::instance->Remove(surfaceBindGroup);
	gfx::ResourceManager::instance->Remove(surfaceVertex);
	gfx::ResourceManager::instance->Remove(surfaceIndex);
	gfx::ResourceManager::instance->Remove(surfaceShader);
}

void mainStage::UIPass::init()
{ 
}

void mainStage::UIPass::render(gfx::CommandBuffer* cmdBuf)
{
	cmdBuf->BeginImGuiPass();
}

void mainStage::UIPass::destroy()
{

}
