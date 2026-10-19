#pragma once
#include <array>
namespace retail_smoke {
// Exact named Smoke 4e7c60/4e7d80/4e7e50. Quality<3 kills at Init.
struct Puff {float x=0,y=0,z=0,size=0,vx=0,vy=0,vz=0,vsize=0,rotation=0;int life=0;};
static_assert(sizeof(Puff)==40);
struct State {
 std::array<Puff,50> puffs{};float centerx=0,centery=0,vx=.2f,vy=.2f;int ticks=0;bool alive=true;
 template<class Random>void Reset(int i,Random random){
  auto&p=puffs[i];constexpr double reciprocal=double(1.f/32767.f);
  p.x=float(double(random())*reciprocal*50.-25.+double(centerx));
  p.y=float(double(random())*reciprocal*50.-25.+double(centery));p.z=p.rotation=0;
  p.vx=float(double(random())*reciprocal-.5);p.vy=float(double(random())*reciprocal-.5);p.vz=float(double(random())*reciprocal);
  p.life=int(double(random())*reciprocal*100.);p.size=float(double(random())*reciprocal*double(2.7f)+double(.1f));
 }
 template<class Random>void Initialize(Random random,int quality){*this=State{};for(int i=0;i<50;++i)Reset(i,random);alive=quality>=3;}
 template<class Random>void Advance(Random random){
  if(!alive)return;centerx+=vx;centery+=vy;if(centerx>50||centerx< -50)vx=-vx;if(centery>50||centery< -50)vy=-vy;
  for(int i=0;i<50;++i){auto&p=puffs[i];p.vz-=.01f;p.x+=p.vx;p.y+=p.vy;if(p.z>0)p.z+=p.vz;else p.z-=.008f;++p.life;p.size+=.02f;if(p.life>200){p.size-=.2f;if(p.size<=.01f)Reset(i,random);}}
  ++ticks;
 }
};
}
