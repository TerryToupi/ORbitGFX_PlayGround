#ifndef __MESH_LOADER_HPP__
#define __MESH_LOADER_HPP__

#include <math.hpp>
#include <mesh.hpp>

void loadMesh(const char* path, Meshes& meshes, utils::Handle<gfx::BindGroupLayout> material, Vec3 position, Vec3 rotation, Vec3 scale);

#endif // !__MESH_LOADER_HPP__
