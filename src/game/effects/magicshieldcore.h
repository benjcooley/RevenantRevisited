#pragma once
#include "engine/core/math3d.h"
#include "engine/core/revtypes.h"
#include <array>
namespace magicshield_core {
struct Owner {int phase=0,countdown=15;bool dead=false;template<class Bonus>void Pulse(bool spell,int minimum,Bonus bonus){if(dead)return;if(!spell)dead=true;if(countdown==0){++phase;if(phase==1||phase==4)countdown=10;else if(phase==2){countdown=100;bonus(float(double(minimum)*double(.009999999776482582f)));}else if(phase==3){countdown=15;bonus(0.f);}if(phase>4)dead=true;}--countdown;}};
struct State {float angle=0;hmm_vec3 scale={.01f,.01f,.01f};S3DPoint position{};std::array<float,20> heights{},angles{};int tier=0;
 template<class Random>void Initialize(Random random){for(int i=0;i<(tier+2)*5;++i){heights[i]=float(random(5,20)*2);angles[i]=float(double(random(0,255))*double(.024736950173974037f));}}
 template<class Random>void Animate(int phase,const S3DPoint&actor,Random random){position=actor;if(phase==0&&scale.Z<1.f)scale.Z+=.06666667014360428f;else if(phase==1&&scale.X<1.f)scale.X=scale.Y=scale.X+.10000000149011612f;else if(phase==3&&scale.X>.004999999888241291f)scale.X=scale.Y=scale.X-.0989999994635582f;else if(phase==4&&scale.Z>.004999999888241291f)scale.Z-=.06566666811704636f;angle+=.14300000667572021f;
  for(int i=0;i<(tier+2)*5;++i){if(heights[i]>=60.f){if(phase==2){heights[i]=float(random(5,20)*2);angles[i]=float(double(random(0,255))*double(.024736950173974037f));}else heights[i]=60.f;}else heights[i]+=1.399999976158142f;}}
 static float Z(double raw){return float(raw/(double(1.4600000381469727f)-raw*double(.0033333334140479565f)*double(.009999999776482582f))*double(1.0379999876022339f));}
 template<class Submit>void Render(int phase,Submit submit)const{hmm_mat4 m;MtxClear(&m);MtxScale(&m,const_cast<hmm_vec3*>(&scale));MtxRotateZ(&m,angle);hmm_vec3 p={float(position.x),float(position.y),Z(double(position.z)+double(10.f)+double(scale.Z)*double(40.f))};MtxTranslate(&m,&p);if(scale.X>0&&scale.Y>0&&scale.Z>0)submit(0,-1,m);
  for(int i=0;i<(tier+2)*5;++i){if(phase<2||heights[i]>60.f)continue;float s=60.f-heights[i];s=s<=20.f?float(double(s)*double(.05000000074505806f)):1.f;hmm_vec3 size={s,s,1};MtxClear(&m);MtxScale(&m,&size);MtxRotateZ(&m,-.5235987901687622f);MtxRotateY(&m,.2617993950843811f);hmm_vec3 offset={0,-15,0};MtxTranslate(&m,&offset);MtxRotateZ(&m,float(double(angles[i])+double(angle)));p={float(position.x),float(position.y),float(double(position.z)+double(heights[i]))};MtxTranslate(&m,&p);submit(1,i,m);}}
};
}
