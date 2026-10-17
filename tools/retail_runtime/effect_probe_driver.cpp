// Bounded fixture from RevenantRetailLab/research/partsys/state_driver.cpp.
// Executes the actual production module; no update equations are copied here.
#include "authoredpartsys.h"
#include <cstdio>
#include <cstdlib>
using namespace authored_partsys;
int main(int argc,char**argv) {
    int shape=argc>1?std::atoi(argv[1]):0,face=argc>2?std::atoi(argv[2]):0;
    bool render=argc>3;
    Definition d;d.objects={"emitter"};d.particle="#particle";
    d.pps.constant[0]=24;d.lifespan.constant={10,10,0};
    d.initialvelocity.constant={2,2,0};d.gravity.constant[0]=.5f;
    d.color.constant={106,126,155};d.bounce.constant={-10,-10,0};
    if(render) {
        d.localrotation.constant={1,2,3};d.rlocalrotation.constant={10,20,30};
        d.alpha.constant[0]=.3333f;
    }
    d.spread.constant[0]=30;d.azimuth.constant[0]=30;
    d.emittersize=2;d.emittertype=shape;
    TickInputs input;input.owner_position={10,20,30};input.owner_face=face;
    input.emitters.resize(1);input.ground_height=[](int,int,int){return 0;};
    State state;std::string diagnostic;
    if(!state.Initialize(d,input,3,diagnostic))return 2;
    unsigned rng=1;
    RandomRange random=[&](int a,int b) {
        rng=rng*214013u+2531011u;return a+((rng>>16)&32767u)%(b-a+1);
    };
    for(int tick=0;tick<30;++tick) {
        input.animation_frame=tick;state.Advance(input,random);
        const auto&p=state.Particles()[0];
        auto sample=render?state.SampleRender(0,random):RenderSample{};
        std::printf("%d %u %d %d %.9g %.9g %.9g %.9g %.9g %.9g %.9g",
            tick,rng,p.alive,p.age,p.position[0],p.position[1],p.position[2],
            p.velocity[0],p.velocity[1],p.velocity[2],state.EmissionCredit());
        if(render)std::printf(" %.9g %.9g %.9g %.9g %.9g %.9g %.9g %.9g %.9g %.9g %.9g",
            sample.position[0],sample.position[1],sample.position[2],
            sample.rotation[0],sample.rotation[1],sample.rotation[2],
            sample.color[0],sample.color[1],sample.color[2],sample.alpha,sample.scale);
        std::puts("");
    }
}
