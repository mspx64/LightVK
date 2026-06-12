#pragma once

#include <functional>
#include <glm/glm.hpp>
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

    Entity createEntity(std::string name = "Entity");
    void   destroyEntity(Entity entity);
    void   update(float deltaTime);

    entt::registry&       registry() { return m_Registry; }
    const entt::registry& registry() const { return m_Registry; }
    Gpu::DrawList         drawList();

private:
    entt::registry m_Registry;
};

} // namespace Lgt