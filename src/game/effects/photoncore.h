#pragma once
#include <array>
#include <cstdint>
#include "engine/core/math3d.h"

// Original Photon animator5104c0/510600. The missile owns movement/state;
// this module owns only its 32 local particle slots and launch handshake.
namespace photon_core {
struct State {
    std::array<hmm_vec3,32> position{},velocity{};
    std::array<float,32> scale{};
    std::array<int32_t,32> frame{};
    int active=15,old_state=0;
    bool ready=true;
    template<class Random> void Initialize(Random random) {
        *this=State{};scale[0]=1;
        for(int i=1;i<16;++i) {
            position[i]={float(random(-20,20)),float(random(-20,20)),float(random(-20,20))};
            if(i>5)for(int axis=0;axis<3;++axis)
                velocity[i].Elements[axis]=position[i].Elements[axis]*(-0.0555555559694767f);
            frame[i]=random(-12,0);
        }
        for(int i=16;i<32;++i)frame[i]=-1;
    }
    template<class Random> void Animate(int state,const std::array<int32_t,3>& fixed_velocity,Random random) {
        ready=false;
        if(state==0) {
            ++frame[0];if(frame[0]>12&&scale[0]<5.5f)scale[0]+=.25f;
            for(int i=1;i<16;++i) {
                ++frame[i];if(frame[i]<0)continue;
                for(int axis=0;axis<3;++axis)position[i].Elements[axis]+=velocity[i].Elements[axis];
                if(frame[i]>12) {
                    if(scale[i]!=0) {scale[i]-=.25f;if(scale[i]<=0) {--active;if(active<5)ready=true;scale[i]=0;}}
                } else scale[i]+=.25f;
            }
        } else if(state==1) {
            if(old_state!=1) {
                position[0]={0,0,0};scale[0]=3.5f;frame[0]=0;
                for(int i=16,t=1;i<32;++i,++t) {
                    frame[i]=-t;frame[t]=-1;
                    for(int axis=0;axis<3;++axis)position[i].Elements[axis]=float(-int64_t(fixed_velocity[axis])*t)/65536.0f;
                    scale[i]=3.5f-float(t)*.25f;
                }
            } else for(int i=16;i<32;++i)++frame[i];
        } else if(state==2) {
            if(old_state!=2) {
                for(int i=0;i<16;++i) {
                    position[i]={0,0,0};scale[i]=3;
                    velocity[i]={float(random(-3,3))*1.5f,float(random(-3,3))*1.5f,float(random(-3,3))*1.5f};frame[i]=0;
                }
                active=16;
            } else for(int i=0;i<16;++i) {
                ++frame[i];for(int axis=0;axis<3;++axis)position[i].Elements[axis]+=velocity[i].Elements[axis];
                if(scale[i]>0) {scale[i]-=.125f;if(scale[i]<=0) {scale[i]=0;--active;}}
                const int tail=i+16;
                if(scale[tail]!=0) {++frame[tail];scale[tail]-=.125f;if(scale[tail]<=0)scale[tail]=0;}
            }
        }
        old_state=state;
    }
    bool Finished(int state) const {
        if(state!=2||active!=0)return false;
        for(float value:scale)if(value>0)return false;
        return true;
    }
};
inline std::array<hmm_vec3,4> BuildQuad(const std::array<hmm_vec3,4>& authored,
    float scale,const hmm_vec3& position,const hmm_mat4& owner) {
    hmm_mat4 local{},world{};MtxClear(&local);const hmm_vec3 uniform{scale,scale,scale};
    MtxScale(&local,&uniform);MtxTranslate(&local,&position);MtxMultiply(&world,&local,&owner);
    std::array<hmm_vec3,4> result{};
    for(int i=0;i<4;++i)MtxTransform(&world,&authored[i],&result[i]);
    return result;
}
}
