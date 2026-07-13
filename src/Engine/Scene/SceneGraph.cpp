#include "SceneGraph.h"
#include "Engine/Scene/Components.h"

namespace Lgt {

void SceneGraph::Detach(Entity entity) {

    auto& entity_h = entity.Get<Component::Hierarchy>();

    if (!entity_h.parent.IsValid())
        return;

    auto& parent_h = entity_h.parent.Get<Component::Hierarchy>();

    if (entity_h.nextSibling.IsValid()) {
        entity_h.nextSibling.Get<Component::Hierarchy>().prevSibling = entity_h.prevSibling;
    }

    if (entity_h.prevSibling.IsValid()) {
        entity_h.prevSibling.Get<Component::Hierarchy>().nextSibling = entity_h.nextSibling;
    } else {
        parent_h.firstChild = entity_h.nextSibling;
    }

    entity_h.parent      = Entity::Null();
    entity_h.nextSibling = Entity::Null();
    entity_h.prevSibling = Entity::Null();
}

void SceneGraph::Attach(Entity child, Entity parent) {

    // detach the child from the current parent before attaching to the new parent;
    Detach(child);

    auto& parent_h = parent.Get<Component::Hierarchy>();
    auto& child_h  = child.Get<Component::Hierarchy>();

    if (parent_h.firstChild.IsValid()) {
        auto& parent_child_h       = parent_h.firstChild.Get<Component::Hierarchy>();
        parent_child_h.prevSibling = child;
    }

    child_h.parent      = parent;
    parent_h.firstChild = child;
    child_h.prevSibling = Entity::Null();
    child_h.nextSibling = parent_h.firstChild;
}

void SceneGraph::SetParent(Entity child, Entity parent) {
    // TODO add cycle detections here
    // this function handels the graph validation
    Attach(child, parent);
}

} // namespace  Lgt
