#pragma once
#include <cstdint>

// Original TEffect 4de800 controls, enabled only by a proved public binding.
// Flags are the literal original OF_ANIMATE/INVISIBLE/KILL/PULSE values.
namespace retail_effect_owner {
struct State {
    bool enabled = false;
    int delay = 0, follow_remaining = -1, remaining = -1, blink = 0;
    bool timed = false;
    int light_in = 0, light_out = 0, light_value = 0, light_peak = 0;
    void ResetLight() { light_in=light_out=light_value=light_peak=0; }
    void Configure(int duration, int blink_ticks, int start_delay) {
        enabled=true; follow_remaining=remaining=duration; blink=blink_ticks;
        timed=true; delay=start_delay;
    }
    bool Begin(uint32_t& flags) {
        if (delay<=0) return true;
        if(delay>1) flags&=~0x4000u; else flags|=0x4000u;
        --delay; return false;
    }
    int Light(int intensity, int fadein, int fadeout) {
        if(light_out>0 && fadeout>0) light_value=light_peak*(--light_out)/fadeout;
        else {
            if(light_in<fadein && fadein>0) light_value=intensity*(++light_in)/fadein;
            else light_value=intensity;
            light_peak=light_value;
        }
        return light_value;
    }
    void FollowTick(uint32_t& flags) {
        if(follow_remaining>0)--follow_remaining;
        if(follow_remaining==0)flags|=0x9000u;
    }
    void LifetimeTick(uint32_t& flags) {
        if(!timed)return;
        if(remaining>0)--remaining;
        if(remaining==0){flags|=0x9000u;return;}
        if(remaining>=blink || blink==0)return;
        // Original x87 expression, then _ftol; signed remainder is intentional.
        const int phase=int((1.0-double(blink-remaining)/double(blink))*4.0);
        bool hide=false;
        switch(phase){case 0:hide=remaining%4==0;break;
            case 1:hide=remaining%3==0;break;case 2:hide=remaining%2==0;break;
            case 3:hide=remaining%3!=0;break;default:return;}
        if(hide)flags|=0x80u;else flags&=~0x80u;
    }
};
}
