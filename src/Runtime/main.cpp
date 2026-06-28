#include "Core/Application.h"
#include "Core/SceneSerializer.h"
#include "Core/Components.h"
#include "Core/Logger.h"

class RuntimeApp : public Lgt::Application {
public:
    void OnInit() override {
        Lgt::SceneSerializer serializer(&world_);
        if (serializer.DeserializeBinary("scene.bin")) {
            LIGHTVK_INFO("Successfully loaded scene.bin in Runtime!");
        } else {
            LIGHTVK_WARN("Failed to load scene.bin");
        }
    }
    
    void OnUpdate(uint32_t currentFrame) override {
        auto view = world_.Registry().view<Lgt::Component::Transform>();
        for (auto entity : view) {
            auto& transform = view.get<Lgt::Component::Transform>(entity);
            // Simulate game logic (e.g., spinning objects)
            transform.position.y += 0.01f;
        }
    }
};

int main() {
    RuntimeApp app;
    app.Init();
    app.Run();
    app.Shutdown();
    return 0;
}
