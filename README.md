# LightVK

A Vulkan-based game engine I'm building from scratch. The goal is to learn and implement modern GPU-driven rendering — bindless resources, ECS, a proper render graph, and eventually a level editor with a separate game runtime.

This is early in development. A lot of things are incomplete or actively being worked on.

---

## What it is

LightVK is a personal project focused on understanding how modern game engines and renderers actually work at a low level. Rather than using a high-level abstraction or an existing engine, I'm building everything from the ground up — windowing, Vulkan setup, memory management, ECS, asset loading, and rendering.

The rendering backend is built around `VK_EXT_descriptor_heap`, which allows a fully bindless resource model — no descriptor sets per draw, just indices into a global heap. This is the approach modern engines like Unreal 5 and id Tech use, and it's one of the main things I wanted to understand by building this.
 
---

## What's not done yet

- Full deferred PBR pipeline
- Scene serialization (save/load to binary)
- Material system and light system GPU upload
- Gizmos and camera controls in the editor
- Game runtime separate from the editor
- Texture loading and bindless texture table
- Shadow maps, any kind of GI

---

Two CMake targets:
- `RenderXCore` — static lib, all engine code
- `RenderX_Editor` — executable, links Core + ImGui

The plan is to eventually add a third `RenderX_Game` target that loads a binary scene file exported from the editor and runs hardcoded game logic — no scripting.

---

## Dependencies

Managed via vcpkg.

| Library | Purpose |
|---|---|
| Vulkan SDK | Graphics API + Volk loader |
| GLFW | Window and input |
| GLM | Math |
| EnTT | ECS |
| fastgltf | glTF/GLB loading |
| spdlog | Logging |
| ImGui | Editor UI (docking branch) |

---

## Building

Requires Vulkan SDK, vcpkg, and CMake 3.20+. Tested on Windows with clang-cl.

```bash
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build
```

Shaders are compiled automatically by CMake using glslc and copied next to the binary.

---

## Goals

The longer term plan for this project:

- Clustered/deferred PBR with a proper render graph
- Virtual shadow maps or cascaded shadow maps
- A working level editor that exports binary scene files
- Ray-traced shadows or RTGI as a separate render path
- Temporal upscaling (TAA or similar)

Mostly building this to understand how these systems work, not to ship a product.