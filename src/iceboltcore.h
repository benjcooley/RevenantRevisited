#pragma once
#include "math3d.h"
#include <array>
#include <cstdint>
#include <cmath>

inline constexpr int32_t kIceBoltMaxFrost=50,kIceBoltMaxSnow=50;
struct SIceBoltFrostParticle { hmm_vec3 pos{},vel{};float s=0,t=0; };
struct SIceBoltSnowParticle { float h=0,th=0,r=0,rs=0,sz=0; };

// Original animator Initialize4eb570 and Pulse4eba00; geometry is separate.
namespace icebolt_core {
struct State {
    std::array<SIceBoltFrostParticle,50> frost_{};
    std::array<SIceBoltSnowParticle,50> snow_{};
    float length_=500,spherescale_[2]={.5f,1},cylscale_=.5f;
    float ringout_=0,spiralang_=0,spiralscale_=.1f,cy_=0;
    int32_t angle_=0,frameon_=0,subspell_=3;
    template<class Random> void Initialize(Random random,float length=500,int facing=0) {
        *this=State{};length_=length;angle_=facing;
        for(auto& p:frost_) {
            p.pos={float(random(-10,10)),float(random(-10,10)),float(random(-10,10))};
            p.vel={float(random(-3,3)),float(random(-3,3)),float(random(-3,3))};
            p.s=.5f;p.t=float(random(0,33));
        }
        for(auto& p:snow_) {
            p.h=float(random(-50,50));p.th=float(double(random(0,359))*0.017453292522222223);
            p.rs=float(double(random(5,15))*.01);p.sz=float(double(random(3,30))*.01);p.r=float(random(25,50));
        }
    }
    template<class Random> void Pulse(Random random) {
        ++frameon_;
        for(auto& p:snow_) {
            p.th+=p.rs;if(double(p.th)>6.283185308)p.th-=6.2831854820251465f;
            p.rs+=.01f;
        }
        if(frameon_>20) {cy_=float(double(length_)*double(.05f)+double(cy_));if(cy_>=length_)cy_=length_;}
        if(frameon_>50)for(auto& p:snow_) {p.h+=5;p.r-=1;if(p.r<1)p.r=1;p.rs+=.05f;}
        for(auto& p:frost_) {
            p.t-=1;if(p.t>0)continue;p.t=0;
            p.pos.X+=p.vel.X;p.pos.Y+=p.vel.Y;p.pos.Z+=p.vel.Z;p.s-=.03f;
            if(p.s<0) {
                p.pos={float(random(-10,10)),float(random(-10,10)),float(random(-10,10))};
                p.vel={float(random(-3,3)),float(random(-3,3)),float(random(-3,3))};
                p.s=.5f;p.t=float(random(0,16));if(frameon_>30)p.t=100;
            }
        }
        auto spin=[this] {double value=double(spiralang_)+1;if(value>6.283185308)value-=double(6.2831854820251465f);spiralang_=float(value);};
        if(frameon_<10) {spherescale_[0]+=.5f;spherescale_[1]+=1;cylscale_+=.5f;spiralscale_+=.1f;spin();}
        if(frameon_>=10&&frameon_<20) {cylscale_-=.25f;spherescale_[0]+=.5f;spherescale_[1]+=.5f;spin();}
        if(frameon_>=20&&frameon_<30) {cylscale_-=.25f;spherescale_[0]-=.5f;spherescale_[1]-=.5f;spin();}
        if(frameon_>=30&&frameon_<40) {spherescale_[0]-=.5f;spherescale_[1]-=1;spiralscale_-=.1f;}
        double ring=double(ringout_)+20;if(ring>length_)ring-=length_;ringout_=float(ring);
    }
};
// Original4eb816..4eb9e4: float-stored ten-unit advances, integer world queries.
template<class Stop> float MarchLength(int32_t x,int32_t y,uint8_t facing,Stop stop) {
    const double angle=double(facing)*360.0*0.00390625*double(0.01745329238474369f);
    const float dx=float(std::sin(angle)*10.0),dy=float(std::cos(angle)*-10.0);
    float px=float(x),py=float(y);
    for(int steps=1;steps<=50;++steps) {
        px+=dx;py+=dy;if(stop(int32_t(px),int32_t(py))||steps==50)return float(steps*10);
    }
    return 500;
}
struct Packet { int object;hmm_mat4 local; };
template<class Submit> void Render(const State& state,uint8_t owner_facing,Submit submit) {
    auto emit=[&](int object,hmm_vec3 scale,hmm_vec3 position,float ry,bool card) {
        hmm_mat4 m;MtxClear(&m);MtxScale(&m,&scale);
        if(card) {
            MtxRotateX(&m,-1.5707963705062866f);MtxRotateX(&m,-0.5235987901687622f);
            MtxRotateZ(&m,-0.7853981852531433f);
            MtxRotateZ(&m,float(double(-int(owner_facing)*360/256)*0.017453292522222223));
        } else {
            MtxRotateX(&m,1.5707963705062866f);MtxRotateY(&m,ry);MtxRotateZ(&m,0);
        }
        MtxTranslate(&m,&position);submit(Packet{object,m});
    };
    if(state.subspell_>1)for(int i=0;i<4;++i)
        emit(i,{state.cylscale_*.5f,state.cylscale_*.5f,state.length_/64},{0,0,0},0,false);
    if(state.frameon_<30) {
        const int revolutions=int(state.length_)/100;
        if(state.subspell_>0&&revolutions>0) {
            const float step=state.length_/float(revolutions);
            for(int j=0;j<revolutions;++j)for(int side=0;side<2;++side)
                emit(6,{state.spiralscale_,state.spiralscale_,step/64},{0,-float(j)*step,0},
                     side?float(3.141592654-double(state.spiralang_)):state.spiralang_,false);
        }
        if(state.length_>0)for(int i=0;i<int(double(state.length_)*double(.01f));++i)
            emit(5,{state.cylscale_*.5f,state.cylscale_*.5f,1},
                 {0,float(-(int(state.ringout_+float(100*i))%int(state.length_))),0},state.spiralang_,false);
    }
    if(state.subspell_>0)for(int i=0;i<2;++i) {
        const float size=state.spherescale_[i]*.5f;
        emit(4,{size,size,size},{0,-state.length_*float(i),0},0,true);
    }
    for(const auto& p:state.frost_)if(p.t<=0)
        emit(4,{p.s,p.s,p.s},{p.pos.X,p.pos.Y-state.length_,p.pos.Z},0,true);
    if(state.subspell_>2)for(const auto& p:state.snow_) {
        const float x=float(double(p.r)*std::cos(double(p.th)));
        const float y=float(double(p.r)*std::sin(double(p.th))-(state.frameon_>20?double(state.cy_):0));
        emit(4,{p.sz,p.sz,p.sz},{x,y,p.h},0,true);
    }
}

}
