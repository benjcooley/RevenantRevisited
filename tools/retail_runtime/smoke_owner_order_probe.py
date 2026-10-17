#!/usr/bin/env python3
"""Execute Smoke owner order and deliberately reverse its completion handshake."""
import argparse,json,zipfile
from pathlib import Path
from smoke_visual_reference import NativeSmoke,ROOT,sha

def run(out):
 out.mkdir(parents=True,exist_ok=False)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/misc/smoke.i3d'))
 good=NativeSmoke(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data);good.advance();assert good.state()['alive'] and good.state()['tick']==1
 bad=NativeSmoke(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data);v=bad.vm;v.call(0x470cc0,this=bad.owner);v.call(0x4de800,this=bad.owner);assert not bad.state()['alive'] and bad.state()['tick']==0
 missing=NativeSmoke(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data);v=missing.vm;v.call(0x4de800,this=missing.owner);v.call(0x470cc0,this=missing.owner);v.call(0x4de800,this=missing.owner);assert not missing.state()['alive']
 low=[]
 for quality in(0,1,2):
  f=NativeSmoke(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data,quality);assert not f.state()['alive'] and not any(f.render());low.append(dict(quality=quality,alive=False,seed=f.vm.rng_seed))
 report=dict(status='pass',literal_state_flags='0x2000',good_order='Pulse4708e0/4de800 then NextFrame470cc0 then Animate4e7e50 clearing CommandDone at4e7e62',good_tick=good.state()['tick'],reverse_order_kills=True,omitting_animate_clear_kills=True,low_quality=low,fixture_sha256=sha((ROOT/'tools/retail_runtime/smoke_visual_reference.py').read_bytes()),scope='Actual original owner methods; wrong-order and omitted actualAnimate negative controls. Base object script/bookkeeping facade retained; no looping-flag override.');(out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);run(p.parse_args().output)
