

#include "glm/glm.hpp"

namespace Lgt::Component {

struct Camera {};

struct Mesh {
    uint32_t vertexBufferIndex = 0;
    uint32_t indexBufferIndex  = 0;
    uint32_t materialIndex     = 0;
};

struct Transform {};
struct Tag {};

} // namespace Lgt::Component