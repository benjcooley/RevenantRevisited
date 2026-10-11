"""Replay observed CombatFlash normal packets through original software raster.

Input must be the read-only loaded original observation. Per-draw clear isolates
pixel contributions; it does not replace the full effect reference or grant a pass.
"""
import argparse
import json,struct,zipfile,hashlib
from pathlib import Path
from lit_software_probe import LitSoftwareFixture
from speed_controller_preflight import ROOT,read_asset
from software_probe import save_rgb565_png
def run(trace_path, out):
 out.mkdir(parents=True,exist_ok=False)
 trace=json.loads(trace_path.read_text())
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/misc/impact.i3d'))
 u=lambda p:struct.unpack_from('<I',data,p)[0];r=lambda p:p+u(p);body=20+u(16);asset=read_asset(data);textures=r(body+40)
 f=LitSoftwareFixture(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',640,480);f.lighting((38,38,38),(1,1,1));cases=[]
 for row in trace['rows']:
  for draw in row['draws']:
   if draw['arguments'][1]!=274:continue
   obj=asset['objects'][draw['object']];runs=obj['texture_runs'];slot=next(i//2-1 for i in range(0,len(runs),2)if runs[i+1]);assert slot>=0
   tex=textures+slot*120;h,w=struct.unpack_from('<2I',data,tex+8);pix=r(r(tex+108));raw=data[pix:pix+w*h*2];f.set_texture(w,h,raw);f.clear()
   for key,val in draw['states'].items():f.vm.call(0x56d400,(int(key),val))
   vertices=list(struct.iter_unpack('<8f',bytes.fromhex(draw['vertices_hex'])));indices=list(struct.unpack('<'+'H'*draw['arguments'][5],bytes.fromhex(draw['indices_hex'])));pixels,depth,t=f.draw_authored(vertices,indices,draw['matrix'],cull=int(draw['states']['22']),z_enabled=bool(draw['states']['7']),z_write=bool(draw['states']['14']))
   count=sum(v!=0 for(v,)in struct.iter_unpack('<H',pixels));case=dict(tick=row['tick'],object=draw['object'],slot=slot,nonblack=count,texture_sha256=hashlib.sha256(raw).hexdigest());cases.append(case)
   if draw['object']==2 or count:save_rgb565_png(out/f"tick{row['tick']:02d}-object{draw['object']:02d}.png",pixels,640,480)
 (out/'manifest.json').write_text(json.dumps(dict(status='diagnostic',accepted=False,cases=cases,trace_sha256=hashlib.sha256(trace_path.read_bytes()).hexdigest(),asset_sha256=hashlib.sha256(data).hexdigest(),probe_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),scope='All observed original normal draw packets replayed independently across18liveframes with literal owntextures/states/matrices and normal-light inputs. Isolated pixel contributions only; not full original compositing or port acceptance.'),indent=2)+'\n');print('draws',len(cases),'extra-visible',[(c['tick'],c['object'],c['nonblack'])for c in cases if c['object']!=2 and c['nonblack']])

if __name__=="__main__":
 parser=argparse.ArgumentParser();parser.add_argument("trace",type=Path);parser.add_argument("--output",type=Path,required=True)
 args=parser.parse_args();run(args.trace,args.output)
