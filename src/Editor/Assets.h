#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <glm/glm.hpp>

#include "Engine/Renderer/Gpu/Resource.h"
// forward decals

namespace Lgt::Assets {

// textures
struct Texture {};
// embedded textures in .glb;
struct EmbeddedTexture {
    std::string          name;
    std::vector<uint8_t> data;
};

struct Material {
    std::string name;

    std::string baseColorTexture;
    std::string normalTexture;
    std::string metallicRoughnessTexture; // G=roughness, B=metallic
    std::string emissiveTexture;          // Principled BSDF → Emission

    float     metallicFactor  = 1.f;          // fallback if no texture
    float     roughnessFactor = 1.f;          // fallback if no texture
    glm::vec4 baseColorFactor = {1, 1, 1, 1}; // tint / solid color objects
};

// TODO
struct Shader {};

struct Mesh {
    std ::string             name;
    std::vector<Gpu::Vertex> vertices;
    std::vector<uint32_t>    indices;
    uint32_t                 materialIndex = 0;
};

struct Model {
    std::string           name;
    std::vector<Mesh>     meshes;
    std::vector<Material> materials;
};

bool LoadGltf(const std::filesystem::path& path, Model* outmodel);
bool LoadFbx(const std::filesystem::path& path, Model* outmodel);
bool LoadObj(const std::filesystem::path& path, Mesh* outmesh);
bool LoadStb(const std::filesystem::path& path, Texture* outtexture);
bool LoadGlsl(const std::filesystem::path& path, Shader* outshader);

} // namespace Lgt::Assets
