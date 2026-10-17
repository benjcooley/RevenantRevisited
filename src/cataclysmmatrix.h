#pragma once
#include "cataclysmcore.h"
#include "retailsoftwaretransform.h"
namespace cataclysm_core {
struct Draw { int object=0,mode=2;float scale=1;Point position{};Vec rotation{};uint32_t color=0xffffffffu; };
template<class Submit> void ForEachDraw(const State& s,Submit submit) {
    if(!s.alive)return;
    const auto& p=s.pool;
    if(s.phase==2) {
        submit(Draw{4,2,20,s.position});
        for(int i=0;i<50;++i)if(p.glow_scale[i]>0)submit(Draw{3,8,p.glow_scale[i],p.glow_position[i]});
        // Clean allocation/null-spell variant0 leaves the falling head in mode8.
        submit(Draw{0,8,10,s.position,p.rotation});
    } else if(s.phase==3) {
        for(int i=0;i<70;++i)if(p.dust_scale[i]>0) {
            const uint32_t alpha=uint32_t(int(double(p.dust_scale[i])*255.0f));
            submit(Draw{2,2,p.dust_size[i],p.dust_position[i],{},(alpha<<24)|0x4c4c4cu});
        }
        const uint32_t alpha=uint32_t(int(double(p.boom_fade)*255.0f));
        submit(Draw{1,2,p.boom_scale,s.position,{},(alpha<<24)|0xb27f19u});
        for(int i=0;i<30;++i)if(p.position[i].z>0)submit(Draw{4,2,p.scale[i]*2,p.position[i]});
        for(int i=0;i<30;++i)if(p.position[i].z>0) {
            // Actual switch falls through all remaining assignments.
            const int k=i%9;const float z=p.rotation.z;
            submit(Draw{0,2,p.scale[i],p.position[i],{k<=6?z:0,k<=7?z:0,z}});
        }
    }
}
inline void Matrix(hmm_mat4& m,const Draw& d,bool raw_local_z=false) {
    MtxClear(&m);const hmm_vec3 scale={d.scale,d.scale,d.scale};MtxScale(&m,&scale);
    MtxRotateX(&m,d.rotation.x);MtxRotateY(&m,d.rotation.y);MtxRotateZ(&m,d.rotation.z);
    const hmm_vec3 p={float(d.position.x),float(d.position.y),raw_local_z?float(d.position.z):RetailSoftwareOwnerZ(d.position.z)};MtxTranslate(&m,&p);
}
}
