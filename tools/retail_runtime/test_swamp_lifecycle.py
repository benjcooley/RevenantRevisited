"""Swamp's original generic Pulse updates controllers before its death test."""
import subprocess,tempfile,unittest
from pathlib import Path
from speed_profile_contract import function
from test_authored_preview_lifecycle import PRELUDE,ROOT

class SwampLifecycleTests(unittest.TestCase):
 def test_original_final_update_and_no_updates_after_kill(self):
  actual=function((ROOT/'src/game/effects/legacy/swamppreview.cpp').read_text(),'void Advance(State* s,double seconds)')
  marker='s->owner->Pulse();++s->ticks;'
  self.assertEqual(actual.count(marker),1)
  early=actual.replace(marker,marker+'if(!Alive(s))return;')
  driver=PRELUDE+'\nnamespace actual{'+actual+'}\nnamespace early{'+early+r'''}
void Check(void(*advance)(State*,double),bool skipped){
 rng_calls=0;Owner owner;State state{&owner};
 advance(&state,TTime::LegacyFrameSeconds);
 assert(owner.frames==1 && state.particles.advances==1 && rng_calls==1);
 owner.kill=true;advance(&state,TTime::LegacyFrameSeconds);
 assert(!owner.alive && owner.frames==2 && state.ticks==2);
 assert(state.particles.advances==(skipped?1:2) && rng_calls==(skipped?1:2));
 advance(&state,TTime::LegacyFrameSeconds*4);
 assert(owner.frames==2 && state.ticks==2 && rng_calls==(skipped?1:2));
}
int main(){Check(actual::Advance,false);Check(early::Advance,true);}
'''
  with tempfile.TemporaryDirectory()as folder:
   p=Path(folder);(p/'test.cpp').write_text(driver)
   subprocess.run(['clang++','-std=c++17','-O1','-fsanitize=address,undefined',str(p/'test.cpp'),'-o',str(p/'test')],check=True)
   subprocess.run([str(p/'test')],check=True)
if __name__=='__main__':unittest.main()
