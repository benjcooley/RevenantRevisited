"""Actual standalone Advance stops before simulation after owner death."""
import subprocess,tempfile,unittest
from pathlib import Path
from speed_profile_contract import function
ROOT=Path(__file__).resolve().parents[2]
class EnergyLifecycle(unittest.TestCase):
 def test_actual_advance_external_and_phase_completion(self):
  s=(ROOT/'src/game/effects/legacy/energyspraypreview.cpp').read_text();body=function(s,'void Advance(State* s,double seconds)');alive=function(s,'bool Alive(State* s)');marker='s->owner->Pulse();if(!Alive(s))return;';self.assertEqual(body.count(marker),1)
  pre=r'''
#include "energyspraycore.h"
#include <cassert>
#include <cmath>
constexpr unsigned OF_KILL=4096;int rng_calls=0;int random(int a,int){++rng_calls;return a;}
struct Owner{unsigned flags=0;bool kill=false;int frames=0;void NextFrame(){++frames;}void Pulse(){if(kill)flags|=OF_KILL;}unsigned GetFlags(){return flags;}void SetFlags(unsigned f){flags=f;}};
struct State{Owner*owner;retail_energyspray::State core;double accumulator=0;int ticks=0;};
namespace TTime{constexpr double LegacyFrameSeconds=1./24.;}
'''
  main=r'''
int main(){for(int old=0;old<2;++old){rng_calls=0;Owner owner;State s{&owner};s.core.Initialize([](int a,int b){return random(a,b);});auto step=old?negative::Advance:correct::Advance;step(&s,14*TTime::LegacyFrameSeconds);int before=rng_calls;assert(s.ticks==14 && owner.frames==14);owner.kill=true;step(&s,TTime::LegacyFrameSeconds);assert(!Alive(&s)&&owner.frames==15&&s.ticks==(old?15:14)&&rng_calls==before+(old?3:0));step(&s,3*TTime::LegacyFrameSeconds);assert(owner.frames==15);}
Owner owner;State s{&owner};s.core.Initialize([](int a,int b){return random(a,b);});correct::Advance(&s,145*TTime::LegacyFrameSeconds);assert(s.ticks==136&&owner.frames==136&&!s.core.alive&&s.core.phase==4&&(owner.flags&OF_KILL));int before=rng_calls;correct::Advance(&s,10*TTime::LegacyFrameSeconds);assert(s.ticks==136&&rng_calls==before);}
'''
  with tempfile.TemporaryDirectory()as tmp:
   p=Path(tmp);(p/'test.cpp').write_text(pre+alive+'\nnamespace correct{'+body+'}\nnamespace negative{'+body.replace(marker,'s->owner->Pulse();')+'}\n'+main);subprocess.run(['clang++','-std=c++17','-O1','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),str(p/'test.cpp'),'-o',str(p/'test')],check=True);subprocess.run([str(p/'test')],check=True)
if __name__=='__main__':unittest.main()
