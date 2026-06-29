#include "Core/LighVk.h"

namespace Game {

namespace Components {
struct Player {
    std::string name;
    uint64_t    health;
    bool        isAlive;
};
} // namespace  Components

class PlayerSystem {
public:
    void Update(float dt);
    void Init(const Lgt::World& world);
    void ShutDown();

private:
    Lgt::World  m_World;
    Lgt::Entity player;
};
} // namespace Game
