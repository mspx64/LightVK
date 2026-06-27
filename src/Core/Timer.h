#pragma once
// include/RenderX/core/Timer.hpp
// ─────────────────────────────────────────────────────────────────────────────
// High-resolution frame timer using GLFW's monotonic clock.
// Call Tick() once per frame; access DeltaTime / TotalTime / FPS after.
// ─────────────────────────────────────────────────────────────────────────────

#include "Core.h"
namespace Lgt {

class Timer {
public:
    Timer();

    // Call at the very start of each frame
    void Tick();

    [[nodiscard]] f64 DeltaTime() const { return m_Delta; } // seconds
    [[nodiscard]] f64 TotalTime() const { return m_Total; } // seconds since Init
    [[nodiscard]] f64 FPS() const { return m_Fps; }
    [[nodiscard]] u64 FrameIndex() const { return m_Frame; }

    // Smooth FPS over N samples (default 60)
    void SetSmoothWindow(u32 samples);

private:
    f64 m_StartTime = 0.0;
    f64 m_LastTime  = 0.0;
    f64 m_Delta     = 0.0;
    f64 m_Total     = 0.0;
    f64 m_Fps       = 0.0;
    u64 m_Frame     = 0;

    // Exponential moving average for FPS
    f64 m_FpsAlpha  = 0.05; // weight of newest sample
    f64 m_FpsSmooth = 0.0;
};

} // namespace Lgt
