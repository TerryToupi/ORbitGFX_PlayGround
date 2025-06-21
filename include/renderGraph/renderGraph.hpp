#ifndef __RENDER_GRAPH_HPP__
#define __RENDER_GRAPH_HPP__

#include <math.hpp>
#include <mesh.hpp>

#include <handle.hpp>
#include <resources/bindGroup.hpp>
#include <resources/bindGroupLayout.hpp>
#include <resources/buffer.hpp>

namespace renderGlobals
{
	struct data
	{
		Mat4 viewProj;
		Vec3 cameraPos;
		float _padding;
	};
}

namespace materialGlobals
{
	struct data
	{
		float roughness;
	};
}

namespace shaderGlobals
{
	struct lightData
	{
		Vec3 lightColor;
		float lightIntensity;
		Vec3 lightPos;
		float _padding;
	};
}

namespace renderGraph
{
	void init();
	void render(Meshes& meshes);
	void destroy(); 
}

#endif // !__RENDER_GRAPH_HPP__
