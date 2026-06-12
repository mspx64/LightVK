#include "Assets.h"
#include "Logger.h"

#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>

namespace Lgt::Assets {

bool LoadGltf(const std::filesystem::path& path, Model* model) {

    fastgltf::Parser parser;
    auto             data = fastgltf::GltfDataBuffer::FromPath(path);
    LGT_ASSERT_MSG(data.error() == fastgltf::Error::None, "Canot open gltf file {}", path.string());
    auto gltf = parser.loadGltf(
        data.get(), path.parent_path(), fastgltf::Options::LoadExternalBuffers | fastgltf::Options::GenerateMeshIndices);
    LGT_ASSERT_MSG(gltf.error() == fastgltf::Error::None, "Failed to parse gltf file {}", path.string());

    // Load materials
    for (auto& material : gltf->materials) {
        Material m;
        if (material.pbrData.baseColorTexture.has_value()) {
            auto& tex = gltf->textures[material.pbrData.baseColorTexture->textureIndex];
            if (tex.imageIndex.has_value())
                m.baseColorTexture = gltf->images[*tex.imageIndex].name;
        } else {
            m.baseColorFactor = {material.pbrData.baseColorFactor[0],
                                 material.pbrData.baseColorFactor[1],
                                 material.pbrData.baseColorFactor[2],
                                 material.pbrData.baseColorFactor[3]};
        }

        if (material.pbrData.metallicRoughnessTexture.has_value()) {
            auto& tex = gltf->textures[material.pbrData.metallicRoughnessTexture->textureIndex];
            if (tex.imageIndex.has_value())
                m.metallicRoughnessTexture = gltf->images[*tex.imageIndex].name;
        } else {
            m.metallicFactor  = material.pbrData.metallicFactor;
            m.roughnessFactor = material.pbrData.roughnessFactor;
        }

        if (material.normalTexture.has_value()) {
            auto& tex = gltf->textures[material.normalTexture->textureIndex];
            if (tex.imageIndex.has_value())
                m.normalTexture = gltf->images[*tex.imageIndex].name;
        }
        model->materials.push_back(std::move(m));
    }

    // Load mheshes
    for (auto& mesh : gltf->meshes) {
        for (auto& prim : mesh.primitives) {
            Assets::Mesh importedMesh;
            importedMesh.name = std::string(mesh.name);

            if (prim.materialIndex.has_value())
                importedMesh.materialIndex = static_cast<uint32_t>(*prim.materialIndex);

            // Positions
            auto posIt = prim.findAttribute("POSITION");
            if (posIt == prim.attributes.end())
                continue;
            fastgltf::iterateAccessor<fastgltf::math::fvec3>(gltf.get(), gltf->accessors[posIt->accessorIndex], [&](auto v) {
                Gpu::Vertex vtx{};
                vtx.position.x = v.x();
                vtx.position.y = v.y();
                vtx.position.z = v.z();
                importedMesh.vertices.push_back(vtx);
            });

            // Normals
            if (auto it = prim.findAttribute("NORMAL"); it != prim.attributes.end()) {
                size_t i = 0;
                fastgltf::iterateAccessor<fastgltf::math::fvec3>(gltf.get(), gltf->accessors[it->accessorIndex], [&](auto n) {
                    importedMesh.vertices[i].normal.x = n.x();
                    importedMesh.vertices[i].normal.y = n.y();
                    importedMesh.vertices[i].normal.z = n.z();
                    ++i;
                });
            }

            // UVs
            if (auto it = prim.findAttribute("TEXCOORD_0"); it != prim.attributes.end()) {
                size_t i = 0;
                fastgltf::iterateAccessor<fastgltf::math::fvec2>(gltf.get(), gltf->accessors[it->accessorIndex], [&](auto uv) {
                    importedMesh.vertices[i].uv.x = uv.x();
                    importedMesh.vertices[i].uv.y = uv.y();
                    ++i;
                });
            }

            // Tangents
            if (auto it = prim.findAttribute("TANGENT"); it != prim.attributes.end()) {
                size_t i = 0;
                fastgltf::iterateAccessor<fastgltf::math::fvec4>(gltf.get(), gltf->accessors[it->accessorIndex], [&](auto t) {
                    importedMesh.vertices[i].tangent[0] = t.x();
                    importedMesh.vertices[i].tangent[1] = t.y();
                    importedMesh.vertices[i].tangent[2] = t.z();
                    importedMesh.vertices[i].tangent[3] = t.w();
                    ++i;
                });
            }

            // Indices
            if (prim.indicesAccessor.has_value()) {
                fastgltf::iterateAccessor<uint32_t>(gltf.get(), gltf->accessors[*prim.indicesAccessor], [&](uint32_t idx) {
                    importedMesh.indices.push_back(idx);
                });
            }

            model->meshes.push_back(std::move(importedMesh));
        }
    }

    RENDERX_INFO(
        "GltfImporter: '{}' : {} meshes, {} materials", path.filename().string(), model->meshes.size(), model->materials.size());
    return true;
}

bool LoadFbx(const std::filesystem::path& path, Model* model) {
    return false;
}

bool LoadObj(const std::filesystem::path& path, Mesh* mesh) {
    return false;
}

bool LoadStb(const std::filesystem::path& path, Texture* texture) {
    return false;
}

bool LoadGlsl(const std::filesystem::path& path, Shader* shader) {
    return false;
}

} // namespace Lgt::Assets