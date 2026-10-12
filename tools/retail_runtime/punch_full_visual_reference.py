"""PunchAndJudy exact loaded owner and whole581-frame authored default renderer.

Every original owner Pulse/Animate/Render executes; selected full software images
are retained. No decoded-key, object matrix, material or geometry substitution.
"""
import argparse,json,struct,zipfile,hashlib
from pathlib import Path
from character_native_preflight import AuthenticCharacters,ROOT,sha
from tornado_target_visual_reference import TornadoTargetScene
from lit_software_probe import LitSoftwareFixture
from software_probe import save_rgb565_png
from unicorn import UC_HOOK_CODE
SOURCE=(3185,3185,16)
SAMPLES={0,1,20,50,100,150,200,250,300,350,400,450,500,550,579,580,581,582,600}
class PunchScene:
 def __init__(self,exe,boot):
  self.actors=AuthenticCharacters(exe,boot);self.actors.select_screen_prefix();self.world=self.actors.world;self.vm=v=self.actors.vm;call=self.world.call
  self.owner=self.actors.new_object('PunchAndJudy',SOURCE,25,0x5a85ac);assert v.u32(self.owner+0xe4)==0
  assert call(0x46e8b0,this=self.owner,instruction_limit=100000000)==1;self.animator=v.u32(self.owner+0x58);assert v.u32(self.animator)==0x5a370c
  self.rng_calls=0;v.uc.hook_add(UC_HOOK_CODE,self.rand_entry,begin=0x58c582,end=0x58c582);call(0x58c575,(1,));self.tls=call(0x58fbf1)
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
  s=self.software=LitSoftwareFixture(exe,1280,960,camera=(*SOURCE[:2],0),vm=v);s.lighting((38,)*3,(1,)*3);v.put_u32(0x675fc0,640);v.put_u32(0x675fc4,640)
  self.draws=[];self.states={};self.current_object=-1
  for address,method in[(0x56eb30,self.observe_draw),(0x56d400,self.observe_state),(0x40a8f0,self.observe_object)]:v.uc.hook_add(UC_HOOK_CODE,method,begin=address,end=address)
  call(0x40e460,(0,),this=self.animator)
 observe_draw=TornadoTargetScene.observe_draw
 observe_state=TornadoTargetScene.observe_state
 observe_object=TornadoTargetScene.observe_object
 def rand_entry(self,*_):self.rng_calls+=1
 def state(self):
  v=self.vm
  return dict(frame=struct.unpack('<h',v.uc.mem_read(self.owner+0x5c,2))[0],animator_frame=v.u32(self.animator+0x14),flags=v.u32(self.owner+8),seed=v.u32(self.tls+0x14),rng_calls=self.rng_calls,controller_count=v.u32(self.animator+0x58),previous_state=struct.unpack('<i',v.uc.mem_read(v.u32(self.owner+0x54)+0xa4,4))[0],previous_frame=v.u32(v.u32(self.owner+0x54)+0xa8))
 def step(self,tick):
  v=self.vm;call=self.world.call;v.put_u32(0x65cb38,tick);call(0x470cc0,this=self.owner);call(0x4de800,this=self.owner);call(0x40e460,(0,),this=self.animator)
  assert self.state()['frame']==tick%581 and not(v.u32(self.owner+8)&0x1000)
 def render(self):
  self.draws=[];v=self.vm;s=self.software;s.clear();call=self.world.call
  call(0x416f70,this=0x65a57c);call(0x40e740,this=self.animator)
  # Whole PreRender binds the real display again. Redirect only the declared
  # software output buffers/pitch after that authentic call.
  v.call(0x54de20,(s.screen.address,s.depth.address),this=s.camera);v.call(0x54dd70,(0,1279,0,959,1280),this=s.camera)
  call(0x417d60,(1,1),this=0x65a57c);call(0x40e8d0,this=self.animator,instruction_limit=100000000);call(0x40eb00,this=self.animator);call(0x416fb0,this=0x65a57c)
  return v.surface_bytes('screen')
def run(exe,boot,out,frames):
 out.mkdir(parents=True,exist_ok=False);f=PunchScene(exe,boot);rows=[]
 for tick in range(frames+1):
  if tick:f.step(tick)
  raw=f.render();row=dict(tick=tick,state=f.state(),draws=f.draws)
  assert row['state']['rng_calls']==0 and row['state']['controller_count']==0
  if tick in SAMPLES or tick==frames:
   p=out/f'native-{tick:03}.png';save_rgb565_png(p,raw,1280,960);row.update(image=str(p),image_sha256=sha(p),pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',raw)))
  rows.append(row)
  with(out/'trace.jsonl').open('a')as h:h.write(json.dumps(row)+'\n')
  if tick%25==0 or'image'in row:print(tick,row['state']['frame'],len(f.draws),row.get('pixels'),flush=True)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:asset=z.read(next(n for n in z.namelist()if n.lower()=='imagery/misc/punchandjudy.i3d'))
 report=dict(status='complete_native_reference',accepted=False,visual_pass=False,type_id='0x0c05263a',scope=__doc__,source_sha256=sha(__file__),executable_sha256=sha(exe),boot_image_sha256=sha(boot),asset_sha256=hashlib.sha256(asset).hexdigest(),records=rows,viewport=[1280,960],center=[640,640],source=SOURCE,camera=[3185,3185,0],lighting=dict(ambient=[38]*3,directional=[1]*3),scene_mode=1)
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--boot-image',type=Path,required=True);p.add_argument('--output',type=Path,required=True);p.add_argument('--frames',type=int,default=600);a=p.parse_args();run(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',a.boot_image,a.output,a.frames)
