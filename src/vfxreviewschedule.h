#pragma once

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>

// Stateless geometry in logical screen pixels. Stations run upper-right and
// the camera follows them, so stationary effects travel lower-left on screen.
struct VfxReviewSchedule
{
    struct Vec2 { double x = 0.0, y = 0.0; };
    struct Endpoints { Vec2 source, target; };

    std::size_t count = 0;
    double spacing = 420.0; // distance along the normalized screen path
    double speed = 16.0;    // logical screen pixels per second
    double ratio = 2.5;     // positive screen X / negative screen Y
    double gap = 720.0;     // path distance before first and after last station

    bool IsValid() const
    {
        return count > 0 && std::isfinite(spacing) && spacing > 0.0 &&
            std::isfinite(speed) && speed > 0.0 && std::isfinite(ratio) && ratio > 0.0 &&
            std::isfinite(gap) && gap >= 0.0 && std::isfinite(PathLength()) &&
            PathLength() > 0.0 && std::isfinite(PathLength() / speed) &&
            PathLength() / speed > 0.0;
    }

    Vec2 StepDirection() const
    {
        if (!std::isfinite(ratio) || ratio <= 0.0) return {};
        const double length = std::hypot(ratio, 1.0);
        return {ratio / length, -1.0 / length};
    }

    double PathLength() const
    {
        return count ? 2.0 * gap + double(count - 1) * spacing : 0.0;
    }

    double Duration() const { return IsValid() ? PathLength() / speed : 0.0; }

    // Invalid/empty schedules and negative/nonfinite elapsed time stay at the
    // start. At exactly Duration(), the camera wraps back to the starting gap.
    double Progress(double elapsed) const
    {
        const double duration = Duration();
        if (duration == 0.0 || !std::isfinite(elapsed) || elapsed <= 0.0) return 0.0;
        return std::fmod(elapsed, duration) / duration;
    }

    std::uint64_t LoopCount(double elapsed) const
    {
        const double duration = Duration();
        if (duration == 0.0 || !std::isfinite(elapsed) || elapsed <= 0.0) return 0;
        const double loops = std::floor(elapsed / duration);
        const auto maximum = std::numeric_limits<std::uint64_t>::max();
        return loops >= double(maximum) ? maximum : std::uint64_t(loops);
    }

    Vec2 SlotScreenOffset(std::size_t index) const
    {
        if (!IsValid()) return {};
        return Along(double(index) * spacing);
    }

    Vec2 CameraScreenOffset(double elapsed) const
    {
        if (!IsValid()) return {};
        return Along(-gap + Progress(elapsed) * PathLength());
    }

    // Both endpoints share a fixed station center. A projectile travels from
    // lower-left source to upper-right target, independently of camera motion.
    Endpoints SlotEndpoints(std::size_t index, double length) const
    {
        if (!IsValid() || !std::isfinite(length) || length < 0.0) return {};
        const double center = double(index) * spacing;
        return {Along(center - length * 0.5), Along(center + length * 0.5)};
    }

    static Vec2 ScreenToWorld(Vec2 screen)
    {
        // Inverse of screen=(worldX-worldY, (worldX+worldY)/2), at fixed Z.
        return {screen.x * 0.5 + screen.y, screen.y - screen.x * 0.5};
    }

private:
    Vec2 Along(double distance) const
    {
        const Vec2 direction = StepDirection();
        return {direction.x * distance, direction.y * distance};
    }
};
