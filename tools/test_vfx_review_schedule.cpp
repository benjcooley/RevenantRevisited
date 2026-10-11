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
    assert(VfxReviewSchedule{}.speed == 48.0);
    for (double ratio : {2.0, 2.5, 3.0}) {
        VfxReviewSchedule schedule; schedule.count = 176; schedule.ratio = ratio; schedule.speed = 16;
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
    // Per-station gaps 420, 720 and 1100 retain one constant-speed path.
    for (double ratio : {2.0, 2.5, 3.0}) {
        VfxReviewSchedule mixed; mixed.count = 4; mixed.ratio = ratio; mixed.speed = 16;
        mixed.station_distances = {0.0, 420.0, 1140.0, 2240.0};
        assert(mixed.IsValid()); assert(mixed.PathLength() == 3680.0);
        assert(mixed.Duration() == 230.0);
        assert(mixed.NearestSlot(0.0) == 0);
        assert(mixed.NearestSlot(std::nextafter(mixed.Duration(), 0.0)) == 3);
        for (std::size_t index = 0; index < mixed.count; ++index) {
            const double distance = mixed.SlotDistance(index);
            const double centerTime = (mixed.gap+distance)/mixed.speed;
            const Vec2 center = mixed.SlotScreenOffset(index);
            Equal(center, mixed.CameraScreenOffset(centerTime));
            Equal(Project(VfxReviewSchedule::ScreenToWorld(center)), center);
            assert(mixed.NearestSlot(centerTime) == index);
            const auto ends = mixed.SlotEndpoints(index, 220.0);
            assert(ends.source.x < center.x && ends.source.y > center.y);
            assert(ends.target.x > center.x && ends.target.y < center.y);
            assert(Near(Length(Subtract(ends.target, ends.source)), 220.0));
            Equal(Project(VfxReviewSchedule::ScreenToWorld(ends.target)), ends.target);
            if (index) {
                assert(Near(Length(Subtract(center,mixed.SlotScreenOffset(index-1))),
                            distance-mixed.SlotDistance(index-1)));
                const double midpoint = (distance+mixed.SlotDistance(index-1))*0.5;
                assert(mixed.NearestSlot((mixed.gap+midpoint-0.01)/mixed.speed) == index-1);
                assert(mixed.NearestSlot((mixed.gap+midpoint)/mixed.speed) == index);
                assert(mixed.NearestSlot((mixed.gap+midpoint+0.01)/mixed.speed) == index);
            }
        }
        const Vec2 motion=Subtract(mixed.CameraScreenOffset(11.0),mixed.CameraScreenOffset(10.0));
        assert(Near(Length(motion),mixed.speed)); assert(Near(motion.x/-motion.y,ratio));
        for (unsigned loops : {1u, 2u, 10u}) {
            const double time=loops*mixed.Duration();
            assert(mixed.LoopCount(time)==loops && mixed.Progress(time)==0.0);
            assert(mixed.NearestSlot(time)==0);
            Equal(mixed.CameraScreenOffset(time),mixed.CameraScreenOffset(0.0));
        }
        Equal(mixed.CameraScreenOffset(mixed.Duration()+55.0),mixed.CameraScreenOffset(55.0));
    }
    Vec2 baselinePath{};
    for (double scrollRatio : {2.0, 2.5, 3.0}) {
        VfxReviewSchedule path; path.count=4; path.ratio=scrollRatio;
        path.station_distances={0.0,420.0,1140.0,2240.0};
        const Vec2 center=path.SlotScreenOffset(2);
        const auto ends=path.SlotPathEndpoints(2,200.0);
        const Vec2 direction=Subtract(ends.target,ends.source);
        assert(Near(Length(direction),200.0));
        assert(Near(direction.x/-direction.y,0.25));
        assert(ends.source.x<center.x && ends.source.y>center.y);
        assert(ends.target.x>center.x && ends.target.y<center.y);
        Equal({(ends.source.x+ends.target.x)*0.5,(ends.source.y+ends.target.y)*0.5},center);
        Equal(Project(VfxReviewSchedule::ScreenToWorld(ends.source)),ends.source);
        Equal(Project(VfxReviewSchedule::ScreenToWorld(ends.target)),ends.target);
        if (scrollRatio==2.0) baselinePath=direction;
        else Equal(direction,baselinePath); // scroll ratio cannot rotate the path
        const auto vertical=path.SlotPathEndpoints(2,200.0,0.0);
        Equal(vertical.source,{center.x,center.y+100.0});
        Equal(vertical.target,{center.x,center.y-100.0});
        const auto perpendicular=path.SlotPathEndpoints(2,200.0,-1.0/scrollRatio);
        const Vec2 tangent=Subtract(perpendicular.target,perpendicular.source);
        const Vec2 scroll=path.StepDirection();
        assert(Near(tangent.x*scroll.x+tangent.y*scroll.y,0.0));
        assert(Near(Length(tangent),200.0));
        const auto point=path.SlotPathEndpoints(2,0.0);
        Equal(point.source,center); Equal(point.target,center);
        // Legacy endpoints continue to follow the conveyor, not the new path.
        const auto legacy=path.SlotEndpoints(2,200.0);
        const Vec2 oldDirection=Subtract(legacy.target,legacy.source);
        assert(Near(oldDirection.x/-oldDirection.y,scrollRatio));
        const auto invalid=path.SlotPathEndpoints(2,200.0,std::numeric_limits<double>::quiet_NaN());
        Equal(invalid.source,{}); Equal(invalid.target,{});
    }
    VfxReviewSchedule malformed; malformed.count=4;
    for (const std::vector<double>& distances : {
            std::vector<double>{0,420,1140}, {1,420,1140,2240}, {0,420,420,2240},
            {0,420,300,2240}, {0,420,1140,std::numeric_limits<double>::infinity()},
            {0,420,1140,std::numeric_limits<double>::quiet_NaN()}}) {
        malformed.station_distances=distances;
        assert(!malformed.IsValid()); assert(malformed.NearestSlot(50.0)==0);
        Equal(malformed.CameraScreenOffset(50.0),{});
    }
    VfxReviewSchedule uniform; uniform.count=4;
    assert(uniform.SlotDistance(3)==1260.0);
    assert(uniform.NearestSlot((uniform.gap+210.0)/uniform.speed)==1);
    assert(uniform.NearestSlot((uniform.gap+1250.0)/uniform.speed)==3);
    VfxReviewSchedule empty;
    assert(!empty.IsValid() && empty.Duration() == 0.0 && empty.LoopCount(100.0) == 0);
    Equal(empty.CameraScreenOffset(100.0), {});
    VfxReviewSchedule single; single.count = 1;
    single.station_distances={0.0};
    assert(single.NearestSlot(50.0)==0);
    assert(single.Duration() == 30.0); Equal(single.CameraScreenOffset(15.0), {});
    Equal(single.CameraScreenOffset(-10.0), single.CameraScreenOffset(0.0));
    for (double bad : {0.0, -1.0, std::numeric_limits<double>::infinity(),
                       std::numeric_limits<double>::quiet_NaN()}) {
        auto invalid = single; invalid.speed = bad;
        assert(!invalid.IsValid()); Equal(invalid.CameraScreenOffset(1.0), {});
    }
    std::puts("VFX review schedule: direction, inverse projection, padding, wrap, endpoints and cadence pass");
}
