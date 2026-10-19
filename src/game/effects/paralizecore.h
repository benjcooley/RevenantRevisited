#pragma once
#include <array>
#include <cmath>
#include <cstdint>
#include <cstddef>

// Original Physical Paralysis variant: null-invoker route and an explicit
// one-target route whose actor position/radius come from the real caller.
namespace paralize_core {
struct Vec {float x=0,y=0,z=0;};
struct State {
 std::array<Vec,15> direction{},acceleration{};
 std::array<float,15> radius{},speed{},home_z{};
 float max_knot=0,min_radius=0,max_radius=0;
 int32_t phase=0;
 std::array<uint32_t,5> targets{};
 std::array<int32_t,15> current_target{};
 uint8_t hit=0,initialized=0,pad0=0,pad1=0;
 int32_t variant=0;
 std::array<int32_t,15> start{};
 int32_t count=0;
 std::array<Vec,15> position{};
 float uv=0,knot=0,scale=0;
 std::array<std::array<int32_t,3>,5> target_position{};
 int32_t num_targets=0;
 bool alive=true;
 template<class Random>void Initialize(Random random,Vec origin={}) {
  // Actual 4fa540 recognizes this shipped name as variant1, count240.
  variant=1;count=240;initialized=1;max_knot=20;min_radius=75;max_radius=76;
  for(int i=0;i<15;++i){
   position[i]={origin.x+float(random(-15,15))/10.f,origin.y+float(random(-15,15))/10.f,origin.z+60.f+float(random(-5,5))/10.f};
   direction[i]={-2.f*float(random(-55,55))/80.f,-2.f*float(random(-55,55))/80.f,float(random(-5,0))/100.f};
   radius[i]=75.f+float(random(0,100))/100.f;home_z[i]=1;speed[i]=.24f;start[i]=count-i*2;
  }
  scale=.2f;knot=0;uv=.04f;phase=1;hit=1;
 }
 // Target inputs are obtained from real character Radius/GetPos by the caller.
 // Z is the original integer FIX_Z domain; token identifies that real target.
 template<class Random>void InitializeTarget(Random random,Vec origin,
     const std::array<int32_t,3>& target,int32_t actor_radius,uint32_t token) {
  variant=1;count=240;initialized=1;num_targets=1;targets[0]=token;
  target_position[0]=target;max_knot=float(actor_radius)*.4f;
  min_radius=float(actor_radius)*1.5f;max_radius=min_radius+1.f;
  const float x=origin.x-target[0],y=origin.y-target[1];
  const float length=float(std::sqrt(double(x)*x+double(y)*y));
  for(int i=0;i<15;++i){
   position[i]={origin.x+float(random(-15,15))/10.f,origin.y+float(random(-15,15))/10.f,origin.z+60.f+float(random(-5,5))/10.f};
   direction[i]={-2.f*(x+float(random(-55,55)))/length,-2.f*(y+float(random(-55,55)))/length,float(random(-5,0))/100.f};
   radius[i]=min_radius+float(random(0,100))/100.f;home_z[i]=1;speed[i]=.24f;start[i]=count-i*2;
  }
  scale=.2f;knot=0;uv=.04f;phase=1;hit=1;
 }
 template<class Random>void TickTarget(Random random,Vec origin,
     const std::array<int32_t,3>& target,int32_t actor_radius,uint32_t token) {
  if(!alive)return;
  if(!initialized)InitializeTarget(random,origin,target,actor_radius,token);
  target_position[0]={target[0]+20,target[1]+20,target[2]};
  if(phase==1){scale+=.2f;if(scale>=2.f)phase=2;return;}
  if(phase==3){scale-=.2f;knot-=.6f;if(scale<=0)alive=false;}
  if(phase!=2 && phase!=3)return;
  const auto& t=target_position[0];
  float x=position[0].x-t[0],y=position[0].y-t[1];
  float length=float(std::sqrt(double(x)*x+double(y)*y));
  float ratio=radius[0]/length;
  float angle=float(std::atan2(double(y*ratio),double(x*ratio))+speed[0]);
  bool grow=false;
  for(int i=0;i<15;++i){
   x=position[i].x-t[0];y=position[i].y-t[1];
   length=float(std::sqrt(double(x)*x+double(y)*y));ratio=radius[i]/length;
   const float cx=x*ratio,cy=y*ratio;
   const float next_angle=float(std::atan2(double(cy),double(cx))+speed[i]);
   if(ratio>.75f){grow=true;angle-=float(6.28/15.0);if(next_angle>angle)angle=float(double(next_angle)+1.57/15.0);}else angle=next_angle;
   const float nx=float(double(radius[i])*std::cos(double(angle))-cx);
   const float ny=float(double(radius[i])*std::sin(double(angle))-cy);
   acceleration[i].x=(x+float(random(-25,25)))*1.2f/length;
   acceleration[i].y=(y+float(random(-25,25)))*1.2f/length;
   acceleration[i].z=(position[i].z-(float(t[2])+home_z[i])+float(random(-5,5)))*1.2f/length;
   direction[i].x=ratio*nx+(1.f-ratio)*(direction[i].x-acceleration[i].x);
   direction[i].y=ratio*ny+(1.f-ratio)*(direction[i].y-acceleration[i].y);
   direction[i].z=home_z[i]==1.f?-.3f:.3f;
   position[i].x+=direction[i].x;position[i].y+=direction[i].y;position[i].z+=direction[i].z;
   if(position[i].z<t[2]+10)home_z[i]=-1;else if(position[i].z>t[2]+60)home_z[i]=1;
  }
  --count;if(count==0)phase=3;
  if(grow && knot<max_knot && phase!=3)knot+=.6f;
 }
 template<class Random>void Tick(Random random,Vec origin={}) {
  if(!alive)return;
  if(!initialized)Initialize(random,origin);
  if(phase==1){scale+=.2f;if(scale>=2.f)phase=2;return;}
  if(phase==3){scale-=.2f;knot-=.6f;if(scale<=0)alive=false;}
  // Original SHRINK falls through MOVE even on the tick that posts owner kill.
  if(phase!=2 && phase!=3)return;
  float angle=float(std::atan2(double(position[0].y),double(position[0].x))+speed[0]);
  for(int i=0;i<15;++i){
   const float x=position[i].x,y=position[i].y;
   const float length=float(std::sqrt(double(x)*x+double(y)*y));
   const float ratio=radius[i]/length;
   const float cx=x*ratio,cy=y*ratio;
   const float next_angle=float(std::atan2(double(cy),double(cx))+speed[i]);
   if(ratio>.75f){angle-=float(6.28/15.0);if(next_angle>angle)angle=float(double(next_angle)+1.57/15.0);}else angle=next_angle;
   const float nx=float(double(radius[i])*std::cos(double(angle))-cx);
   const float ny=float(double(radius[i])*std::sin(double(angle))-cy);
   acceleration[i].x=(x+float(random(-25,25)))*1.2f/length;
   acceleration[i].y=(y+float(random(-25,25)))*1.2f/length;
   acceleration[i].z=(position[i].z-home_z[i]+float(random(-5,5)))*1.2f/length;
   direction[i].x=ratio*nx+(1.f-ratio)*(direction[i].x-acceleration[i].x);
   direction[i].y=ratio*ny+(1.f-ratio)*(direction[i].y-acceleration[i].y);
   direction[i].z=home_z[i]==1.f?-.3f:.3f;
   position[i].x+=direction[i].x;position[i].y+=direction[i].y;position[i].z+=direction[i].z;
   if(position[i].z<10)home_z[i]=-1;else if(position[i].z>60)home_z[i]=1;
  }
  --count;phase=3; // Original no-target branch, independent of remaining duration.
 }
};
struct Draw {int object;Vec position;float scale,uv;};
template<class Submit>void ForEachDraw(const State& s,float sphere_uv,Submit submit)
{
 if(!s.alive)return;
 if(s.scale>1e-5f)for(const auto& p:s.position)submit(Draw{0,p,s.scale,0});
 if(s.knot>1e-5f)for(int pass=0;pass<3;++pass)for(int i=0;i<s.num_targets;++i){
  const auto& t=s.target_position[i];
  // Executed4fb7c5/4fb85f: unchanged target XY, literal5a4a64=40.
  submit(Draw{1,{float(t[0]),float(t[1]),float(t[2]+40)},s.knot,sphere_uv-float(pass)*s.uv*40.f});
 }
}
static_assert(offsetof(State,phase)==0x3ac-0x184);
static_assert(offsetof(State,variant)==0x404-0x184);
static_assert(offsetof(State,position)==0x448-0x184);
static_assert(offsetof(State,alive)==0x548-0x184);
}
