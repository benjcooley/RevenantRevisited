"""Compile actual adapter Advance functions; owner death must stop simulation/RNG."""
import subprocess
import tempfile
import unittest
from pathlib import Path
from speed_profile_contract import function

ROOT = Path(__file__).resolve().parents[2]
NAMES = ('quick', 'restorelife', 'fairy', 'labback', 'dexterity', 'charm', 'antimagic', 'skin')
PRELUDE = r'''
#include <cassert>
#include <cmath>
int rng_calls=0;
int random(int a,int){++rng_calls;return a;}
struct Owner {
 bool alive=true,kill=false; int frames=0;
 void NextFrame(){++frames;}
 void Pulse(){if(kill)alive=false;}
};
struct Particles {
 int advances=0;
 template<class T,class R>void Advance(T&,R rng){++advances;rng(0,1);}
};
struct State {Owner*owner;Particles particles;int inputs=0,ticks=0;double accumulator=0;};
namespace TTime {constexpr double LegacyFrameSeconds=1.0/24.0;}
bool Alive(State*s){return s&&s->owner->alive;}
'''

class AuthoredPreviewLifecycleTests(unittest.TestCase):
 def test_actual_live_kill_and_subsequent_tick_rng(self):
  bodies=[]
  for name in NAMES:
   source=(ROOT/'src/effects'/f'{name}preview.cpp').read_text()
   actual=function(source,'void Advance(State* s,double seconds)')
   marker='s->owner->Pulse();++s->ticks;if(!Alive(s))return;'
   self.assertEqual(actual.count(marker),1,name)
   # Restore only the former post-Pulse scheduling to verify this test catches
   # the bug. The initial already-dead guard remains in the negative control.
   negative=actual.replace(marker,'s->owner->Pulse();++s->ticks;')
   bodies.append(f'namespace {name} {{\n{actual}\n}}\nnamespace old_{name} {{\n{negative}\n}}')
  main=r'''
void Check(void(*advance)(State*,double),bool old){
 rng_calls=0;Owner owner;State s{&owner};
 advance(&s,TTime::LegacyFrameSeconds);
 assert(owner.frames==1 && s.ticks==1 && s.particles.advances==1 && rng_calls==1);
 owner.kill=true;advance(&s,TTime::LegacyFrameSeconds);
 assert(!owner.alive && owner.frames==2 && s.ticks==2);
 assert(s.particles.advances==(old?2:1) && rng_calls==(old?2:1));
 advance(&s,TTime::LegacyFrameSeconds*3);
 assert(owner.frames==2 && s.ticks==2);
 assert(s.particles.advances==(old?2:1) && rng_calls==(old?2:1));
}
int main(){
'''+''.join(f'Check({name}::Advance,false);Check(old_{name}::Advance,true);\n'for name in NAMES)+'}\n'
  with tempfile.TemporaryDirectory()as folder:
   p=Path(folder);(p/'test.cpp').write_text(PRELUDE+'\n'.join(bodies)+main)
   subprocess.run(['clang++','-std=c++17','-O1','-fsanitize=address,undefined',str(p/'test.cpp'),'-o',str(p/'test')],check=True)
   subprocess.run([str(p/'test')],check=True)

if __name__=='__main__':unittest.main()
