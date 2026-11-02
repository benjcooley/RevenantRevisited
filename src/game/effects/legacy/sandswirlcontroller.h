#pragma once

#include <array>
#include <cstdint>

// Original parent scheduling/target contract (4ee030). This chooses no imagery
// and implements no substitute particle simulation. An integration must supply
// a verified particle child and the real map query/spawn interfaces first.
namespace retail_sandswirl {
struct Point { int32_t x=0, y=0, z=0; };
struct Target { uintptr_t identity=0; Point position; int32_t health=0; };
struct Caller {
    bool valid=false;
    uintptr_t caster=0;
    Point origin;
    std::array<Target,10> found{};
    unsigned count=0;
};
struct TargetInfo {
    bool seeking=true;
    unsigned count=1;
    std::array<std::array<float,3>,5> positions{{{{0,0,100}}}};
    float turn=0.5f, speed=5.0f, autorange=0.0f, hitrange=0.0f;
};
enum class Stop { None, MissingChild, MissingCaller, Completed };
struct Result {
    bool reset_targets=false;
    bool query_targets=false;
    bool kill=false;
    unsigned spawn_count=0;
    std::array<uintptr_t,5> spawn_for{};
};
class Controller {
public:
    // Fail closed rather than reproduce the original null-child dereference.
    bool Start(bool verified_child) {
        frame_=0;
        targets_=TargetInfo{};
        stop_=verified_child ? Stop::None : Stop::MissingChild;
        return verified_child;
    }
    bool Alive() const { return stop_==Stop::None; }
    int Frame() const { return frame_; }
    Stop Reason() const { return stop_; }
    const TargetInfo& Targets() const { return targets_; }
    bool NeedsQueryNextTick() const { return Alive() && frame_==74; }
    Result Advance(const Caller& caller={}) {
        Result out;
        if (!Alive()) return out;
        ++frame_;
        if (frame_<75) {
            targets_.speed=static_cast<float>(5.0+static_cast<double>(frame_)*0.1);
            out.reset_targets=true;
        } else if (frame_==75) {
            out.query_targets=true;
            if (!caller.valid || !caller.caster) {
                stop_=Stop::MissingCaller;
                out.kill=true;
                return out;
            }
            targets_.count=0;
            for (unsigned i=0;i<caller.count && i<caller.found.size() && targets_.count<5;++i) {
                const auto& target=caller.found[i];
                if (!target.identity || target.identity==caller.caster || target.health<=0) continue;
                const unsigned index=targets_.count++;
                out.spawn_for[out.spawn_count++]=target.identity;
                targets_.positions[index]={static_cast<float>(target.position.x-caller.origin.x),
                                          static_cast<float>(target.position.y-caller.origin.y),
                                          static_cast<float>(target.position.z-caller.origin.z)-32.0f};
            }
            targets_.seeking=targets_.count>0;
            targets_.speed=7.5f;
            targets_.hitrange=10.0f;
            out.reset_targets=true;
        } else if (frame_==300) {
            stop_=Stop::Completed;
            out.kill=true;
        }
        return out;
    }
private:
    int frame_=0;
    Stop stop_=Stop::MissingChild;
    TargetInfo targets_;
};
} // namespace retail_sandswirl
