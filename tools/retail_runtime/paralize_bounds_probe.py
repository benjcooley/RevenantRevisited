#!/usr/bin/env python3
"""Executed original projection of every submitted no-target Paralize1 sprite."""
import argparse,json,struct,zipfile,math
from pathlib import Path
from lit_software_probe import LitSoftwareFixture
from speed_controller_preflight import ROOT,read_asset,sha

def run(trace,output):
 output.mkdir(parents=True,exist_ok=False);ref=json.loads(trace.read_text());f=LitSoftwareFixture(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',1280,960,camera=(0,0,0));v=f.vm
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:a=read_asset(z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/paralize.i3d')))
 points=[list(x[:3])for x in struct.iter_unpack('<8f',bytes.fromhex(a['objects'][0]['vertex_hex']))];rows=[];allpoints=[]
 for r in ref['records']:
  b=bytes.fromhex(r['state']['owner_hex']);scale=struct.unpack_from('<f',b,0x504-0x184)[0]
  if r['state']['flags']&0x1000 or scale<=1e-5:assert r['draw_count']==0;continue
  assert r['draw_count']==15
  for i in range(15):
   position=list(struct.unpack_from('<3f',b,0x448-0x184+12*i));matrix=[scale,0,0,0,0,scale,0,0,0,0,scale,0,*position,1]
   if 'draws'in r:assert r['draws'][i]['object']==0 and r['draws'][i]['matrix']==matrix
   p=[(x,y+160,z)for x,y,z in f.project(points,world_matrix=matrix,camera=(0,0,0))];assert all(math.isfinite(x)for point in p for x in point);bounds=[min(x[0]for x in p),min(x[1]for x in p),max(x[0]for x in p),max(x[1]for x in p)];allpoints+=p;rows.append(dict(tick=r['tick']+1,slot=i,bounds=bounds))
 bounds=[min(x[0]for x in allpoints),min(x[1]for x in allpoints),max(x[0]for x in allpoints),max(x[1]for x in allpoints)];report=dict(status='complete_bounds_diagnostic',accepted=False,visual_pass=False,bounds=bounds,span=[bounds[2]-bounds[0],bounds[3]-bounds[1]],draws=rows,native_trace_sha256=sha(trace.read_bytes()),scope='Actual original43aa90/43ad80 view/projection for every native submitted lightface0 sprite, own literalvertices and observedownerpositions/scales, verified against retained fullDraw packets. No camera or geometry fit; full projected trajectory coverage differs from the early fixedviewport visibility proof.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(bounds,report['span']);return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('trace',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.trace,a.output)
