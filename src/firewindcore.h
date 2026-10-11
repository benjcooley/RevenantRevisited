#pragma once
#include "math3d.h"
#include <array>
#include <cstdint>

// Shipped animator Initialize4e2400/Pulse4e2620/Render4e2c80.
namespace firewind_core {
struct Particle {
    hmm_vec3 pos{},pivot{},vel{},angle{},angvel{};
    int32_t state=0;float scale=0,dist=0;
    int32_t life=0,startfade=0,stopfade=0,color=0,whichring=0;
};
static_assert(sizeof(Particle)==92);
struct State {
    std::array<Particle,400> particles{};
    hmm_vec3 sphere={0,0,30};float facing=0,mainscale=0,gsize=1,rsize=0;
    int frame=0,ring_size=30,ring_new=0;
    template<class Random>void Initialize(Random random,float face=0) {
        *this=State{};facing=face;
        for(int i=0;i<100;++i) {
            auto& p=particles[i];
            p.angle={float(double(random(0,359))*double(.01745329238474369f)),
                     float(double(random(0,359))*double(.01745329238474369f)),
                     float(double(random(0,359))*double(.01745329238474369f))};
            p.angvel={.12f,.12f,.12f};p.state=2;
        }
    }
    template<class Random,class Vector>void Pulse(Random random,Vector vector) {
        ++frame;
        if(frame>=28&&frame<40) {if(gsize<40)gsize+=3;else gsize=40;}
        else if(frame==40) {
            int remaining=200;
            for(auto& p:particles)if(p.state==0||p.state==2) {
                if(remaining>0) {
                    p=Particle{};p.angle.Z=float(double(remaining)*double(6.2831854820251465f)*double(.004999999888241291f));
                    p.angvel.Z=.3f;p.scale=.55f;p.state=3;--remaining;
                } else p.state=0;
            }
            rsize=gsize;
        } else if(frame>40) {
            rsize+=13;if(rsize>=300){for(auto& p:particles)p.state=0;rsize=0;}
        }
        int births=0;
        if(frame<5){births=4;mainscale=.3f;}
        else if(frame<16){births=7;mainscale=.3f;}
        else if(frame<25){births=2;mainscale=.3f;}
        ring_new=0;
        if(frame>=45&&frame<120){ring_size+=4;ring_new=28;mainscale=.5f;}
        for(auto& p:particles) {
            if(p.state==0) {
                if(births) {
                    p=Particle{};int distance=random(0,15),angle=random(0,255);
                    const auto point=vector(angle,distance);p.pos={float(point.X),float(point.Y),1};
                    p.life=20+25*(15-random(0,distance))/15;p.startfade=p.life*20/100;p.stopfade=p.life;
                    p.vel.Z=2+float(random(0,2));p.state=1;--births;
                } else if(ring_new) {
                    p=Particle{};p.angle.Z=float(random(0,255));const auto point=vector(int(p.angle.Z),ring_size);
                    p.pos={float(point.X),float(point.Y),1};p.life=10+15*random(0,10)/10;
                    p.startfade=p.life*20/100;p.stopfade=p.life;p.vel.Z=1+float(random(0,3));p.state=4;--ring_new;
                }
            } else if(p.state==1||p.state==4) {
                if(p.state==4){const auto point=vector(int(p.angle.Z),ring_size);p.pos.X=float(point.X);p.pos.Y=float(point.Y);}
                p.pos.Z+=p.vel.Z;if(--p.life==0)p.state=0;
            } else if(p.state==2||p.state==3) {
                p.pivot=sphere;p.angle.X+=p.angvel.X;p.angle.Y+=p.angvel.Y;p.angle.Z+=p.angvel.Z;
                p.dist=p.state==2?gsize:rsize;if(p.state==2)p.scale=gsize*.01f;
                hmm_mat4 matrix;MtxClear(&matrix);MtxRotateX(&matrix,p.angle.X);MtxRotateY(&matrix,p.angle.Y);MtxRotateZ(&matrix,p.angle.Z);
                hmm_vec3 point={0,p.dist,0};MtxTransform(&matrix,&point,&point);
                p.pos={p.pivot.X+point.X,p.pivot.Y+point.Y,p.pivot.Z+point.Z};
            }
        }
    }
    template<class Submit>void Render(Submit submit) {
        for(auto& p:particles) {
            int object=1;
            if(p.state==1||p.state==4) {
                object=(p.stopfade-p.life)>=(p.stopfade-p.startfade)*70/100?0:1;
                if(p.life>0)p.scale=(p.stopfade-p.life)>=p.startfade
                    ? float(double(mainscale)*p.life/(p.stopfade-p.startfade)+double(.05f)):mainscale;
            } else if(p.state==2) {if(frame<28)continue;}
            else if(p.state!=3)continue;
            if(p.scale<=0)continue;
            hmm_mat4 local;MtxClear(&local);MtxRotateX(&local,-2.094395160675049f);
            MtxRotateZ(&local,-.7853981852531433f);MtxRotateZ(&local,float(-double(facing)*double(.00390625f)*6.283185308));
            hmm_vec3 scale={p.scale,p.scale,p.scale};MtxScale(&local,&scale);MtxTranslate(&local,&p.pos);
            submit(object,local);
        }
    }
};
}
