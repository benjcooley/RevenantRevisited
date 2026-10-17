// clang++ -std=c++17 -iquote src tools/test_partsys_zero_position_jitter.cpp src/partsysdefinition.cpp -o /tmp/test_partsys_zero_position_jitter
#include "partsysdefinition.h"
#include <cassert>
#include <string>
int main() {
 using namespace authored_partsys;
 const std::string prefix="obj=(ground01,ground02),particle=#cloud,pps=100,";
 Definition d;std::string error;
 assert(ParseDefinition(prefix+"posjitter=0:0,scljitter=0.2:0.4",d,error));
 assert(d.objects.size()==2 && d.pps.constant[0]==100 && d.scljitter.constant[0]==.2f && d.scljitter.constant[1]==.4f);
 for(const char* value:{"1:1","1e-100:0","0:1","-1:0","[0:(0,0)]","0","0:0,posjitter=0:0","0:0,unknown=0"}) {
  d.particle="unchanged";assert(!ParseDefinition(prefix+"posjitter="+value,d,error));
  assert(d.particle=="unchanged" && !error.empty());
 }
 assert(ParseDefinition(prefix+"scale=2",d,error));assert(d.scale.constant[0]==2);
}
