#include "Player.h"

namespace Game {
void PlayerSystem::Init(Lgt::World& world) {

    player = world.CreateEntity("Player");
    player.Add<Components::Player>("Mahesh", 100, true);
    player.Add<Lgt::Component::Transform>();
    player.Add<Lgt::Component::Camera>();
    // player.Add<Lgt::Component::Mesh>();
}

void PlayerSystem::Update(float dt) {}

} // namespace Game