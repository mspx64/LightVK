#pragma once
#include <cstdint>
#include <memory>
#include <string>

#include <Core/Vulkan/Helpers.h>

#include <Core/Logger.h>
#include <Core/Core.h>

namespace Lgt::Gpu {

struct Vertex {
    glm::vec3 position;
    uint32_t  pad0;
    glm::vec3 normal;
    uint32_t  pad1;
    glm::vec2 uv;
    uint32_t  pad3[2];
    glm::vec4 tangent;
};

struct Buffer {
    VkBuffer        buffer = VK_NULL_HANDLE;
    void*           mapped = nullptr;
    VkDeviceAddress deviceAddress;
    VmaAllocation   allocation = VK_NULL_HANDLE;
    // VmaAllocationInfo allocInfo  = {};
    VkDeviceSize size = 0;
    std::string  debugName;
};

struct Texture {
    VkImage       image            = VK_NULL_HANDLE;
    VmaAllocation allocation       = VK_NULL_HANDLE;
    VkFormat      format           = VK_FORMAT_UNDEFINED;
    uint32_t      width            = 0;
    uint32_t      height           = 0;
    uint32_t      mipLevels        = 1;
    uint32_t      arrayLayers      = 1;
    bool          isSwapchainImage = false;
    std::string   debugName;
};

LGT_DEFINE_HANDLE(Buffer);
LGT_DEFINE_HANDLE(Texture);

// Resource Pool Template
template <typename ResourceType, typename Handle> class ResourcePool {
public:
    using ValueType = uint32_t;

    ResourcePool() {
        _My_resource.emplace_back();
        _My_generation.emplace_back(1);
        _My_key = GenerateKey();
    }

    Handle allocate(ResourceType resource) {
        ValueType index;
        Handle    handle;

        if (_My_freelist.empty()) {
            index = static_cast<ValueType>(_My_resource.size());
            _My_resource.push_back(resource);
            _My_generation.push_back(1);
        } else {
            index = _My_freelist.back();
            _My_freelist.pop_back();
            _My_resource[index] = resource;
            ++_My_generation[index];
        }

        uint64_t raw = (static_cast<uint64_t>(_My_generation[index]) << 32) | static_cast<uint64_t>(index);

        handle.id = Encrypt(raw);
        return handle;
    }

    void free(Handle& handle) {
        LIGHTVK_ASSERT_MSG(handle.isValid(), "ResourcePool::free: trying to free an invalid handle");

        uint64_t raw   = Decrypt(handle.id);
        auto     index = static_cast<ValueType>(raw & 0xFFFFFFFF);
        auto     gen   = static_cast<ValueType>(raw >> 32);

        LGT_ASSERT_MSG(index < _My_resource.size() && _My_generation[index] == gen,
                       "ResourcePool::free: stale or foreign handle detected");

        handle.id           = 0;
        _My_resource[index] = ResourceType{};
        _My_freelist.push_back(index);
    }

    ResourceType* get(const Handle& handle) {
        if (!handle.isValid()) {
            LIGHTVK_WARN("ResourcePool::get : invalid handle");
            return nullptr;
        }

        uint64_t raw   = Decrypt(handle.id);
        auto     index = static_cast<ValueType>(raw & 0xFFFFFFFF);
        auto     gen   = static_cast<ValueType>(raw >> 32);

        if (!(index < _My_resource.size() && _My_generation[index] == gen)) {
            LIGHTVK_WARN("stale or foreign handle detected");
            return nullptr;
        }

        return &_My_resource[index];
    }

    template <typename Fn> void ForEach(Fn&& fn) {
        for (size_t i = 1; i < _My_resource.size(); ++i) {
            if (_My_generation[i] != 0) {
                fn(_My_resource[i]);
            }
        }
    }

    template <typename Fn> void ForEachAlive(Fn&& fn) {
        for (ValueType i = 1; i < _My_resource.size(); ++i) {
            // skip freed slots
            if (std::find(_My_freelist.begin(), _My_freelist.end(), i) != _My_freelist.end())
                continue;

            uint32_t gen = _My_generation[i];

            uint64_t raw = (static_cast<uint64_t>(gen) << 32) | static_cast<uint64_t>(i);

            Handle handle;
            handle.id = Encrypt(raw);

            fn(_My_resource[i], handle);
        }
    }

    bool IsAlive(const Handle& handle) const {
        if (!handle.isValid())
            return false;

        uint64_t raw   = Decrypt(handle.id);
        auto     index = static_cast<ValueType>(raw & 0xFFFFFFFF);
        auto     gen   = static_cast<ValueType>(raw >> 32);

        return index < _My_resource.size() && _My_generation[index] == gen;
    }

    void clear() {
        _My_resource.clear();
        _My_generation.clear();
        _My_freelist.clear();
        _My_resource.emplace_back();
        _My_generation.emplace_back(1);
    }

private:
    static uint64_t RotateLeft(uint64_t x, int r) { return (x << r) | (x >> (64 - r)); }
    static uint64_t RotateRight(uint64_t x, int r) { return (x >> r) | (x << (64 - r)); }

    uint64_t Encrypt(uint64_t value) const {
        value ^= _My_key;
        value  = RotateLeft(value, 17);
        return value;
    }

    uint64_t Decrypt(uint64_t value) const {
        value  = RotateRight(value, 17);
        value ^= _My_key;
        return value;
    }

    static uint64_t GenerateKey() {
        static std::atomic<uint64_t> counter{0xA5B35705F00DBAAD};
        return counter.fetch_add(0x9E3779B97F4A7C15ull);
    }

private:
    std::vector<ResourceType> _My_resource;
    std::vector<ValueType>    _My_generation;
    std::vector<ValueType>    _My_freelist;
    uint64_t                  _My_key = 0;
};

// BufferHandle CreateIBO(size_t size, bool dynamic = false);
// BufferHandle CreateVBO(size_t size, bool dynamic = false);
BufferHandle CreateSSBO(size_t size, bool dynamic = false);
BufferHandle CreateUBO(size_t size);

// TextureHandle createTexture();
// samplerHandle createSampler();

} // namespace Lgt::Gpu
