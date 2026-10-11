#pragma once
#include "math3d.h"
#include <array>
#include <cmath>
#include <cstdint>
namespace blood_core{
struct Slot{hmm_vec3 velocity{},position{};float scale=0;int32_t used=0;uint8_t object=0,stage=0,age=0,delay=0;};static_assert(sizeof(Slot)==36);
struct Params{int height=40,hangle=224,vangle=3,hspread=5,vspread=5,num=1;};
struct State{
 std::array<Slot,30>slots{};uint32_t rng;Params params;int done=0,ticks=0;
 int Random(int a,int b){if(a>b){int t=a;a=b;b=t;}if(a==b)return a;rng=rng*214013u+2531011u;return a+int((rng>>16)&32767u)%(b-a+1);}
 State(uint32_t seed=2,const Params&p={},int detail=4):rng(seed),params(p){int cap=3*detail;if(cap>14)cap=14;int n=cap-Random(0,cap);if(n>0)n=cap-Random(0,cap);else n=0;
  for(int i=0;i<n;++i){auto&a=slots[i];auto&b=slots[n+i];float h=float(double((params.hangle+Random(-params.hspread,params.hspread))&255)*double(360.f)*double(.00390625f)*.017453292522222223);float v=float(double((params.vangle+Random(-params.vspread,params.vspread))&255)*double(360.f)*double(.00390625f)*.017453292522222223);a.used=1;a.object=uint8_t(Random(0,2));a.position.Z=float(params.height);double ch=std::cos(double(h)),sh=std::sin(double(h)),sv=std::sin(double(v));a.velocity.X=float(double(Random(100,300))*2*ch*.01);a.velocity.Y=float(double(Random(100,300))*2*sh*.01);a.velocity.Z=float(double(Random(100,300))*2*sv*.005);a.scale=.5f;
   b.used=1;b.object=uint8_t(Random(0,b.object));b.position.Z=float(params.height);b.velocity.X=float(double(Random(60,80))*double(a.velocity.X)*ch*.014285714285714285);b.velocity.Y=float(double(Random(60,80))*double(a.velocity.Y)*sh*.014285714285714285);b.velocity.Z=float(double(Random(60,80))*double(a.velocity.Z)*sv*.0125);b.delay=uint8_t(int(a.delay)+Random(-1,1)+3);b.scale=.5f;
  }
 }
 void Advance(){if(done)return;++ticks;done=1;for(auto&p:slots){if(!p.used)continue;if(p.delay){--p.delay;continue;}done=0;
   if(p.stage==0){p.position.X+=p.velocity.X;p.position.Y+=p.velocity.Y;p.position.Z+=p.velocity.Z;p.velocity.X*=.95f;p.velocity.Y*=.95f;p.velocity.Z-=.37f;p.scale+=.01f;if(p.position.Z<=20){p.position.Z=20;p.stage=1;p.age=0;if(p.object==0)p.used=0;}}
   else if(p.stage==1){if(!p.age)p.scale=float(double(1.7999999523162842f)-double(Random(0,7))*double(.1f));if(++p.age>30){p.stage=2;p.age=0;}if(p.object==2)p.object=3;}
   else if(p.stage==2){p.scale-=.1f;if(p.scale<=0){p.scale=0;p.used=0;}}
  }
 }
 static uint32_t PackedColor(int ambient,float summedBrightness=0,int lights=0){float value=float(2*ambient)+summedBrightness;value/=float(lights>1?lights:1);const uint32_t v=uint32_t(int(double(value)*double(255.f)));uint32_t c=v|0xffffff00u;c=(c<<8)|v;return(c<<8)|v;}
 template<class Submit>void Render(Submit&&submit)const{if(done)return;for(int i=0;i<30;++i){const auto&p=slots[i];if(!p.used)continue;hmm_mat4 m;MtxClear(&m);hmm_vec3 scale={p.scale,p.scale,p.scale};MtxScale(&m,&scale);if(p.stage==0){MtxRotateX(&m,-1.5707963705062866f);MtxRotateZ(&m,-.7853981852531433f);}hmm_vec3 pos={p.position.X,p.position.Y,p.position.Z-float(params.height)};MtxTranslate(&m,&pos);submit(i,int(p.object)+4,0,m);submit(i,int(p.object),1,m);}}
};
}
