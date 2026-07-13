#include "Engine/Scene/Entity.h"
#include "Engine/Scene/World.h"
#include "Engine/Renderer/Gpu/Renderer.h"

namespace Lgt {

World::World() {}

Entity World::CreateEntity(std::string name) {
    auto   handle = m_Registry.create();
    Entity e(handle, this);
    return e;
}

void World::destroyEntity(Entity entity) {
    m_Registry.destroy(entity.Handle());
}

void World::update(float /*deltaTime*/) {
    // Placeholder for physics, animation, script systems
}

Gpu::DrawList World::DrawList() {
    return Gpu::DrawList();
}

} // namespace Lgt