"""Loaded Drip plus explicitly scheduled genuine standalone Ripple children.

Actual empty-map child allocation failures remain observed. The harness then
constructs a separate original child through its public class/setter interface;
this is a declared managed-child schedule, not natural sector residency.
"""
import argparse,json,struct
from pathlib import Path
from character_native_preflight import AuthenticCharacters,ROOT,sha
from dragonattack_visual_reference import DragonAttackScene
from lit_software_probe import LitSoftwareFixture
from software_probe import save_rgb565_png
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EAX

class DripLoadedScene(AuthenticCharacters):
 observe_rng=DragonAttackScene.observe_rng
 observe_render_state=DragonAttackScene.observe_render_state
 observe_object=DragonAttackScene.observe_object
 observe_draw=DragonAttackScene.observe_draw
 draw_bounds=DragonAttackScene.draw_bounds
 def __init__(self,executable,boot_image):
  super().__init__(executable,boot_image);self.select_screen_prefix();v,call=self.vm,self.world.call
  self.width,self.height,self.center=1280,960,(640,640)
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
  self.owner=self.new_object('Drip',(0,0,0),25,0x5ac630)
  call(v.u32(v.u32(self.owner)+0x200),(20,128,1),this=self.owner)
  assert call(0x46e8b0,this=self.owner)==1;self.animator=v.u32(self.owner+0x58);assert v.u32(self.animator)==0x5ac83c
  self.children=[];self.births=[];self.deaths=[];self.requests=[];self.map_returns=[];self.tick=0
  self.render_states={};self.draws=[];self.points=[];self.current_object=-1;self.raster_enabled=True
  for address,observer in((0x40a8f0,self.observe_object),(0x56eb30,self.observe_draw),(0x56d400,self.observe_render_state),(0x4f1140,self.observe_request),(0x4f11b0,self.observe_map_return)):
   v.uc.hook_add(UC_HOOK_CODE,observer,begin=address,end=address)
 def observe_request(self,uc,*args):
  sp=uc.reg_read(UC_X86_REG_ESP);self.requests.append(dict(tick=self.tick,arguments=list(struct.unpack('<4i',self.vm.uc.mem_read(sp+4,16)))))
 def observe_map_return(self,uc,*args):
  self.map_returns.append(dict(tick=self.tick,index=uc.reg_read(UC_X86_REG_EAX)));assert uc.reg_read(UC_X86_REG_EAX)==0xffffffff
 def step(self):
  self.tick+=1;v,call=self.vm,self.world.call;v.put_u32(0x65cb38,self.tick)
  for c in self.children:
   call(0x470cc0,this=c['owner']);call(0x4f0750,this=c['owner'])
   if v.u32(c['owner']+8)&4096:self.deaths.append(dict(tick=self.tick,id=c['id']))
  self.children=[c for c in self.children if not(v.u32(c['owner']+8)&4096)]
  if self.tick==121:call(v.u32(v.u32(self.owner)+0x200),(20,128,1000000),this=self.owner)
  n=len(self.requests);call(0x470cc0,this=self.owner);call(0x4de800,this=self.owner)
  for event in self.requests[n:]:
   assert event['arguments']==[0,0,0,20]
   owner=self.new_object('Ripple',(0,0,0),25,0x5ac3a4);call(v.u32(v.u32(owner)+0x204),(20,),this=owner)
   assert call(0x46e8b0,this=owner)==1;animator=v.u32(owner+0x58);assert v.u32(animator)==0x5ac5b0
   c=dict(id=len(self.births)+1,owner=owner,animator=animator);self.children.append(c);self.births.append(dict(tick=self.tick,id=c['id']))
 def state(self):
  v=self.vm;a=self.animator
  return dict(tick=self.tick,seed=v.u32(self.thread_data+0x14),rng_draws=self.rng_draws,parent=dict(dead=bool(v.u32(a+0x114)),time=v.u32(a+0x118),values=list(struct.unpack('<6f',v.uc.mem_read(a+0xfc,24)))),children=[dict(id=c['id'],frameon=v.u32(c['animator']+0x104),ripframe=v.u32(c['animator']+0x100),length=v.u32(c['owner']+0x184),splash_count=v.u32(c['animator']+0x108))for c in self.children])
 def render(self):
  v,call,s=self.vm,self.world.call,self.software;s.clear();self.draws=[];self.points=[]
  call(0x416f70,this=0x65a57c)
  for owner,animator,render in [(self.owner,self.animator,0x4f13c0)]+[(c['owner'],c['animator'],0x4f0c10)for c in self.children]:
   self.animobjs=[v.u32(v.u32(animator+0x54)+4*i)for i in range(v.u32(animator+0x44))]
   call(0x40e460,(0,),this=animator);call(0x40e740,this=animator)
   call(0x54de20,(s.screen.address,s.depth.address),this=s.camera);call(0x54dd70,(0,self.width-1,0,self.height-1,self.width),this=s.camera)
   v.put_u32(0x675fc0,self.center[0]);v.put_u32(0x675fc4,self.center[1]);call(render,this=animator,instruction_limit=100000000);call(0x40eb00,this=animator)
  call(0x416fb0,this=0x65a57c);return v.surface_bytes('screen')

def run(executable,boot_image,output):
 output.mkdir(parents=True,exist_ok=False);f=DripLoadedScene(executable,boot_image);rows=[];samples=sorted(set(range(1,145,6))|{23,24,25,42,44,45,48,49,120,121,140,141,144})
 for tick in range(1,145):
  f.step();before=f.state();f.raster_enabled=tick in samples;pixels=f.render();row=dict(tick=tick,before=before,after=f.state(),draw_bounds=f.draw_bounds(),draws=f.draws if f.raster_enabled else [])
  if f.raster_enabled:
   name=f'native-{tick:03}.png';save_rgb565_png(output/name,pixels,f.width,f.height);row.update(image=name,sha256=sha(output/name),nonzero_pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',pixels)))
  rows.append(row);print(tick,len(f.children),len(f.draws),row.get('nonzero_pixels'),flush=True)
 result=dict(effect='Drip',type_id='0x5975abde',parameters=[20,128,1],records=rows,births=f.births,deaths=f.deaths,requests=f.requests,map_returns=f.map_returns,samples=samples,executable_sha256=sha(executable),boot_image_sha256=sha(boot_image),scope='Actual loaded parent and standalone child class factories/public setters/custom initializers, full software scene/renderers. Empty-map allocation fails unchanged; explicit child scheduler constructs original Ripple after each request, newborn age0 renders then advances next tick. Length20 no-splash; period changes to1000000 before121 for tail. Every24Hz owner Pulse, Animate and Render executes, only final raster omitted off samples. Declared camera0/ambient38dir1/1280x960 center640,640. Natural sectors, audio/device/exactpixels remain open.')
 (output/'manifest.json').write_text(json.dumps(result,indent=2)+'\n');return result
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--executable',type=Path,default=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe');p.add_argument('--boot-image',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.executable,a.boot_image,a.output)
