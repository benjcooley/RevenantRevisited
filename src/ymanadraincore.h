#pragma once
#include "manadraincore.h"
namespace ymanadrain_core {
// Original508369 deliberately leaves transfers above maximum uncapped.
inline int TransferAmount(int current,int maximum,int victim,int amount){
 if(current+amount<=maximum)amount=maximum-current;
 return amount<victim?amount:victim;
}
struct State:manadrain_core::State {
 int facing=0;
 // Original5087a0 is animator Pulse, invoked by owner base Pulse before
 // 5082c0 advances its phase. Unlike ManaDrain it is not Animate.
 template<class Random,class Distance,class Transfer,class Kill>
 void Advance(manadrain_core::Point source,manadrain_core::Point target,int direction,int minimum,int maximum,Random random,Distance distance,Transfer transfer,Kill kill){
  if(!alive)return;
  if(stage>=0){facing=direction;Animate(source,target,direction,random,distance);}
  Pulse(minimum,maximum,random,transfer,kill);
 }
};
}
