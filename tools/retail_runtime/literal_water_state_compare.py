#!/usr/bin/env python3
"""Full native factory pool/RNG trace versus compiled production state bodies."""
import argparse,json,struct,zipfile
from pathlib import Path
from speed_controller_preflight import ROOT
from firecone_state_probe import sha
from waterfall_probe import build_port as waterfall_port
from water_probe import build_port as water_port
def run(native,output):
 output.mkdir(parents=True,exist_ok=False);report=json.loads((native/'manifest.json').read_text());assert report['status']=='reference_complete';cases=[]
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read('Imagery/Misc/water.i3d')
 for name,build in [('Waterfall',waterfall_port),('Water',water_port)]:
  folder=output/name;folder.mkdir();path=native/name;trace=json.loads((path/'state-trace.json').read_text());frames,compiled=build(folder,data,path/'rng-inputs.txt',60);errors=[];checks=0
  for row in trace:
   source=frames[row['tick']];assert len(source['particles'])==len(row['particles']);checks+=1
   if source['random_count']!=row['random_count']:errors.append(dict(tick=row['tick'],field='random_count'))
   for i,(p,q)in enumerate(zip(row['particles'],source['particles'])):
    checks+=10
    if p[-1]!=q['time']:errors.append(dict(tick=row['tick'],particle=i,field='time'))
    for k,(x,y)in enumerate(zip(p[:9],q['values'])):
     if struct.pack('<f',x)!=struct.pack('<f',y):errors.append(dict(tick=row['tick'],particle=i,field=k,native=x,production=y))
  case=dict(name=name,status='pass'if not errors else'differences_found',errors=errors,checks=checks,native_trace_sha256=sha((path/'state-trace.json').read_bytes()),native_rng_sha256=sha((path/'rng-inputs.txt').read_bytes()),compiled=compiled);cases.append(case);print(name,case['status'],checks,flush=True)
 result=dict(status='pass'if all(c['status']=='pass'for c in cases)else'differences_found',accepted=False,cases=cases,native_manifest_sha256=sha((native/'manifest.json').read_bytes()),probe_sha256=sha(Path(__file__).read_bytes()),scope='All61 fully initialized native pool/RNG states versus actual compiled InitParticle/UpdateStuff/Initialize warmup/Advance; all9float channels bit-exact plus time/count. Range RNG provider replays independently executed original values, checking exact request order. Old diagnostic quad submission remains extracted fallback, real normal-lit submission independently checked by production guard + actual Metal capture; no fullpixel/device/natural acceptance.');(output/'manifest.json').write_text(json.dumps(result,indent=2)+'\n');return result
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--native',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();print(run(a.native,a.output)['status'])
