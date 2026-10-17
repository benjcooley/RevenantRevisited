"""Createfood genuine loaded owner, complete renderer and adjacent color-table observations."""
import argparse,json,struct,hashlib
from pathlib import Path
from character_native_preflight import AuthenticCharacters,ROOT,sha
from dragonattack_visual_reference import DragonAttackScene
from lit_software_probe import LitSoftwareFixture
from software_probe import save_rgb565_png
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP

class CreatefoodLoadedScene(AuthenticCharacters):
 def observe_rng(self,uc,*unused):
  self.rng_draws+=1
  if getattr(self,'tick',0)==60:
   sp=uc.reg_read(UC_X86_REG_ESP);self.raw_calls.append(hex(self.vm.u32(sp)))
 observe_render_state=DragonAttackScene.observe_render_state
 observe_object=DragonAttackScene.observe_object
 observe_draw=DragonAttackScene.observe_draw
 draw_bounds=DragonAttackScene.draw_bounds
 def __init__(self,executable,boot_image,initialize_lighting=False):
  super().__init__(executable,boot_image);self.select_screen_prefix()
  self.owner=self.new_object('Createfood',(3185,3185,16),25,0x5a8b08)
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
  assert v.u32(self.animator)==0x5a8aa4
  self.animobjs=[v.u32(v.u32(self.animator+0x54)+4*i)for i in range(v.u32(self.animator+0x44))]
  self.render_states={};self.draws=[];self.points=[];self.current_object=-1;self.raster_enabled=True
  for address,observer in((0x40a8f0,self.observe_object),(0x56eb30,self.observe_draw),(0x56d400,self.observe_render_state)):
   v.uc.hook_add(UC_HOOK_CODE,observer,begin=address,end=address)
  self.tick=0;self.index_events=[];self.raw_calls=[];self.random_requests=[];self.vectors=[];self.food=[];self.lights=[];self.rendering=False
  for addr,observer in((0x483300,self.random_request),(0x46db20,self.vector),(0x450e40,self.food_request),(0x41da10,self.light)):
   v.uc.hook_add(UC_HOOK_CODE,observer,begin=addr,end=addr)
  for addr in (0x44ce30,0x44ce50,0x44ce60,0x44ce6d,0x44cea5):
   v.uc.hook_add(UC_HOOK_CODE,self.index_event,begin=addr,end=addr)
  if initialize_lighting:
   call(0x41dff0,instruction_limit=200000000)
   white=v.allocate(4);v.write(white,bytes([255,255,255,0]));call(0x41da10,(0,white,28),instruction_limit=100000000)
   call(0x41d730,(32,),instruction_limit=100000000)
 def index_event(self,uc,address,*unused):
  from unicorn.x86_const import UC_X86_REG_EAX,UC_X86_REG_ESI
  self.index_events.append(dict(tick=self.tick,address=hex(address),eax=uc.reg_read(UC_X86_REG_EAX),esi=uc.reg_read(UC_X86_REG_ESI),top=self.vm.u32(uc.reg_read(UC_X86_REG_ESP)),milliseconds=self.vm.milliseconds,seed=self.vm.u32(self.thread_data+0x14)))
 def random_request(self,uc,*unused):
  v=self.vm;sp=uc.reg_read(UC_X86_REG_ESP);self.random_requests.append(dict(tick=self.tick,caller=hex(v.u32(sp)),lo=struct.unpack('<i',v.uc.mem_read(sp+4,4))[0],hi=struct.unpack('<i',v.uc.mem_read(sp+8,4))[0]))
 def vector(self,uc,*unused):
  v=self.vm;sp=uc.reg_read(UC_X86_REG_ESP);angle,distance=struct.unpack('<2i',v.uc.mem_read(sp+4,8));yi=angle-128 if angle>128 else 128-angle;addresses=[0x634d44+angle*2,0x634f44+yi*2]
  self.vectors.append(dict(tick=self.tick,rendering=self.rendering,angle=angle,distance=distance,addresses=addresses,words=[struct.unpack('<h',v.uc.mem_read(a,2))[0]for a in addresses]))
 def light(self,uc,*unused):
  v=self.vm;sp=uc.reg_read(UC_X86_REG_ESP);self.lights.append(dict(tick=self.tick,args=[v.u32(sp+4+i*4)for i in range(3)]))
 def food_request(self,uc,*unused):
  v=self.vm;sp=uc.reg_read(UC_X86_REG_ESP);self.food.append(dict(tick=self.tick,definition=bytes(v.uc.mem_read(v.u32(sp+4),52)).hex()))
 def state(self):
  v=self.vm;a=self.animator;n=v.u32(a+0x3034)
  return dict(flags=v.u32(self.owner+8),quality=v.u32(0x5d79e4),owner_counter=v.u32(self.owner+0x188),life=v.u32(a+0x3044),scale=struct.unpack('<f',v.uc.mem_read(a+0x150,4))[0],capacity=n,pool_hex=bytes(v.uc.mem_read(a+0x154,n*0x50)).hex(),seed=v.u32(self.thread_data+0x14),rng_draws=self.rng_draws,mmx_sha256=hashlib.sha256(bytes(v.uc.mem_read(0x635144,1024))).hexdigest(),mmx_ambient=v.u32(0x635544),light_colors=bytes(v.uc.mem_read(0x63da50,16)).hex(),lookup_hex=bytes(v.uc.mem_read(0x634d44,2048)).hex())
 def render(self):
  self.draws,self.points=[],[];s,v,call=self.software,self.vm,self.world.call;s.clear()
  if not v.u32(self.owner+8)&4096:
   call(0x40e460,(0,),this=self.animator);call(0x416f70,this=0x65a57c);call(0x40e740,this=self.animator)
   call(0x54de20,(s.screen.address,s.depth.address),this=s.camera);call(0x54dd70,(0,self.width-1,0,self.height-1,self.width),this=s.camera);v.put_u32(0x675fc0,self.center[0]);v.put_u32(0x675fc4,self.center[1])
   self.rendering=True;call(0x4e0650,this=self.animator,instruction_limit=100000000);self.rendering=False;call(0x40eb00,this=self.animator);call(0x416fb0,this=0x65a57c)
  return v.surface_bytes('screen')
