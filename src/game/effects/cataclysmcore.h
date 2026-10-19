#pragma once
#include <array>
#include <cstdint>

// Original null-spell Cataclysm owner 4fea00 and animator 4fec80/4fef10.
// Actor selection/damage and named spell audio are deliberately outside this core.
namespace cataclysm_core {
struct Vec { float x=0,y=0,z=0; };
struct Point { int32_t x=0,y=0,z=0; };
struct Pool {
    Vec rotation{}; float boom_scale=0,boom_fade=1;
    std::array<Vec,30> velocity{};
    std::array<Point,30> position{};
    std::array<float,30> zacceleration{},scale{};
    std::array<Vec,70> dust_acceleration{},dust_velocity{};
    std::array<Point,70> dust_position{};
    std::array<float,70> dust_scale{},dust_size{};
    std::array<float,50> glow_scale{};
    std::array<Point,50> glow_position{};
};
static_assert(sizeof(Pool)==0x13f8-0xfc);
struct State {
    Pool pool{};
    int32_t life=0,phase=1; Vec velocity{}; Point position{}; bool alive=true;
    template<class Random> void Initialize(Random random) {
        *this=State{};
        pool.rotation={random(0,10)*.1f,random(0,10)*.1f,random(0,10)*.1f};
        for(int i=0;i<30;++i) {
            auto& v=pool.velocity[i];v={random(5,130)*.1f,random(5,130)*.1f,random(20,70)*.1f};
            pool.zacceleration[i]=-.4f;
            if(random(0,1)==0)v.x=-v.x;
            if(random(0,1)==0)v.y=-v.y;
            pool.scale[i]=random(4,20)*.2f;
        }
        for(int i=0;i<70;++i) {
            auto& v=pool.dust_velocity[i];auto& a=pool.dust_acceleration[i];
            v={random(10,120)*.1f,random(10,120)*.1f,random(5,10)*.1f};
            a={-.5f,-.5f,random(2,5)*.1f};
            if(random(0,1)==0){v.x=-v.x;a.x=-a.x;}
            if(random(0,1)==0){v.y=-v.y;a.y=-a.y;}
            pool.dust_scale[i]=1;pool.dust_size[i]=random(3,6)*.1f;
        }
    }
    template<class Random> void Animate(Random random) {
        if(phase==2) {
            pool.rotation.x+=.2f;pool.rotation.y-=.2f;pool.rotation.z+=.1f;
            int births=0;
            for(int i=0;i<50;++i) {
                auto& scale=pool.glow_scale[i];
                if(scale>0)scale-=.15f;
                else if(births<3) {
                    int offset=random(-40,40),magnitude=offset<0?-offset:offset;
                    scale=magnitude>20?1.5f:magnitude>10?1.8f:2.f;
                    pool.glow_position[i]={position.x+offset,position.y+offset,position.z};++births;
                } else scale=0;
            }
        } else if(phase==3) {
            for(int i=0;i<30;++i) {
                auto& p=pool.position[i];auto& v=pool.velocity[i];
                p.x+=int32_t(v.x);p.y+=int32_t(v.y);p.z+=int32_t(v.z);
                v.z+=pool.zacceleration[i];if(pool.scale[i]>0)pool.scale[i]-=.1f;
            }
            for(int i=0;i<70;++i) {
                auto& p=pool.dust_position[i];auto& v=pool.dust_velocity[i];auto& a=pool.dust_acceleration[i];
                p.z+=int32_t(v.z);
                if(pool.dust_scale[i]>.8f){p.x+=int32_t(v.x);p.y+=int32_t(v.y);v.x+=a.x;v.y+=a.y;v.z+=a.z;}
                pool.dust_scale[i]-=.03f;
            }
            pool.boom_scale+=.5f;pool.boom_fade-=.05f;if(pool.boom_fade<0)pool.boom_fade=0;
        }
    }
    template<class Random> void Pulse(Random random) {
        if(!alive)return;
        if(phase==1) {
            const bool side=random(0,1)!=0;
            position={random(150,200),random(150,200),350};
            if(side)position.x=-position.x;else position.y=-position.y;
            velocity={position.x*.05f,position.y*.05f,17.5f};life=0;phase=2;
        } else if(phase==2) {
            ++life;position.x-=int32_t(velocity.x);position.y-=int32_t(velocity.y);position.z-=int32_t(velocity.z);
            if(life>=20) {
                phase=3;
                for(auto& p:pool.position)p={position.x+random(0,50),position.y+random(0,50),position.z+random(0,50)};
                for(auto& p:pool.dust_position)p=position;
            }
        } else if(phase==3) {if(++life>=80)alive=false;}
        // The original owner calls base Pulse even on the tick that sets OF_KILL.
        Animate(random);
    }
};
}
