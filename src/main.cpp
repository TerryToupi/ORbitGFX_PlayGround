#include <test.hpp>
#include <renderGraph/renderGraph.hpp>

void running()
{
    gfx::CommandBuffer* commandBuffer = gfx::Renderer::instance->BeginCommandRecording(gfx::CommandBufferType::MAIN);
    {
        std::vector<gfx::Draw> stream;
        auto alloc = uniformBuffer->BumpAllocate<Color>();

        alloc.ptr->r = 1;
        alloc.ptr->g = 0;
        alloc.ptr->b = 1;
        alloc.ptr->a = 1;

        auto alloc1 = uniformBuffer->BumpAllocate<Color>(); 

        alloc1.ptr->r = 1;
        alloc1.ptr->g = 0;
        alloc1.ptr->b = 0;
        alloc1.ptr->a = 1;

        stream.push_back({
            .shader = shader2,
            .bindGroups = { bindGroup },
            .dynamicBuffer = uniformBuffer->GetBuffer(),
            .indexBuffer = indexBuffer,
            .vertexBuffers = { vertexBuffer2, uvBuffer },
            .dynamicBufferOffset = alloc.offset,
            .triangleCount = (sizeof(indexData) / sizeof(uint32_t)) / 3
            }); 

        uniformBuffer->Upload();
        commandBuffer->BeginRenderPass(renderPass, utils::Span<gfx::Draw>(stream.data(), stream.size()));
    }
    {
        //std::vector<gfx::Draw> stream;
        //stream.push_back({
        //    .shader = shader2,
        //    .bindGroups = { bindGroup },
        //    .indexBuffer = indexBuffer,
        //    .vertexBuffers = { vertexBuffer1, uvBuffer },
        //    .triangleCount = (sizeof(indexData) / sizeof(uint32_t)) / 3
        //    });
        //stream.push_back({
        //    .shader = shader2,
        //    .bindGroups = { bindGroup },
        //    .indexBuffer = indexBuffer,
        //    .vertexBuffers = { vertexBuffer2, uvBuffer },
        //    .triangleCount = (sizeof(indexData) / sizeof(uint32_t)) / 3
        //    });

        //commandBuffer->BeginRenderPass(renderPass, mainFrame, utils::Span<gfx::Draw>(stream.data(), stream.size()));
    }
    commandBuffer->Submit();

    return;
}

void Init(const gfx::WindowDescriptor& wDesc)
{
    gfx::Renderer::INIT();

    surfaceFormat = gfx::Window::instance->GetSurfaceFormat();
}

void ShutDown()
{
    gfx::Renderer::SHUTDOWN();
}

