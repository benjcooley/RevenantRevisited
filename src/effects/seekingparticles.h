#pragma once
#include <array>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdint>

// Original 4e55b0/4e54b0/4e5830 particle contract. This core chooses no
// resource or factory. Sandswirl's missing `sand` is never aliased here.
namespace retail_seeking_particles {
struct Vec { float x=0,y=0,z=0; };
struct Params {
    int32_t count=0; Vec pos,pspread,dir,spread; float gravity=0;
    int32_t trails=0,minstart=0,maxstart=0,minlife=0,maxlife=0,bounce=0,killobj=0;
    uint32_t objflags=0; int32_t seeking=0,seekz=0,target_count=0;
    std::array<Vec,5> targets{}; float turn=0,speed=0,autorange=0,hitrange=0;
};
static_assert(sizeof(Params)==176,"original parameter ABI");
struct Particle { Vec pos,vel; int32_t life=0,start=0,variant=0,target=0; };
struct Random {
    uint32_t seed=1; uint64_t calls=0;
    int Range(int lo,int hi) { if(lo==hi)return lo; seed=seed*214013u+2531011u;++calls;
        return lo+int(((seed>>16)&32767u)%uint32_t(hi-lo+1)); }
};
class State {
public:
    Params params; std::vector<Particle> pool; bool killed=false,done=false;
    bool Init(const Params& p,unsigned objects,Random& rng) {
        if(p.count<=0 || p.count>4096 || p.target_count<0 || p.target_count>5 ||
           objects==0 || objects>32 || p.minlife>p.maxlife || p.minstart>p.maxstart)return false;
        params=p;pool.assign(p.count,{});killed=done=false;
        std::vector<int> variants;for(unsigned i=0;i<objects;++i)if(p.objflags&(1u<<i))variants.push_back(int(i));
        if(variants.empty())variants.push_back(0);
        auto coordinate=[&](float base,float spread){return float(double(base)+double(spread)*(rng.Range(-100,100)*double(0.01f)));};
        for(auto& q:pool) {
            q.pos={coordinate(p.pos.x,p.pspread.x),coordinate(p.pos.y,p.pspread.y),coordinate(p.pos.z,p.pspread.z)};
            q.vel={coordinate(p.dir.x,p.spread.x),coordinate(p.dir.y,p.spread.y),coordinate(p.dir.z,p.spread.z)};
            q.life=rng.Range(p.minlife,p.maxlife);q.start=rng.Range(p.minstart,p.maxstart);
            q.variant=variants[rng.Range(0,int(variants.size())-1)];
            if(p.target_count)q.target=rng.Range(0,p.target_count-1);
        }return true;
    }
    bool ResetTargets(const Params& p,Random& rng) {
        // Original allocates using the OLD target count. Do not admit its
        // null-array write when changing an initially targetless pool to seeking.
        if(p.target_count<0 || p.target_count>5 || (!params.target_count && p.target_count))return false;
        params.seeking=p.seeking;params.speed=p.speed;params.turn=p.turn;
        params.autorange=p.autorange;params.hitrange=p.hitrange;params.target_count=p.target_count;
        for(int i=0;i<p.target_count;++i)params.targets[i]=p.targets[i];
        for(auto& q:pool)if(p.target_count)q.target=rng.Range(0,p.target_count-1);
        return true;
    }
    void Advance() {
        if(killed)return;done=true;
        constexpr double two_pi=6.283185308;constexpr float two_pi_float=6.2831854820251465f;
        auto wrap=[](double v){if(v<0)v+=double(two_pi_float);if(v>two_pi)v-=double(two_pi_float);return v;};
        for(auto& q:pool) {
            if(q.life<=0)continue;done=false;if(q.start>0){--q.start;continue;}
            q.pos.x+=q.vel.x;q.pos.y+=q.vel.y;q.pos.z+=q.vel.z;--q.life;
            if(!params.seeking)q.vel.z-=params.gravity;
            if(params.bounce && q.pos.z<0){q.pos.z=-q.pos.z;q.vel.z=float(-double(q.vel.z)*0.5);if(q.vel.z<2)q.life=0;}
            if(!params.seeking || params.target_count<=0)continue;
            const auto& t=params.targets[q.target];
            auto inside=[&](float r){return q.pos.x>double(t.x)-r && q.pos.x<double(t.x)+r && q.pos.y>double(t.y)-r && q.pos.y<double(t.y)+r;};
            if(inside(params.autorange)) {
                q.pos={float((double(q.pos.x)+t.x)*.5),float((double(q.pos.y)+t.y)*.5),float((double(q.pos.z)+t.z)*.5)};q.vel={};
            }else {
                const double dx=double(t.x)-q.pos.x,dy=double(t.y)-q.pos.y,dz=double(t.z)-q.pos.z;
                const float desired=float(std::atan2(dy,dx)),desiredz=float(std::atan2(dz,std::sqrt(dx*dx+dy*dy)));
                const float current=float(std::atan2(double(q.vel.y),double(q.vel.x)));
                double currentz=std::atan2(double(q.vel.z),std::sqrt(double(q.vel.x)*q.vel.x+double(q.vel.y)*q.vel.y));
                float left=float(double(desired)-current);if(left<0)left=float(double(left)+two_pi_float);if(left>two_pi)left=float(double(left)-two_pi_float);
                const double right=wrap(double(current)-desired);
                const double angle=left<right?double(current)+std::min(params.turn,left):double(current)-std::min(double(params.turn),right);
                if(params.seekz)currentz+=wrap(double(desiredz)-currentz)<wrap(currentz-desiredz)?params.turn:-double(params.turn);
                q.vel.x=float(params.speed*std::cos(angle));const double vy=params.speed*std::sin(angle);q.vel.y=float(vy);
                q.vel.z=float(std::sqrt(double(q.vel.x)*q.vel.x+vy*vy)*std::sin(currentz));
            }
            if(inside(params.hitrange))q.life=0;
        }
        if(done && params.killobj)killed=true;
    }
    // The original Render creates transient trails, never mutating the pool/RNG.
    template<class Submit> void Render(Submit submit) const {
        if(killed)return;
        for(const auto& q:pool)if(q.life>0 && q.start==0){Vec p=q.pos,v=q.vel;
            for(int j=0;j<params.trails;++j){submit(q.variant,p);p.x+=v.x;p.y+=v.y;p.z+=v.z;v.z-=params.gravity;
                if(params.bounce && p.z<0){p.z=-p.z;v.z=float(-double(v.z)*.5);if(v.z<2)break;}}
        }
    }
};
}
