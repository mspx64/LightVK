#include "Core/LighVk.h"
#include "Player.h"

class RuntimeApp : public Lgt::Application {
public:
    void OnInit() override {
        Lgt::SceneSerializer serializer(&world_);
        if (serializer.DeserializeBinary("scene.bin")) {
            LIGHTVK_INFO("Successfully loaded scene.bin in Runtime!");
        } else {
            LIGHTVK_WARN("Failed to load scene.bin");
        }

        auto player = world_.CreateEntity("Player");
        player.Add<Game::Components::Player>("Mahesh", 100, true);
        player.Add<Lgt::Component::Transform>();
    }

    void OnUpdate(uint32_t currentFrame) override {}
};

int main() {
    RuntimeApp app;
    app.Init();
    app.Run();
    app.Shutdown();
    return 0;
}
