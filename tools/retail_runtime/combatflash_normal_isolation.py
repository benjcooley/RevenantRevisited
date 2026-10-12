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
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP
def run(trace_path, out):
 out.mkdir(parents=True,exist_ok=False)
 trace=json.loads(trace_path.read_text())
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/misc/impact.i3d'))
 u=lambda p:struct.unpack_from('<I',data,p)[0];r=lambda p:p+u(p);body=20+u(16);asset=read_asset(data);textures=r(body+40)
 f=LitSoftwareFixture(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',640,480);f.lighting((38,38,38),(1,1,1));cases=[]
 packets=[];current={};transformed=[]
 def observe_transform(uc,*_):
  sp=uc.reg_read(UC_X86_REG_ESP);n=f.vm.u32(sp+4)
  transformed[:]=list(struct.iter_unpack('<3f4I2f',uc.mem_read(f.vm.u32(0x675fb4),n*36)))
 def observe_packet(uc,*_):
  sp=uc.reg_read(UC_X86_REG_ESP);args=list(struct.unpack('<29i',uc.mem_read(sp+4,116)))
  actual=[args[1+i*8:9+i*8]for i in range(3)]
  expected=[[int(q[0]),int(-q[1]),int(q[2]),int(q[7]*current['width']),int(q[8]*current['height']),*q[3:6]]for q in transformed]
  assert all(q in expected for q in actual),(current,actual,expected)
  packets.append(dict(tick=current['tick'],object=current['object'],integer=actual))
 f.vm.uc.hook_add(UC_HOOK_CODE,observe_transform,begin=0x56d960,end=0x56d960)
 f.vm.uc.hook_add(UC_HOOK_CODE,observe_packet,begin=0x54e3a0,end=0x54e3a0)
 for row in trace['rows']:
  for draw in row['draws']:
   if draw['arguments'][1]!=274:continue
   obj=asset['objects'][draw['object']];runs=obj['texture_runs'];slot=next(i//2-1 for i in range(0,len(runs),2)if runs[i+1]);assert slot>=0
   tex=textures+slot*120;h,w=struct.unpack_from('<2I',data,tex+8);pix=r(r(tex+108));raw=data[pix:pix+w*h*2];current.update(tick=row['tick'],object=draw['object'],width=w,height=h);f.set_texture(w,h,raw);f.clear()
   for key,val in draw['states'].items():f.vm.call(0x56d400,(int(key),val))
   vertices=list(struct.iter_unpack('<8f',bytes.fromhex(draw['vertices_hex'])));indices=list(struct.unpack('<'+'H'*draw['arguments'][5],bytes.fromhex(draw['indices_hex'])));pixels,depth,t=f.draw_authored(vertices,indices,draw['matrix'],cull=int(draw['states']['22']),z_enabled=bool(draw['states']['7']),z_write=bool(draw['states']['14']))
   count=sum(v!=0 for(v,)in struct.iter_unpack('<H',pixels));case=dict(tick=row['tick'],object=draw['object'],slot=slot,nonblack=count,texture_sha256=hashlib.sha256(raw).hexdigest());cases.append(case)
   if draw['object']==2 or count:save_rgb565_png(out/f"tick{row['tick']:02d}-object{draw['object']:02d}.png",pixels,640,480)
 (out/'manifest.json').write_text(json.dumps(dict(status='diagnostic',accepted=False,cases=cases,integer_packets=packets,integer_fields=len(packets)*24,trace_sha256=hashlib.sha256(trace_path.read_bytes()).hexdigest(),asset_sha256=hashlib.sha256(data).hexdigest(),probe_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),scope='All observed original normal draw packets replayed independently across18liveframes with literal owntextures/states/matrices and normal-light inputs. Isolated pixel contributions only; not full original compositing or port acceptance.'),indent=2)+'\n');print('draws',len(cases),'extra-visible',[(c['tick'],c['object'],c['nonblack'])for c in cases if c['object']!=2 and c['nonblack']])

if __name__=="__main__":
 parser=argparse.ArgumentParser();parser.add_argument("trace",type=Path);parser.add_argument("--output",type=Path,required=True)
 args=parser.parse_args();run(args.trace,args.output)
