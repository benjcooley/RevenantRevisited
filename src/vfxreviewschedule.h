#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

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

    std::vector<double> station_distances; // optional cumulative distances; first is zero

    bool IsValid() const
    {
        if (!station_distances.empty()) {
            if (station_distances.size() != count || station_distances.front() != 0.0)
                return false;
            for (std::size_t i = 0; i < station_distances.size(); ++i)
                if (!std::isfinite(station_distances[i]) ||
                    (i && station_distances[i] <= station_distances[i-1])) return false;
        }
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
        return count ? 2.0 * gap + (station_distances.empty()
            ? double(count - 1) * spacing : station_distances.back()) : 0.0;
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

    double SlotDistance(std::size_t index) const
    {
        return index < station_distances.size() ? station_distances[index] : double(index) * spacing;
    }

    // The closest station to the camera, clamped through both padding gaps.
    // Exact midpoint ties select the upcoming station, matching uniform round.
    std::size_t NearestSlot(double elapsed) const
    {
        if (!IsValid()) return 0;
        // Keep seconds until the final multiplication, avoiding an extra
        // divide/multiply round trip at exactly representable midpoints.
        const double seconds = std::isfinite(elapsed) && elapsed > 0.0
            ? std::fmod(elapsed, Duration()) : 0.0;
        const double distance = -gap + seconds * speed;
        if (distance <= 0.0) return 0;
        if (distance >= SlotDistance(count-1)) return count-1;
        if (station_distances.empty())
            return std::size_t(std::floor(distance / spacing + 0.5));
        const auto next = std::lower_bound(station_distances.begin(), station_distances.end(), distance);
        const std::size_t index = std::size_t(next - station_distances.begin());
        return distance - station_distances[index-1] < *next - distance ? index-1 : index;
    }

    Vec2 SlotScreenOffset(std::size_t index) const
    {
        if (!IsValid()) return {};
        return Along(SlotDistance(index));
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
        const double center = SlotDistance(index);
        return {Along(center - length * 0.5), Along(center + length * 0.5)};
    }

    // A mostly vertical projectile/beam path independent of scroll direction.
    // Positive tilt places the source lower-left and target upper-right.
    Endpoints SlotPathEndpoints(std::size_t index, double length, double tilt = 0.25) const
    {
        if (!IsValid() || !std::isfinite(length) || length < 0.0 || !std::isfinite(tilt))
            return {};
        const Vec2 center = SlotScreenOffset(index);
        const double norm = std::hypot(tilt, 1.0);
        const Vec2 half = {(tilt / norm) * (length * 0.5),
                           (-1.0 / norm) * (length * 0.5)};
        return {{center.x-half.x, center.y-half.y},
                {center.x+half.x, center.y+half.y}};
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
