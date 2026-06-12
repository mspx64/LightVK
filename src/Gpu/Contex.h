#pragma once

#include "Renderer.h"
#include "Resource.h"
#include "DescriptorHeap.h"

namespace Lgt::Gpu {

struct Context {
    Renderer*       renderer     = nullptr;
    DescriptorHeap* resourceHeap = nullptr;
    DescriptorHeap* samplerHeap  = nullptr;
    
    void Init(GLFWwindow* window);  
    void Shoutdown();
};

extern Context                              g_Contex;
extern ResourcePool<Texture, TextureHandle> g_Textures;
extern ResourcePool<Buffer, BufferHandle>   g_Buffers;

} // namespace Lgt::Gpu