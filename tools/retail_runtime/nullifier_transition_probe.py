"""Observe full loaded nullifier emitter transitions and original hardware packets.

No pose/controller replacement. Raw409430 is a negative control for actual
409950; the original cached matrices, previous pose, pools/RNG and full Render
are retained. Source comparison consumes opt-in actual SubmitPartSys logs.
"""
import argparse, hashlib, json, re, struct
from pathlib import Path
from dust_device_reference import DustDevice
from character_native_preflight import ROOT,sha
from fspray_device_reference import packed

def native(exe,boot,out,frames=77):
 out.mkdir(parents=True,exist_ok=False);f=DustDevice(exe,boot,name='nullifier',source=(0,0,16));v=f.vm;im=v.u32(f.owner+0x54);vectors=v.allocate(36);rows=[]
 for tick in range(frames):
  if tick:f.step(tick)
  packets=f.render_device();state=f.state();frame=state['frame'];keys=[]
  for obj in (0,2,3,4,5,6,7,8):
   vals=[]
   for address in (0x409950,0x409430):
    assert v.call(address,(obj,0,frame,vectors,vectors+12,vectors+24),this=im)==1
    vals.append(list(struct.unpack('<9f',v.uc.mem_read(vectors,36))))
   ap=v.u32(v.u32(f.animator+0x54)+4*obj)
   keys.append(dict(object=obj,interpolated=vals[0],raw=vals[1],cached_matrix=list(struct.unpack('<16f',v.uc.mem_read(ap+0x58,64)))))
  rows.append(dict(tick=tick,state=state,owner_previous=list(struct.unpack('<2h',v.uc.mem_read(f.owner+0x60,4))),imagery_previous=[v.u32(im+0xa4),v.u32(im+0xa8)],keys=keys,packet_sha256=packed(packets),packets=[{k:p[k]for k in ('vertices_hex','indices','world_matrix_hex','projected','states','stages')}for p in packets]))
  if tick%25==0:print(tick,len(packets),flush=True)
 assert all(r['owner_previous']==r['imagery_previous']==[0,0]for r in rows)
 errors=[max(abs(a-b)for k in r['keys']for a,b in zip(k['interpolated'],k['raw']))for r in rows]
 assert all((e>0)==(r['state']['frame']in(2,3))for r,e in zip(rows,errors))
 report=dict(status='pass',accepted=False,scope=__doc__,rows=rows,raw_negative_max=max(errors),executable_sha256=sha(exe),boot_sha256=sha(boot),tool_sha256=sha(__file__))
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report

def compare(reference,log,out,old=False):
 n=json.loads(reference.read_text());rows={};q=None
 for line in log.read_text().splitlines():
  if '[null-packet]'in line:
   m=re.search(r'tick=(\d+) frame=(\d+) slot=(\d+) color=(.*)',line);q=dict(slot=int(m[3]),color=list(map(float,m[4].split(','))),vertices=[]);rows.setdefault(int(m[1]),[]).append(q)
  elif '[null-corner]'in line:q['vertices'].append(list(map(float,line.split('[null-corner] ')[1].split(','))))
 comparisons=[]
 for r in n['rows']:
  tick=r['tick']
  if tick not in rows:continue
  packets=r['packets'];source=rows[tick];assert len(packets)==len(source),(tick,len(packets),len(source))
  for i,(p,q)in enumerate(zip(packets,source)):
   assert p['indices']==[2,3,0,1,2,0]
   vp=[[320+1.0101525783538818*(x-y),320+.5050762891769409*(x+y)-1.237179160118103*z,2750-.8748177289962769*(x+y)-.7142857909202576*z]for x,y,z,u,w in q['vertices']]
   order=(0,1,3,2);raw=bytes.fromhex(p['vertices_hex']);uv=[struct.unpack_from('<2f',raw,j*32+24)for j in range(4)];color=struct.unpack_from('<I',raw,16)[0]
   xyz=max(abs(vp[c][d]-p['projected'][order[c]][d])for c in range(4)for d in range(3));uve=max(abs(q['vertices'][c][3+d]-uv[order[c]][d])for c in range(4)for d in range(2));ce=max(abs(q['color'][c]*255-((color>>s)&255))for c,s in enumerate((16,8,0)))
   assert uve<1e-7 and ce<2e-5
   if not old:assert xyz<.001,(tick,i,xyz)
   comparisons.append(dict(tick=tick,draw=i,slot=q['slot'],xyz_max=xyz,uv_max=uve,rgb8_max=ce))
 assert comparisons
 report=dict(status='negative-control'if old else'pass',accepted=False,frames=len(rows),quads=len(comparisons),max_position=max(r['xyz_max']for r in comparisons),max_uv=max(r['uv_max']for r in comparisons),max_color=max(r['rgb8_max']for r in comparisons),reference_sha256=sha(reference),source_log_sha256=sha(log),tool_sha256=sha(__file__),comparisons=comparisons)
 out.write_text(json.dumps(report,indent=2)+'\n');print(report['status'],report['frames'],report['quads'],report['max_position']);return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--boot-image',type=Path);p.add_argument('--output',type=Path,required=True);p.add_argument('--reference',type=Path);p.add_argument('--source-log',type=Path);p.add_argument('--old',action='store_true');a=p.parse_args()
 if a.source_log:compare(a.reference,a.source_log,a.output,a.old)
 else:native(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',a.boot_image,a.output)
