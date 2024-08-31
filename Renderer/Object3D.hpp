#ifndef OBJECT3D_HPP
#define OBJECT3D_HPP

#include "GeometricData.hpp"
#include "Collisions.hpp"
#include "Images.hpp"

#pragma warning(push)
#pragma warning(disable: 4244)		// VS complains at the Assimp lib

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

std::filesystem::path ResolvePath(const std::filesystem::path& path, const std::filesystem::path& parent) noexcept;

template <minVertex vertexType = Vertex>
struct Object3D
{
	A::array<Mesh<vertexType>> meshArr;
	A::array<AABB> collisionBoxes;
	A::array<Image> diffuseTextures;

	Vec3f positionInSpace;
	Vec3f rotation;
	Vec3f scale;

	[[nodiscard]] RESULT_VALUE LoadFromFile(std::filesystem::path filePath);
};

template<minVertex vertexType>
inline RESULT_VALUE Object3D<vertexType>::LoadFromFile(std::filesystem::path filePath)
{
    RESULT_VALUE r_value = RESULT_VALUE::OK;

    if (filePath.empty())
    {
        return RESULT_VALUE::MISSING_FILEPATH;
    }

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(filePath.string().c_str(),
        aiProcess_ConvertToLeftHanded |
        aiProcess_PopulateArmatureData |
        aiProcess_Triangulate |
        aiProcess_SortByPType |
        aiProcess_JoinIdenticalVertices |
        aiProcess_OptimizeMeshes |
        aiProcess_ImproveCacheLocality |
        aiProcess_GenSmoothNormals |
        aiProcess_OptimizeGraph |
        aiProcess_LimitBoneWeights |
        aiProcess_CalcTangentSpace |
        aiProcess_FindInvalidData
    );

    if (scene == nullptr)
    {
        return RESULT_VALUE::ASSIMP_FAILURE;
    }

    const size_t numMeshes = scene->mNumMeshes;

    meshArr.make_array(meshArr, numMeshes);
    diffuseTextures.make_array(diffuseTextures, numMeshes);
    collisionBoxes.make_array(collisionBoxes, numMeshes);

    // loop 0 to make all vertices side-by-side in memory
    for (size_t i = 0; i < numMeshes; i++)
    {
        const aiMesh* _mesh = scene->mMeshes[i];
        const size_t numVerts = _mesh->mNumVertices;
        meshArr.emplace_back({}); // increase size()
        auto& vertices = meshArr[i].vertices;
        vertices.make_array(vertices, numVerts);

        for (size_t j = 0; j < numVerts; j++)
        {
            Vec3f positions;
            Vec3f normals;
            Vec2f uvs;

            if (_mesh->HasPositions()) // fail-safe, you never know what's in a file
            {
                positions = Vec3f{ _mesh->mVertices[j].x, _mesh->mVertices[j].y, _mesh->mVertices[j].z };
            }
            if (_mesh->HasNormals()) // some don't have normals, so we have to check before accessing any of it
            {
                normals = Vec3f{ _mesh->mNormals[j].x, _mesh->mNormals[j].y, _mesh->mNormals[j].z };
            }
            if (_mesh->HasTextureCoords(0)) // same
            {
                uvs = Vec2f{ _mesh->mTextureCoords[0][j].x, _mesh->mTextureCoords[0][j].y };
            }
            vertices.emplace_back(Vertex{ positions, normals, uvs });
        }

        // AABBs
        AABB aabb;
        aabb.min = Vec3f{ _mesh->mAABB.mMin.x, _mesh->mAABB.mMin.y, _mesh->mAABB.mMin.z };
        aabb.max = Vec3f{ _mesh->mAABB.mMax.x, _mesh->mAABB.mMax.y, _mesh->mAABB.mMax.z };

        collisionBoxes.emplace_back(aabb);
    }

    // loop 1 to make all indices side-by-side in memory
    for (size_t i = 0; i < numMeshes; i++)
    {
        const aiMesh* _mesh = scene->mMeshes[i];
        const size_t numFaces = _mesh->mNumFaces;
        auto& indices = meshArr[i].indices;

        indices.make_array(indices, numFaces * 3);

        for (size_t j = 0; j < numFaces; j++)
        {
            if (scene->mMeshes[i]->mFaces[j].mNumIndices == 3) [[likely]]
            {
                const aiFace& face = scene->mMeshes[i]->mFaces[j];
                indices.emplace_back((uint32_t)face.mIndices[0]);
                indices.emplace_back((uint32_t)face.mIndices[1]);
                indices.emplace_back((uint32_t)face.mIndices[2]);
            }
        }
    }

    using namespace std;

    for (size_t i = 0; i < numMeshes; i++)
    {
        const aiMesh* _mesh = scene->mMeshes[i];
        
        if (_mesh->mMaterialIndex >= 0)
        {
            aiMaterial* material = scene->mMaterials[_mesh->mMaterialIndex];

            aiTextureType tType = aiTextureType_DIFFUSE;
            unsigned int diffuseTextureCount = material->GetTextureCount(tType);

            if (diffuseTextureCount == 0)
            {
                tType = aiTextureType_BASE_COLOR;
                diffuseTextureCount = material->GetTextureCount(tType);
            }

            for (unsigned int j = 0; j < diffuseTextureCount; j++)
            {
                aiString path;
                if (material->GetTexture(tType, j, &path) == AI_SUCCESS)
                {
                    const filesystem::path aiPath = path.C_Str();
                    const filesystem::path txPath = ResolvePath(aiPath, filePath.parent_path());

                    if (filesystem::exists(txPath))
                    {
                        r_value = diffuseTextures[i].LoadFromFile(txPath);
                    }
                    else
                    {
                        r_value = RESULT_VALUE::MISSING_FILEPATH;
                        std::cerr << "Diffuse texture specified at path: " << path.C_Str() << ", but wasn't found in the directory path\n" 
                                  << "You may want to load it manually or fix the specified texture location in the folder\n";
                    }
                }
            }
        }
        else
        {
            std::cerr << "No texture specified in the file\n";
        }
    }

    return r_value;
}

#pragma warning(pop)

#endif