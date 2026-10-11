// clang++ -std=c++17 -O2 -Wall -Wextra -pedantic tools/test_vfx_review_schedule.cpp -o /tmp/test_vfx_review_schedule
#include "../src/vfxreviewschedule.h"
#ifdef NDEBUG
#undef NDEBUG // Keep standalone checks active in Release CMake builds.
#endif
#include <cassert>
#include <cstdio>
#include <initializer_list>

using Vec2 = VfxReviewSchedule::Vec2;
static bool Near(double a, double b, double tolerance = 1e-8)
{
    return std::abs(a - b) <= tolerance;
}
static void Equal(Vec2 a, Vec2 b, double tolerance = 1e-8)
{
    assert(Near(a.x, b.x, tolerance)); assert(Near(a.y, b.y, tolerance));
}
static Vec2 Subtract(Vec2 a, Vec2 b) { return {a.x-b.x, a.y-b.y}; }
static Vec2 Project(Vec2 world) { return {world.x-world.y, (world.x+world.y)*0.5}; }
static double Length(Vec2 value) { return std::hypot(value.x, value.y); }

int main()
{
    for (double ratio : {2.0, 2.5, 3.0}) {
        VfxReviewSchedule schedule; schedule.count = 176; schedule.ratio = ratio;
        assert(schedule.IsValid());
        const Vec2 direction = schedule.StepDirection();
        assert(Near(Length(direction), 1.0));
        assert(direction.x > 0.0 && direction.y < 0.0);
        assert(Near(direction.x / -direction.y, ratio));
        assert(Near(schedule.Duration(), (2*720.0 + 175*420.0)/16.0));
        for (std::size_t index : {std::size_t(0), std::size_t(17), std::size_t(175)}) {
            const Vec2 center = schedule.SlotScreenOffset(index);
            Equal(Project(VfxReviewSchedule::ScreenToWorld(center)), center);
            if (index) assert(Near(Length(Subtract(center, schedule.SlotScreenOffset(index-1))), 420.0));
            const auto endpoints = schedule.SlotEndpoints(index, 240.0);
            assert(endpoints.source.x < center.x && endpoints.source.y > center.y);
            assert(endpoints.target.x > center.x && endpoints.target.y < center.y);
            assert(Near(Length(Subtract(endpoints.target, endpoints.source)), 240.0));
            Equal({(endpoints.source.x+endpoints.target.x)*0.5,
                   (endpoints.source.y+endpoints.target.y)*0.5}, center);
            Equal(Project(VfxReviewSchedule::ScreenToWorld(endpoints.source)), endpoints.source);
            Equal(Project(VfxReviewSchedule::ScreenToWorld(endpoints.target)), endpoints.target);
        }
        // Padding is along-path distance, not separate X/Y padding. It puts
        // first and last centers outside a 640x340 viewport before the wrap.
        const Vec2 start = schedule.CameraScreenOffset(0.0);
        const Vec2 first = Subtract(schedule.SlotScreenOffset(0), start);
        assert(Near(Length(first), 720.0)); assert(first.x > 320.0 && first.y < -170.0);
        const double justBeforeWrap = std::nextafter(schedule.Duration(), 0.0);
        const Vec2 last = Subtract(schedule.SlotScreenOffset(175), schedule.CameraScreenOffset(justBeforeWrap));
        assert(Near(Length(last), 720.0, 1e-7)); assert(last.x < -320.0 && last.y > 170.0);
        assert(schedule.LoopCount(justBeforeWrap) == 0);
        assert(schedule.Progress(justBeforeWrap) > 0.999999999);
        for (unsigned loops : {1u, 2u, 10u}) {
            const double elapsed = loops * schedule.Duration();
            Equal(schedule.CameraScreenOffset(elapsed), start);
            assert(schedule.Progress(elapsed) == 0.0);
            assert(schedule.LoopCount(elapsed) == loops);
        }
        // Analytic scheduling gives identical positions at shared elapsed
        // times regardless of render rate; accumulated dt only adds FP noise.
        for (int fps : {24, 30, 60, 144}) {
            double elapsed = 0.0;
            for (int frame = 1; frame <= fps*12; ++frame) {
                elapsed += 1.0 / fps;
                Equal(schedule.CameraScreenOffset(elapsed),
                      schedule.CameraScreenOffset(double(frame)/fps), 1e-8);
                if (frame % fps == 0)
                    Equal(schedule.CameraScreenOffset(elapsed), schedule.CameraScreenOffset(frame/fps), 1e-8);
            }
        }
        double irregularElapsed = 0.0;
        for (double dt : {0.003, 0.2, 1.7, 0.097, 10.0}) {
            irregularElapsed += dt;
            const Vec2 camera = schedule.CameraScreenOffset(irregularElapsed);
            Equal(camera, {direction.x * (-schedule.gap + irregularElapsed * schedule.speed),
                           direction.y * (-schedule.gap + irregularElapsed * schedule.speed)});
        }
        const Vec2 relative0 = Subtract(schedule.SlotScreenOffset(5), schedule.CameraScreenOffset(20.0));
        const Vec2 relative1 = Subtract(schedule.SlotScreenOffset(5), schedule.CameraScreenOffset(21.0));
        const Vec2 motion = Subtract(relative1, relative0);
        assert(motion.x < 0.0 && motion.y > 0.0);
        assert(Near(Length(motion), 16.0)); assert(Near(-motion.x/motion.y, ratio));
        Equal(schedule.CameraScreenOffset(schedule.Duration()+123.25), schedule.CameraScreenOffset(123.25));
    }
    VfxReviewSchedule empty;
    assert(!empty.IsValid() && empty.Duration() == 0.0 && empty.LoopCount(100.0) == 0);
    Equal(empty.CameraScreenOffset(100.0), {});
    VfxReviewSchedule single; single.count = 1;
    assert(single.Duration() == 90.0); Equal(single.CameraScreenOffset(45.0), {});
    Equal(single.CameraScreenOffset(-10.0), single.CameraScreenOffset(0.0));
    for (double bad : {0.0, -1.0, std::numeric_limits<double>::infinity(),
                       std::numeric_limits<double>::quiet_NaN()}) {
        auto invalid = single; invalid.speed = bad;
        assert(!invalid.IsValid()); Equal(invalid.CameraScreenOffset(1.0), {});
    }
    std::puts("VFX review schedule: direction, inverse projection, padding, wrap, endpoints and cadence pass");
}
