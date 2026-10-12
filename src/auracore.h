#pragma once
#include "burncore.h"
#include "revtypes.h"
namespace aura_core {
using Slot=burn_core::Slot;
struct State {
 std::array<Slot,100> particles{};int frame=0,to_add=0;bool done=false;
 template<class Random,class Bone>void Advance(Random random,Bone bone,const hmm_mat4&root,int bone_limit,const S3DPoint&actor){
  if(done)return;++frame;if(frame<720 && to_add<2)++to_add;else if(frame>=720 && to_add>0)--to_add;
  for(auto&p:particles)if(p.used){if(p.life>=p.span){p.used=0;continue;}++p.life;for(int i=0;i<3;++i){p.position.Elements[i]+=p.velocity.Elements[i];p.velocity.Elements[i]*=p.acceleration.Elements[i];}}
  for(int i=0;i<to_add;++i){const int index=random(0,bone_limit-1);hmm_mat4 local;if(!bone(index,local))continue;hmm_mat4 world;MtxMultiply(&world,&local,const_cast<hmm_mat4*>(&root));hmm_vec3 zero={};Slot p;MtxTransform(&world,&zero,&p.position);
   p.position.X+=float(random(-2,2)-actor.x);p.position.Y+=float(random(-2,2)-actor.y);p.position.Z=float(random(-2,2));
   float scale=float(double(random(25,60))*double(.009999999776482582f));p.scale={scale,scale,scale};p.rotation={-90,0,-45};p.acceleration={1,1,1};p.velocity={0,0,float(double(random(5,30))*double(.10000000149011612f))};p.span=random(15,30);
   for(auto&slot:particles)if(!slot.used){slot=p;slot.used=1;slot.life=0;break;}
  }
  bool alive=false;for(auto&p:particles)if(p.used){alive=true;for(int i=0;i<3;++i)p.scale.Elements[i]*=.9700000286102295f;}if(frame>=720 && !to_add && !alive)done=true;
 }
 template<class Submit>void Render(Submit submit)const{if(done)return;for(int i=0;i<100;++i){const auto&p=particles[i];if(!p.used)continue;hmm_mat4 m;MtxClear(&m);MtxRotateX(&m,float(double(p.rotation.X)*.017453292522222223));MtxRotateY(&m,float(double(p.rotation.Y)*.017453292522222223));MtxRotateZ(&m,float(double(p.rotation.Z)*.017453292522222223));MtxRotateX(&m,-1.5707963705062866f);MtxRotateZ(&m,-.7853981852531433f);MtxRotateZ(&m,0);MtxScale(&m,const_cast<hmm_vec3*>(&p.scale));MtxTranslate(&m,const_cast<hmm_vec3*>(&p.position));submit(i,m);}}
};
}
