#ifndef __RENDER_GRAPH_HPP__
#define __RENDER_GRAPH_HPP__

#include <vector>
#include <math.hpp>
#include <mesh.hpp>
#include <window.hpp>
#include <resources/resourceManger.hpp>
#include <render/renderer.hpp>
#include <render/uniforms.hpp>
#include <render/commands.hpp>
#include <render/passRenderer.hpp>

#include <loaders/shaderModuleLoader.hpp>
#include <loaders/imageLoader.hpp>
#include <camera/editorCamera.hpp>

struct Globals 
{
	Mat4 viewProj;
	Vec3 cameraPos;
	float _padding;
};

struct Light
{
	Vec3 color;
	float intensity;
	Vec3 pos;
	float _padding;
};

struct CallData
{
	Mat4 model;
	Mat4 inverseModel;
};

namespace renderGraph
{
	void init();
	void render(Meshes& meshes, EditorCamera& cam);
	void destroy(); 
}

namespace offscreenStage 
{
	namespace pass 
	{
		void init();
		void render(gfx::CommandBuffer* cmdBuf, Meshes& meshes);
		void destroy(); 
	}
}

namespace mainStage
{
	namespace mainPass 
	{
		void init();
		void render(gfx::CommandBuffer* cmdBuf, Meshes& meshes);
		void destroy(); 
	}

	namespace surfacePass
	{
		void init();
		void render(gfx::CommandBuffer* cmdBuf);
		void destroy(); 
	}
}

#endif // !__RENDER_GRAPH_HPP__
