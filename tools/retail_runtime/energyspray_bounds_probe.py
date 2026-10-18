#!/usr/bin/env python3
"""EnergySpray exact-state original raster-span negative control."""
import argparse,json,struct,zipfile,subprocess
from pathlib import Path
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EFLAGS
from energyspray_visual_reference import EnergySprayScene,ROOT,sha
from software_probe import save_rgb565_png

def run(exe,out,tick=111,width=640,height=480):
 out.mkdir(parents=True,exist_ok=False)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/energyspray.i3d'))
 f=EnergySprayScene(exe,data,width,height);f.raster_enabled=False
 for current in range(1,tick+1):
  f.advance()
  if current<tick:f.render()
 v=f.vm;v.checkpoint();count=f.rng_draws;seed=v.rng_seed;material=f.material_state;guards=[]
 def observe(uc,address,*_):
  guards.append(dict(object=f.current_object,draw=len(f.draws)-1,address=hex(address),reject=bool(uc.reg_read(UC_X86_REG_EFLAGS)&64)))
 for address in(0x56da08,0x56da33,0x56da59,0x56da7f,0x56e006,0x56e036,0x56e05c,0x56e082,0x56e36a,0x56e395,0x56e3bb,0x56e3e1,0x56e6f1,0x56e71c,0x56e742,0x56e768):v.uc.hook_add(UC_HOOK_CODE,observe,begin=address,end=address)
 rows=[]
 for name,limits in(('original',(640,480)),('widened_negative_control',(6400,4800))):
  v.restore();v.rng_seed=seed;f.rng_draws=count;f.material_state=material;guards.clear();v.write(0x5b9cc0,struct.pack('<f',limits[0]));v.write(0x5b9cbc,struct.pack('<f',limits[1]));f.raster_enabled=True;raw=f.render();p=out/(name+'.png');save_rgb565_png(p,raw,width,height);rows.append(dict(name=name,limits=limits,guards=guards.copy(),state=f.state(),draws=f.draws,image=str(p),sha256=sha(p.read_bytes()),pixels=sum(x[0]!=0 for x in struct.iter_unpack('<H',raw))))
 assert rows[0]['state']==rows[1]['state']and rows[0]['draws']==rows[1]['draws'];assert rows[1]['pixels']>rows[0]['pixels']
 if tick==111:assert rows[0]['pixels']==0
 assert any(g['reject']for g in rows[0]['guards'])and not any(g['reject']for g in rows[1]['guards'])
 # Independently compile the unchanged shared gate against each actual prelit
 # triangle's input matrix/vertices; compare actual executed untextured guards.
 admission=[]
 if tick<111:
  source=out/'admission.cpp';source.write_text('#include "cometsoftwarebounds.h"\n#include <cstdio>\nint main(){hmm_mat4 m;hmm_vec3 p[3];while(scanf("%f",&m.Elements[0][0])==1){for(int i=1;i<16;++i)scanf("%f",(&m.Elements[0][0])+i);for(auto&v:p)scanf("%f%f%f",&v.X,&v.Y,&v.Z);printf("%d\\n",RetailCometTriangleAllowed(p,m));}}')
  binary=out/'admission';subprocess.run(['clang++','-std=c++17','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(source),str(ROOT/'src/engine/core/math3d.cpp'),'-o',str(binary)],check=True)
  packets=[(i,d)for i,d in enumerate(rows[0]['draws'])if d['object']==2];values=[]
  for i,d in packets:
   raw=bytes.fromhex(d['vertices_hex']);values.append(d['matrix']+[x for off in(0,32,64)for x in struct.unpack_from('<3f',raw,off)])
  q=subprocess.run([str(binary)],input='\n'.join(' '.join(map(str,v))for v in values),capture_output=True,text=True,check=True)
  for (i,d),current in zip(packets,map(int,q.stdout.split())):
   guards=[g for g in rows[0]['guards']if g['draw']==i];assert guards
   accepted=not any(g['reject']for g in guards);assert accepted==bool(current)
   admission.append(dict(draw=i,native=accepted,source=bool(current)))
  assert len(admission)==20 and any(not a['native']for a in admission)
 report=dict(admission=admission,status='pass' ,tick=tick,viewport=[width,height],cases=rows,probe_sha256=sha(Path(__file__).read_bytes()),scope='Only actual original .rdata span constants changed in second branch; native state, RNG, draw vertices/colors/matrices identical. Widened rendering is a negative control, not original appearance.')
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print('pass',[(r['name'],r['pixels'])for r in rows],len(admission),'triangle admissions')
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('executable',type=Path);p.add_argument('--output',type=Path,required=True);p.add_argument('--tick',type=int,default=111);p.add_argument('--width',type=int,default=640);p.add_argument('--height',type=int,default=480);a=p.parse_args();run(a.executable,a.output,a.tick,a.width,a.height)
