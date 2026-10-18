#pragma once
#include "engine/core/math3d.h"
#include <array>
#include <cstdint>
#include <cmath>
#include <cassert>
namespace retail_createfood {
struct Point {int32_t x=0,y=0,z=0;};
struct Particle {Point pos{},pivot{},vel{},angvel{};int32_t state=0;float size=0;int32_t angle=0,dist=0,life=0,startfade=0,stopfade=0,color=0;};
static_assert(sizeof(Particle)==80);
// Exact contiguous original DistX/DistY/MMX context, with one declared white
// base light0(multiplier28), ambient32. Unused lights remain unassigned/zero.
// Explicit bounds preserve the original spill without C++ out-of-bounds reads.
struct Lookup {
 std::array<int16_t,1024> words{};
 Lookup(){
  auto round=[](double x){return int(x)+(x-int(x)>=.5);};
  words[0]=0;words[256]=256;
  for(int i=1;i<64;++i){double a=double(i)*6.283185308/256.;words[i]=int16_t(round(256.*std::sin(a)));words[256+i]=i==32?words[i]:int16_t(round(256.*std::cos(a)));}
  for(int i=64;i<128;++i){words[i]=words[256+i-64];words[256+i]=-words[i-64];}
  for(int i=128;i<256;++i){words[i]=-words[i-128];words[256+i]=-words[256+i-128];}
  const double ambient=32./255.;const int base=int(80.*ambient);
  for(int i=0;i<64;++i){const int color=int(80.*(1.-ambient)*(double(i)/63.)*1.4+base);words[512+i*8]=int16_t(color|(color<<8));words[513+i*8]=int16_t(color);}
 }
 Point Vector(int angle,int distance)const{
  const int yi=256+(angle>128?angle-128:128-angle);
  assert(angle>=0&&angle<int(words.size())&&yi>=0&&yi<int(words.size()));
  return {int(words[angle])*distance/256,int(words[yi])*distance/256,0};
 }
};
// Original 44ce30 index service in the declared empty-map fixture: epoch
// 946598400, GetTickCount=0, map-index prefix=0. It reseeds the CRT; this
// is part of the observed failed FOOD insertion, not particle randomness.
template<class Draw,class Seed>uint32_t EmptyMapFoodIndex(Draw draw,Seed seed){
 const uint32_t elapsed=946598400u-0x34d6574cu;
 const uint32_t previous=draw();seed(previous);
 return ((elapsed<<15)|uint32_t(draw()))&0x7fffffffu;
}
struct State {
 std::array<Particle,150> particles{};
 int ticks=0,owner_counter=0;float glow=.01f,main_scale=0;bool alive=true;int food_type=-1;
 template<class Random,class FoodRequest>void Pulse(Random random,const Lookup& lookup,FoodRequest request_food){
  if(!alive)return;++ticks;
  if(ticks<10){if(glow<1.2f)glow+=.2f;else glow=1.2f;}
  else if(ticks<20)glow-=.03f;else if(ticks<30)glow+=.03f;else if(ticks<40)glow-=.03f;else if(ticks<50)glow+=.03f;
  else if(ticks<80){if(glow>0)glow-=.2f;else glow=0;}else glow=0;
  int swirls=ticks>=3&&ticks<13?1:0;
  int smoke=0;if(ticks>=30&&ticks<40)smoke=1;else if(ticks<65&&ticks>=40)smoke=7;else if(ticks>=65&&ticks<70)smoke=2;
  if(smoke)main_scale=.3f;
  for(auto&p:particles){
   if(p.state==0){
    if(swirls){p.color=random(0,1);p.pivot={0,-25,65};p.angvel={0,0,8};p.dist=random(20,28);p.angle=random(0,255);p.size=.1f;p.state=1;--swirls;}
    else if(smoke){int distance=random(0,15),angle=random(0,255);p.pos=lookup.Vector(angle,distance);p.pos.z=1;p.life=5+15*(15-random(0,distance))/15;p.startfade=p.life*20/100;p.stopfade=p.life;p.vel.z=2+random(0,2);p.state=3;--smoke;}
   }else if(p.state==3){p.pos.z+=p.vel.z;if(--p.life==0)p.state=0;}
   else if(p.state==1){p.pivot.z+=2;if(p.size<.1f)p.size+=.02f;p.angle+=p.angvel.z;p.dist+=3;if(p.pivot.z>=95)p.state=2;}
   else if(p.state==2){p.pivot.z-=2;p.angle+=p.angvel.z;p.size-=.02f;if(p.dist>15)p.dist-=4;else p.dist=15;if(p.pivot.z<=10)p.state=0;}
  }
  if(ticks>=80)alive=false;
  ++owner_counter;if(owner_counter==60){food_type=random(0,18);request_food(food_type);}
 }
 template<class Submit>void Render(const Lookup&lookup,Submit submit)const{
  if(!alive)return;
  auto draw=[&](int object,float size,Point position){
   hmm_mat4 m;MtxClear(&m);MtxRotateX(&m,-2.094395160675049f);MtxRotateZ(&m,-.7853981852531433f);MtxRotateZ(&m,0);
   hmm_vec3 scale={size,size,size},pos={float(position.x),float(position.y),float(position.z)};MtxScale(&m,&scale);MtxTranslate(&m,&pos);submit(object,m);
  };
  const Point flame=lookup.Vector(96,500);
  for(const auto&p:particles){
   if(p.state==3&&p.life>0){int object=(p.stopfade-p.life)>=(p.stopfade-p.startfade)/2?0:1;float size=(p.stopfade-p.life)>=p.startfade?float(double(main_scale)*p.life/(p.stopfade-p.startfade)):main_scale;draw(object,size,{p.pos.x+flame.x,p.pos.y+flame.y-25,p.pos.z+280});}
   else if(p.state==1||p.state==2){auto pt=lookup.Vector(p.angle,p.dist);draw(p.color,p.size,{p.pivot.x+pt.x,p.pivot.y+pt.y,p.pivot.z});}
  }
  if(glow>0){draw(0,glow,{20,-5,75});draw(1,glow,{-20,-5,75});}
 }
};
}
