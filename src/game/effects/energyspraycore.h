#pragma once
#include <array>
#include <cstdint>
namespace retail_energyspray {
struct State {
 int phase=0,countdown=0;
 bool alive=true;
 std::array<float,20> angles_a{},angles_b{},lengths{};
 std::array<std::array<uint8_t,20>,3> colors{};
 std::array<std::array<int,20>,3> directions{};
 std::array<float,25> radii{},radial_angles{},velocities{},scales{};
 std::array<int,25> status{};
 float core_scale=0;
 template<class Random>void Initialize(Random random){
  for(int i=0;i<25;++i)status[i]=-i;
  for(int i=0;i<20;++i){angles_a[i]=float(random(0,127)*0.024736950031496063);angles_b[i]=float(random(0,127)*0.024736950031496063);for(int c=0;c<3;++c)colors[c][i]=uint8_t(random(16,128));for(int c=0;c<3;++c)directions[c][i]=random(0,1);}
 }
 void Pulse(){if(!alive)return;if(countdown==0){++phase;if(phase==1)countdown=60;else if(phase==2)countdown=50;else if(phase==3)countdown=25;if(phase>3)alive=false;}--countdown;}
 template<class Random>void Animate(Random random){
  if(!alive)return;
  if(phase==1)core_scale+=.02500000037252903f;
  else if(phase==2 && core_scale<5.f)core_scale+=.18000000715255737f;
  else if(phase==3)core_scale-=.20000000298023224f;
  for(int i=0;i<25;++i){if(status[i]==0){if(phase<3){radii[i]=float(random(80,120));radial_angles[i]=float(random(0,255))*.024736950173974037f;velocities[i]=radii[i]/float(random(20,30));}scales[i]=0;status[i]=1;}
   else if(status[i]>0){radii[i]-=velocities[i];if(scales[i]<.4000000059604645f)scales[i]+=.019999999552965164f;if(radii[i]<1.f)status[i]=0;}
   else ++status[i];}
  for(int i=0;i<20;++i){angles_a[i]+=.12300000339746475f;angles_b[i]+=.12300000339746475f;if(double(angles_a[i])>3.141592654)angles_a[i]=float(double(angles_a[i])-3.141592654);if(double(angles_b[i])>3.141592654)angles_b[i]=float(double(angles_b[i])-3.141592654);
   if(phase==2)lengths[i]+=.25f;else if(phase==3)lengths[i]-=.5f;
   // Original movsx compares signed bytes. Upward direction can wrap at255.
   for(int c=0;c<3;++c){auto& color=colors[c][i];auto& up=directions[c][i];if(up){if(int(int8_t(color))<128)++color;else up=0;}else{if(int(int8_t(color))>16)--color;else up=1;}}
  }
 }
 template<class Random>float RenderScale(Random random){return alive?float(double(random(-25,25))*.009999999776482582+double(core_scale)):0.f;}
};
}
