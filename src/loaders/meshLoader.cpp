#include <loaders/meshLoader.hpp>
#include <loaders/imageLoader.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <resources/resourceManger.hpp>

#include <iostream>
#include <unordered_map>

struct vertex2
{
    Half tangent[4];
    Half normal[4];
    uint8_t color[4];
    Half texcoord[2];
};
static_assert(sizeof(vertex2) == 24);

void FillVertexBuffers(aiMesh* mesh, Meshes& meshes)
{
	std::vector<float> position;
	position.reserve(mesh->mNumVertices * 3);

	std::vector<uint32_t> indices;
	indices.reserve(mesh->mNumFaces * 3);

	std::vector<vertex2> Vertex2(mesh->mNumVertices);
	
	for (unsigned int v = 0; v < mesh->mNumVertices; ++v)
	{
		position.emplace_back(mesh->mVertices[v].x);
		position.emplace_back(mesh->mVertices[v].y);
		position.emplace_back(mesh->mVertices[v].z);
	} 

	for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
		indices.emplace_back(mesh->mFaces[i].mIndices[0]);
		indices.emplace_back(mesh->mFaces[i].mIndices[1]);
		indices.emplace_back(mesh->mFaces[i].mIndices[2]);
	}

	for (unsigned i = 0; i < mesh->mNumVertices; ++i) {
		vertex2& v = Vertex2[i];

		// Tangent (aiVector3D, promote to vec4 with w=1, convert to half)
		if (mesh->mTangents) 
		{
			v.tangent[0] = glm::detail::toFloat16(mesh->mTangents[i].x);
			v.tangent[1] = glm::detail::toFloat16(mesh->mTangents[i].y);
			v.tangent[2] = glm::detail::toFloat16(mesh->mTangents[i].z);
			v.tangent[3] = glm::detail::toFloat16(1.0f); // Tangent.w
		}
		else 
		{
			memset(v.tangent, 0, sizeof(v.tangent));
		}

		// Normal (aiVector3D, promote to vec4 with w=0, convert to half)
		if (mesh->mNormals) 
		{
			v.normal[0] = glm::detail::toFloat16(mesh->mNormals[i].x);
			v.normal[1] = glm::detail::toFloat16(mesh->mNormals[i].y);
			v.normal[2] = glm::detail::toFloat16(mesh->mNormals[i].z);
			v.normal[3] = glm::detail::toFloat16(0.0f); // Normal.w
		}
		else 
		{
			memset(v.normal, 0, sizeof(v.normal));
		}

		// Color (aiColor4D, convert to 0-255 RGBA8)
		if (mesh->HasVertexColors(0)) 
		{
			v.color[0] = static_cast<uint8_t>(mesh->mColors[0][i].r * 255.0f);
			v.color[1] = static_cast<uint8_t>(mesh->mColors[0][i].g * 255.0f);
			v.color[2] = static_cast<uint8_t>(mesh->mColors[0][i].b * 255.0f);
			v.color[3] = static_cast<uint8_t>(mesh->mColors[0][i].a * 255.0f);
		}
		else 
		{
			v.color[0] = v.color[1] = v.color[2] = 255;
			v.color[3] = 255;
		}

		// Texcoord (aiVector3D, use x/y, convert to half)
		if (mesh->HasTextureCoords(0)) 
		{
			v.texcoord[0] = glm::detail::toFloat16(mesh->mTextureCoords[0][i].x);
			v.texcoord[1] = glm::detail::toFloat16(mesh->mTextureCoords[0][i].y);
		}
		else 
		{
			v.texcoord[0] = v.texcoord[1] = 0;
		}
	}

	meshes.push_back({
		.vertex1 = gfx::ResourceManager::instance->Create(gfx::BufferDescriptor{
			.usage = gfx::BufferUsage::VERTEX,
			.byteSize = position.size() * sizeof(float),
			.initialData = utils::Span<uint8_t>(reinterpret_cast<const uint8_t*>(position.data()), position.size() * sizeof(float))
		}),
		.vertex2 = gfx::ResourceManager::instance->Create(gfx::BufferDescriptor{
			.usage = gfx::BufferUsage::VERTEX,
			.byteSize = Vertex2.size() * sizeof(vertex2),
			.initialData = utils::Span<uint8_t>(reinterpret_cast<const uint8_t*>(Vertex2.data()), Vertex2.size() * sizeof(vertex2))
		}),
		.index = gfx::ResourceManager::instance->Create(gfx::BufferDescriptor{
			.usage = gfx::BufferUsage::INDEX,
			.byteSize = indices.size() * sizeof(uint32_t),
			.initialData = utils::Span<uint8_t>(reinterpret_cast<const uint8_t*>(indices.data()), indices.size() * sizeof(uint32_t))
		}),
		.triangles = mesh->mNumFaces
	});
}

