#pragma once
#include <array>
#include <cstdint>
namespace rockstorm_core {
struct Pool {
 int32_t level=1,stage=0,count=0;uint32_t target=0;
 int32_t target_position[3]={0,0,0};float min_radius=0;
 std::array<float,7> radius{},angular_velocity{},angle{};
 std::array<float,8> destination_height{},velocity{},height{},scale{},maximum_scale{};
};
static_assert(sizeof(Pool)==0x210-0xfc);
struct State {
 Pool pool{};bool alive=true;int owner_stage=0,damage_events=0;
 template<class Random>void Initialize(Random random,int target_radius=24){
  *this=State{};auto& p=pool;p.count=4+random(0,3);p.min_radius=float(target_radius)+5;
  for(int i=0;i<p.count;++i){p.velocity[i]=.25f;p.maximum_scale[i]=float(1.+double(random(-5,10))*double(.1f));p.angle[i]=float(random(0,359));p.height[i]=7;p.destination_height[i]=float(random(45,55));p.radius[i]=float(random(50,60));p.angular_velocity[i]=float(random(6,9))*.0246f;}
 }
 void Pulse(){
  if(!alive)return;auto& p=pool;owner_stage=p.stage;
  switch(p.stage){
  case 0:
   p.stage=1;
   for(int i=0;i<p.count;++i){if(p.scale[i]<p.maximum_scale[i]){p.stage=0;p.scale[i]=float(double(p.maximum_scale[i])*double(.05f)+double(p.scale[i]));}p.height[i]+=p.velocity[i];p.velocity[i]+=.07f;}break;
  case 1:{
   for(int i=0;i<p.count;++i){p.angle[i]+=p.angular_velocity[i];if(p.height[i]<p.destination_height[i]){p.height[i]+=p.velocity[i];p.velocity[i]+=.1f;if(p.height[i]>=p.destination_height[i])p.velocity[i]=0;}else{p.velocity[i]+=.25f;p.angular_velocity[i]+=.001f;p.radius[i]-=p.velocity[i];}}
   bool done=true;for(int i=0;i<p.count;++i)if(p.radius[i]>p.min_radius)done=false;if(done)p.stage=2;break;}
  case 2:p.stage=3;for(int i=0;i<p.count;++i)p.velocity[i]=4;break;
  case 3:{
   for(int i=0;i<p.count;++i){if(p.scale[i]>.001f)p.scale[i]-=.05f;p.height[i]+=p.velocity[i];p.velocity[i]-=.5f;p.radius[i]+=1.4f;}
   bool done=true;for(int i=0;i<p.count;++i)if(p.height[i]>-5)done=false;if(done)p.stage=4;break;}
  case 4:alive=false;break;
  }
  if(owner_stage==2)damage_events+=2; // Observed damage and target-reaction boundary events.
 }
};
}
