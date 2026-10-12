"""Distinct arrow factories and QueenArrow moving pulse/Render cadence regression."""
import json,subprocess,sys,tempfile,unittest
from pathlib import Path
from speed_profile_contract import ROOT
from test_vfxreviewspawn import function
class ArrowTests(unittest.TestCase):
 def test_actual_adapter_tick_rng_and_draw_cache_at_display_rates(self):
  advance=function((ROOT/'src/effects/arroworiginal.cpp').read_text(),'void Advance(')
  cpp=r'''
#include <cassert>
#include <cmath>
#include <cstdint>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include "arrowcore.h"
struct S3DPoint{int x,y,z;};constexpr int OF_KILL=4096,OF_PULSE=1;
struct Owner{int flags=0;void NextFrame(){};void ForcePos(S3DPoint){};int GetFlags(){return flags;}void SetFlags(int f){flags=f;}};
struct TTime{static constexpr double LegacyFrameSeconds=1./24.;};
uint32_t seed,drawcount;int random(int lo,int hi){++drawcount;seed=seed*214013u+2531011u;return lo+int((seed>>16)&32767u)%(hi-lo+1);}
struct State{std::unique_ptr<Owner>owner=std::make_unique<Owner>();arrow_core::State core;arrow_core::AttachedCaller attached;bool queen;double accumulated=0;int facing=64,ticks=0;S3DPoint source{-180,0,38},target{180,0,78};std::vector<arrow_core::Draw>draws;arrow_core::State&Core(){return queen?core:attached.core;}};
bool IsAlive(const State*s){return s&&s->owner&&!(s->owner->GetFlags()&OF_KILL)&&(s->queen?s->core:s->attached.core).alive;}
'''+advance+r'''
std::vector<std::string>run(bool queen,int fps){seed=1;drawcount=0;State s;s.queen=queen;s.attached.position={-180,0,78};s.attached.velocity_fixed={20*65536,0,0};std::vector<std::string>rows;int previous=0;for(int i=0;i<fps;++i){Advance(&s,1./fps);if(s.ticks!=previous){previous=s.ticks;std::ostringstream o;const auto&c=s.Core();o<<s.ticks<<' '<<c.position.x<<' '<<c.position.y<<' '<<c.position.z<<' '<<c.grow<<' '<<c.countdown<<' '<<c.alive<<' '<<seed<<' '<<drawcount;for(const auto&d:s.draws)o<<' '<<d.object<<' '<<d.x<<' '<<d.y<<' '<<d.z<<' '<<d.scale;rows.push_back(o.str());}}return rows;}
int main(){for(bool queen:{false,true}){auto original=run(queen,24);assert(!original.empty());for(int fps:{60,120})assert(run(queen,fps)==original);}}
'''
  with tempfile.TemporaryDirectory(prefix='arrow-adapter-cadence-')as tmp:
   p=Path(tmp);driver=p/'driver.cpp';driver.write_text(cpp);binary=p/'driver'
   subprocess.run(['clang++','-std=c++17','-ffp-contract=off','-fsanitize=address,undefined','-I'+str(ROOT/'thirdparty/handmademath'),'-iquote',str(ROOT/'src'),str(driver),str(ROOT/'src/math3d.cpp'),'-o',str(binary)],check=True)
   subprocess.run([str(binary)],check=True)
 def invoke(self,tool,*args):
  p=subprocess.run([sys.executable,str(ROOT/'tools/retail_runtime'/tool),*map(str,args)],cwd=ROOT,capture_output=True,text=True,timeout=100)
  self.assertEqual(p.returncode,0,p.stdout+p.stderr)
 def test_distinct_complete_native_factories(self):
  with tempfile.TemporaryDirectory(prefix='arrow-factories-')as tmp:
   out=Path(tmp);self.invoke('arrow_native_preflight.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',out)
   d=json.loads((out/'manifest.json').read_text());self.assertEqual([r['color']for r in d['bindings']],[0,2]);self.assertEqual(d['base_calls'],2)
 def test_actual_gallery_diagonal_original_movement_and_trail(self):
  with tempfile.TemporaryDirectory(prefix='arrow-gallery-direction-')as tmp:
   out=Path(tmp);native=out/'native';self.invoke('arrow_attached_visual_reference.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',native,'--frames',24,'--state-only','--gallery-direction')
   d=json.loads((native/'trace.json').read_text());self.assertEqual(d['facing'],229);self.assertEqual(d['records'][0]['state']['position'],[-12,-15,144]);self.assertTrue(any(r['state']['deleted']for r in d['records']))
   self.invoke('arrow_core_probe.py',native/'trace.json','--output',out/'source');r=json.loads((out/'source/manifest.json').read_text());self.assertEqual(r['status'],'pass');self.assertEqual(r['matrix_fields'],3648);self.assertLess(r['max_matrix_error'],1e-6)
 def test_original_motion_collision_kill_and_render_rng_at_two_cadences(self):
  with tempfile.TemporaryDirectory(prefix='queen-arrow-')as tmp:
   for stride in(1,3):
    out=Path(tmp)/str(stride);native=out/'native';self.invoke('arrow_visual_reference.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',native,'--frames',24,'--state-only','--render-stride',stride,'--resolution',640,480,'--center',320,320)
    rows=json.loads((native/'trace.json').read_text())['records'];self.assertEqual(rows[0]['state']['position'],[-157,0,78]);self.assertEqual(rows[15]['state']['position'],[188,0,78]);self.assertEqual(rows[16]['state']['countdown'],3);self.assertEqual(rows[17]['state']['countdown'],1);self.assertTrue(rows[18]['state']['flags']&4096)
    self.invoke('arrow_core_probe.py',native/'trace.json','--output',out/'source');self.assertEqual(json.loads((out/'source/manifest.json').read_text())['status'],'pass')
 def test_original_moving_attachment_terrain_and_deletion(self):
  with tempfile.TemporaryDirectory(prefix='arrow-attached-')as tmp:
   for stride in(1,3):
    out=Path(tmp)/str(stride);native=out/'native';self.invoke('arrow_attached_visual_reference.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',native,'--frames',24,'--state-only','--render-stride',stride)
    rows=json.loads((native/'trace.json').read_text())['records'];self.assertEqual(rows[0]['state']['position'],[-160,0,78]);self.assertEqual(rows[17]['state']['position'],[180,0,78]);self.assertNotIn('countdown',rows[0]['state']);self.assertTrue(rows[17]['state']['attached']);self.assertFalse(rows[18]['state']['attached']);self.assertTrue(rows[18]['state']['flags']&4096)
    self.invoke('arrow_core_probe.py',native/'trace.json','--output',out/'source');r=json.loads((out/'source/manifest.json').read_text());self.assertEqual(r['status'],'pass');self.assertGreater(r['matrix_fields'],1000)
if __name__=='__main__':unittest.main()
