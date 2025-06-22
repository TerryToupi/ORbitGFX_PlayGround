#include <loaders/meshLoader.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>

void loadMesh(const char* path, Meshes& meshes, Vec3 position, Vec3 rotation, Vec3 scale)
{
    std::string file = std::string(ASSETS) + path;
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        file,
        aiProcessPreset_TargetRealtime_Fast
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Assimp error: " << importer.GetErrorString() << std::endl;
        return;
    }
    
    std::cout << "Model loaded: " << file << std::endl;
    std::cout << "Meshes: " << scene->mNumMeshes << std::endl;

    meshes.reserve(meshes.size() + scene->mNumMeshes);

    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[i];

		std::vector<float> position;
        position.reserve(mesh->mNumVertices * 3);

		std::vector<uint32_t> indices;
        indices.reserve(mesh->mNumFaces * 3);

        std::cout << "Mesh " << i << ": " << mesh->mNumVertices * 3 << " vertices" << std::endl;
        std::cout << "Mesh " << i << ": " << mesh->mNumFaces * 3 << " indices" << std::endl;
        
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
        
        // Example: print out the first 3 vertices
        //for (unsigned int v = 0; v < std::min(3u, mesh->mNumVertices); ++v) {
        //    aiVector3D pos = mesh->mVertices[v];
        //    std::cout << "  Vertex " << v << ": (" << pos.x << ", " << pos.y << ", " << pos.z << ")\n";
        //}
    }
}
