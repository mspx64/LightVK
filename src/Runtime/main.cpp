#include "Core/LighVk.h"
#include "Player.h"

class RuntimeApp : public Lgt::Application {
public:
    void OnInit() override {
        Lgt::SceneSerializer serializer(world_.get());
        if (serializer.DeserializeBinary("scene.bin")) {
            LIGHTVK_INFO("Successfully loaded scene.bin in Runtime!");
        } else {
            LIGHTVK_ERROR("Failed to load scene.bin");
        }

        player_system_.Init(world_.get());
    }

    void OnUpdate(uint32_t currentFrame) override {
        auto view = world_->Registry().view<Game::Components::Player, Lgt::Component::Transform>();
        for (auto&& [enitty, player, transform] : view.each()) {
            LIGHTVK_TRACE(
                "Player {} : x {} , y{} , z {}", player.name, transform.position.x, transform.position.y, transform.position.z);
        }
        player_system_.Update(timer_->DeltaTime(), input_.get(), world_.get());
    }

private:
    Game::PlayerSystem player_system_;
};

int main() {
    RuntimeApp app;
    app.Init();
    app.Run();
    app.Shutdown();
    return 0;
}
