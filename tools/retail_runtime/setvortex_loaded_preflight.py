"""Genuine loaded SetVortex persistent null-spell owner and complete renderer."""
import argparse,json,struct
from pathlib import Path
from character_native_preflight import AuthenticCharacters,ROOT,sha
from dragonattack_visual_reference import DragonAttackScene
from lit_software_probe import LitSoftwareFixture
from software_probe import save_rgb565_png
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX,UC_X86_REG_ESP

class SetVortexScene(AuthenticCharacters):
 observe_rng=DragonAttackScene.observe_rng
 observe_render_state=DragonAttackScene.observe_render_state
 observe_object=DragonAttackScene.observe_object
 observe_draw=DragonAttackScene.observe_draw
 draw_bounds=DragonAttackScene.draw_bounds
 def __init__(self,executable,boot_image):
  name="setvortex"
  super().__init__(executable,boot_image);self.select_screen_prefix()
  self.name=name;self.owner=self.new_object(name,(0,0,0),25,0x5a9f9c)
  v,call=self.vm,self.world.call;self.width,self.height,self.center=1280,960,(640,480)
  self.software=LitSoftwareFixture(executable,self.width,self.height,camera=(0,0,0),vm=v)
  self.software.lighting((38,38,38),(1,1,1));self.material_state=0;self.material_calls=[]
  original=self.world.ddraw._device_method
  def device(interface,method,args):
   if method in('GetLightState','SetLightState'):
    assert args[1]==1
    if method=='GetLightState':v.put_u32(args[2],self.material_state)
    else:self.material_state=args[2]
    self.material_calls.append((method,self.material_state));return 0
   return original(interface,method,args)
  self.world.ddraw._device_method=device
  call(0x58c575,(1,));self.thread_data=call(0x58fbf1);self.rng_draws=0
  v.uc.hook_add(UC_HOOK_CODE,self.observe_rng,begin=0x58c582,end=0x58c582)
  assert call(0x46e8b0,this=self.owner)==1
  self.animator=v.u32(self.owner+0x58);self.imagery=v.u32(self.owner+0x54)
  assert v.u32(self.animator)==0x5aa19c
  self.animobjs=[v.u32(v.u32(self.animator+0x54)+4*i)for i in range(v.u32(self.animator+0x44))]
  self.render_states={};self.draws=[];self.points=[];self.current_object=-1;self.raster_enabled=True;self.lookups=[]
  for address,observer in((0x40a8f0,self.observe_object),(0x56eb30,self.observe_draw),(0x56d400,self.observe_render_state)):
   v.uc.hook_add(UC_HOOK_CODE,observer,begin=address,end=address)
 def state(self):
  v=self.vm
  return dict(flags=v.u32(self.owner+8),position=list(struct.unpack('<3i',v.uc.mem_read(self.owner+0x10,12))),frame=struct.unpack('<h',v.uc.mem_read(self.owner+0x5c,2))[0],state=struct.unpack('<h',v.uc.mem_read(self.owner+0xc,2))[0],animator_frame=v.u32(self.animator+0x14),seed=v.u32(self.thread_data+0x14),rng_draws=self.rng_draws,objects=len(self.animobjs),matrices=[list(struct.unpack('<16f',v.uc.mem_read(x+0x58,64)))for x in self.animobjs])
 def render(self):
  s,v,call=self.software,self.vm,self.world.call;s.clear();self.draws=[];self.points=[]
  if not v.u32(self.owner+8)&4096:
   call(0x4e6890,(0,),this=self.animator);call(0x416f70,this=0x65a57c);call(0x40e740,this=self.animator)
   call(0x54de20,(s.screen.address,s.depth.address),this=s.camera);call(0x54dd70,(0,self.width-1,0,self.height-1,self.width),this=s.camera)
   v.put_u32(0x675fc0,self.center[0]);v.put_u32(0x675fc4,self.center[1])
   call(0x4e6920,this=self.animator,instruction_limit=100000000);call(0x40eb00,this=self.animator);call(0x416fb0,this=0x65a57c)
  return v.surface_bytes('screen')

def run(executable,boot_image,output,frames=105):
 output.mkdir(parents=True,exist_ok=False);f=SetVortexScene(executable,boot_image);v,call=f.vm,f.world.call;rows=[]
 for tick in range(1,frames+1):
  v.put_u32(0x65cb38,tick);call(0x470cc0,this=f.owner);call(0x4e67d0,this=f.owner)
  f.raster_enabled=tick in range(1,106,4);pixels=f.render()
  row=dict(tick=tick,state=f.state(),direction=struct.unpack('<i',v.uc.mem_read(f.animator+0xfc,4))[0],height=struct.unpack('<f',v.uc.mem_read(f.animator+0x100,4))[0],cached=list(struct.unpack('<3f',v.uc.mem_read(f.animator+0x78,12))),draws=f.draws,bounds=f.draw_bounds())
  if f.raster_enabled:
   name=f'native-{tick:03}.png';save_rgb565_png(output/name,pixels,f.width,f.height);row.update(image=name,sha256=sha(output/name),nonzero_pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',pixels)))
  rows.append(row)
 report=dict(accepted=False,type_id='0xad92bc15',records=rows,executable_sha256=sha(executable),boot_image_sha256=sha(boot_image),probe_sha256=sha(Path(__file__)),scope='Genuine loaded null-spell SetVortex owner, actual custom Animate/full Render, Begin/Pre/Post/End and explicit RAM1280x960 camera0/source0 ambient38dir1. No owner/texture/triangle substitutions. Unselected final raster omitted only.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print('done',len(rows),[(x['tick'],x.get('nonzero_pixels'))for x in rows if'image'in x]);return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--executable',type=Path,default=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe');p.add_argument('--boot-image',type=Path,required=True);p.add_argument('--output',type=Path,required=True);p.add_argument('--frames',type=int,default=105);a=p.parse_args();run(a.executable,a.boot_image,a.output,a.frames)
