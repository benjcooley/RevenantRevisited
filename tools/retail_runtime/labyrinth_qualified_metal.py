"""Selected complete actualMetal phases, every precedingRender/RNG executed."""
import argparse,json
from pathlib import Path
from port_capture import capture
from speed_controller_preflight import ROOT
from speed_profile_contract import sha
PHASES=(1,6,16,30,60,99,100,101,170)
def run(binary,out):
 out.mkdir(parents=True,exist_ok=False);pinned=sha(binary.read_bytes());cases=[]
 for tick in PHASES:
  scenario=dict(binary=str(binary),data_root=str(ROOT/'data'),assets_root=str(ROOT/'assets'),effect='Labyrinth_QUALIFIED_BOUND',frames=1,fps=24,seed=1,warmup=tick-1,resolution=[1920,1080],origin=[0,0,0],camera=[960,600],rectangle=[0,0,1920,1080],native_domain=True,ambient=[32,255,255,255],timeout_seconds=120)
  r=capture(scenario,out/f'phase-{tick:03d}');assert r['status']=='pass'and r['binary_sha256']==pinned and r['sample_simulation_ticks']==[tick];cases.append(dict(tick=tick,path=str(out/f'phase-{tick:03d}/manifest.json'),sha256=sha((out/f'phase-{tick:03d}/manifest.json').read_bytes())));print('PASS',tick,flush=True)
 r=dict(status='pass',accepted=False,binary_sha256=pinned,cases=cases,source_sha256=sha((ROOT/'src/effects/labyrinthqualifiedpreview.cpp').read_bytes()),scope='Sequential fresh actualGPU cases executeevery precedinglogical24Hz owner/coreAdvance andwholeRender/RNG, captureonlydeclared1/6/16/30/60/99/100/101/170 phases. Fixedsame1920x1080center960,600 camera, unchangedasset/seed1. Qualifiedinitializer4→3/nativeSWbackgroundextentgate only; no unmodifiedretail appearance claim.');(out/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');return r
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--binary',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.binary.resolve(),a.output.resolve())
