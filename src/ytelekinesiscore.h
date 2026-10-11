#pragma once
#include <array>
#include <cmath>
#include <cstdint>
namespace ytelekinesis_core {
struct Vec{float x=0,y=0,z=0;};
struct Rock{Vec position,velocity,rotation,rotation_velocity,scale;int dead=0,hit=0,mesh=0,angle=0,life=0;};
struct State{
 std::array<Rock,4> rocks{};int count=0;bool alive=true;
 template<class Random>void Initialize(Random random){
  *this=State{};count=random(2,4);
  for(int i=0;i<count;++i){auto& r=rocks[i];r.angle=random(0,255);float angle=float(double(r.angle)*6.283185308*double(.00390625f));const int radius=random(480,640);
   r.position={float(std::cos(double(angle))*radius),float(std::sin(double(angle))*radius),50};
   angle=float(double(angle)+double(3.1415927410125732f));if(double(angle)>=6.283185308)angle-=6.2831854820251465f;
   const int speed=random(6,12);r.velocity={float(std::cos(double(angle))*speed),float(std::sin(double(angle))*speed),0};
   r.rotation={float(double(random(0,255))*6.283185308*double(.00390625f)),float(double(random(0,255))*6.283185308*double(.00390625f)),float(double(random(0,255))*6.283185308*double(.00390625f))};
   r.rotation_velocity={float(random(50,200))*.0008333333535119891f,float(random(50,200))*.0008333333535119891f,float(random(50,200))*.0008333333535119891f};
   switch(random(1,3)){case 1:r.rotation_velocity.x=0;break;case 2:r.rotation_velocity.y=0;break;case 3:r.rotation_velocity.z=0;break;}
   r.scale={8.5f,8.5f,8.5f};r.life=1000/speed;r.mesh=random(0,3);
  }
 }
 void Pulse(float target_x=0,float target_y=0){
  if(!alive)return;bool done=true;
  for(int i=0;i<count;++i){auto& r=rocks[i];if(r.dead)continue;
   r.position.x+=r.velocity.x;r.position.y+=r.velocity.y;r.position.z+=r.velocity.z;
   r.velocity.x*=1.0099999904632568f;r.velocity.y*=1.0099999904632568f;r.velocity.z*=1.0099999904632568f;
   r.rotation.x+=r.rotation_velocity.x;r.rotation.y+=r.rotation_velocity.y;r.rotation.z+=r.rotation_velocity.z;
   r.rotation_velocity.x*=1.0099999904632568f;r.rotation_velocity.y*=1.0099999904632568f;r.rotation_velocity.z*=1.0099999904632568f;
   if(!r.hit && target_x-30<r.position.x && target_x+30>r.position.x && target_y-30<r.position.y && target_y+30>r.position.y)r.hit=1;
   if(--r.life<=0)r.dead=1;if(!r.dead)done=false;
  }if(done)alive=false;
 }
};
}
