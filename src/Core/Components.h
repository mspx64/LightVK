#pragma once
#include <string>

#include <glm/glm.hpp>

// REQUIRED extensions
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Lgt::Component {

struct Tag {
    std::string name = "Entity";
};

struct Transform {
    glm::vec3 position = {0.f, 0.f, 0.f};
    glm::quat rotation = glm::identity<glm::quat>();
    glm::vec3 scale    = {1.f, 1.f, 1.f};

    glm::mat4 Matrix() const {
        glm::mat4 t = glm::translate(glm::mat4(1.f), position);
        glm::mat4 r = glm::mat4_cast(rotation);
        glm::mat4 s = glm::scale(glm::mat4(1.f), scale);
        return t * r * s;
    }
};

struct Material {
    glm::vec4 albedo           = {1.f, 1.f, 1.f, 1.f};
    float     roughness        = 0.5f;
    float     metallic         = 0.0f;
    float     emissiveStrength = 0.0f;
};

struct DirectionalLight {
    glm::vec3 direction = glm::normalize(glm::vec3(-0.5f, -1.f, -0.8f));
    glm::vec3 color     = {1.f, 1.f, 1.f};
    float     intensity = 1.f;
};

struct PointLight {
    glm::vec3 color     = {1.f, 1.f, 1.f};
    float     intensity = 1.f;
    float     radius    = 10.f;
};

struct Mesh {
    uint32_t vertexBufferIndex = 0;
    uint32_t indexBufferIndex  = 0;
    uint32_t materialIndex     = 0;
};

struct Camera {};

} // namespace Lgt::Component