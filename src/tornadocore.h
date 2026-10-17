#pragma once
#include <array>
#include <cmath>
#include <cstdint>
namespace tornado_core {
struct Vec {float x=0,y=0,z=0;};
struct Swirl {Vec pos;float scale=0,dscale=0,theta=0,dtheta=0,height_angle=0,dheight_angle=0,radius_angle=0,dradius_angle=0;int32_t count=0;};
struct Ice {Vec pos,vel,angular_velocity,angle,scale;int32_t time=0,stage=0;};
struct Flame {Vec pos,vel;float scale=0,dscale=0;};
static_assert(sizeof(Swirl)==48 && sizeof(Ice)==68 && sizeof(Flame)==32);
struct State {
 std::array<Swirl,100> swirls{};std::array<Ice,30> ice{};std::array<Flame,15> flames{};
 int frame=0,flame_on=0,damage_events=0;bool alive=true;
 static constexpr double pi=3.141592654,tau=6.283185308,radians=.017453292522222223;
 template<class Random>void InitSwirl(int i,Random random){auto& p=swirls[i];p.scale=float(random(3,7)*.01);p.dscale=float(random(2,6)*.001);p.radius_angle=float(random(0,359)*radians);p.dradius_angle=0;p.theta=float(random(0,359)*radians);p.dtheta=float(random(1,3)*.01);p.height_angle=float(random(0,359)*radians);p.dheight_angle=float(random(3,5)*.01);p.count=random(10,20);}
 template<class Random>void InitIce(int i,Random random){auto& p=ice[i];p.pos={float(random(-200,200)),float(random(-200,200)),200};p.vel={float(random(0,200)*.01),float(random(0,200)*.01),float(random(-100,-50)*.04)};p.angle={float(random(0,6)),float(random(0,6)),float(random(0,6))};p.angular_velocity={float(random(-50,50)*.01),float(random(-50,50)*.01),float(random(-50,50)*.01)};p.scale={float(random(50,100)*.01),float(random(50,100)*.01),float(random(50,100)*.01)};p.time=random(0,100);p.stage=0;}
 template<class Random>void InitFlame(Vec pos,Vec vel,Random random){auto& p=flames[flame_on];p.scale=float(random(1,5));p.dscale=float(random(1,5)*.1);p.pos=pos;p.vel=vel;if(++flame_on>=15)flame_on=0;}
 template<class Random>void Initialize(Random random){*this=State{};for(int i=0;i<100;++i)InitSwirl(i,random);for(auto& p:ice)p.time=1;for(int i=0;i<30;i+=6)InitIce(i,random);}
 static void Spin(Ice& p){p.angle.x+=p.angular_velocity.x;if(p.angle.x>tau)p.angle.x-=float(tau);p.angle.y+=p.angular_velocity.y;if(p.angle.y>tau)p.angle.y-=float(tau);p.angle.z+=p.angular_velocity.z;if(p.angle.z>tau)p.angle.z-=float(tau);}
 template<class Random>void Advance(Random random){
  if(!alive)return;bool done=true;
  // 4eeb50 is animator Pulse, before the owner's frame increment.
  for(int i=0;i<100;++i){auto& p=swirls[i];p.pos={float(200.*std::cos(double(p.radius_angle))*std::cos(double(p.theta))),float(200.*std::cos(double(p.radius_angle))*std::sin(double(p.theta))),float(100.*(1.+std::cos(double(p.height_angle))))};p.height_angle+=p.dheight_angle;p.radius_angle+=p.dradius_angle;p.theta+=p.dtheta;if(--p.count<0)p.scale-=p.dscale;else p.scale+=p.dscale;if(p.radius_angle>pi)p.radius_angle=float(pi);if(p.height_angle>pi){if(frame<200)InitSwirl(i,random);else p.height_angle=float(pi);}else done=false;if(p.scale<0){if(frame<200)InitSwirl(i,random);else p.scale=0;}else done=false;if(p.theta>tau)p.theta-=float(tau);}
  for(int i=0;i<30;i+=6){auto& p=ice[i];
   switch(p.stage){
   case 0:
    if(p.time)--p.time;
    else {p.pos.x+=p.vel.x;p.pos.y+=p.vel.y;p.pos.z+=p.vel.z;
     // Original cdecl arguments evaluate right-to-left, including nested RNG.
     double z=double(p.vel.z)*random(1,9)*.1;z+=random(1,4);const float vz=float(z);const float vy=float(double(p.vel.y)*random(1,9)*.1);const float vx=float(double(p.vel.x)*random(1,9)*.1);InitFlame(p.pos,{vx,vy,vz},random);Spin(p);p.vel.z-=.2f;done=false;}
    if(p.pos.z<40){p.time=10;p.stage=1;for(int j=1;j<6;++j){auto& q=ice[i+j];q.pos={p.pos.x,p.pos.y,24};q.vel={float(random(-200,200)*.02),float(random(-200,200)*.02),float(random(0,100)*(1./15.))};q.angle={float(random(0,6)),float(random(0,6)),float(random(0,6))};q.angular_velocity={float(random(1,5)*.01),float(random(1,5)*.01),float(random(1,5)*.01)};q.scale={float(random(15,35)*.01),float(random(15,35)*.01),float(random(15,35)*.01)};q.time=0;}}
    break;
   case 1:
    if(p.time){done=false;if(--p.time>5){const float vz=float(random(4,10));const float vy=float(double(p.vel.y)*random(1,9)*.1);const float vx=float(double(p.vel.x)*random(1,9)*.1);const float z=float(double(p.pos.z)+random(-10,10));const float y=float(double(p.pos.y)+random(-10,10));const float x=float(double(p.pos.x)+random(-10,10));InitFlame({x,y,z},{vx,vy,vz},random);}}
    else {p.stage=2;p.time=1;}break;
   case 2:if(frame<180)InitIce(i,random);else {p.time=1;p.stage=3;}break;
   }
  }
  for(int i=0;i<30;++i){if(i%6==0)continue;auto& p=ice[i];if(!p.time){p.pos.x+=p.vel.x;p.pos.y+=p.vel.y;p.pos.z+=p.vel.z;Spin(p);p.vel.z-=.2f;if(p.pos.z<20)p.time=200;else done=false;}}
  for(auto& p:flames){p.scale-=p.dscale;if(p.scale<=0)continue;p.pos.x+=p.vel.x;p.pos.y+=p.vel.y;p.pos.z=float(double(p.pos.z)+double(p.vel.z)+3.);}
  if(frame>=300 && done)alive=false;
  // Owner4ee460 continues after base Pulse, even on the final kill tick.
  if(frame%100==0)++damage_events;
  ++frame;
 }
};
}
