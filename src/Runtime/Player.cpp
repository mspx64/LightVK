#include "Player.h"

namespace Game {
void PlayerSystem::Init(const Lgt::World& world) {
    m_World = world;
    player  = m_World.CreateEntity("Player");
    player.Add<Components::Player>("Mahesh", 100, true);
    player.Add<Lgt::Component::Transform>();
}

void PlayerSystem::Update(float dt) {
    // palayer movement code
}

} // namespace Game