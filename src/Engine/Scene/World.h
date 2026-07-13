#pragma once

#include <functional>
#include "Engine/Core/GlmConfig.h"
#include "Engine/Scene/Systems/Transform.h"
#include <entt/entt.hpp>

namespace Lgt {

namespace Gpu {
struct DrawList;
}

class Entity;

class World {
public:
    explicit World();
    ~World() = default;

    Entity CreateEntity(std::string name = "Entity");
    void   destroyEntity(Entity entity);
    void   update(float deltaTime);

    entt::registry&       Registry() { return m_Registry; }
    const entt::registry& Registry() const { return m_Registry; }
    Gpu::DrawList         DrawList();

private:
    Systems::Transform transform_sys;
    entt::registry     m_Registry;
};

} // namespace Lgt