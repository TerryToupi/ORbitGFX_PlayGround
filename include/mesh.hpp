#ifndef __MESH_HPP__
#define __MESH_HPP__

#include <vector>
#include <math.hpp>
#include <handle.hpp>
#include <resources/buffer.hpp>
#include <resources/bindGroup.hpp>

struct Mesh 
{
	utils::Handle<gfx::Buffer> vertex1;
	utils::Handle<gfx::Buffer> vertex2; 
	utils::Handle<gfx::Buffer> index;

	utils::Handle<gfx::BindGroup> materialBG;
	
	Mat4 model;
	Mat4 inverseModel;

	uint32_t triangles;
};

using Meshes = std::vector<Mesh>;

#endif // !__MESH_HPP__
