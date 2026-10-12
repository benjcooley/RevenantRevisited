#pragma once
#include "math3d.h"
#include <array>
namespace burn_core {
struct Slot {hmm_vec3 position{},scale{},rotation{},acceleration{},velocity{},temp{};int flicker=0,life=0,span=0,used=0;};static_assert(sizeof(Slot)==88);
struct State {
 std::array<Slot,70> fire{},smoke{};int frame=0,to_add=0;bool done=false;
 static void Add(std::array<Slot,70>&pool,const Slot&input){for(auto&p:pool)if(!p.used){p=input;p.used=1;p.life=0;return;}}
 static void Move(std::array<Slot,70>&pool){for(auto&p:pool)if(p.used){if(p.life>=p.span){p.used=0;continue;}++p.life;for(int i=0;i<3;++i){p.position.Elements[i]+=p.velocity.Elements[i];p.velocity.Elements[i]*=p.acceleration.Elements[i];}}}
 template<class Random,class Bone>void Advance(Random random,Bone bone,const hmm_mat4&root,int bones){
  if(done)return;++frame;if(frame<50 && to_add<8)++to_add;else if(frame>=50 && to_add>0)--to_add;
  for(auto&p:fire)if(p.used && p.life==p.span){p.used=0;p.span=p.span*2/3;for(int i=0;i<3;++i){p.scale.Elements[i]*=1.25f;}Add(smoke,p);}
  Move(fire);Move(smoke);
  for(int i=0;i<to_add;++i){const int index=random(0,bones-1);hmm_mat4 local;if(!bone(index,local))continue;hmm_mat4 world;MtxMultiply(&world,&local,const_cast<hmm_mat4*>(&root));hmm_vec3 zero={};Slot p;MtxTransform(&world,&zero,&p.position);p.position.Z=float(double(p.position.Z)*double(1.4600000381469727f));
   for(int j=0;j<3;++j)p.position.Elements[j]+=float(random(-2,2));float scale=float(double(random(15,40))*double(.009999999776482582));p.scale={scale,scale,scale};p.rotation={-90,0,-45};p.acceleration={1,1,1};p.velocity={0,0,float(double(random(30,75))*double(.10000000149011612f))};p.span=random(5,15);Add(fire,p);
  }
  bool alive=false;for(auto*pool:{&fire,&smoke})for(auto&p:*pool)if(p.used){alive=true;for(int i=0;i<3;++i)p.scale.Elements[i]*=.9700000286102295f;}
  if(frame>=50 && to_add==0 && !alive)done=true;
 }
 template<class Submit>void Render(Submit submit)const{if(done)return;for(int pool=0;pool<2;++pool)for(int slot=0;slot<70;++slot){const auto&p=(pool?smoke:fire)[slot];if(!p.used)continue;hmm_mat4 m;MtxClear(&m);MtxRotateX(&m,float(double(p.rotation.X)*.017453292522222223));MtxRotateY(&m,float(double(p.rotation.Y)*.017453292522222223));MtxRotateZ(&m,float(double(p.rotation.Z)*.017453292522222223));MtxRotateX(&m,-1.5707963705062866f);MtxRotateZ(&m,-.7853981852531433f);MtxRotateZ(&m,0);MtxScale(&m,const_cast<hmm_vec3*>(&p.scale));hmm_vec3 position={p.position.X,p.position.Y,float(double(p.position.Z)/(double(1.4600000381469727f)-double(p.position.Z)*double(.0033333334140479565f)*double(.009999999776482582f))*double(1.0379999876022339f))};MtxTranslate(&m,&position);submit(pool,slot,pool?0:1,m);}}
};
}
