#include "Entity.h"
#include "World.h"
#include "Components.h"
#include "Gpu/Renderer.h"

namespace Lgt {

World::World() {}

Entity World::CreateEntity(std::string name) {
    auto   handle = m_Registry.create();
    Entity e(handle, this);
    e.Add<Component::Tag>(name);
    e.Add<Component::Transform>();
    return e;
}

void World::destroyEntity(Entity entity) {
    m_Registry.destroy(entity.Handle());
}

void World::update(float /*deltaTime*/) {
    // Placeholder for physics, animation, script systems
}

Gpu::DrawList World::BuildDrawList() {
    auto view = m_Registry.view<Component::Mesh, Component::Transform>();

    // Count only entities with actual drawable mesh data
    uint32_t count = 0;
    for (auto entity : view) {
        auto& mesh = view.get<Component::Mesh>(entity);
        if (mesh.indexCount > 0)
            count++;
    }

    if (count == 0)
        return {};

    auto* commands    = new Gpu::DrawCommand[count];
    auto* indexCounts = new uint32_t[count];

    uint32_t i = 0;
    for (auto entity : view) {
        auto& mesh = view.get<Component::Mesh>(entity);
        if (mesh.indexCount == 0)
            continue;

        auto& transform = view.get<Component::Transform>(entity);

        commands[i].vertexBufferIndex = mesh.vertexBufferIndex;
        commands[i].indexBufferIndex  = mesh.indexBufferIndex;
        commands[i].materialIndex     = mesh.materialIndex;
        commands[i].transform         = transform.Matrix();

        indexCounts[i] = mesh.indexCount;
        i++;
    }

    return {commands, indexCounts, count};
}

} // namespace Lgt