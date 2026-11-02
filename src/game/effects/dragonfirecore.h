#pragma once
#include <array>
#include <cstdint>
namespace retail_dragonfire {
// Executed4eacf0, distinct from FireCone's three-pool controller. The shipped
// binary uses unshifted initial random positions, unlike effect_old.cpp.
struct Vec {float x=0,y=0,z=0;};
struct Particle {Vec pos,scale,rotation,acceleration,velocity,temporary;int32_t flicker=0,life=0,span=0,used=0;};
static_assert(sizeof(Particle)==88,"Original particle carrier");
struct Pool {
 std::array<Particle,300> particles{};
 void Add(Particle p){for(auto& slot:particles)if(!slot.used){p.used=1;p.life=0;slot=p;return;}}
 void Animate(){for(auto& p:particles)if(p.used){if(p.life>=p.span){p.used=0;continue;}++p.life;p.pos.x+=p.velocity.x;p.pos.y+=p.velocity.y;p.pos.z+=p.velocity.z;p.velocity.x*=p.acceleration.x;p.velocity.y*=p.acceleration.y;p.velocity.z*=p.acceleration.z;}}
};
struct State {
 Pool fire,smoke;int frame=0;bool done=false,alive=true;
 template<class Random>void Advance(Random random){
  if(!alive)return;if(frame>=34)done=true;else ++frame;
  smoke.Animate();
  for(const auto& p:fire.particles)if(p.used && p.life>=p.span){Particle next=p;next.velocity.x=next.velocity.y=0;next.acceleration.x=next.acceleration.y=0;next.span=15;smoke.Add(next);}
  fire.Animate();
  for(int i=0;i<18 && !done;++i){Particle p;p.pos={float(random(-4,4)),float(random(-4,4)),float(random(-4,4))};
   const float scale=float(random(5,25))*.01f;p.temporary={scale,scale,scale};
   p.velocity={float(random(-200,200))*.01f,-20.f,float(random(-800,-300))*.01f};
   p.acceleration={float(random(83,87))*.01f,float(random(83,87))*.01f,float(random(20,30))*.01f};p.span=random(10,45);fire.Add(p);
  }
  int count=0;
  for(auto& p:fire.particles)if(p.used){++count;if(double(p.life)/double(p.span)>.5)p.velocity.z+=1.5f;
   const double scale=(125.-double(p.pos.z))*double(.01f);p.scale={float(p.temporary.x*scale),float(p.temporary.y*scale),float(p.temporary.z*scale)};}
  for(auto& p:smoke.particles)if(p.used){++count;p.velocity.z+=3.f;double scale=(125.-double(p.pos.z))*double(.01f);if(scale<0)scale=0;p.scale={float(p.temporary.x*scale),float(p.temporary.y*scale),float(p.temporary.z*scale)};}
  if(!count && done)alive=false;
 }
};
}
