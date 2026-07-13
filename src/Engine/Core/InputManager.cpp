// src/core/InputManager.cpp
#include "Engine/Core/InputManager.h"
#include <GLFW/glfw3.h>

namespace Lgt {

float InputManager::s_ScrollAccum = 0.f;

void InputManager::ScrollCallback(GLFWwindow*, double, double yOffset) {
    s_ScrollAccum += static_cast<float>(yOffset);
}

InputManager::InputManager(GLFWwindow* window)
    : m_Window(window) {
    glfwSetScrollCallback(window, ScrollCallback);
    m_KeyCurr.fill(false);
    m_KeyPrev.fill(false);
    m_BtnCurr.fill(false);
    m_BtnPrev.fill(false);
}

void InputManager::BeginFrame() {
    // Snapshot previous state
    m_KeyPrev = m_KeyCurr;
    m_BtnPrev = m_BtnCurr;

    // Poll keys
    for (int k = GLFW_KEY_SPACE; k <= GLFW_KEY_LAST; ++k)
        m_KeyCurr[k] = glfwGetKey(m_Window, k) == GLFW_PRESS;

    // Poll mouse buttons
    for (int b = 0; b < kMaxButtons; ++b)
        m_BtnCurr[b] = glfwGetMouseButton(m_Window, b) == GLFW_PRESS;

    // Mouse position + delta
    double mx = 0.0, my = 0.0;
    glfwGetCursorPos(m_Window, &mx, &my);
    glm::vec2 newPos = {static_cast<f32>(mx), static_cast<f32>(my)};
    m_MouseDelta     = m_CursorCaptured ? (newPos - m_MousePrev) : glm::vec2(0.f);
    m_MousePrev      = newPos;
    m_MousePos       = newPos;

    // Consume scroll
    m_ScrollDelta = s_ScrollAccum;
    s_ScrollAccum = 0.f;
}

bool InputManager::IsKeyDown(int key) const {
    return key >= 0 && key < kMaxKeys && m_KeyCurr[key];
}
bool InputManager::WasKeyPressed(int key) const {
    return key >= 0 && key < kMaxKeys && m_KeyCurr[key] && !m_KeyPrev[key];
}
bool InputManager::WasKeyReleased(int key) const {
    return key >= 0 && key < kMaxKeys && !m_KeyCurr[key] && m_KeyPrev[key];
}

bool InputManager::IsMouseDown(int b) const {
    return b >= 0 && b < kMaxButtons && m_BtnCurr[b];
}
bool InputManager::WasMousePressed(int b) const {
    return b >= 0 && b < kMaxButtons && m_BtnCurr[b] && !m_BtnPrev[b];
}

void InputManager::SetCursorCaptured(bool captured) {
    m_CursorCaptured = captured;
    glfwSetInputMode(m_Window, GLFW_CURSOR, captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    // Reset delta so we don't get a jump on first captured frame
    double mx = 0.0, my = 0.0;
    glfwGetCursorPos(m_Window, &mx, &my);
    m_MousePrev = {static_cast<f32>(mx), static_cast<f32>(my)};
}

} // namespace Lgt