int main()
{
    gfx::WindowDescriptor windowDesc =
    {
        .name = "Dawn Demo",
        .width = 1024,
        .height = 720,
    };
    Init(windowDesc);
    // -------------------------------------------------------------------------
    renderGraph::init();

    gfx::UniformRingBuffer uniform(128 * 1024 * 1024);
    uniformBuffer = &uniform;

    renderPassLayout = gfx::ResourceManager::instance->Create(gfx::RenderPassLayoutDescriptor{
    .depth = {.depthTarget = false },
    .colorTargets = {{.format = surfaceFormat }},
        });

    renderPass = gfx::ResourceManager::instance->Create(gfx::RenderPassDescriptor{
        .colorTargets = {{}},
        .layout = renderPassLayout
        });

    mainTexture = loadImage("b.png");
    mainSampler = gfx::ResourceManager::instance->Create(gfx::SamplerDescriptor{});

    utils::Handle<gfx::Buffer> b = gfx::ResourceManager::instance->Create(gfx::BufferDescriptor{
        .usage = gfx::BufferUsage::UNIFORM,
        .byteSize = 64u * 1024
        });
     
    frameTexture = gfx::ResourceManager::instance->Create(gfx::TextureDescriptor{
        .format = gfx::TextureFormat::BGRA8_UNORM,
        .usage = gfx::TextureUsage::TEXTURE_BINDING | gfx::TextureUsage::RENDER_ATTACHMENT | gfx::TextureUsage::COPY_DST,
        .width = 1920,
        .height = 1080,
        });

    mainFrame = gfx::ResourceManager::instance->Create(gfx::FrameBufferDescriptor{
        .colorTargets = {
            frameTexture
        }
        });

    bindGroupLayout = gfx::ResourceManager::instance->Create(gfx::BindGroupLayoutDescriptor{ 
        .bufferBindings = {
            {.slot = 2 }
        },
        .textureBindings = {
            {.slot = 0, .type = gfx::TextureSampleType::FLOAT }
        },
        .samplerBindings = {
            {.slot = 1 }
        }
        });

    bindGroup = gfx::ResourceManager::instance->Create(gfx::BindGroupDescriptor{
        .layout = bindGroupLayout,
        .buffers = {
            {.slot = 2, .buffer = b, .range = 64u * 1024 }
        },
        .textures = {
            {.slot = 0, .texture = mainTexture }
        },
        .samplers = {
            {.slot = 1, .sampler = mainSampler }
        }
        });
    
    vertexBuffer1 = gfx::ResourceManager::instance->Create(gfx::BufferDescriptor{
        .usage = gfx::BufferUsage::VERTEX,
        .byteSize = sizeof(topLeftQuad),
        .initialData = utils::Span(reinterpret_cast<const uint8_t*>(topLeftQuad), sizeof(topLeftQuad)),
        });

    vertexBuffer2 = gfx::ResourceManager::instance->Create(gfx::BufferDescriptor{
        .usage = gfx::BufferUsage::VERTEX,
        .byteSize = sizeof(fsQuad),
        .initialData = utils::Span(reinterpret_cast<const uint8_t*>(fsQuad), sizeof(fsQuad)),
        });

    indexBuffer = gfx::ResourceManager::instance->Create(gfx::BufferDescriptor{
        .usage = gfx::BufferUsage::INDEX,
        .byteSize = sizeof(indexData),
        .initialData = utils::Span(reinterpret_cast<const uint8_t*>(indexData), sizeof(indexData)),
        });

    uvBuffer = gfx::ResourceManager::instance->Create(gfx::BufferDescriptor{
        .usage = gfx::BufferUsage::VERTEX,
        .byteSize = sizeof(uvData),
        .initialData = utils::Span(reinterpret_cast<const uint8_t*>(uvData), sizeof(uvData))
        });

    normalBuffer = gfx::ResourceManager::instance->Create(gfx::BufferDescriptor{
        .usage = gfx::BufferUsage::VERTEX,
        .byteSize = sizeof(normalData),
        .initialData = utils::Span(reinterpret_cast<const uint8_t*>(normalData), sizeof(normalData))
        });

    std::vector<uint8_t> vsBytes = loadModule("simple.vert"); 
    std::vector<uint8_t> fsBytes = loadModule("simple.frag");

    shader2 = gfx::ResourceManager::instance->Create(gfx::ShaderDescriptor{
        .type = gfx::ShaderPipelineType::GRAPHICS,
        .VS = {.enabled = true, .sourceCode = utils::Span<uint8_t>(vsBytes.data(), vsBytes.size()) },
        .PS = {.enabled = true, .sourceCode = utils::Span<uint8_t>(fsBytes.data(), fsBytes.size()) },
        .bindLayouts = {
            { bindGroupLayout },
            {},
            {},
            { uniformBuffer->GetLayout() }
        },
        .graphicsState = {
            .depthTest = gfx::Compare::GREATER_OR_EQUAL,
            .vertexBufferBindings = {
                {
                    .byteStride = 3 * sizeof(float),
                    .attributes = {
                        {.byteOffset = 0, .format = gfx::VertexFormat::F32x3},
                    }
                },
                {
                    .byteStride = 2 * sizeof(float),
                    .attributes = {
                        {.byteOffset = 0, .format = gfx::VertexFormat::F32x2},
                    }
                }
            },
            .renderPassLayout = renderPassLayout,
        },
        });
    // -------------------------------------------------------------------------
    gfx::Window::instance->Run(running);
    renderGraph::destroy();
    ShutDown();

    return 0;
}