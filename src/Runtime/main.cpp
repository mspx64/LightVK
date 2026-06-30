#include "Core/LighVk.h"
#include "Core/Gpu/Context.h"
#include "Core/Vulkan/Context.h"
#include "Player.h"

class RuntimeApp : public Lgt::Application {
public:
    void OnInit() override {
        // 1. Load scene (populates World with entities)
        Lgt::SceneSerializer serializer(world_.get());
        if (serializer.DeserializeBinary("scene.bin")) {
            LIGHTVK_INFO("Successfully loaded scene.bin in Runtime!");
        } else {
            LIGHTVK_ERROR("Failed to load scene.bin");
        }

        player_system_.Init(world_.get());
        input_->SetCursorCaptured(true);
    }

    void OnUpdate(uint32_t currentFrame) override {
        float dt = static_cast<float>(timer_->DeltaTime());

        player_system_.Update(dt, input_.get(), world_.get());

        auto  playerEntity = player_system_.GetPlayer();
        auto& cam          = playerEntity.Get<Lgt::Component::Camera>();
        auto& transform    = playerEntity.Get<Lgt::Component::Transform>();

        float aspect = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);

        Lgt::Gpu::FrameUBO frameData{};
        frameData.view  = cam.ViewMatrix(transform.position);
        frameData.proj  = cam.ProjectionMatrix(aspect);
        frameData.color = {1.f, 1.f, 1.f, 1.f};

        auto drawList = world_->BuildDrawList();

        Lgt::Gpu::g_Context.renderer->Render(&drawList, currentFrame, frameData);

        delete[] drawList.commands;
        delete[] drawList.indexCounts;
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
