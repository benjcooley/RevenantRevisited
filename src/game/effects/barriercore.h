#pragma once
#include <array>
#include <cstddef>

namespace retail_barrier {
struct Vec3 { float x=0, y=0, z=0; };
struct Flare { Vec3 position, velocity; int type=0, motion=0; };
// Original Setup4e7550 and Animate4e7700. Render rejects the zero-Y
// flare matrices; their state and random consumption still matter.
struct State {
    std::array<Flare,30> flares{};
    Vec3 upper, lower;
    float middle_y=0;
    int delay=0;

    void Initialize(const Vec3* vertices, std::size_t count) {
        *this=State{};
        upper={100000,100000,-100000};
        lower={-100000,-100000,100000};
        for(std::size_t i=0;i<count;++i) {
            const auto& v=vertices[i];
            if(v.x<=upper.x && v.y<=upper.y && v.z>=upper.z) upper=v;
            if(v.x>=lower.x && v.y>=lower.y && v.z<=lower.z) lower=v;
            middle_y+=v.y;
        }
        if(count) middle_y/=float(count);
    }
    template<class Random> void Advance(Random random) {
        for(auto& f:flares) {
            if(!f.type) continue;
            f.position.x+=f.velocity.x;
            f.position.z+=f.velocity.z;
            if((f.motion==0 && f.position.x>lower.x) ||
               (f.motion==1 && f.position.x<upper.x) ||
               (f.motion==2 && f.position.z<lower.z) ||
               (f.motion==3 && f.position.z>upper.z)) f.type=0;
        }
        if(delay) { --delay; return; }
        for(auto& f:flares) {
            if(f.type) continue;
            f.velocity.y=0; f.position.y=middle_y;
            f.motion=random(0,3);
            switch(f.motion) {
                case 0:
                    f.position.x=upper.x;
                    f.position.z=float(random(int(lower.z),int(upper.z)));
                    f.velocity.x=float(random(2,10)); f.velocity.z=0; break;
                case 1:
                    f.position.x=lower.x;
                    f.position.z=float(random(int(lower.z),int(upper.z)));
                    f.velocity.x=float(-random(2,10)); f.velocity.z=0; break;
                case 2:
                    f.position.x=float(random(int(upper.x)*100,int(lower.x)*100)/100);
                    f.position.z=upper.z;
                    f.velocity.x=0; f.velocity.z=float(-random(2,10)); break;
                case 3:
                    f.position.x=float(random(int(upper.x),int(lower.x)));
                    f.position.z=lower.z;
                    f.velocity.x=0; f.velocity.z=float(random(2,10)); break;
            }
            f.type=random(1,2);
            break;
        }
        delay=random(1,5);
    }
};
}
