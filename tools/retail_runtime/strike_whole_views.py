#!/usr/bin/env python3
"""Fixed overlapping views of the genuine world-dependent Strike particles.

Each supplemental view restores the same original pre-Render VM and host state.
All original caller/controller Render calls execute, and identical state/RNG/draw
packets are asserted before returning to the authoritative base-view timeline.
Only projection center/output viewport change; original640/480 span guards stay.
"""
import argparse,copy,json,struct,hashlib
from pathlib import Path
from strike_loaded_visual_reference import StrikeScene,ROOT,sha
from software_probe import save_rgb565_png
CENTERS=(6460,5500,4540,3580,2620,1660,700)
SAMPLES=(11,21,31,41,51,61)
def stripped(state):
 state=copy.deepcopy(state)
 for o in state['owners']:
  for c in o['pools']:c.pop('address',None);c.pop('credit',None)
 return state
def run(boot,reference,out):
 out.mkdir(parents=True,exist_ok=False);exe=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe';f=StrikeScene(exe,boot);prior=json.loads(reference.read_text());rows=[]
 for tick in range(1,101):
  f.step(tick);selected=tick in SAMPLES;views=[]
  if selected:
   f.world.checkpoint();saved=(f.rng_calls,f.srand_calls[:],dict(f.states),f.material)
   for center in CENTERS:
    f.world.restore();f.rng_calls,f.srand_calls,f.states,f.material=copy.deepcopy(saved);f.vm.put_u32(0x675fc4,center);f.raster_enabled=True;raw=f.render();p=out/f'native-{tick:03}-y{center}.png';save_rgb565_png(p,raw,1920,1080)
    state=f.state();draws=copy.deepcopy(f.draws)
    if views:assert state==views[0]['state'] and draws==views[0]['draws'],(tick,center)
    views.append(dict(center=[960,center],image=str(p),image_sha256=sha(p),pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',raw)),state=state,draws=draws))
    print(tick,center,views[-1]['pixels'],flush=True)
  else:f.raster_enabled=False;f.render()
  old=prior['records'][tick-1];assert stripped(f.state())==old['state'],tick;assert json.loads(json.dumps(f.draws))==old['draws'],tick
  rows.append(dict(tick=tick,state_sha256=hashlib.sha256(json.dumps(stripped(f.state()),sort_keys=True).encode()).hexdigest(),views=views))
 report=dict(status='pass',scope=__doc__,source_sha256=sha(__file__),reference=str(reference),reference_sha256=sha(reference),original_sha256=sha(exe),viewport=[1920,1080],fixed_centers=[[960,c]for c in CENTERS],camera=[3185,3185,0],samples=SAMPLES,records=rows,all100_state_pool_rng_packets_exact=True)
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--boot-image',type=Path,required=True);p.add_argument('--reference',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.boot_image,a.reference,a.output)
