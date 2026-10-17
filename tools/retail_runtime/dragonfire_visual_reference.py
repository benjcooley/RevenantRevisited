"""Exact DragonFire null-spell owner, two original pools and complete software Render.

Original loaded class/imagery registries, constructor, Init, owner Pulse, animator
Animate and Render execute unchanged. Declared scene output/lighting is isolated;
no actor, resource, particle or raster handler is replaced.
"""
import argparse,json,struct,zipfile,hashlib
from pathlib import Path
from character_native_preflight import AuthenticCharacters,ROOT,sha
from tornado_target_visual_reference import TornadoTargetScene
from lit_software_probe import LitSoftwareFixture
from firecone_owner_probe import native_owner
from software_probe import save_rgb565_png
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EAX
SOURCE=(3185,3185,16)
class DragonFireScene:
 def __init__(self,exe,boot,width=1280,height=960):
  self.actors=AuthenticCharacters(exe,boot);self.actors.select_screen_prefix();self.world=self.actors.world;self.vm=v=self.actors.vm;call=self.world.call
  self.owner=self.actors.new_object('dragonfire',SOURCE,25,0x5aad24)
  assert v.u32(self.owner+0xe4)==0
  self.rng_calls=0;self.random=[];self.pending=[]
  v.uc.hook_add(UC_HOOK_CODE,self.rand_entry,begin=0x58c582,end=0x58c582)
  v.uc.hook_add(UC_HOOK_CODE,self.range_entry,begin=0x483300,end=0x483300)
  v.uc.hook_add(UC_HOOK_CODE,self.range_exit,begin=0x48332c,end=0x48332c)
  call(0x58c575,(1,));self.tls=call(0x58fbf1)
  assert call(0x46e8b0,this=self.owner,instruction_limit=100000000)==1
  self.animator=v.u32(self.owner+0x58);assert v.u32(self.animator)==0x5aaf28
  assert all(v.u32(self.animator+off+0x1c)==300 for off in (0xfc,0x124))
  self.material=0
  for name,args in self.world.ddraw.device_calls:
   if name=='IDirect3DDevice3::SetLightState'and args[0]==1:self.material=args[1]
  original=self.world.ddraw._device_method
  def device(interface,method,args):
   if interface=='IDirect3DDevice3'and method in('GetLightState','SetLightState')and args[1]==1:
    if method=='GetLightState':v.put_u32(args[2],self.material)
    else:self.material=args[2]
    return 0
   return original(interface,method,args)
  self.world.ddraw._device_method=device
  self.software=LitSoftwareFixture(exe,width,height,camera=(*SOURCE[:2],0),vm=v);self.software.lighting((38,)*3,(1,)*3)
  v.put_u32(0x675fc0,width//2);v.put_u32(0x675fc4,height*2//3)
  self.draws=[];self.states={};self.current_object=-1
  for address,method in[(0x56eb30,self.observe_draw),(0x56d400,self.observe_state),(0x40a8f0,self.observe_object)]:v.uc.hook_add(UC_HOOK_CODE,method,begin=address,end=address)
  self.initial=self.state()
 observe_draw=TornadoTargetScene.observe_draw
 observe_state=TornadoTargetScene.observe_state
 observe_object=TornadoTargetScene.observe_object
 def rand_entry(self,*_):self.rng_calls+=1
 def range_entry(self,uc,*_):
  sp=uc.reg_read(UC_X86_REG_ESP);self.pending.append(struct.unpack('<2i',uc.mem_read(sp+4,8)))
 def range_exit(self,uc,*_):self.random.append((*self.pending.pop(),struct.unpack('<i',struct.pack('<I',uc.reg_read(UC_X86_REG_EAX)))[0]))
 def state(self):
  v=self.vm
  return dict(frame=v.u32(self.animator+0x150),done=v.u32(self.animator+0x14c),flags=v.u32(self.owner+8),seed=v.u32(self.tls+0x14),rng_calls=self.rng_calls,pools={name:bytes(v.uc.mem_read(v.u32(self.animator+off+4),300*88)).hex()for name,off in [('fire',0xfc),('smoke',0x124)]})
 def step(self):
  if self.vm.u32(self.owner+8)&0x1000:return
  call=self.world.call;call(0x470cc0,this=self.owner);call(0x4eabf0,this=self.owner)
  if not self.vm.u32(self.owner+8)&0x1000:call(0x40e460,(0,),this=self.animator)
 def render(self):
  self.draws=[];v=self.vm;s=self.software;s.clear()
  if v.u32(self.owner+8)&0x1000:return v.surface_bytes('screen')
  native_owner(self);call=self.world.call;call(0x416f70,this=0x65a57c)
  v.call(0x54de20,(s.screen.address,s.depth.address),this=s.camera);v.call(0x54dd70,(0,s.width-1,0,s.height-1,s.width),this=s.camera)
  before=self.rng_calls;call(0x4eb130,this=self.animator,instruction_limit=100000000);call(0x416fb0,this=0x65a57c)
  assert self.rng_calls==before
  return v.surface_bytes('screen')
def run(exe,boot,out,frames):
 out.mkdir(parents=True,exist_ok=False);f=DragonFireScene(exe,boot);rows=[]
 for tick in range(frames+1):
  if tick:f.step()
  raw=f.render();row=dict(tick=tick,state=f.state(),draws=f.draws)
  if tick in (0,1,2,5,10,20,30,34,40,50,60,70,80,90,100)or tick==frames:
   p=out/f'native-{tick:03}.png';save_rgb565_png(p,raw,1280,960);row.update(image=str(p),image_sha256=sha(p),pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',raw)))
  rows.append(row)
  with(out/'trace.jsonl').open('a')as h:h.write(json.dumps(row)+'\n')
  if tick%10==0 or'image'in row:print(tick,row['state']['frame'],hex(row['state']['flags']),len(f.draws),row.get('pixels'),f.rng_calls,flush=True)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:asset=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/firecone.i3d'))
 report=dict(status='complete_native_reference',accepted=False,visual_pass=False,type_id='0xad92bc1a',scope=__doc__,source_sha256=sha(__file__),executable_sha256=sha(exe),boot_image_sha256=sha(boot),asset_sha256=hashlib.sha256(asset).hexdigest(),initial=f.initial,records=rows,random=f.random,viewport=[1280,960],center=[640,640],source=SOURCE,camera=[3185,3185,0],lighting=dict(ambient=[38]*3,directional=[1]*3))
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--boot-image',type=Path,required=True);p.add_argument('--output',type=Path,required=True);p.add_argument('--frames',type=int,default=100);a=p.parse_args();run(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',a.boot_image,a.output,a.frames)
