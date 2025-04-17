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

std::vector<uint8_t> generateCheckerboard(int width, int height, int tileSize) {
    std::vector<uint8_t> texture(width * height * 4); // RGBA format (uint8_t for 8-bit channels)

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // Determine whether this pixel belongs to a white or black tile
            bool isWhite = ((x / tileSize) % 2 == (y / tileSize) % 2);

            // Calculate the index in the flat array
            int index = (y * width + x) * 4;

            // Set the color (white or black) with full opacity
            if (isWhite) {
                texture[index] = 255;     // Red
                texture[index + 1] = 255; // Green
                texture[index + 2] = 255; // Blue
                texture[index + 3] = 255; // Alpha (fully opaque)
            }
            else {
                texture[index] = 0;       // Red
                texture[index + 1] = 0;   // Green
                texture[index + 2] = 0;   // Blue
                texture[index + 3] = 255; // Alpha (fully opaque)
            }
        }
    }

    return texture;
}

void running()
{
    {
        gfx::CommandBuffer* commandBuffer = gfx::Renderer::instance->BeginCommandRecording();
        gfx::RenderPassRenderer* passRenderer = commandBuffer->BeginSurfacePass(renderPass);

        gfx::DrawStream stream;

        stream.Insert({
            .shader = shader2,
            .bindGroups = { bindGroup },
            .indexBuffer = indexBuffer,
            .vertexBuffers = { vertexBuffer1, uvBuffer },
            .triangleCount = (sizeof(indexData) / sizeof(uint32_t)) / 3
            });

        stream.Insert({
            .shader = shader2,
            .bindGroups = { bindGroup },
            .indexBuffer = indexBuffer,
            .vertexBuffers = { vertexBuffer2, uvBuffer },
            .triangleCount = (sizeof(indexData) / sizeof(uint32_t)) / 3
            });

        passRenderer->DrawPass(stream);

        commandBuffer->EndCommandRecording(passRenderer);
        commandBuffer->Submit();
    }

    gfx::ResourceManager::instance->Flush();

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
    std::vector<uint8_t> textData = generateCheckerboard(1920, 1080, 12);

    renderPassLayout = gfx::ResourceManager::instance->Create(gfx::RenderPassLayoutDescriptor{
    .colorTargets = {{.enabled = true, .format = surfaceFormat }},
    .depth = {.depthTarget = false }
        });

    renderPass = gfx::ResourceManager::instance->Create(gfx::RenderPassDescriptor{
        .colorTargets = {{}},
        .layout = renderPassLayout
        });

    mainTexture = gfx::ResourceManager::instance->Create(gfx::TextureDescriptor{
        .format = gfx::TextureFormat::BGRA8_UNORM,
        .usage = gfx::TextureUsage::TEXTURE_BINDING | gfx::TextureUsage::RENDER_ATTACHMENT | gfx::TextureUsage::COPY_DST,
        .width = 1920,
        .height = 1080,
        .views = {
            {
                .format = gfx::TextureFormat::BGRA8_UNORM,
                .usage = gfx::TextureUsage::TEXTURE_BINDING | gfx::TextureUsage::RENDER_ATTACHMENT | gfx::TextureUsage::COPY_DST
            }
        },
        .uploadDesc = {
            .upload = true,
            .uploadSize = textData.size(),
            .uploadData = textData.data()
        }
        });

    mainSampler = gfx::ResourceManager::instance->Create(gfx::SamplerDescriptor{});

    bindGroupLayout = gfx::ResourceManager::instance->Create(gfx::BindGroupLayoutDescriptor{
        .textureBindings = {
            {.slot = 0, .visibility = gfx::ShaderStage::FRAGMENT }
        },
        .samplerBindings = {
            {.slot = 1, .visibility = gfx::ShaderStage::FRAGMENT }
        }
        });

    bindGroup = gfx::ResourceManager::instance->Create(gfx::BindGroupDescriptor{
        .layout = bindGroupLayout,
        .textures = {
            {.slot = 0, .textureView = mainTexture }
        },
        .samplers = {
            {.slot = 1, .sampler = mainSampler }
        }
        });

    vertexBuffer1 = gfx::ResourceManager::instance->Create(gfx::BufferDescriptor{
        .usage = gfx::BufferUsage::VERTEX,
        .size = sizeof(topLeftQuad),
        .data = topLeftQuad
        });

    vertexBuffer2 = gfx::ResourceManager::instance->Create(gfx::BufferDescriptor{
        .usage = gfx::BufferUsage::VERTEX,
        .size = sizeof(bottomRightQuad),
        .data = bottomRightQuad
        });

    indexBuffer = gfx::ResourceManager::instance->Create(gfx::BufferDescriptor{
        .usage = gfx::BufferUsage::INDEX,
        .size = sizeof(indexData),
        .data = indexData
        });

    uvBuffer = gfx::ResourceManager::instance->Create(gfx::BufferDescriptor{
        .usage = gfx::BufferUsage::VERTEX,
        .size = sizeof(uvData),
        .data = uvData
        });

    normalBuffer = gfx::ResourceManager::instance->Create(gfx::BufferDescriptor{
        .usage = gfx::BufferUsage::VERTEX,
        .size = sizeof(normalData),
        .data = normalData
        });

    shader2 = gfx::ResourceManager::instance->Create(gfx::ShaderDescriptor{
        .type = gfx::ShaderPipelineType::GRAPHICS,
        .VS = {.enabled = true, .sourceCode = R"(
        #version 450

        layout(location = 0) in vec3 pos;
        layout(location = 1) in vec2 uv;
 
        layout(location = 0) out vec2 outUV;

        void main()
		{
		    gl_Position = vec4(pos, 1.0); // Standard position assignment 
            outUV = uv; 
		}
        )"},
        .PS = {.enabled = true, .sourceCode = R"(
		#version 450
	 
		// Fragment Shader
		layout(location = 0) in vec2 uv; // Input color with location 0

        layout(set = 0, binding = 0) uniform texture2D meTexture;
        layout(set = 0, binding = 1) uniform sampler meSampler;

		layout(location = 0) out vec4 color;    // Output color with location 0

		void main() {
            
			color = texture(sampler2D(meTexture, meSampler), uv); 
		}
        )"},
        .graphicsState = {
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
        .bindLayouts = { bindGroupLayout }
        });

    shader = gfx::ResourceManager::instance->Create(gfx::ShaderDescriptor{
        .type = gfx::ShaderPipelineType::GRAPHICS,
        .VS = {.enabled = true, .sourceCode = R"(
        #version 450

		// Vertex Shader
		layout(location = 0) in vec3 pos;       // Input position with location 0
		layout(location = 1) in vec2 uv;       // Input uv with location 0
		layout(location = 2) in vec3 norm;       // Input norm with location 0
		layout(location = 0) out vec4 fragColor; // Output color with location 0

		void main() {
			gl_Position = vec4(pos, 1.0); // Standard position assignment
		}
        )"},
        .PS = {.enabled = true, .sourceCode = R"(
		#version 450
	 
		// Fragment Shader
		layout(location = 0) in vec4 fragColor; // Input color with location 0
		layout(location = 0) out vec4 color;    // Output color with location 0

		void main() {
			color = vec4(1.0, 1.0, 0.0, 1.0); // Set fragment color to yellow
		}
        )"},
        .graphicsState = {
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
                },
                {
                    .byteStride = 3 * sizeof(float),
                    .attributes = {
                        {.byteOffset = 0, .format = gfx::VertexFormat::F32x3},
                    }
                }
            },
            .renderPassLayout = renderPassLayout,
        }
        });
    // -------------------------------------------------------------------------
    gfx::Window::instance->Run(running);
    ShutDown();

    return 0;
}