#pragma once
#include "glm/glm.hpp"

namespace Lgt {

#define LGT_DEFINE_HANDLE(Name)                                                                                                  \
    namespace HandleType {                                                                                                       \
    struct Name {};                                                                                                              \
    }                                                                                                                            \
    using Name##Handle = Handle<HandleType::Name>;

// Base Handle Template
template <typename Tag> struct Handle {
public:
    using ValueType                    = uint64_t;
    static constexpr ValueType INVALID = 0;

    Handle(ValueType key)
        : id(key) {}
    Handle() = default;

    bool isValid() const { return id != INVALID; }

    // operators
    bool operator==(const Handle& o) const { return id == o.id; }
    bool operator!=(const Handle& o) const { return id != o.id; }
    bool operator<(const Handle& o) const { return id < o.id; }

    ValueType id = INVALID;
};

// GLM type aliases for consistency Temp (only
// using them for the testing/debug)
using Vec2  = glm::vec2;
using Vec3  = glm::vec3;
using Vec4  = glm::vec4;
using Mat3  = glm::mat3;
using Mat4  = glm::mat4;
using IVec2 = glm::ivec2;
using IVec3 = glm::ivec3;
using IVec4 = glm::ivec4;
using UVec2 = glm::uvec2;
using UVec3 = glm::uvec3;
using UVec4 = glm::uvec4;
using Quat  = glm::quat;

using u8  = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using i8  = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;
using f32 = float;
using f64 = double;

} // namespace Lgt