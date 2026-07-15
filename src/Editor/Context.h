#pragma once
#include "Engine/Scene/Entity.h"

namespace Lgt {

namespace Editor {

struct Context {
    World* world          = nullptr;
    Entity selectedEntity = Entity::Null();
};

} // namespace Editor

} // namespace Lgt