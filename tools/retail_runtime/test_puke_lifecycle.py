"""Actual preview Advance: external owner kill before birth and native own kill."""
import subprocess,tempfile,unittest
from pathlib import Path
from speed_profile_contract import function
ROOT=Path(__file__).resolve().parents[2]
class PukeLifecycleTests(unittest.TestCase):
 def test_actual_advance_guards_birth_rng_and_owner_completion(self):
  source=(ROOT/'src/effects/pukepreview.cpp').read_text();body=function(source,'void Advance(State* s,double seconds)');alive=function(source,'bool Alive(State* s)')
  marker='s->owner->Pulse();if(!Alive(s))return;';self.assertEqual(body.count(marker),1)
  prelude=r'''
#include "pukecore.h"
#include <cassert>
#include <cmath>
constexpr unsigned OF_KILL=4096;
int rng_calls=0;
int random(int a,int){++rng_calls;return a;}
struct Owner{unsigned flags=0;bool kill=false;int frames=0;void NextFrame(){++frames;}void Pulse(){if(kill)flags|=OF_KILL;}unsigned GetFlags(){return flags;}void SetFlags(unsigned f){flags=f;}};
struct State{Owner*owner;retail_puke::State core;double accumulator=0;int ticks=0;};
namespace TTime{constexpr double LegacyFrameSeconds=1./24.;}
'''
  main=r'''
int main(){
 for(int old=0;old<2;++old){rng_calls=0;Owner owner;State s{&owner};auto advance=old?negative::Advance:correct::Advance;
 advance(&s,14*TTime::LegacyFrameSeconds);assert(s.ticks==14 && rng_calls==0 && s.core.life==31);
 owner.kill=true;advance(&s,TTime::LegacyFrameSeconds);assert(owner.frames==15 && !Alive(&s));assert(rng_calls==(old?75:0));assert(s.ticks==(old?15:14));
 advance(&s,3*TTime::LegacyFrameSeconds);assert(owner.frames==15 && rng_calls==(old?75:0));}
 rng_calls=0;Owner owner;State s{&owner};correct::Advance(&s,32*TTime::LegacyFrameSeconds);assert(s.ticks==26 && owner.frames==26 && !s.core.alive && (owner.flags&OF_KILL) && rng_calls==75);
 correct::Advance(&s,3*TTime::LegacyFrameSeconds);assert(s.ticks==26 && rng_calls==75);
}
'''
  with tempfile.TemporaryDirectory()as tmp:
   p=Path(tmp);(p/'test.cpp').write_text(prelude+alive+'\nnamespace correct{'+body+'}\nnamespace negative{'+body.replace(marker,'s->owner->Pulse();')+'}\n'+main)
   subprocess.run(['clang++','-std=c++17','-O1','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),str(p/'test.cpp'),'-o',str(p/'test')],check=True);subprocess.run([str(p/'test')],check=True)
if __name__=='__main__':unittest.main()
