"""StrikeEffect via the complete original named Meteor Storm Timer and map APIs.

Genuine Acolytes and fighting action, original SetMapPos/UpdateSectors create the
active empty sector window. No map allocator, object lookup, spell, effect, RNG,
controller, resource or raster routine is replaced. Actor simulation and natural
spell scheduler are outside this declared isolated caller sequence.
"""
import argparse,json,struct,hashlib
from pathlib import Path
from character_native_preflight import AuthenticCharacters,ROOT,SOURCE,TARGET,sha
from tornado_target_visual_reference import TornadoTargetScene
from lit_software_probe import LitSoftwareFixture
from software_probe import save_rgb565_png
from speed_controller_preflight import NativeSpeed
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_ECX
class StrikeScene:
 raster_enabled=True
 def __init__(self,exe,boot):
  self.actors=AuthenticCharacters(exe,boot);self.actors.select_screen_prefix();self.world=self.actors.world;self.vm=v=self.actors.vm;call=self.world.call
  self.invoker=self.actors.new_character('Acolyte',SOURCE);self.target=self.actors.new_character('Acolyte',TARGET);call(0x4d3b90,(self.target,3),this=self.invoker);self.before=[self.actors.dump(x)for x in(self.invoker,self.target)]
  assert call(0x53ead0,this=0x667c38,instruction_limit=100000000)==1
  matches=[]
  for i in range(v.u32(0x667c3c)):
   sd=v.u32(v.u32(0x667c4c)+4*i)
   for j in range(v.u32(sd)):
    variant=v.u32(v.u32(sd+0x10)+4*j)
    if v.string(variant+4)=='Meteor Storm':matches.append((sd,variant))
  assert len(matches)==1;sd,var=matches[0];assert v.string(var+0x2a)=='StrikeEffect';control=v.u32(var+0x70)
  self.variant=dict(name=v.string(var+4),effect=v.string(var+0x2a),control_hex=bytes(v.uc.mem_read(control,0xc4)).hex(),spell_hex=bytes(v.uc.mem_read(sd,0xd0)).hex())
  targets=v.allocate(4);v.put_u32(targets,self.target);source=v.allocate(12);v.write(source,struct.pack('<3i',0,0,0))
  self.spell=call(0x542290,(self.invoker,targets,1,source,sd,var,0),this=0x670628);assert v.u32(self.spell)==0x5b9bdc
  camera=v.allocate(12);v.write(camera,struct.pack('<3i',*SOURCE));call(0x450d20,(camera,),this=0x6668d8);call(0x459220,this=0x6668d8,instruction_limit=100000000)
  self.sectors=[]
  for i in range(v.u32(0x668578)):
   s=v.u32(v.u32(0x668588)+4*i);self.sectors.append(dict(position=list(struct.unpack('<3i',v.uc.mem_read(s+4,12))),active=v.u32(s+0x98),objects=v.u32(s+0xb8)))
  assert all(s['objects']==0 for s in self.sectors)
  self.rng_calls=0;self.srand_calls=[];self.creations=[];v.uc.hook_add(UC_HOOK_CODE,self.rand_entry,begin=0x58c582,end=0x58c582);v.uc.hook_add(UC_HOOK_CODE,self.srand_entry,begin=0x58c575,end=0x58c575);v.uc.hook_add(UC_HOOK_CODE,self.new_entry,begin=0x450e40,end=0x450e40);call(0x58c575,(1,));self.tls=call(0x58fbf1)
  for _ in range(6):
   call(0x540d70,this=self.spell);assert call(0x540eb0,this=self.spell,instruction_limit=100000000)==0
  n=v.u32(self.spell+0x13c);assert n==4;self.owners=[v.u32(v.u32(self.spell+0x14c)+4*i)for i in range(n)]
  assert all(v.u32(o)==0x5b99d8 and v.u32(o+0xd8)==self.spell for o in self.owners)
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
  self.software=LitSoftwareFixture(exe,1920,1080,camera=(*SOURCE[:2],0),vm=v);self.software.lighting((38,)*3,(1,)*3);v.put_u32(0x675fc0,960);v.put_u32(0x675fc4,700)
  self.draws=[];self.states={};self.current_object=-1;self.current_owner=-1
  for address,method in[(0x56eb30,self.observe_draw),(0x56d400,self.observe_state),(0x40a8f0,self.observe_object)]:v.uc.hook_add(UC_HOOK_CODE,method,begin=address,end=address)
  self.animators=[]
  for o in self.owners:
   assert call(0x46e8b0,this=o,instruction_limit=100000000)==1
   a=v.u32(o+0x58);assert v.u32(a)==0x5a370c;self.animators.append(a)
  self.pulse_inputs=[];v.uc.hook_add(UC_HOOK_CODE,self.observe_pulse,begin=0x403760,end=0x403760)
  self.initial=self.state()
 def rand_entry(self,*_):self.rng_calls+=1
 def srand_entry(self,uc,*_):self.srand_calls.append(self.vm.u32(uc.reg_read(UC_X86_REG_ESP)+4))
 def new_entry(self,uc,*_):
  v=self.vm;s=uc.reg_read(UC_X86_REG_ESP);self.creations.append(dict(definition_hex=bytes(v.uc.mem_read(v.u32(s+4),0x34)).hex(),imagery=v.u32(s+8)))
 observe_state=TornadoTargetScene.observe_state
 observe_object=TornadoTargetScene.observe_object
 def observe_draw(self,uc,*args):
  TornadoTargetScene.observe_draw(self,uc,*args);self.draws[-1]['owner']=self.current_owner
  if not self.raster_enabled:NativeSpeed.ret(self,0,28)
 def observe_pulse(self,uc,*_):
  v=self.vm;c=uc.reg_read(UC_X86_REG_ECX);emitters=[]
  for i in range(v.u32(c+0x18)):
   e=v.u32(v.u32(c+0x28)+4*i);emitters.append(dict(position=list(struct.unpack('<3f',v.uc.mem_read(e+0x10,12))),scale=list(struct.unpack('<3f',v.uc.mem_read(e+0x40,12))),matrix=list(struct.unpack('<16f',v.uc.mem_read(e+0x58,64)))))
  i=self.current_owner;o=self.owners[i];a=self.animators[i]
  self.pulse_inputs.append(dict(owner=i,controller=c,frame=v.u32(a+0x14),position=list(struct.unpack('<3i',v.uc.mem_read(o+0x10,12))),origin=list(struct.unpack('<3f',v.uc.mem_read(c+0x170,12))),emitters=emitters))
 def state(self):
  v=self.vm;rows=[]
  for o,a in zip(self.owners,self.animators):
   pools=[]
   for i in range(v.u32(a+0x58)):
    c=v.u32(v.u32(a+0x68)+4*i)
    if v.u32(c)!=0x5a3544:pools.append(dict(vtable=hex(v.u32(c)),header_hex=bytes(v.uc.mem_read(c,0x28)).hex()));continue
    capacity=v.u32(c+0xf8);pool=v.u32(c+0x2c);pools.append(dict(vtable=hex(v.u32(c)),capacity=capacity,address=c,credit=struct.unpack('<f',v.uc.mem_read(c+0x16c,4))[0],controller_hex=bytes(v.uc.mem_read(c,0x100)).hex(),pool_hex=bytes(v.uc.mem_read(pool,252*capacity)).hex()))
   rows.append(dict(position=list(struct.unpack('<3i',v.uc.mem_read(o+0x10,12))),frame=struct.unpack('<h',v.uc.mem_read(o+0x5c,2))[0],flags=v.u32(o+8),wait=v.u32(o+0x138),lifetime=v.u32(o+0x12c),repeat=v.u32(o+0x130),pools=pools))
  return dict(owners=rows,seed=v.u32(self.tls+0x14),rng_calls=self.rng_calls,srand_calls=self.srand_calls[:],spell_timer=v.u32(self.spell+0x110),spell_frame=v.u32(self.spell+0x114))
 def step(self,tick):
  v=self.vm;call=self.world.call;v.put_u32(0x65cb38,tick);self.pulse_inputs=[]
  if v.u32(self.spell+0x110)!=0:
   call(0x540d70,this=self.spell);call(0x540eb0,this=self.spell)
  for i,(o,a) in enumerate(zip(self.owners,self.animators)):
   self.current_owner=i
   if v.u32(o+8)&0x1000:continue
   if v.u32(o+8)&0x4000:call(0x470cc0,this=o)
   call(0x4de800,this=o)
   if not(v.u32(o+8)&0x1000)and v.u32(o+8)&0x4000:call(0x40e460,(0,),this=a)
 def render(self):
  self.draws=[];v=self.vm;s=self.software;call=self.world.call;s.clear();call(0x416f70,this=0x65a57c)
  for i,(o,a)in enumerate(zip(self.owners,self.animators)):
   if v.u32(o+8)&0x1000 or not(v.u32(o+8)&0x4000):continue
   self.current_owner=i;call(0x40e740,this=a);v.call(0x54de20,(s.screen.address,s.depth.address),this=s.camera);v.call(0x54dd70,(0,s.width-1,0,s.height-1,s.width),this=s.camera);call(0x417d60,(1,1),this=0x65a57c);call(0x40e8d0,this=a,instruction_limit=100000000);call(0x40eb00,this=a)
  call(0x416fb0,this=0x65a57c);return v.surface_bytes('screen')
