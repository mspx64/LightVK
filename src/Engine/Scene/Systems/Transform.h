#pragma once

namespace Lgt {
class World;
class Entity;
} // namespace  Lgt

namespace Lgt::System {

class Transform {
public:
    explicit Transform(World* world);
    void Update();

private:
    void   ComputeWorld(Entity entity);
    void   UpdateSubtree(Entity entity);
    World* world_ = nullptr;
};
} // namespace Lgt::Systems
