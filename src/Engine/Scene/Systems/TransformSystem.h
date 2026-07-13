#pragma once;

namespace Lgt {
class World;
} // namespace  Lgt

namespace Lgt::Systems {

class Transform {
public:
    explicit Transform(World* world);
    void Update();

private:
    World* world_ = nullptr;
};
} // namespace Lgt::Systems
