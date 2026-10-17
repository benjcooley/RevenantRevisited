"""Exact Fire public constructor and complete default renderer in original HUD VM."""
import argparse,json,struct
from pathlib import Path
from character_native_preflight import AuthenticCharacters,ROOT,sha
from dragonattack_visual_reference import DragonAttackScene
from lit_software_probe import LitSoftwareFixture
from software_probe import save_rgb565_png
from unicorn import UC_HOOK_CODE

class FireScene(AuthenticCharacters):
 observe_rng=DragonAttackScene.observe_rng
 observe_render_state=DragonAttackScene.observe_render_state
 observe_object=DragonAttackScene.observe_object
 observe_draw=DragonAttackScene.observe_draw
 render=DragonAttackScene.render
 draw_bounds=DragonAttackScene.draw_bounds
 def __init__(self,executable,boot_image):
  super().__init__(executable,boot_image);self.select_screen_prefix()
  self.owner=self.new_object('Fire',(3185,3185,16),25,0x5a85ac)
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
  assert call(0x46e8b0,this=self.owner)==1
  self.animator=v.u32(self.owner+0x58);self.imagery=v.u32(self.owner+0x54)
  assert v.u32(self.animator)==0x5a370c
  self.animobjs=[v.u32(v.u32(self.animator+0x54)+4*i)for i in range(v.u32(self.animator+0x44))]
  self.render_states={};self.draws=[];self.points=[];self.current_object=-1;self.raster_enabled=True
  for address,observer in((0x40a8f0,self.observe_object),(0x56eb30,self.observe_draw),(0x56d400,self.observe_render_state)):
   v.uc.hook_add(UC_HOOK_CODE,observer,begin=address,end=address)
 def state(self):
  v=self.vm
  return dict(flags=v.u32(self.owner+8),position=list(struct.unpack('<3i',v.uc.mem_read(self.owner+0x10,12))),frame=struct.unpack('<h',v.uc.mem_read(self.owner+0x5c,2))[0],state=struct.unpack('<h',v.uc.mem_read(self.owner+0xc,2))[0],animator_frame=v.u32(self.animator+0x14),texture_frame=v.u32(v.u32(v.u32(self.imagery+0x5c))+0x80),seed=v.u32(self.thread_data+0x14),rng_draws=self.rng_draws,objects=len(self.animobjs),matrices=[list(struct.unpack('<16f',v.uc.mem_read(x+0x58,64)))for x in self.animobjs])

def run(executable,boot_image,output):
 output.mkdir(parents=True,exist_ok=False);f=FireScene(executable,boot_image);v,call=f.vm,f.world.call;rows=[]
 for tick in range(67):
  call(0x470cc0,this=f.owner);call(0x4de800,this=f.owner);v.put_u32(0x65cb38,tick)
  f.raster_enabled=tick in(0,1,2,16,32,33,66);pixels=f.render();row=dict(tick=tick,state=f.state(),draws=f.draws,draw_bounds=f.draw_bounds())
  if f.raster_enabled:
   name=f'native-{tick:03}.png';save_rgb565_png(output/name,pixels,f.width,f.height);row.update(image=name,sha256=sha(output/name),nonzero_pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',pixels)))
  rows.append(row);print(tick,len(f.draws),row.get('nonzero_pixels'),flush=True)
 result=dict(effect='Fire',type_id='0x119f01e3',records=rows,executable_sha256=sha(executable),boot_image_sha256=sha(boot_image),scope='Actual typed NewObject and loaded old-body resource/texture COM, default CreateAnimator, NextFrame/Pulse/Animate and BeginScene/PreRender/Render/PostRender/EndScene. Declared empty-map screen prefix and RAM target/camera3185,3185,0, source3185,3185,16, ambient38/dir1; material LightState1 boundary only. No forced atlas timing, scale, culling or synthetic resource. Natural map/device/caller remains open.')
 (output/'manifest.json').write_text(json.dumps(result,indent=2)+'\n');return result
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--executable',type=Path,default=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe');p.add_argument('--boot-image',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.executable,a.boot_image,a.output)
