#pragma once

#include "../Core.h"
#include "Renderer.h"
#include "Resource.h"
#include "DescriptorHeap.h"

namespace Lgt::Gpu {

struct Context {
    Renderer*       renderer     = nullptr;
    DescriptorHeap* resourceHeap = nullptr;
    DescriptorHeap* samplerHeap  = nullptr;
    
    void Init(GLFWwindow* window);  
    void Shutdown();
};

extern LIGHTVK_API Context                              g_Context;
extern LIGHTVK_API ResourcePool<Texture, TextureHandle> g_Textures;
extern LIGHTVK_API ResourcePool<Buffer, BufferHandle>   g_Buffers;

} // namespace Lgt::Gpu