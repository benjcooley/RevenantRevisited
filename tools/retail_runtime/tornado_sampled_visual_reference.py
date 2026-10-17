"""Sample pixel work while executing every original Tornado owner/Render packet.

Only unselected56eb30 lighting/raster submissions return0 after their complete
input packet is captured. Selected frames execute that original pipeline fully.
All owner/animator/controller/material/geometry and RNG calls remain executed.
This boundary must pass full first10 state/packet/selected-PNG controls before
being used for the longer selected-phase appearance reference.
"""
import argparse,json,struct
from pathlib import Path
from tornado_target_visual_reference import TornadoTargetScene,ROOT,sha,save_rgb565_png
from speed_controller_preflight import NativeSpeed

class SampledScene(TornadoTargetScene):
 raster_enabled=True
 def observe_draw(self,uc,*args):
  super().observe_draw(uc,*args)
  if not self.raster_enabled:NativeSpeed.ret(self,0,28)

def run(boot,out,frames=380,control=None):
 out.mkdir(parents=True,exist_ok=False);f=SampledScene(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',boot);rows=[]
 samples={1,5,10,20,40,60,80,100,150,180,200,240,280,300,320,340,350,359,360,380}
 for t in range(1,frames+1):
  f.raster_enabled=t in samples;f.step();raw=f.render();row=dict(tick=t,state=f.state(),draw_count=len(f.draws),draws=f.draws,raster_executed=f.raster_enabled)
  if f.raster_enabled:
   p=out/f'native-{t:03}.png';save_rgb565_png(p,raw,1280,960);row.update(image=str(p),image_sha256=sha(p),pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',raw)))
  rows.append(row)
  with(out/'trace.jsonl').open('a')as h:h.write(json.dumps(row)+'\n')
  if t%10==0 or t in samples:print(t,row['state']['stage'],row['draw_count'],row.get('pixels'),flush=True)
 checks={}
 if control:
  c=json.loads(control.read_text());assert c['initial']==f.initial
  for original,sampled in zip(c['records'],rows):
   assert original['state']==sampled['state'],original['tick']
   assert original['draws']==json.loads(json.dumps(sampled['draws'])),original['tick']
   if 'image_sha256'in original:assert original['image_sha256']==sampled['image_sha256'],original['tick']
  checks=dict(status='pass',frames=len(c['records']),initial_state_exact=True,all_effect_pool_rng_draw_packets_exact=True,selected_png_hashes_exact=True,full_control_path=str(control),full_control_sha256=sha(control))
 report=dict(status='sampled_reference',accepted=False,visual_pass=False,type_id='0x2784abde',initial=f.initial,records=rows,variant=f.spell_variant,actors_before=f.before,actors_after=[f.actors.dump(x)for x in(f.invoker,f.target)],damage_calls=f.damage_calls,executable_sha256=sha(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe'),boot_image_sha256=sha(boot),source_sha256=sha(__file__),whole_caller_tool_sha256=sha(Path(__file__).with_name('tornado_target_visual_reference.py')),viewport=[1280,960],center=[640,640],camera=[3185,3185,0],lighting=dict(ambient=[38]*3,directional=[1]*3),full_control=checks,scope=__doc__)
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print('complete',checks,flush=True);return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--boot-image',type=Path,required=True);p.add_argument('--output',type=Path,required=True);p.add_argument('--frames',type=int,default=380);p.add_argument('--full-control',type=Path);a=p.parse_args();run(a.boot_image,a.output,a.frames,a.full_control)
