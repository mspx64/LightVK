#include "Core/LighVk.h"

namespace Game {

namespace Components {
struct Player {
    std::string name;
    uint64_t    health  = 100;
    bool        isAlive = true;
};
} // namespace Components

struct PlayerConfig {
    float moveSpeed  = 5.0f;  // units/sec
    float sprintMult = 2.0f;  // shift multiplier
    float mouseSens  = 0.1f;  // degrees/pixel
};

class PlayerSystem {
public:
    void Init(Lgt::World* world);
    void Update(float dt, Lgt::InputManager* input, Lgt::World* world);
    void ShutDown();

    Lgt::Entity GetPlayer() const { return player_; }

private:
    Lgt::Entity  player_;
    PlayerConfig config_;

    void HandleMovement(float dt, Lgt::InputManager* input);
    void HandleMouseLook(float dt, Lgt::InputManager* input);
};
} // namespace Game
