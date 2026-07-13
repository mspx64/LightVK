#include "Engine/Scene/Entity.h"

namespace Lgt {
class SceneGraph {
    explicit SceneGraph(World* world);
    void Detach(Entity entity);
    void Attach(Entity child, Entity parent);
    void SetParent(Entity child, Entity parent);
};
} // namespace Lgt
