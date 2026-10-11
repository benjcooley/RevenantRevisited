#pragma once

#include <algorithm>
#include <cmath>

// Logical screen-pixel layout policy. Footprints are projected lengths along
// the conveyor, so separation also bounds their one-dimensional overlap.
struct VfxReviewLayout
{
    double ratio = 2.5;
    double width = 640.0;
    double height = 480.0;
    double requested_minimum = 420.0;

    double ViewSpan() const
    {
        if (!std::isfinite(ratio) || ratio <= 0.0 || !std::isfinite(width) ||
            width <= 0.0 || !std::isfinite(height) || height <= 0.0) return 0.0;
        const double norm = std::hypot(ratio, 1.0);
        return (std::min)(width / (ratio / norm), height / (1.0 / norm));
    }

    double MinimumClearance() const
    {
        return (std::max)({320.0, Nonnegative(requested_minimum), ViewSpan() / 2.25});
    }

    double Clearance(double footprint, double requested = 0.0) const
    {
        footprint = Nonnegative(footprint);
        const double span = ViewSpan();
        const double margin = footprint >= 0.75 * span ? span + 96.0 : 96.0;
        return (std::max)({MinimumClearance(), Nonnegative(requested), footprint + margin});
    }

    double Separation(double leftFootprint, double leftClearance,
                      double rightFootprint, double rightClearance) const
    {
        return (std::max)({Nonnegative(leftClearance), Nonnegative(rightClearance),
            (Nonnegative(leftFootprint) + Nonnegative(rightFootprint)) * 0.5 + 96.0});
    }

private:
    static double Nonnegative(double value)
    {
        return std::isfinite(value) && value > 0.0 ? value : 0.0;
    }
};
