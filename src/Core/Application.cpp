#include "Application.h"
#include "Vulkan/Contex.h"
#include "Gpu/Contex.h"

#include "Math.h"

namespace Lgt {

void Application::Init() {

    LOG_INIT();

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window_ = glfwCreateWindow(WIDTH, HEIGHT, "DoggEngine", nullptr, nullptr);

    Vulkan::g_Contex.Init(window_);
    auto props = Vulkan::g_Contex.device->DescriptorHeapProperties();
    RENDERX_INFO(
        "bufferDescriptorSize={}, bufferDescriptorAlignment={}", props.bufferDescriptorSize, props.bufferDescriptorAlignment);

    Gpu::g_Contex.Init(window_);

    auto prespective = Matrix::Perspective(1.45, WIDTH / HEIGHT, 1, 100);
    prespective.print();

}

void Application::Run() {
    uint32_t currentFrame = 0;

    Assets::Model model;

    Assets::LoadGltf("D:/DEV/cpp/Vulkan_exp/Assets/Models/Sphere/Untitled.gltf", &model);
    // Assets::LoadGltf("D:/DEV/cpp/Vulkan_exp/Assets/Models/Cube/Untitled.gltf", &model);
    // Assets::LoadGltf("D:/DEV/cpp/Vulkan_exp/Assets/Models/sopnza_palace/Sponza_palace.gltf", &model);
    // Assets::LoadGltf("D:/DEV/cpp/Vulkan_exp/Assets/Models/Monkey/Monkey.gltf", &model);

    auto drawList = BuildDrawList(model);
    while (!glfwWindowShouldClose(window_)) {
        Gpu::g_Contex.renderer->Render(&drawList, currentFrame);
        glfwPollEvents();
        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }
}

void Application::Shutdown() {
    Gpu::g_Contex.Shoutdown();
    Vulkan::g_Contex.Shoutdown();
}

Gpu::DrawList Application::BuildDrawList(const Assets::Model& model) {

    Gpu::DrawCommand* commands    = new Gpu::DrawCommand[model.meshes.size()];
    uint32_t*         indexCounts = new uint32_t[model.meshes.size()];

    for (unsigned int i = 0; i < model.meshes.size(); ++i) {

        auto vbo = Gpu::CreateSSBO(model.meshes[i].vertices.size() * sizeof(Gpu::Vertex));
        auto ibo = Gpu::CreateSSBO(model.meshes[i].indices.size() * sizeof(uint32_t));

        Vulkan::g_Contex.uploader->uploadBuffer(Gpu::g_Buffers.get(vbo)->buffer,
                                                model.meshes[i].vertices.data(),
                                                model.meshes[i].vertices.size() * sizeof(Gpu::Vertex));

        Vulkan::g_Contex.uploader->uploadBuffer(
            Gpu::g_Buffers.get(ibo)->buffer, model.meshes[i].indices.data(), model.meshes[i].indices.size() * sizeof(uint32_t));

        commands[i].vertexBufferIndex = Gpu::g_Contex.resourceHeap->AllocateSSBO(vbo);
        commands[i].indexBufferIndex  = Gpu::g_Contex.resourceHeap->AllocateSSBO(ibo);
        indexCounts[i]                = model.meshes[i].indices.size();
    }

    Gpu::DrawList drawlist{};
    drawlist.commands    = commands;
    drawlist.count       = model.meshes.size();
    drawlist.indexCounts = indexCounts;
    Vulkan::g_Contex.uploader->flush();

    return drawlist;
}

} // namespace Lgt
