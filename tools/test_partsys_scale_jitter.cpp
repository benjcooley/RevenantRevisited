// clang++ -std=c++17 -Isrc tools/test_partsys_scale_jitter.cpp src/authoredpartsys.cpp src/partsysdefinition.cpp -o /tmp/test_partsys_scale_jitter
#include "authoredpartsys.h"
#include <cassert>
#include <cmath>
#include <string>
int main() {
 using namespace authored_partsys;
 Definition d;std::string error;
 assert(ParseDefinition("obj=(emitter),particle=dust,pps=24,lifespan=20:20,scale=2,scljitter=-1:1",d,error));
 TickInputs in;in.emitters.resize(1);State state;assert(state.Initialize(d,in,0,error));
 bool scale_call=false;auto rng=[&](int a,int b){if(a==-10 && b==10){scale_call=true;return 6;}return a;};
 state.Advance(in,rng);assert(!scale_call);state.Advance(in,rng);assert(scale_call);
 assert(std::fabs(state.Particles()[0].scale-2.6f)<1e-6f);
 assert(ParseDefinition("obj=(emitter),particle=dust,pps=24,lifespan=20:20,scale=2",d,error));assert(state.Initialize(d,in,0,error));scale_call=false;state.Advance(in,rng);state.Advance(in,rng);assert(!scale_call && state.Particles()[0].scale==2.f);
 assert(ParseDefinition("obj=(emitter),particle=dust,scljitter=[0:(0,0),100:(1,1)]",d,error));assert(!state.Initialize(d,in,0,error));
}
