#include "Transform.h"
#include "Engine/Core/Logger.h"
#include "Engine/Scene/Components.h"

namespace Lgt {

Systems::Transform::Transform(World* world) {
    LGT_ASSERT(world);
    world_ = world;
}

void Systems::Transform::Update() {

    const auto& view = world_->Registry().view<Component::Hierarchy, Component::WorldTransform, Component::LocalTransform>();

    for (auto e : view) {
        Entity entity(e, world_);
        auto&  entity_h = entity.Get<Component::Hierarchy>();

        if (!entity_h.parent.IsValid())
            UpdateSubtree(entity);
    }
}

void Systems::Transform::ComputeWorld(Entity entity) {
    if (!entity.IsValid())
        return;

    auto& entity_h       = entity.Get<Component::Hierarchy>();
    auto& entity_world_t = entity.Get<Component::WorldTransform>();
    auto& entity_local_t = entity.Get<Component::LocalTransform>();

    if (!entity_h.parent.IsValid()) {
        entity_world_t.matrix = entity_local_t.Matrix();
        return;
    } else {
        auto& parent_world_t  = entity_h.parent.Get<Component::WorldTransform>();
        entity_world_t.matrix = parent_world_t.matrix * entity_local_t.Matrix();
    }
}

void Systems::Transform::UpdateSubtree(Entity entity) {

    if (!entity.IsValid())
        return;

    ComputeWorld(entity);

    auto& entity_h = entity.Get<Component::Hierarchy>();
    auto  next     = entity_h.firstChild;

    while (next.IsValid()) {
        UpdateSubtree(next);
        next = next.Get<Component::Hierarchy>().nextSibling;
    }
}

} // namespace  Lgt
