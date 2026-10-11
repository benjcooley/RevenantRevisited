#pragma once
#include "math3d.h"
#include <array>
#include <cstdint>
namespace invisible_core {
struct Particle{int x=0,y=0,z=0,velocity=3,delay=0;float scale=0;};
struct State{
 std::array<float,6> z{{0,80,0,80,0,80}},scale{};std::array<int,6>delay{{0,0,8,8,16,16}};
 std::array<Particle,70>particles{};uint32_t rng=1;int state=1,life=0;float rotation=0;
 int Random(int lo,int hi){rng=rng*214013u+2531011u;return lo+int((rng>>16)&0x7fffu)%(hi-lo+1);}
 void ResetParticle(int i,int velocity){auto&p=particles[i];p.x=Random(-15,15);p.y=Random(-15,15);p.z=0;p.velocity=velocity;p.scale=float(double(Random(2,4))*double(.1f));}
 State(){for(int i=0;i<70;++i){ResetParticle(i,3);particles[i].delay=i/4;}}
 bool Alive()const{return life<500;}
 void Advance(){if(!Alive())return;if(++life>=500)return;rotation+=.2f;
  if(state==1||state==3){for(int i=0;i<6;++i){if(--delay[i]>0)continue;const bool up=(state==1)==(i%2==0);const float middle=i%2?60.f:20.f;const float end=state==1?40.f:(i%2?80.f:0.f);const bool growing=up?z[i]<middle:z[i]>middle;const bool moving=up?z[i]<end:z[i]>end;
    if(growing){z[i]+=up?2.f:-2.f;scale[i]+= .05f;if(scale[i]>.8f)scale[i]=.8f;}
    else if(moving){z[i]+=up?2.f:-2.f;scale[i]-=.05f;if(scale[i]<0)scale[i]=0;}else z[i]=end;
   }
   for(int i=0;i<70;++i){auto&p=particles[i];if(p.delay>0)--p.delay;else{p.z+=p.velocity;if(p.z>50)p.scale-=.05f;}if(state==1&&p.scale<=0)ResetParticle(i,3);}
   if(state==1&&z[5]==40){state=2;for(int i=0;i<6;++i)delay[i]=(i/2)*8;}
  }else if(state==2){if(life>430){state=3;for(int i=0;i<70;++i){ResetParticle(i,2);particles[i].delay=i/4;}}
   for(auto&p:particles)if(p.delay<=0&&p.scale>0){p.z+=p.velocity;if(p.z>50)p.scale-=.05f;}
  }
 }
 static float PositionZ(float z){return float(double(z)*double(1.0379999876022339f)/(double(1.4600000381469727f)-double(z)*double(.0033333334140479565f)*double(.009999999776482582f)));}
 template<class Submit>void Render(Submit&&submit,float ownerX=0,float ownerY=0,float ownerZ=0){if(!Alive())return;
  if(state==1||state==3)for(int i=0;i<6;++i)if(scale[i]>1e-5f){hmm_mat4 m;MtxClear(&m);hmm_vec3 s={scale[i],scale[i],scale[i]},p={ownerX,ownerY,PositionZ(ownerZ+z[i])};MtxScale(&m,&s);MtxRotateZ(&m,rotation);MtxTranslate(&m,&p);submit(0,m);}
  for(auto&p:particles)if(p.delay<=0&&p.scale>1e-5f){hmm_mat4 m;MtxClear(&m);hmm_vec3 s={p.scale,p.scale,p.scale},v={ownerX+p.x,ownerY+p.y,PositionZ(ownerZ+p.z)};MtxScale(&m,&s);MtxTranslate(&m,&v);submit(1,m);}
 }
};
}
