#pragma once
#include <array>
#include <cstdint>
namespace ribbon_state {
struct Spark {std::array<float,3> p{},v{};float scale=0;int frame=0;};
struct State {
 std::array<Spark,30> sparks{};std::array<float,3> rotations{{0,2,4}};float scale=.05f,tilt=0,tilt_delta=.001f;int ticks=0;
 template<class Rng>void Reset(Spark& s,Rng&& rng){s.p={float(rng(-35,35)),float(rng(-35,35)),float(rng(0,17))};s.v={float(-double(s.p[0])/24.0/2.0),float(-double(s.p[1])/24.0/2.0),.5f};s.scale=0;s.frame=0;}
 template<class Rng>void Initialize(Rng&& rng){*this=State{};for(auto&s:sparks){Reset(s,rng);s.frame=rng(-30,0);}}
 template<class Rng>void Advance(Rng&& rng){++ticks;for(float&r:rotations)r=float(double(r)+double(.2f));if(scale<=4.5f)scale=float(double(scale)+double(.2225f));for(auto&s:sparks){++s.frame;if(s.frame<=0)continue;if(s.frame>=24)Reset(s,rng);else if(s.frame>12)s.scale-=.25f;else s.scale+=.25f;for(int c=0;c<3;++c)s.p[c]+=s.v[c];s.v[2]+=.5f;}tilt+=tilt_delta;if(tilt>.25f || tilt<-.25f)tilt_delta=-tilt_delta;}
 template<class Rng>float FloorScale(Rng&& rng)const{return float(double(scale)*2.5+double(rng(0,10))/20.0);}
};
}
