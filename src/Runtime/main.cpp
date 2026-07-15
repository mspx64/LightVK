#include "Engine/Core/LightVK.h"
#include "Engine/Core/Logger.h"
#include "Player.h"
#include "Engine/Renderer/Gpu/Context.h"
#include "Engine/Renderer/Gpu/Renderer.h"
#include "Engine/UI/ImGuiLayer.h"

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
        player_system_.Update(timer_->DeltaTime(), input_.get(), world_.get());

        if (Lgt::Gpu::g_Context.renderer->BeginFrame(currentFrame)) {
            auto renderer = Lgt::Gpu::g_Context.renderer;

            BeginUi();
            EndUi();

            // renderer->Render(&drawList, currentFrame);

            renderer->EndFrame();
        }
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
