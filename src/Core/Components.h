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
    uint32_t indexCount        = 0; // number of indices (for vkCmdDraw)
};

struct Camera {
    glm::vec3 up    = {0.f, 0.f, 1.f};
    glm::vec3 front = {0.f, 1.f, 0.f};
    glm::vec3 pos   = {0.f, 0.f, 0.f}; // relative to the parent entity

    float yaw   = -90.f; // degrees
    float pitch =   0.f;

    float fov       = 60.f;  // vertical FOV in degrees
    float nearClip  = 0.1f;
    float farClip   = 500.f;
    float sensitivity = 0.1f;

    // View matrix from entity world position + camera offset
    glm::mat4 ViewMatrix(const glm::vec3& entityPos) const {
        glm::vec3 eye = entityPos + pos;
        return glm::lookAt(eye, eye + front, up);
    }

    // Perspective projection matrix
    glm::mat4 ProjectionMatrix(float aspectRatio) const {
        return glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
    }
};

} // namespace Lgt::Component