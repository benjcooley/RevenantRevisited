#pragma once
#include "engine/core/math3d.h"
#include <array>
#include <cstdint>
namespace labyrinth_qualified_core {
struct Star {float radius=0,angle=0,length=0,speed=0;int selector=0;int position[3]{};};
struct State {
 std::array<Star,150>stars{};int ticks=0;
 template<class Random>static float Speed(Random&random){
  // Preserve all branches and random calls in505375..505404, including
  // the four draws reached by every actual1..6 input; no clamp shortcut.
  constexpr float factor=.0010000000474974513f;
  if(double(random(1,6))*factor<1.f){if(double(random(1,6))*factor<=0.f)return 0.f;}
  if(double(random(1,6))*factor<1.f)return float(double(random(1,6))*factor);
  return 1.f;
 }
 template<class Random>void Initialize(Random random){for(auto&s:stars){s.radius=float(random(80,360));s.angle=float(random(0,255));s.speed=Speed(random);s.selector=random(1,3);}}
 template<class Random>void Advance(Random random){++ticks;for(auto&s:stars){s.radius=float(double(s.radius)+double(s.radius)*.0027777778450399637f*6.f);if(s.radius>360.f){s.angle=float(random(0,255));s.speed=Speed(random);s.radius=float(random(55,105));}s.length=float(double(s.radius)*.0027777778450399637f*.25f);s.angle+=s.speed;}}
 template<class Random,class Submit>void Render(Random random,Submit submit){
  for(auto&s:stars){int segment=0;hmm_mat4 last{};int bound=random(3,4);while(segment<bound){hmm_mat4 m;MtxClear(&m);const float side=float(double(s.length)-double(segment)*.0833333358168602f);const hmm_vec3 scale={s.length,side,side};MtxScale(&m,&scale);const hmm_vec3 p={float(double(s.radius)+segment),0,0};MtxTranslate(&m,&p);MtxRotateZ(&m,s.angle);MtxRotateY(&m,64.f);MtxRotateX(&m,-64.f);MtxRotateZ(&m,64.f);submit(s.selector,m);last=m;++segment;bound=random(3,4);}for(int j=0;j<3;++j)s.position[j]=int(last.Elements[3][j]);}
  hmm_mat4 m;MtxClear(&m);const hmm_vec3 scale={30,30,30};MtxScale(&m,&scale);MtxRotateY(&m,64.f);MtxRotateX(&m,-32.f);MtxRotateX(&m,-32.f);submit(0,m);
 }
};
}
