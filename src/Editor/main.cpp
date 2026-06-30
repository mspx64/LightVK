#include "Core/Application.h"
#include "Core/LighVk.h" // For WIDTH/HEIGHT macros and other engine bits
#include <algorithm>
#include "Editor/Assets.h"
#include "Core/Vulkan/Context.h"
#include "Core/Gpu/Context.h"
#include "Core/Gpu/Resource.h"
#include "Core/SceneSerializer.h"
#include "Core/Components.h"
#include "Core/Entity.h"

class EditorApp : public Lgt::Application {
public:
    Lgt::Gpu::DrawList drawList;

    // Editor Camera
    Lgt::Component::Camera editorCamera;
    glm::vec3              cameraPos{0.f, 2.f, 5.f};
    float                  moveSpeed = 5.0f;
    float                  mouseSens = 0.1f;

    void OnInit() override {
        Lgt::Assets::Model model;
        Lgt::Assets::LoadGltf("D:/DEV/cpp/LightVK/Assets/Sphere/Untitled.gltf", &model);
        drawList = BuildDrawList(model);

        // Test scene setup
        auto e = world_->CreateEntity("TestSphere"); // Tag + Transform auto-added

        // Save scene
        // Save scene
        Lgt::SceneSerializer serializer(world_.get());
        serializer.SerializeBinary("scene.bin");

        // Init camera facing forward
        editorCamera.yaw   = -90.f;
        editorCamera.pitch = 0.f;
    }

    void UpdateCamera(float dt) {
        // Only move/look when right mouse button is held
        if (input_->IsMouseDown(Lgt::Mouse::Right)) {
            input_->SetCursorCaptured(true);

            // Mouse look
            glm::vec2 delta     = input_->GetMouseDelta();
            editorCamera.yaw   += delta.x * mouseSens;
            editorCamera.pitch -= delta.y * mouseSens;
            editorCamera.pitch  = std::clamp(editorCamera.pitch, -89.f, 89.f);

            glm::vec3 front;
            front.x            = cosf(glm::radians(editorCamera.pitch)) * cosf(glm::radians(editorCamera.yaw));
            front.y            = cosf(glm::radians(editorCamera.pitch)) * sinf(glm::radians(editorCamera.yaw));
            front.z            = sinf(glm::radians(editorCamera.pitch));
            editorCamera.front = glm::normalize(front);

            // Movement
            glm::vec3 forward = glm::normalize(glm::vec3(editorCamera.front.x, editorCamera.front.y, 0.f));
            glm::vec3 right   = glm::normalize(glm::cross(forward, editorCamera.up));

            float speed = moveSpeed;
            if (input_->IsKeyDown(Lgt::Key::LeftShift))
                speed *= 2.0f;

            glm::vec3 move{0.f};
            if (input_->IsKeyDown(Lgt::Key::W))
                move += forward;
            if (input_->IsKeyDown(Lgt::Key::S))
                move -= forward;
            if (input_->IsKeyDown(Lgt::Key::A))
                move -= right;
            if (input_->IsKeyDown(Lgt::Key::D))
                move += right;
            if (input_->IsKeyDown(Lgt::Key::E))
                move.z += 1.f;
            if (input_->IsKeyDown(Lgt::Key::Q))
                move.z -= 1.f;

            if (glm::length(move) > 0.001f) {
                cameraPos += glm::normalize(move) * speed * dt;
            }
        } else {
            input_->SetCursorCaptured(false);
        }
    }

    void OnUpdate(uint32_t currentFrame) override {
        float dt = static_cast<float>(timer_->DeltaTime());
        UpdateCamera(dt);

        float aspect = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);

        Lgt::Gpu::FrameUBO frameData{};
        frameData.view  = editorCamera.ViewMatrix(cameraPos);
        frameData.proj  = editorCamera.ProjectionMatrix(aspect);
        float time      = static_cast<float>(timer_->TotalTime());
        frameData.color = {
            (sinf(time * 2.0f) + 1.0f) * 0.5f, (cosf(time * 1.5f) + 1.0f) * 0.5f, (sinf(time * 3.0f) + 1.0f) * 0.5f, 1.0f};

        Lgt::Gpu::g_Context.renderer->Render(&drawList, currentFrame, frameData);
    }

    Lgt::Gpu::DrawList BuildDrawList(const Lgt::Assets::Model& model) {

        Lgt::Gpu::DrawCommand* commands    = new Lgt::Gpu::DrawCommand[model.meshes.size()];
        uint32_t*              indexCounts = new uint32_t[model.meshes.size()];

        for (unsigned int i = 0; i < model.meshes.size(); ++i) {

            auto vbo = Lgt::Gpu::CreateSSBO(model.meshes[i].vertices.size() * sizeof(Lgt::Gpu::Vertex));
            auto ibo = Lgt::Gpu::CreateSSBO(model.meshes[i].indices.size() * sizeof(uint32_t));

            Lgt::Vulkan::g_Context.uploader->UploadBuffer(Lgt::Gpu::g_Buffers.Get(vbo)->buffer,
                                                          model.meshes[i].vertices.data(),
                                                          model.meshes[i].vertices.size() * sizeof(Lgt::Gpu::Vertex));

            Lgt::Vulkan::g_Context.uploader->UploadBuffer(Lgt::Gpu::g_Buffers.Get(ibo)->buffer,
                                                          model.meshes[i].indices.data(),
                                                          model.meshes[i].indices.size() * sizeof(uint32_t));

            commands[i].vertexBufferIndex = Lgt::Gpu::g_Context.resourceHeap->AllocateSSBO(vbo);
            commands[i].indexBufferIndex  = Lgt::Gpu::g_Context.resourceHeap->AllocateSSBO(ibo);
            indexCounts[i]                = model.meshes[i].indices.size();
        }

        Lgt::Gpu::DrawList drawlist{};
        drawlist.commands    = commands;
        drawlist.count       = model.meshes.size();
        drawlist.indexCounts = indexCounts;
        Lgt::Vulkan::g_Context.uploader->Flush();

        return drawlist;
    }
};

int main() {
    EditorApp app;
    app.Init();
    app.Run();
    app.Shutdown();
    return 0;
}
