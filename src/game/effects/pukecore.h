#pragma once
#include <array>
namespace retail_puke {
// Executed 4fcd70/4fcda0 null-spell branch. Natural mouth attachment and
// damage queries belong to the original spell caller, outside this fixture.
struct State {
 std::array<std::array<float,3>,25> position{},velocity{};
 std::array<int,25> starts{};
 int life=45;
 float scale=.1f;
 bool alive=true;
 template<class Random> void Advance(Random random) {
  if(!alive)return;
  --life;
  if(life==30){
   for(int i=0;i<25;++i){position[i]={0,0,0};velocity[i]={float(double(random(-5,5))*.10000000149011612),float(double(random(35,45))*-.10000000149011612),float(double(random(20,50))*-.03333333507180214)};starts[i]=i>>4;}
  }else if(life<30){
   for(int i=0;i<25;++i){if(starts[i]>0){--starts[i];continue;}for(int c=0;c<3;++c)position[i][c]+=velocity[i][c];velocity[i][2]-=.5f;velocity[i][1]-=.20000000298023224f;if(position[i][2]<10 && life>10)life=10;}
   if(life<10){scale=float(double(scale)-.009999999776482582);if(scale<0)scale=0;}
   if(life<=0)alive=false;
  }
 }
};
}
