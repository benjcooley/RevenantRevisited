"""Genuine loaded Vortex default owner at the declared gallery station/camera.

No replacement factory, imagery, key provider, animation or raster routines.
The software target and material-handle COM interface are declared fixtures.
"""
import argparse,json,struct,zipfile
from pathlib import Path
from character_native_preflight import AuthenticCharacters,ROOT,sha
from blood_actor_preflight import BOOT
from lit_software_probe import LitSoftwareFixture
from software_probe import save_rgb565_png
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP

def run(output,ticks=2,reset_cull=False):
 output.mkdir(parents=True,exist_ok=False);exe=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe'
 f=AuthenticCharacters(exe,BOOT);f.select_screen_prefix();v=f.vm;call=f.world.call
 # One-entry review schedule: fixed source and camera XY3185; the map projector
 # explicitly uses absolute worldZ, so declared source cameraZ is0.
 origin=(3185,3185,16);camera=(3185,3185,0)
 s=LitSoftwareFixture(exe,640,480,camera=camera,vm=v);s.lighting((38,)*3,(1,)*3)
 states={}
 def state_entry(uc,*_):
  sp=uc.reg_read(UC_X86_REG_ESP);states[v.u32(sp+4)]=v.u32(sp+8)
 v.uc.hook_add(UC_HOOK_CODE,state_entry,begin=0x56d400,end=0x56d400)
 material=[0]
 for name,args in f.world.ddraw.device_calls:
  if name=='IDirect3DDevice3::SetLightState'and args[0]==1:material[0]=args[1]
 original=f.world.ddraw._device_method
 def device(interface,method,args):
  if interface=='IDirect3DDevice3'and method in('GetLightState','SetLightState')and args[1]==1:
   if method=='GetLightState':v.put_u32(args[2],material[0])
   else:material[0]=args[2]
   return 0
  return original(interface,method,args)
 f.world.ddraw._device_method=device
 owner=f.new_object('Vortex',origin,25,0x5a85ac);assert call(0x46e8b0,this=owner,instruction_limit=100000000)==1
 animator=v.u32(owner+0x58);assert v.u32(animator)==0x5a370c;assert v.u32(animator+0x44)==3 and v.u32(animator+0x58)==0
 rows=[];draws=[];current=[-1];objects=[v.u32(v.u32(animator+0x54)+4*i)for i in range(3)]
 def object_entry(uc,*_):current[0]=objects.index(v.u32(uc.reg_read(UC_X86_REG_ESP)+4))
 def draw(uc,*_):
  sp=uc.reg_read(UC_X86_REG_ESP);a=list(struct.unpack('<7I',uc.mem_read(sp+4,28)));assert a[1]==0x112
  draws.append(dict(states=dict(states),material_handle=material[0],object=current[0],fvf=a[1],vertex_count=a[3],index_count=a[5],vertices_hex=bytes(uc.mem_read(a[2],32*a[3])).hex(),indices_hex=bytes(uc.mem_read(a[4],2*a[5])).hex(),world=list(struct.unpack('<16f',uc.mem_read(0x675ed0,64)))))
 v.uc.hook_add(UC_HOOK_CODE,object_entry,begin=0x40a8f0,end=0x40a8f0);v.uc.hook_add(UC_HOOK_CODE,draw,begin=0x56eb30,end=0x56eb30)
 for tick in range(1,ticks+1):
  v.put_u32(0x65cb38,tick);call(0x470cc0,this=owner);call(0x4de800,this=owner);call(0x40e460,(0,),this=animator);s.clear();draws.clear();call(0x416f70,this=0x65a57c);call(0x40e740,this=animator)
  v.call(0x54de20,(s.screen.address,s.depth.address),this=s.camera);v.call(0x54dd70,(0,639,0,479,640),this=s.camera);v.put_u32(0x675fc0,320);v.put_u32(0x675fc4,240)
  if reset_cull:
   v.call(0x417d60,(0,1),this=0x65a57c)
   for key,value in ((7,1),(14,1),(22,2)):v.call(0x56d400,(key,value))
  call(0x40e8d0,this=animator,instruction_limit=100000000);call(0x40eb00,this=animator);call(0x416fb0,this=0x65a57c)
  raw=v.surface_bytes('screen');path=output/f'native-{tick:03d}.png';save_rgb565_png(path,raw,640,480)
  rows.append(dict(tick=tick,state=v.u32(owner+0xc)&0xffff,frame=struct.unpack('<h',v.uc.mem_read(owner+0x5c,2))[0],flags=v.u32(owner+8),draws=list(draws),image=str(path),image_sha256=sha(path),rgb565_sha256=__import__('hashlib').sha256(raw).hexdigest()))
  print(tick,len(draws),flush=True)
 assert all(r['state']==0 and r['frame']==0 and len(r['draws'])==3 for r in rows)
 with zipfile.ZipFile(ROOT/'data/resources.rvr')as z:spell=z.read('spell.def').decode('latin1')
 variants=[line.strip()for line in spell.splitlines()if line.lstrip().startswith('VARIANT')and 'vortex'in line.lower()]
 report=dict(status='pass',accepted=False,visual_pass=False,executable_sha256=sha(exe),boot_sha256=sha(BOOT),probe_sha256=sha(__file__),origin=origin,camera=camera,viewport=[640,480],initial_software_fixture_cull=1,reset_cull2_each_frame=reset_cull,distinct_framebuffers=len({r['rgb565_sha256']for r in rows}),normal_lighting={'ambient':[38]*3,'directional':[1]*3},owner_vtable=hex(v.u32(owner)),animator_vtable=hex(v.u32(animator)),objects=3,controllers=0,rows=rows,vortex_named_spell_variants=variants,boundaries=f.world.boundary_calls,scope=__doc__,limits='Declared exact loaded owner/camera/static poses. Initial CULL_NONE is a SoftwareFixture boundary, not proof of natural scene state. Optional perframe scene0/cull2 resets are an explicit negative control; SetTexture can change blending/cull on first bind only. Natural gameplay spell/map context remains separate. Native640screen may be shown nearest2x for the current map display scale, explicitly presentation only; no geometry/camera/pose fit.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,required=True);p.add_argument('--ticks',type=int,default=2);p.add_argument('--reset-cull',action='store_true');a=p.parse_args();print(run(a.output,a.ticks,a.reset_cull)['status'])