def comparable_state(state):
 import copy
 state=copy.deepcopy(state)
 for owner in state['owners']:
  for pool in owner['pools']:pool.pop('address',None);pool.pop('credit',None)
 return state
def run(exe,boot,out,frames,sampled=False,control=None,state_only=False):
 out.mkdir(parents=True,exist_ok=False);f=StrikeScene(exe,boot);rows=[]
 for tick in range(1,frames+1):
  selected=tick in(1,5,10,20,30,40,50,60,70,80,85,86,87,88,90,100)or tick==frames
  f.raster_enabled=(not sampled or selected)and not state_only
  f.step(tick);raw=f.render();row=dict(tick=tick,state=f.state(),draws=f.draws,pulse_inputs=f.pulse_inputs,raster_executed=f.raster_enabled)
  if selected and not state_only:
   p=out/f'native-{tick:03}.png';save_rgb565_png(p,raw,1920,1080);row.update(image=str(p),image_sha256=sha(p),pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',raw)))
  rows.append(row)
  with(out/'trace.jsonl').open('a')as h:h.write(json.dumps(row)+'\n')
  print(tick,[(o['frame'],o['wait'],o['lifetime'],bool(o['flags']&0x1000))for o in row['state']['owners']],len(f.draws),row.get('pixels'),flush=True)
 checks={}
 if control:
  c=json.loads(control.read_text());assert comparable_state(c['initial'])==comparable_state(f.initial)
  for a,b in zip(c['records'],rows):
   assert comparable_state(a['state'])==comparable_state(b['state']),a['tick']
   assert a['draws']==json.loads(json.dumps(b['draws'])),a['tick']
   if 'image_sha256'in a and not state_only:assert a['image_sha256']==b['image_sha256'],a['tick']
  checks=dict(status='pass',frames=len(c['records']),all_effect_pool_rng_packets_exact=True,selected_images_exact=True,manifest=str(control),sha256=sha(control))
 report=dict(sampled=sampled,state_only=state_only,full_control=checks,status='state_and_draw_packets_only'if state_only else'sampled_reference'if sampled else'complete_native_reference',accepted=False,visual_pass=False,type_id='0xad92bd32',scope=__doc__,source_sha256=sha(__file__),executable_sha256=sha(exe),boot_image_sha256=sha(boot),records=rows,initial=f.initial,variant=f.variant,creations=f.creations,sectors=f.sectors,actors_before=f.before,actors_after=[f.actors.dump(x)for x in(f.invoker,f.target)],viewport=[1920,1080],center=[960,700],camera=[3185,3185,0],lighting=dict(ambient=[38]*3,directional=[1]*3))
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--boot-image',type=Path,required=True);p.add_argument('--output',type=Path,required=True);p.add_argument('--frames',type=int,default=100);p.add_argument('--sampled',action='store_true');p.add_argument('--full-control',type=Path);p.add_argument('--state-only',action='store_true');a=p.parse_args();run(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',a.boot_image,a.output,a.frames,a.sampled,a.full_control,a.state_only)
