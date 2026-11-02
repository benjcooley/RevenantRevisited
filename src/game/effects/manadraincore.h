#pragma once
#include <array>
#include <cmath>
#include <cstdint>

// Original Mana Drain named variant, genuine caster/fighting-target route.
// The caller supplies actual actor coordinates, facing, distance and mana APIs.
namespace manadrain_core {
struct Point {int32_t x=0,y=0,z=0;};
struct Vec {float x=0,y=0,z=0;};
struct State {
 int32_t stage=-1,ticks=0,amount=0;
 bool alive=true;
 Point caster{},victim{};
 float rotation=0,scale=0;
 std::array<int32_t,15> delay{};
 std::array<Point,15> position{};
 std::array<Vec,15> velocity{};
 std::array<float,15> stream_scale{};
 State(){for(int i=0;i<15;++i)delay[i]=i*3+1;}
 template<class Random,class Transfer,class Kill>void Pulse(int minimum,int maximum,Random random,Transfer transfer,Kill kill){
  if(!alive)return;
  if(ticks==0){
   ++stage;
   if(stage==0){amount=random(minimum,maximum);ticks=20;}
   else if(stage==1){amount=transfer(amount);ticks=50;}
   else if(stage==2)ticks=20;
   else {alive=false;kill();}
  }
  --ticks;
 }
 template<class Random,class Distance>void Animate(Point source,Point target,int facing,Random random,Distance distance){
  if(!alive)return;
  caster=source;victim=target;
  const double angle=double(facing-64)*double(0.024736929684877396f);
  const Point destination={caster.x+int32_t(std::cos(angle)*40.0),caster.y+int32_t(std::sin(angle)*40.0),caster.z+30};
  if(stage<2){if(scale<1.f)scale+=.05f;}else if(scale>0.f)scale-=.05f;
  rotation+=.123f;
  for(int i=0;i<15;++i){
   if(delay[i]==1){
    if(stage!=2){
     position[i].x=int32_t(double(victim.x)+double(random(-200,200))*double(.1f));
     position[i].y=int32_t(double(victim.y)+double(random(-200,200))*double(.1f));
     position[i].z=int32_t(double(victim.z)+30.0+double(random(-200,200))*double(.1f));
     velocity[i]={};delay[i]=0;
    }
    stream_scale[i]=0;
   }else if(delay[i]>1)--delay[i];
   else {
    const int dist=distance(position[i],destination);
    velocity[i].x=float(double(destination.x-position[i].x)/(double(dist/random(7,13))+.001));
    velocity[i].y=float(double(destination.y-position[i].y)/(double(dist/random(7,13))+.001));
    velocity[i].z=float(double(destination.z-position[i].z)/(double(dist/random(7,13))+.001));
    position[i].x+=int32_t(velocity[i].x);position[i].y+=int32_t(velocity[i].y);position[i].z+=int32_t(velocity[i].z);
    if(stage<2){if(stream_scale[i]<.5f)stream_scale[i]+=.01f;}
    else if(stream_scale[i]>0.f)stream_scale[i]-=.025f;
    if(dist<40)delay[i]=1;
   }
  }
 }
 template<class Random>float RenderGlowScale(Random random)const{
  return float(double(scale)+double(random(0,5))*double(.1f));
 }
};
}
