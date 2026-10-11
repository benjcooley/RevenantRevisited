#pragma once
#include <algorithm>
#include <array>
#include <cstdint>

namespace retail_fog {
constexpr int Count=36;
constexpr float Scale=7.475f;
struct Vertex {
    float c=.7f,a=.125f,cv=0,av=0;
    float offset[3]{},velocity[3]{};
};
inline bool Anchored(int i) {
    for (int n : {0,4,6,8,10,3,13,19,25,31,30,32,33,34,35,29,23,17,11,1})
        if (i==n) return true;
    return false;
}
struct State {
    std::array<Vertex,Count> vertices{};
    double accumulator=0;
    int ticks=0;
    template<class Random> void Initialize(Random random) {
        *this=State{};
        for(int i=0;i<Count;++i) {
            auto& v=vertices[i];
            if(Anchored(i)) continue;
            // Original x87 multiplies the stored float constants before the
            // final float store; dividing by a decimal changes the RNG state trace.
            v.c=float(double(random(400,1000))*double(.001f));
            v.a=float(double(random(100,400))*double(.001f));
            for(float& d:v.offset) d=float(double(random(-50,50))*double(.01f));
        }
    }
    template<class Random> void Step(Random random) {
        for(int i=0;i<Count;++i) {
            if(Anchored(i)) continue;
            auto& v=vertices[i];
            v.cv=float(double(v.cv)+double(random(-10,10))*double(.001f));
            v.av=float(double(v.av)+double(random(-10,10))*double(.001f));
            v.c+=v.cv; if(v.cv>1.f || v.cv<.4f) v.cv=0;
            v.c=(std::clamp)(v.c,.4f,1.f);
            v.a+=v.av; if(v.av>.4f || v.av<.1f) v.av=0;
            v.a=(std::clamp)(v.a,.1f,.4f);
            for(float& d:v.velocity) d=float(double(d)+double(random(-10,10))*double(.001f));
            for(int j=0;j<3;++j) {
                v.offset[j]+=v.velocity[j];
                if(v.offset[j]<-.5f || v.offset[j]>.5f) v.velocity[j]=0;
                v.offset[j]=(std::clamp)(v.offset[j],-.5f,.5f);
            }
        }
        ++ticks;
    }
    template<class Random> void Advance(double seconds,Random random) {
        accumulator+=seconds;
        while(accumulator+1e-12>=1.0/24.0) {accumulator-=1.0/24.0;Step(random);}
    }
};
}
