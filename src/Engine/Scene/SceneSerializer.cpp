#include "Engine/Scene/SceneSerializer.h"
#include "Engine/Scene/Components.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Core/Logger.h"

#include <fstream>
#include <vector>

namespace Lgt {

SceneSerializer::SceneSerializer(World* world)
    : m_World(world) {}

template <typename T> static void SerializeComponentArray(std::ofstream& out, entt::registry& reg) {
    auto     view  = reg.view<T>();
    uint32_t count = (uint32_t)view.size();
    out.write(reinterpret_cast<const char*>(&count), sizeof(uint32_t));

    for (auto entity : view) {
        entt::entity e = entity;
        out.write(reinterpret_cast<const char*>(&e), sizeof(entt::entity));
        if constexpr (!std::is_empty_v<T>) {
            T& component = view.template get<T>(entity);
            out.write(reinterpret_cast<const char*>(&component), sizeof(T));
        }
    }
}

template <typename T> static void DeserializeComponentArray(std::ifstream& in, entt::registry& reg) {
    uint32_t count;
    in.read(reinterpret_cast<char*>(&count), sizeof(uint32_t));

    for (uint32_t i = 0; i < count; ++i) {
        entt::entity e;
        in.read(reinterpret_cast<char*>(&e), sizeof(entt::entity));

        if (!reg.valid(e)) {
            e = reg.create(e);
        }

        if constexpr (!std::is_empty_v<T>) {
            T component;
            in.read(reinterpret_cast<char*>(&component), sizeof(T));
            reg.emplace_or_replace<T>(e, component);
        } else {
            reg.emplace_or_replace<T>(e);
        }
    }
}

bool SceneSerializer::SerializeBinary(const std::filesystem::path& filepath) {
    std::ofstream out(filepath, std::ios::binary);
    if (!out.is_open()) {
        LIGHTVK_ERROR("Failed to open file for writing: {}", filepath.string());
        return false;
    }

    entt::registry& reg = m_World->Registry();

    // Serialize Tags (special case for std::string)
    auto     tagView  = reg.view<Component::Tag>();
    uint32_t tagCount = (uint32_t)tagView.size();
    out.write(reinterpret_cast<const char*>(&tagCount), sizeof(uint32_t));
    for (auto entity : tagView) {
        entt::entity e = entity;
        out.write(reinterpret_cast<const char*>(&e), sizeof(entt::entity));
        auto&    tag    = tagView.get<Component::Tag>(entity);
        uint32_t strLen = (uint32_t)tag.name.size();
        out.write(reinterpret_cast<const char*>(&strLen), sizeof(uint32_t));
        out.write(tag.name.data(), strLen);
    }

    // Serialize POD Components
    SerializeComponentArray<Component::WorldTransform>(out, reg);
    SerializeComponentArray<Component::LocalTransform>(out, reg);
    SerializeComponentArray<Component::Hierarchy>(out, reg);
    SerializeComponentArray<Component::Material>(out, reg);
    SerializeComponentArray<Component::DirectionalLight>(out, reg);
    SerializeComponentArray<Component::PointLight>(out, reg);
    SerializeComponentArray<Component::Mesh>(out, reg);
    SerializeComponentArray<Component::Camera>(out, reg);

    out.close();
    return true;
}

bool SceneSerializer::DeserializeBinary(const std::filesystem::path& filepath) {
    std::ifstream in(filepath, std::ios::binary);
    if (!in.is_open()) {
        LIGHTVK_ERROR("Failed to open file for reading: {}", filepath.string());
        return false;
    }

    entt::registry& reg = m_World->Registry();
    reg.clear();

    // Deserialize Tags
    uint32_t tagCount;
    in.read(reinterpret_cast<char*>(&tagCount), sizeof(uint32_t));
    for (uint32_t i = 0; i < tagCount; ++i) {
        entt::entity e;
        in.read(reinterpret_cast<char*>(&e), sizeof(entt::entity));

        uint32_t strLen;
        in.read(reinterpret_cast<char*>(&strLen), sizeof(uint32_t));
        std::string name(strLen, '\0');
        in.read(name.data(), strLen);

        if (!reg.valid(e))
            e = reg.create(e);
        reg.emplace_or_replace<Component::Tag>(e, name);
    }

    // Deserialize POD Components
    DeserializeComponentArray<Component::WorldTransform>(in, reg);
    DeserializeComponentArray<Component::LocalTransform>(in, reg);
    DeserializeComponentArray<Component::Hierarchy>(in, reg);
    DeserializeComponentArray<Component::Material>(in, reg);
    DeserializeComponentArray<Component::DirectionalLight>(in, reg);
    DeserializeComponentArray<Component::PointLight>(in, reg);
    DeserializeComponentArray<Component::Mesh>(in, reg);
    DeserializeComponentArray<Component::Camera>(in, reg);

    in.close();
    return true;
}

} // namespace Lgt
