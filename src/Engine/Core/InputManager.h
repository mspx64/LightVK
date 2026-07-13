#pragma once

#include "Core.h"
#include "Engine/Core/GlmConfig.h"
#include <array>

struct GLFWwindow;

namespace Lgt {

// Re-export GLFW key codes without exposing GLFW headers to consumers
// (values deliberately match GLFW so no translation is needed at runtime)
namespace Key {
static constexpr int Unknown = -1;
static constexpr int Space   = 32;
static constexpr int A = 65, B = 66, C = 67, D = 68, E = 69, F = 70;
static constexpr int G = 71, H = 72, I = 73, J = 74, K = 75, L = 76;
static constexpr int M = 77, N = 78, O = 79, P = 80, Q = 81, R = 82;
static constexpr int S = 83, T = 84, U = 85, V = 86, W = 87, X = 88;
static constexpr int Y = 89, Z = 90;
static constexpr int Escape = 256;
static constexpr int Enter  = 257;
static constexpr int Tab    = 258;
static constexpr int Left = 263, Right = 262, Up = 265, Down = 264;
static constexpr int LeftShift   = 340;
static constexpr int LeftControl = 341;
static constexpr int LeftAlt     = 342;
static constexpr int F1 = 290, F2 = 291, F3 = 292, F11 = 300;
} // namespace Key

namespace Mouse {
static constexpr int Left   = 0;
static constexpr int Right  = 1;
static constexpr int Middle = 2;
} // namespace Mouse

class InputManager {
public:
    explicit InputManager(GLFWwindow* window);

    // Call once at start of frame, before any IsKey* queries
    void BeginFrame();

    // Held this frame
    [[nodiscard]] bool IsKeyDown(int key) const;
    // Pressed this frame (down now, was up last frame)
    [[nodiscard]] bool WasKeyPressed(int key) const;
    // Released this frame
    [[nodiscard]] bool WasKeyReleased(int key) const;

    [[nodiscard]] bool IsMouseDown(int button) const;
    [[nodiscard]] bool WasMousePressed(int button) const;

    // Mouse position in window pixels
    [[nodiscard]] glm::vec2 GetMousePos() const { return m_MousePos; }
    // Delta since last frame (only non-zero when cursor is captured)
    [[nodiscard]] glm::vec2 GetMouseDelta() const { return m_MouseDelta; }
    [[nodiscard]] f32       GetScrollDelta() const { return m_ScrollDelta; }

    // Lock/unlock cursor (for FPS-style look)
    void               SetCursorCaptured(bool captured);
    [[nodiscard]] bool IsCursorCaptured() const { return m_CursorCaptured; }

private:
    static void ScrollCallback(GLFWwindow*, double, double yOffset);

    GLFWwindow* m_Window;

    static constexpr int kMaxKeys    = 512;
    static constexpr int kMaxButtons = 8;

    std::array<bool, kMaxKeys>    m_KeyCurr{};
    std::array<bool, kMaxKeys>    m_KeyPrev{};
    std::array<bool, kMaxButtons> m_BtnCurr{};
    std::array<bool, kMaxButtons> m_BtnPrev{};

    glm::vec2 m_MousePos       = {};
    glm::vec2 m_MousePrev      = {};
    glm::vec2 m_MouseDelta     = {};
    f32       m_ScrollDelta    = 0.f;
    bool      m_CursorCaptured = false;

    // Accumulated scroll between frames (written by GLFW callback)
    static float s_ScrollAccum;
};

} // namespace Lgt
