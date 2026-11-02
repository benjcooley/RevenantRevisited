#pragma once
#include <array>
#include <cmath>
#include <cstdint>
namespace pulp_core {
struct Vec {float x=0,y=0,z=0;};
struct Point {int32_t x=0,y=0,z=0;};
struct Body {Vec velocity,angular_velocity,position,rotation,scale;int32_t done=0,actor_object=0,object=0;};
struct Blood {Vec position,scale,velocity;int32_t used=0;};
static_assert(sizeof(Body)==72 && sizeof(Blood)==40);
struct State {
 std::array<Body,5> bodies{};std::array<Blood,5> blood{};int tick=0;bool valid=false;
 template<class Random,class Rotation,class Ground>void Initialize(Point velocity,Point actor,int objects,Random random,Rotation rotation,Ground ground){
  *this=State{};if(objects<5)return;
  for(int i=0;i<5;++i){bool duplicate;do{duplicate=false;bodies[i].actor_object=random(0,objects-1);for(int j=0;j<i;++j)if(bodies[j].actor_object==bodies[i].actor_object){duplicate=true;break;}}while(duplicate);}
  const float average=float(double(velocity.x+velocity.y+velocity.z)*double(.3333333432674408f));const int overall=int(double(std::abs(velocity.x)+std::abs(velocity.y)+std::abs(velocity.z))*double(.1666666716337204f));
  for(auto& p:bodies){p.velocity={float(velocity.x+random(-overall,overall)),float(velocity.y+random(-overall,overall)),float(velocity.z+random(-overall,overall))};p.angular_velocity={float(random(-30,30)*double(.01745329238474369f)),float(random(-30,30)*double(.01745329238474369f)),float(random(-30,30)*double(.01745329238474369f))};p.position={float(actor.x+random(-15,15)),float(actor.y+random(-15,15)),float(actor.z+random(25,75))};const Point test{int(p.position.x),int(p.position.x),int(p.position.x)};const int height=ground(test);if(test.z<=height)p.position.z=float(height+1);p.rotation=rotation(p.actor_object);p.scale={float(random(75,125)*double(.004000000189989805f)),float(random(75,125)*double(.004000000189989805f)),float(random(75,125)*double(.004000000189989805f))};p.object=random(2,10);}
  // Original Set marks each first-half slot used but writes values to slot0.
  // Other cold fields retain this fixture's declared fresh-zero allocation.
  for(int i=0;i<2;++i){blood[i].used=1;auto& p=blood[0];const float scale=float(random(1,25)*double(.0010000000474974513f));p.scale={scale,scale,scale};p.position=bodies[0].position;
   p.velocity={float((double(velocity.x)+double(average)*2.)*double(random(60,140))*double(.009999999776482582f)*double(.3333333432674408f)),float((double(velocity.y)+double(average)*2.)*double(random(60,140))*double(.009999999776482582f)*double(.3333333432674408f)),float((double(velocity.z)+double(average)*2.)*double(random(60,140))*double(.009999999776482582f)*double(.3333333432674408f))};}
  valid=true;
 }
 static float Angle(float angle,float delta){double a=double(angle)+double(delta);float stored=float(a);while(a>double(6.2831854820251465f)){a=double(stored)-double(6.2831854820251465f);stored=float(a);}while(a<0){a=double(stored)+double(6.2831854820251465f);stored=float(a);}return stored;}
 template<class Random,class Ground,class Decal>void Advance(Point actor,Random random,Ground ground,Decal decal){
  if(!valid)return;++tick;
  for(auto& p:blood)if(p.used){p.position.x+=p.velocity.x;p.position.y+=p.velocity.y;const double z=double(p.position.z)+double(p.velocity.z);p.position.z=float(z);p.velocity.x*=.75f;p.velocity.y*=.75f;if(ground(Point{int(p.position.x),int(p.position.y),int(z)})>=p.position.z)p.used=0;p.velocity.z-=.6000000238418579f;}
  for(int i=0;i<5;++i){auto& p=bodies[i];if(p.done)continue;p.rotation={Angle(p.rotation.x,p.angular_velocity.x),Angle(p.rotation.y,p.angular_velocity.y),Angle(p.rotation.z,p.angular_velocity.z)};Point q{int(p.position.x),int(p.position.y),int(p.position.z)};
   auto damp=[&](bool x){if(x){p.velocity.x*= -.44999998807907104f;p.velocity.y*=.6499999761581421f;}else{p.velocity.y*= -.44999998807907104f;p.velocity.x*=.6499999761581421f;}p.velocity.z*=.6499999761581421f;p.angular_velocity.x*=.6000000238418579f;p.angular_velocity.y*=.6000000238418579f;p.angular_velocity.z*=.6000000238418579f;};
   q.x+=int(p.velocity.x);int height=ground(q);if(q.z<=height || !height)damp(true);q.y+=int(p.velocity.y);height=ground(q);if(q.z<=height || !height)damp(false);q.z+=int(p.velocity.z);height=ground(q);
   if(q.z<=height || !height){int variant;bool settled=std::abs(p.velocity.z)<1.8000000715255737f;if(settled){p.done=1;p.velocity={};p.angular_velocity={};p.position.z=float(height);variant=random(1,3);}else{p.velocity.z*= -.3374999761581421f;p.velocity.x*=.6499999761581421f;p.velocity.y*=.6499999761581421f;p.angular_velocity.x*=.6000000238418579f;p.angular_velocity.y*=.6000000238418579f;p.angular_velocity.z*=.6000000238418579f;p.position.z=float(height);variant=random(1,4);}
    if(p.position.z<float(actor.z+1)){Point point{int(p.position.x)+5,int(p.position.y)-25,0};point.z=ground(point)+1;decal(settled,variant,point);}
   }
   p.scale={1,1,1};p.position.x+=p.velocity.x;p.position.y+=p.velocity.y;p.position.z+=p.velocity.z;if(std::abs(p.velocity.x)<.10000000149011612f)p.velocity.x=0;if(std::abs(p.velocity.y)<.10000000149011612f)p.velocity.y=0;if(!p.done)p.velocity.z-=.8f;
   for(int j=0;j<2;++j){for(auto& drop:blood){if(drop.used)continue;drop.used=1;const float scale=float(random(30,60)*double(.0010000000474974513f));drop.scale={scale,scale,scale};drop.position=p.position;drop.velocity={float(double(p.velocity.x)*random(60,140)*double(.009999999776482582f)),float(double(p.velocity.y)*random(60,140)*double(.009999999776482582f)),float(double(p.velocity.z)*random(60,140)*double(.009999999776482582f))};break;}}
  }
 }
};
}
