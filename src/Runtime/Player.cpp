#include "Player.h"
#include <algorithm>

namespace Game {

void PlayerSystem::Init(Lgt::World* world) {
    player_ = world->CreateEntity("Player"); // auto-gets Tag + Transform

    // Game-specific data
    player_.Add<Components::Player>("Mahesh", 100, true);

    // Camera: offset slightly above the mesh origin (eye height)
    auto& cam = player_.Add<Lgt::Component::Camera>();
    cam.pos   = {0.f, 0.f, 1.7f}; // 1.7 units up (eye height)
    cam.yaw   = -90.f;
    cam.pitch = 0.f;

    // Mesh — indices will be set after asset loading
    player_.Add<Lgt::Component::Mesh>();
}

void PlayerSystem::Update(float dt, Lgt::InputManager* input, Lgt::World* world) {
    // Toggle cursor capture with Escape
    if (input->WasKeyPressed(Lgt::Key::Escape)) {
        input->SetCursorCaptured(!input->IsCursorCaptured());
    }

    HandleMouseLook(dt, input);
    HandleMovement(dt, input);
}

void PlayerSystem::HandleMouseLook(float dt, Lgt::InputManager* input) {
    if (!input->IsCursorCaptured())
        return;

    auto& cam = player_.Get<Lgt::Component::Camera>();

    glm::vec2 delta = input->GetMouseDelta();
    cam.yaw += delta.x * config_.mouseSens;
    cam.pitch -= delta.y * config_.mouseSens; // inverted Y

    // Clamp pitch to avoid gimbal lock
    cam.pitch = std::clamp(cam.pitch, -89.f, 89.f);

    // Recompute front vector from yaw/pitch (spherical -> cartesian)
    glm::vec3 front;
    float     yawRad   = glm::radians(cam.yaw);
    float     pitchRad = glm::radians(cam.pitch);
    front.x            = cosf(pitchRad) * cosf(yawRad);
    front.y            = cosf(pitchRad) * sinf(yawRad);
    front.z            = sinf(pitchRad);
    cam.front          = glm::normalize(front);
}

void PlayerSystem::HandleMovement(float dt, Lgt::InputManager* input) {
    auto& transform = player_.Get<Lgt::Component::Transform>();
    auto& cam       = player_.Get<Lgt::Component::Camera>();

    // Build movement basis from camera direction (flattened to XY plane)
    glm::vec3 forward = glm::normalize(glm::vec3(cam.front.x, cam.front.y, 0.f));
    glm::vec3 right   = glm::normalize(glm::cross(forward, cam.up));

    float speed = config_.moveSpeed;
    if (input->IsKeyDown(Lgt::Key::LeftShift)) {
        speed *= config_.sprintMult;
    }

    glm::vec3 move{0.f};

    if (input->IsKeyDown(Lgt::Key::W))
        move += forward;
    if (input->IsKeyDown(Lgt::Key::S))
        move -= forward;
    if (input->IsKeyDown(Lgt::Key::A))
        move -= right;
    if (input->IsKeyDown(Lgt::Key::D))
        move += right;
    if (input->IsKeyDown(Lgt::Key::Space))
        move.z += 1.f; // fly up
    if (input->IsKeyDown(Lgt::Key::LeftControl))
        move.z -= 1.f; // fly down

    // Normalize to prevent faster diagonal movement
    if (glm::length(move) > 0.001f) {
        move = glm::normalize(move);
    }

    transform.position += move * speed * static_cast<float>(dt);
}

void PlayerSystem::ShutDown() {
    // cleanup if needed
}

} // namespace Game