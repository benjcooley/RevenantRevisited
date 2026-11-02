#pragma once
#include <array>
#include <cstdint>
namespace strike_meteor {
struct Point {int32_t x=0,y=0,z=0;};
struct Owner {
 Point position{};
 int frame=0,wait=0,lifetime=93;
 bool animate=true,alive=true,pulse=false;
};
// Exact Meteor Storm control block only: four random strikes within265,
// duration93, wait0..48, no follow/repeatdamage/attachment. The manager has
// executed its five delayed Pulse/Timer pairs plus the sixth creation pair.
struct State {
 std::array<Owner,4> owners{};
 int manager_frame=6,tick=0;
 bool alive=true;
 template<class Random,class IndexRng,class Vector>void Initialize(Point caster,Random random,IndexRng index_rng,Vector vector){
  *this=State{};
  for(auto& o:owners){
   const int angle=random(0,255),distance=random(0,265);
   const auto offset=vector(angle,distance);
   o.position={caster.x+50+offset.x,caster.y+50+offset.y,caster.z+50};
   // Actual474bb0 class factory and451090 successful map insertion each
   // generate an index, with their original CRT reseeding side effect.
   index_rng();index_rng();o.wait=random(0,48);
  }
 }
 void Advance(){
  if(!alive)return;++tick;
  // Whole540d70 runs before540eb0 and the map owner pass. Its original
  // Kill542340 marks all four effects and clears their spell bindings.
  if(++manager_frame>=93){alive=false;for(auto&o:owners){o.alive=false;o.pulse=false;}return;}
  for(auto&o:owners){
   o.pulse=false;if(!o.alive)continue;
   // Original map's animate-object set excludes start-delay hidden owners.
   if(o.animate && o.frame<44)++o.frame;
   if(o.wait>0){o.animate=o.wait==1;--o.wait;continue;}
   o.pulse=true;if(o.lifetime>0)--o.lifetime;
   if(!o.lifetime)o.alive=false;
  }
 }
};
}
