"""Genuine named Quicksand caller and full original target-bound renderer."""
import argparse,json,struct
from pathlib import Path
from character_native_preflight import AuthenticCharacters,ROOT,sha
from dragonattack_visual_reference import DragonAttackScene
from lit_software_probe import LitSoftwareFixture
from software_probe import save_rgb565_png
from unicorn import UC_HOOK_CODE

class QuicksandScene(AuthenticCharacters):
 observe_rng=DragonAttackScene.observe_rng
 observe_render_state=DragonAttackScene.observe_render_state
 observe_object=DragonAttackScene.observe_object
 observe_draw=DragonAttackScene.observe_draw
 draw_bounds=DragonAttackScene.draw_bounds
 def __init__(self,executable,boot_image,null_spell=False):
  super().__init__(executable,boot_image);self.select_screen_prefix()
  v,call=self.vm,self.world.call;self.width,self.height,self.center=1280,960,(640,480)
  self.software=LitSoftwareFixture(executable,self.width,self.height,camera=(3185,3185,0),vm=v)
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
  self.invoker=self.new_character('Acolyte',(3185,3185,16));self.target=self.new_character('Acolyte',(3281,3185,16))
  call(0x4d3b90,(self.target,3),this=self.invoker)
  assert call(0x53ead0,this=0x667c38,instruction_limit=100000000)==1
  selected=[]
  for i in range(v.u32(0x667c3c)):
   sd=v.u32(v.u32(0x667c4c)+4*i)
   for j in range(v.u32(sd)):
    variant=v.u32(v.u32(sd+0x10)+4*j)
    if v.string(variant+4)=='Quicksand':selected.append((sd,variant))
  assert len(selected)==1
  targets,source=v.allocate(4),v.allocate(12);v.put_u32(targets,self.target);v.write(source,struct.pack('<3i',3185,3185,16))
  self.spell=call(0x542200,(self.invoker,targets,1,source,*selected[0],0),this=0x670630)
  self.owner=self.new_object('Quicksand',(3185,3185,16),25,0x5ab4c4)
  if not null_spell:v.put_u32(self.owner+0xd8,self.spell) # Exact inline public SetSpell binding.
  assert call(0x46e8b0,this=self.owner)==1
  self.animator=v.u32(self.owner+0x58);self.imagery=v.u32(self.owner+0x54)
  assert v.u32(self.animator)==0x5ab6c8
  self.animobjs=[v.u32(v.u32(self.animator+0x54)+4*i)for i in range(v.u32(self.animator+0x44))]
  self.render_states={};self.draws=[];self.points=[];self.current_object=-1;self.raster_enabled=True
  for address,observer in((0x40a8f0,self.observe_object),(0x56eb30,self.observe_draw),(0x56d400,self.observe_render_state)):
   v.uc.hook_add(UC_HOOK_CODE,observer,begin=address,end=address)
 def state(self):
  v=self.vm
  return dict(flags=v.u32(self.owner+8),position=list(struct.unpack('<3i',v.uc.mem_read(self.owner+0x10,12))),frame=struct.unpack('<h',v.uc.mem_read(self.owner+0x5c,2))[0],state=struct.unpack('<h',v.uc.mem_read(self.owner+0xc,2))[0],animator_frame=v.u32(self.animator+0xfc),target_count=v.u32(self.owner+0x1b8),target_position=list(struct.unpack('<3i',v.uc.mem_read(self.owner+0x1bc,12))),target=self.dump(self.target),seed=v.u32(self.thread_data+0x14),rng_draws=self.rng_draws,objects=len(self.animobjs))
 def render(self):
  s,v,call=self.software,self.vm,self.world.call;s.clear();self.draws=[];self.points=[]
  if not v.u32(self.owner+8)&4096:
   call(0x40e460,(0,),this=self.animator);call(0x416f70,this=0x65a57c);call(0x40e740,this=self.animator)
   call(0x54de20,(s.screen.address,s.depth.address),this=s.camera);call(0x54dd70,(0,self.width-1,0,self.height-1,self.width),this=s.camera)
   v.put_u32(0x675fc0,self.center[0]);v.put_u32(0x675fc4,self.center[1])
   call(0x4edb50,this=self.animator,instruction_limit=100000000);call(0x40eb00,this=self.animator);call(0x416fb0,this=0x65a57c)
  return v.surface_bytes('screen')

def run(executable,boot_image,output,null_spell=False,samples=(1,5,10,15,20,25,30,35,40,45,50,55,56,60)):
 output.mkdir(parents=True,exist_ok=False);f=QuicksandScene(executable,boot_image,null_spell);v,call=f.vm,f.world.call;rows=[]
 for tick in range(1,61):
  v.put_u32(0x65cb38,tick)
  if not v.u32(f.owner+8)&4096:call(0x470cc0,this=f.owner);call(0x4ed3d0,this=f.owner)
  f.raster_enabled=tick in samples;pixels=f.render();row=dict(tick=tick,state=f.state(),draws=f.draws,draw_bounds=f.draw_bounds())
  if f.raster_enabled:
   name=f'native-{tick:03}.png';save_rgb565_png(output/name,pixels,f.width,f.height);row.update(image=name,sha256=sha(output/name),nonzero_pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',pixels)))
  rows.append(row);print(tick,len(f.draws),row.get('nonzero_pixels'),flush=True)
 result=dict(effect='Quicksand',type_id='0xbabeface',null_spell=null_spell,records=rows,executable_sha256=sha(executable),boot_image_sha256=sha(boot_image),scope='Genuine Acolyte invoker/Fighting target offset96, actual named Quicksand SpellList/creator and inline SetSpell. Actual typed NewObject and loaded old-body resource/texture COM, default CreateAnimator, NextFrame/Pulse/Animate and BeginScene/PreRender/Render/PostRender/EndScene. Declared empty-map screen prefix and RAM target/camera3185,3185,0, source3185,3185,16, ambient38/dir1; material LightState1 boundary only. No forced atlas timing, scale, culling or synthetic resource. Natural map/device/caller remains open.')
 (output/'manifest.json').write_text(json.dumps(result,indent=2)+'\n');return result
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--executable',type=Path,default=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe');p.add_argument('--boot-image',type=Path,required=True);p.add_argument('--output',type=Path,required=True);p.add_argument('--null-spell',action='store_true');p.add_argument('--samples',default='1,5,10,15,20,25,30,35,40,45,50,55,56,60');a=p.parse_args();run(a.executable,a.boot_image,a.output,a.null_spell,tuple(map(int,a.samples.split(','))))
