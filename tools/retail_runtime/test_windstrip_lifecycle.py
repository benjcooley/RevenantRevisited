"""Compile actual standalone Advance: one-frame owner ordering and death guards."""
import subprocess,tempfile,unittest
from pathlib import Path
from speed_profile_contract import function
ROOT=Path(__file__).resolve().parents[2]
class WindLifecycle(unittest.TestCase):
 def test_actual_advance_order_external_kill_and_native_completion(self):
  source=(ROOT/'src/effects/windstrippreview.cpp').read_text();body=function(source,'void Advance(State* s,double seconds)');alive=function(source,'bool Alive(State* s)');marker='s->owner->Pulse();if(!Alive(s))return;s->owner->NextFrame();';self.assertEqual(body.count(marker),1)
  pre=r'''
#include "windstripcore.h"
#include <cassert>
#include <cmath>
constexpr unsigned OF_KILL=4096;
struct Owner{unsigned flags=0;bool kill=false,done=false;int frames=0,pulses=0;void NextFrame(){++frames;done=true;}void Pulse(){++pulses;if(kill||done)flags|=OF_KILL;}void SetCommandDone(bool v){done=v;}unsigned GetFlags(){return flags;}void SetFlags(unsigned f){flags=f;}};
struct State{Owner*owner;retail_windstrip::State core;double accumulator=0;int ticks=0;};
namespace TTime{constexpr double LegacyFrameSeconds=1./24.;}
'''
  main=r'''
int main(){Owner o;State s{&o};s.core.Initialize([](int a,int){return a;});correct::Advance(&s,20*TTime::LegacyFrameSeconds);assert(Alive(&s)&&s.ticks==20&&!o.done);o.kill=true;correct::Advance(&s,TTime::LegacyFrameSeconds);assert(!Alive(&s)&&s.ticks==20&&o.frames==20&&o.pulses==21);correct::Advance(&s,4*TTime::LegacyFrameSeconds);assert(o.pulses==21);
Owner early;State bad{&early};negative::Advance(&bad,TTime::LegacyFrameSeconds);assert(!Alive(&bad)&&bad.ticks==0&&early.frames==1);
Owner all;State whole{&all};whole.core.Initialize([](int a,int){return a;});correct::Advance(&whole,260*TTime::LegacyFrameSeconds);assert(!Alive(&whole)&&whole.ticks==252&&whole.core.frame==252&&all.frames==252&&all.pulses==252);correct::Advance(&whole,10*TTime::LegacyFrameSeconds);assert(all.pulses==252);}
'''
  with tempfile.TemporaryDirectory()as tmp:
   p=Path(tmp);(p/'test.cpp').write_text(pre+alive+'\nnamespace correct{'+body+'}\nnamespace negative{'+body.replace(marker,'s->owner->NextFrame();s->owner->Pulse();if(!Alive(s))return;')+'}\n'+main);subprocess.run(['clang++','-std=c++17','-O1','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),str(p/'test.cpp'),'-o',str(p/'test')],check=True);subprocess.run([str(p/'test')],check=True)
if __name__=='__main__':unittest.main()
