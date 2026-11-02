#include "game/effects/legacy/seekingparticles.h"
#include <cassert>
#include <cstring>
#include <cstdio>
using namespace retail_seeking_particles;
int main() {
    Params p;p.count=1;p.minlife=p.maxlife=3;p.trails=3;p.dir={1,2,3};p.gravity=.5f;
    Random r{7,0};State s;assert(s.Init(p,4,r));assert(r.calls==6);
    const auto before=s.pool;const auto seed=r.seed;unsigned draws=0;
    s.Render([&](int variant,Vec pos){assert(variant==0);if(draws==2)assert(pos.x==2&&pos.y==4&&pos.z==5.5f);++draws;});
    assert(draws==3 && r.seed==seed && std::memcmp(before.data(),s.pool.data(),sizeof(Particle))==0);
    Params target=p;target.target_count=1;assert(!s.ResetTargets(target,r));
    p.target_count=1;p.seeking=1;p.seekz=41;p.speed=5;p.turn=.5f;p.targets[0]={20,10,100};p.killobj=1;
    assert(s.Init(p,4,r));target=p;target.seekz=0;target.targets[0]={-20,10,100};
    const auto resetseed=r.seed;assert(s.ResetTargets(target,r));assert(s.params.seekz==41 && r.seed==resetseed);
    s.Advance();assert(s.pool[0].vel.x!=1);s.Advance();s.Advance();assert(!s.killed);s.Advance();assert(s.killed);
    auto stopped=s.pool;s.Advance();assert(std::memcmp(stopped.data(),s.pool.data(),sizeof(Particle))==0);
    draws=0;s.Render([&](int,Vec){++draws;});assert(draws==0);
    p.count=0;assert(!s.Init(p,4,r));p.count=1;p.target_count=6;assert(!s.Init(p,4,r));
    p.target_count=1;p.bounce=1;p.seeking=0;p.pos={0,0,-2};p.dir={0,0,-2};p.gravity=0;p.trails=8;
    assert(s.Init(p,4,r));draws=0;s.Render([&](int,Vec){++draws;});assert(draws==1);
    s.Advance();assert(s.pool[0].life==0);s.Advance();assert(s.killed);
    // Runtime adapters may use the engine RNG without changing stream order.
    struct EngineStream { Random inner{19,0}; int Range(int a,int b){return inner.Range(a,b);} } stream;
    Random independent{19,0};State direct,adapted;
    p.count=17;p.bounce=0;p.target_count=2;p.targets[1]={-20,10,100};p.objflags=15;p.pspread={2,3,4};p.spread={1,2,3};
    assert(direct.Init(p,4,independent) && adapted.Init(p,4,stream));
    assert(independent.seed==stream.inner.seed && independent.calls==stream.inner.calls);
    assert(std::memcmp(direct.pool.data(),adapted.pool.data(),direct.pool.size()*sizeof(Particle))==0);
    p.target_count=1;assert(direct.ResetTargets(p,independent) && adapted.ResetTargets(p,stream));
    assert(independent.seed==stream.inner.seed && independent.calls==stream.inner.calls);
    for(int tick=0;tick<10;++tick){direct.Advance();adapted.Advance();assert(std::memcmp(direct.pool.data(),adapted.pool.data(),direct.pool.size()*sizeof(Particle))==0);}
    // Restored Sand has one authored object; equal-range variant choice must
    // consume no RNG. Sparks retains its independent four-object selection.
    Params single;single.count=11;single.objflags=15;single.minlife=10;single.maxlife=20;
    single.maxstart=5;single.trails=1;
    Random one{1,0},four{1,0};State sand,sparks;
    assert(sand.Init(single,1,one) && sparks.Init(single,4,four));
    assert(one.calls==88 && four.calls==99 && one.seed!=four.seed);
    for(const auto&q:sand.pool)assert(q.variant==0);
    bool other=false;for(const auto&q:sparks.pool)other|=q.variant!=0;assert(other);
    std::puts("PASS: render immutability/order/bounce, equal-range RNG, target reset/seekz, completion and invalid allocation guards");
}