std::string ExtractDirectoryFromPath(const std::string& path) {
	size_t found = path.find_last_of("/\\");
	if (found != std::string::npos)
		return path.substr(0, found + 1); // include trailing slash
	else
		return ""; // No directory part, just filename
}

void loadTextures(const std::string& path,  const aiScene* scene, Meshes& meshes, utils::Handle<gfx::BindGroupLayout> material, uint32_t head)
{ 
	utils::Handle<gfx::Sampler> sampler = gfx::ResourceManager::instance->Create(gfx::SamplerDescriptor{});

	std::unordered_map<std::string, utils::Handle<gfx::BindGroup>> bindGroupChace;

	uint32_t currIndex = head;
	for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[i];
		const aiMaterial* aimaterial = scene->mMaterials[mesh->mMaterialIndex];
		aiString texPath;

		if (aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS)
		{
			std::string p = std::string(texPath.C_Str());
			std::string tp = path + p;

			auto it = bindGroupChace.find(tp);
			if (it != bindGroupChace.end())
			{
				meshes[currIndex].materialBG = it->second;
			}
			else
			{
				utils::Handle<gfx::Texture> tex = loadImage(tp.c_str());
				utils::Handle<gfx::BindGroup> bg = gfx::ResourceManager::instance->Create(gfx::BindGroupDescriptor{
					.layout = material,
					.textures = {
						{.slot = 0, .texture = tex}
					},
					.samplers = {
						{.slot = 1, .sampler = sampler}
					}
					});
				bindGroupChace[tp] = bg;

				meshes[currIndex].materialBG = bg;
			}
		}
		
		currIndex++;
	}
}

void loadMesh(const char* path, Meshes& meshes, utils::Handle<gfx::BindGroupLayout> material, Vec3 position, Vec3 rotation, Vec3 scale)
{
    std::string file = std::string(ASSETS) + path;
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        file,
        aiProcessPreset_TargetRealtime_Fast
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
	{
        std::cerr << "Assimp error: " << importer.GetErrorString() << std::endl;
        return;
    }
    
    std::cout << "Model loaded: " << file << std::endl;
    std::cout << "Meshes: " << scene->mNumMeshes << std::endl;

    Mat4 S = glm::scale(glm::mat4(1.0f), scale);
    Mat4 R = glm::eulerAngleXYZ(rotation.x, rotation.y, rotation.z);
    Mat4 T = glm::translate(glm::mat4(1.0f), position);
    Mat4 worldMatrix = T * R * S;

	uint32_t headIndex = meshes.size();
	uint32_t tailIndex = meshes.size() + scene->mNumMeshes;
    meshes.reserve(tailIndex);

    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) 
	{
        aiMesh* mesh = scene->mMeshes[i];
		FillVertexBuffers(mesh, meshes);
    }

	loadTextures(ExtractDirectoryFromPath(path), scene, meshes, material, headIndex);

	for (uint32_t i = headIndex; i < tailIndex; ++i)
	{
		meshes[i].model = worldMatrix;
		meshes[i].inverseModel = glm::inverse(worldMatrix);
	}
}
