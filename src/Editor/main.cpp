#include "Engine/Core/Application.h"
#include "Editor/Assets.h"
#include "Engine/Renderer/Vulkan/Context.h"
#include "Engine/Renderer/Gpu/Context.h"
#include "Engine/Renderer/Gpu/Resource.h"
#include "Engine/Scene/SceneSerializer.h"
#include "Engine/Scene/Components.h"
#include "Engine/Scene/Entity.h"

class EditorApp : public Lgt::Application {
public:
    Lgt::Gpu::DrawList drawList;

    void OnInit() override {
        Lgt::Assets::Model model;
        Lgt::Assets::LoadGltf("D:/DEV/cpp/LightVK/Assets/Sphere/Untitled.gltf", &model);
        drawList = BuildDrawList(model);

        // Test scene setup
        auto e = world_->CreateEntity("TestSphere");

        // Save scene
        Lgt::SceneSerializer serializer(world_.get());
        serializer.SerializeBinary("scene.bin");
    }

    void OnUpdate(uint32_t currentFrame) override { Lgt::Gpu::g_Context.renderer->Render(&drawList, currentFrame); }

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
