#pragma once
#include <array>
namespace retail_shriek {
// Original null-spell owner counter and animator4fca70. The native damage
// helper immediately returns for the absent invoker; no actor is substituted.
struct State {
 std::array<float,8> scale{},position{},fade{};
 std::array<int,8> starts{};
 int counter=0;
 bool alive=true;
 template<class Random>void Initialize(Random random){for(int i=0;i<8;++i){scale[i]=position[i]=0;fade[i]=1;starts[i]=random(0,32);}counter=0;alive=true;}
 void Advance(){
  if(!alive)return;++counter;
  for(int i=0;i<8;++i){if(starts[i]>0){--starts[i];continue;}if(scale[i]==0 && counter>=45)continue;
   scale[i]+=.11999999731779099f;position[i]+=3.f;
   if(scale[i]>2.f){scale[i]=position[i]=0;fade[i]=1;}
   else if(scale[i]>1.5199999809265137f)fade[i]-=.25f;
  }
  if(counter>45)alive=false;
 }
};
}
