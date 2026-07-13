// src/core/Timer.cpp
#include "Engine/Core/Timer.h"
#include <GLFW/glfw3.h>
#include <cmath>

namespace Lgt {

Timer::Timer() {
    m_StartTime = glfwGetTime();
    m_LastTime  = m_StartTime;
}

void Timer::Tick() {
    f64 now    = glfwGetTime();
    m_Delta    = now - m_LastTime;
    m_LastTime = now;
    m_Total    = now - m_StartTime;
    ++m_Frame;

    // Clamp runaway deltas (e.g. debugger pause)
    if (m_Delta > 0.25)
        m_Delta = 0.25;

    // Exponential moving average for FPS
    f64 instantFps = (m_Delta > 0.0) ? 1.0 / m_Delta : 0.0;
    if (m_FpsSmooth == 0.0)
        m_FpsSmooth = instantFps;
    else
        m_FpsSmooth = m_FpsSmooth * (1.0 - m_FpsAlpha) + instantFps * m_FpsAlpha;
    m_Fps = m_FpsSmooth;
}

void Timer::SetSmoothWindow(u32 samples) {
    // α ≈ 2/(N+1) for an EMA approximating an N-sample SMA
    m_FpsAlpha = (samples > 0) ? 2.0 / (static_cast<f64>(samples) + 1.0) : 0.05;
}

} // namespace Lgt
