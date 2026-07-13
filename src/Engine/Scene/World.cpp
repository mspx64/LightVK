#include "Engine/Scene/Entity.h"
#include "Engine/Scene/World.h"
#include "Engine/Renderer/Gpu/Renderer.h"
#include "Engine/Scene/Components.h"

namespace Lgt {

World::World()
    : transform_sys(this) {}

Entity World::CreateEntity(std::string name) {
    auto   handle = m_Registry.create();
    Entity e(handle, this);
    // every entity must have this components;
    e.Add<Component::Hierarchy>();
    e.Add<Component::WorldTransform>();
    e.Add<Component::LocalTransform>();
    return e;
}

void World::destroyEntity(Entity entity) {
    m_Registry.destroy(entity.Handle());
}

void World::update(float /*deltaTime*/) {
    transform_sys.Update();
    // Placeholder for physics, animation, script systems
}

Gpu::DrawList World::DrawList() {
    return Gpu::DrawList();
}

} // namespace Lgt