"""Antimagic's full owner clock and explicit loop-control negative case."""
import struct,unittest,zipfile,subprocess,tempfile
from pathlib import Path
from speed_profile_contract import function
from antimagic_visual_reference import NativeAntimagic,ROOT
class AntimagicTests(unittest.TestCase):
 def test_actual_source_does_not_advance_particles_after_owner_kill(self):
  source=(ROOT/'src/effects/antimagicpreview.cpp').read_text()
  actual=function(source,'void Advance(State* s,double seconds)')
  driver=r'''#include <cassert>
#include <cmath>
struct Owner{bool alive=true,kill=false;int frames=0;void NextFrame(){++frames;}void Pulse(){if(kill)alive=false;}};
struct Particles{int advances=0;template<class T,class R>void Advance(T&,R){++advances;}};
struct State{Owner*owner;Particles particles;int inputs=0,ticks=0;double accumulator=0;};
namespace TTime{constexpr double LegacyFrameSeconds=1.0/24.0;}
bool Alive(State*s){return s&&s->owner->alive;}int random(int a,int){return a;}
'''+actual+r'''
int main(){Owner owner;State s{&owner};Advance(&s,TTime::LegacyFrameSeconds);assert(s.ticks==1&&s.particles.advances==1);owner.kill=true;Advance(&s,TTime::LegacyFrameSeconds);assert(!owner.alive&&s.ticks==2&&s.particles.advances==1);Advance(&s,TTime::LegacyFrameSeconds);assert(s.ticks==2&&s.particles.advances==1);}
'''
  with tempfile.TemporaryDirectory()as t:
   p=Path(t);(p/'test.cpp').write_text(driver);subprocess.run(['clang++','-std=c++17',str(p/'test.cpp'),'-o',str(p/'test')],check=True);subprocess.run([str(p/'test')],check=True)
 def test_original_owner_completion_and_loop_control(self):
  with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/antimagic.i3d'))
  for looping in (False,True):
   f=NativeAntimagic(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data);r=f.initialize_scene();self.assertEqual(r['controllers'],['0x5a357c','0x5a3544']);v=f.vm
   if looping:
    method=v.u32(v.u32(f.imagery)+0x8c);v.write_code(method,b'\xb8'+struct.pack('<I',0x2001)+b'\xc2\x04\x00')
   for tick in range(1,301):
    f.advance();self.assertEqual(bool(v.u32(f.owner+8)&0x1000),not looping and tick==300)
   frame=struct.unpack('<h',v.uc.mem_read(f.owner+0x5c,2))[0];self.assertEqual(frame,0 if looping else 299)
if __name__=='__main__':unittest.main()
