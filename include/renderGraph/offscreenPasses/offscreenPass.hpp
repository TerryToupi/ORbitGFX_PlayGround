#ifndef __OFFSCREEN_PASS_HPP__
#define __OFFSCREEN_PASS_HPP__

#include <math.hpp>
#include <mesh.hpp>

#include <resources/resourceManger.hpp>

#include <render/uniforms.hpp>

namespace offscreen
{
	void init(utils::Handle<gfx::BindGroupLayout> globals);
	void render(Meshes& meshes, utils::Handle<gfx::BindGroup> globals ,gfx::UniformRingBuffer* uniforms);
	void destroy();
}

#endif // !__OFFSCREEN_PASS_HPP__