def run(executable,boot_image,output,frames=85,samples=(1,24,40,60,79,80,85),initialize_lighting=False):
 output.mkdir(parents=True,exist_ok=False);f=CreatefoodLoadedScene(executable,boot_image,initialize_lighting);v,call=f.vm,f.world.call;rows=[];failure=None;initial=f.state()
 for tick in range(1,frames+1):
  f.tick=tick;v.put_u32(0x65cb38,tick)
  try:
   if not v.u32(f.owner+8)&4096:call(0x470cc0,this=f.owner);call(0x4dfe30,this=f.owner)
   f.raster_enabled=tick in samples;pixels=f.render();r=dict(tick=tick,state=f.state(),draws=f.draws,bounds=f.draw_bounds())
   if f.raster_enabled:
    name=f'native-{tick:03}.png';save_rgb565_png(output/name,pixels,f.width,f.height);r.update(image=name,sha256=sha(output/name),nonzero_pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',pixels)))
   rows.append(r);print(tick,len(f.draws),r.get('nonzero_pixels'),flush=True)
  except RuntimeError as error:failure=dict(tick=tick,error=str(error),fault=f.world.last_fault);break
 report=dict(initialized_lighting=initialize_lighting,accepted=False,type_id='0x838cffba',effect='Createfood',initial=initial,records=rows,failure=failure,vectors=f.vectors,random_requests=f.random_requests,raw_calls_tick60=f.raw_calls,index_events=f.index_events,lights=f.lights,food_requests=f.food,executable_sha256=sha(executable),boot_image_sha256=sha(boot_image),probe_sha256=sha(Path(__file__)),scope='Genuine cached loaded owner/imagery/COM, whole owner Pulse/default Animate/full custom Render; actual screen prefix, Begin/PreRender/Post/End and fixed RAM1280x960 center640480 camera3185,3185,0 source3185,3185,16 ambient38/dir1. Original cached MMX/color table and original SetLightColor calls are observed, never replaced; optional declared lighting executes complete MakeColorTables, public SetLightColor(0,white,28) then SetAmbientLight32 with no fabricated display; original unwrapped ConvertToVector reads are recorded. Empty map FOOD request not substituted. Nonsample final raster may be omitted; full caller executes. No visual acceptance, actual map residency/device/audio remain open.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print('DONE',failure);return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--executable',type=Path,default=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe');p.add_argument('--boot-image',type=Path,required=True);p.add_argument('--output',type=Path,required=True);p.add_argument('--frames',type=int,default=85);p.add_argument('--samples',default='1,24,40,60,79,80,85');p.add_argument('--initialize-lighting',action='store_true');a=p.parse_args();run(a.executable,a.boot_image,a.output,a.frames,tuple(map(int,a.samples.split(','))),a.initialize_lighting)
