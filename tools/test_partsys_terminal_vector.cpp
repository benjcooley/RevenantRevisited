// clang++ -std=c++17 -iquote src tools/test_partsys_terminal_vector.cpp src/partsysdefinition.cpp -o /tmp/test_partsys_terminal_vector
#include "partsysdefinition.h"
#include <cassert>
#include <iostream>
#include <string>
using namespace authored_partsys;
int main(){
 const std::string prefix="obj=(#shine01,#shine02),particle=#magic,";
 const std::string curve="color=[0:(90,90,255),25:(18,18,185),100:(180,180,255";
 const std::string tail=",spread=35,azimuth=35,friction=0.5,scale=[0:5,100:0],scljitter=-0.5:0.5";
 Definition shipped,closed;std::string error;
 assert(ParseDefinition(prefix+curve+"]"+tail,shipped,error));
 assert(ParseDefinition(prefix+curve+")]"+tail,closed,error));
 for(int t=0;t<=100;++t)assert(shipped.color.Evaluate(t)==closed.color.Evaluate(t));
 assert(shipped.color.keys.size()==3 && shipped.spread.constant[0]==35 && shipped.azimuth.constant[0]==35 && shipped.friction.constant[0]==.5f);
 assert(shipped.scale.keys.size()==2 && shipped.scljitter.constant[0]==-.5f && shipped.scljitter.constant[1]==.5f);
 for(const auto* invalid:{"color=[0:(1,2)]","color=[0:(1,2,3,4)]","color=[0:(1,2,3,100:(4,5,6)]","color=[0:(1,2,3","color=[0:(1,2,3},100:(4,5,6)]","color=(1,2,3]"}){
  Definition d;d.particle="untouched";assert(!ParseDefinition(prefix+invalid,d,error));assert(d.particle=="untouched");
 }
 std::cout<<"terminal vector: 101 equivalent colors, subsequent fields and 6 rejection controls PASS\n";
}
