#pragma once
#include <array>
#include <cmath>
namespace retail_windstrip {
struct Point {float x=0,y=0,z=0;};
struct Drop {Point position,velocity;float scale=0;bool used=false;};
struct Strip {float r=0,theta=0,h=0,amplitude=0,dr=0,dtheta=0,dh=0,da=0;std::array<Point,10> points{};};
struct State {
 std::array<Strip,20> strips{};std::array<Drop,400> drops{};
 int frame=0;float halo=0;bool alive=true;
 template<class Random>void Initialize(Random random){for(auto& s:strips){s.r=float(random(0,6));s.theta=float(random(0,6));s.h=float(random(0,6));s.dr=float(random(1,20)/100.0);s.dtheta=float(random(15,20)/100.0);s.dh=float(random(1,20)/100.0);s.da=float(random(5,10)/100.0);}}
 static Point Current(const Strip& s){const double radial=(std::cos(double(s.r))+2.0)*(std::sin(double(s.h))+1.0);return {float(std::cos(double(s.theta))*double(s.amplitude)*radial*5.0),float(std::sin(double(s.theta))*double(s.amplitude)*radial*5.0),float(double(s.amplitude)*std::sin(double(s.h))*15.0)};}
 static void Angles(Strip& s){s.theta+=s.dtheta;s.r+=s.dr;s.h+=s.dh;}
 void AddDrop(Point p){int index=-1;for(int i=0;i<400;++i)if(!drops[i].used)index=i;if(index>=0)drops[index]={p,{0,0,-.5f},.4000000059604645f,true};}
 void Animate(){if(!alive)return;for(auto& s:strips){if(frame==0){for(auto& p:s.points){p=Current(s);Angles(s);s.amplitude+=s.da;if(s.amplitude>5.f)s.amplitude=5.f;}}else{for(int i=0;i<9;++i)s.points[i]=s.points[i+1];s.points[9]=Current(s);Angles(s);AddDrop(s.points[9]);if(frame<100){s.amplitude+=s.da;if(s.amplitude>5.f)s.amplitude=5.f;}else{s.amplitude-=s.da;if(s.amplitude<0)s.amplitude=0;}if(frame<50)halo+=.03999999910593033f;if(frame>150)halo-=.03999999910593033f;}}
  for(auto& d:drops)if(d.used){d.position.x+=d.velocity.x;d.position.y+=d.velocity.y;d.position.z+=d.velocity.z;d.velocity.z-=.20000000298023224f;d.scale-=.004999999888241291f;if(d.scale<=0 || d.position.z<20)d.used=false;}
  if(frame>250)alive=false;++frame;
 }
 template<class Random,class Draw>void RenderDrops(Random random,Draw draw)const{if(!alive)return;for(int i=0;i<400;++i)if(drops[i].used && !random(0,1))draw(i,drops[i]);}
};
}
