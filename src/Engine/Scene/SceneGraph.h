#include "Engine/Scene/Entity.h"

namespace Lgt {
class SceneGraph {
public:
    explicit SceneGraph(World* world) {};
    void Detach(Entity entity);
    void Attach(Entity child, Entity parent);
    void SetParent(Entity parent, Entity child);

private:
    [[nodiscard]] bool IsDescendant(Entity parent, Entity entity) const noexcept;
    World*             world_;
};
} // namespace Lgt
