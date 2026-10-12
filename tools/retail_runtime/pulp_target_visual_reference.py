"""Pulp public Set with a genuine actor/animator and original full render.

Complete owner/Pulse/Animate/Render trace: declared velocity(0,0,8),5/5/5counts, unresident
Acolyte and genuine empty terrain. Original collision tile calls are observed,
not replaced. Every live Render executes; selected raster images are persisted. No Metal
appearance or natural mapped-caller claim.
"""
import sys,json,struct,argparse,hashlib
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parent))
from character_native_preflight import *
# Declared genuine actor position outside the gallery's loaded floor; display translation is separate.
SOURCE=(10000,10000,16)
parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--boot-image',type=Path,required=True);parser.add_argument('--output',type=Path,required=True);parser.add_argument('--frames',type=int,default=80);a=parser.parse_args();out=a.output;out.mkdir(parents=True,exist_ok=False);f=AuthenticCharacters(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',a.boot_image);f.select_screen_prefix();v=f.vm;call=f.world.call
ch=f.new_character('Acolyte',SOURCE);print('actor',f.dump(ch),flush=True);print('animator create',call(0x46e8b0,this=ch,instruction_limit=100000000),hex(v.u32(ch+0x58)),flush=True)
pulp=f.new_object('Pulp',SOURCE,25,0x5aa89c);print('pulp',hex(pulp),'anim',call(0x46e8b0,this=pulp,instruction_limit=100000000),flush=True)
vel=v.allocate(12);v.write(vel,struct.pack('<3i',0,0,8));print('beforeSet',bytes(v.uc.mem_read(pulp+0x184,0x24)).hex(),flush=True)
from unicorn import UC_HOOK_CODE
rng_calls=0
from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EAX
current_tick=0;ground=[];pending_ground=[];ground_returns=set()
def ground_entry(uc,*_):
 sp=uc.reg_read(UC_X86_REG_ESP);ret=v.u32(sp);point=v.u32(sp+4)
 ground.append(dict(tick=current_tick,point=list(struct.unpack('<3i',v.uc.mem_read(point,12))),caller=hex(ret)));pending_ground.append((ret,ground[-1]))
 if ret not in ground_returns:
  ground_returns.add(ret);v.uc.hook_add(UC_HOOK_CODE,ground_exit,begin=ret,end=ret)
def ground_exit(uc,address,*_):
 if pending_ground and pending_ground[-1][0]==address:
  _,row=pending_ground.pop();row['result']=uc.reg_read(UC_X86_REG_EAX)
v.uc.hook_add(UC_HOOK_CODE,ground_entry,begin=0x452e10,end=0x452e10)
def rng_entry(*_):
 global rng_calls
 rng_calls+=1
v.uc.hook_add(UC_HOOK_CODE,rng_entry,begin=0x58c582,end=0x58c582)
call(0x58c575,(1,));tls=call(0x58fbf1);result=call(0x4e96e0,(vel,ch,5,5,5),this=pulp,instruction_limit=100000000)
print('set',result,'after',bytes(v.uc.mem_read(pulp+0x184,0x24)).hex(),'actor',f.dump(ch),flush=True)
def state():
 return dict(owner_hex=bytes(v.uc.mem_read(pulp+0x184,0x24)).hex(),flags=v.u32(pulp+8),actor_flags=v.u32(ch+8),seed=v.u32(tls+0x14),rng_calls=rng_calls,body_hex=bytes(v.uc.mem_read(v.u32(pulp+0x190),5*72)).hex(),blood_hex=bytes(v.uc.mem_read(v.u32(pulp+0x188),5*40)).hex(),splat_hex=bytes(v.uc.mem_read(v.u32(pulp+0x18c),5*44)).hex())
initial=state()
report=dict(owner=hex(pulp),character=f.dump(ch),owner_extension=bytes(v.uc.mem_read(pulp+0x184,0x24)).hex())
(out/'initial.json').write_text(json.dumps(report,indent=2))
from tornado_target_visual_reference import TornadoTargetScene
from lit_software_probe import LitSoftwareFixture
from firecone_owner_probe import native_owner
from software_probe import save_rgb565_png
from unicorn import UC_HOOK_CODE
scene=TornadoTargetScene.__new__(TornadoTargetScene);scene.actors=f;scene.world=f.world;scene.vm=v;scene.owner=pulp;scene.animator=v.u32(pulp+0x58);scene.draws=[];scene.states={};scene.current_object=-1
material=0
for name,args in f.world.ddraw.device_calls:
 if name=='IDirect3DDevice3::SetLightState'and args[0]==1:material=args[1]
original=f.world.ddraw._device_method
def device(interface,method,args):
 global material
 if interface=='IDirect3DDevice3'and method in('GetLightState','SetLightState')and args[1]==1:
  if method=='GetLightState':v.put_u32(args[2],material)
  else:material=args[2]
  return 0
 return original(interface,method,args)
f.world.ddraw._device_method=device
saved_state_pointer=v.allocate(4);v.call(0x56d4b0,(22,saved_state_pointer));boot_cull=v.u32(saved_state_pointer);print('boot cull',boot_cull,'scene cache',v.u32(0x5e8790),flush=True)
s=scene.software=LitSoftwareFixture(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',1280,960,camera=(*SOURCE[:2],0),vm=v);s.lighting((38,)*3,(1,)*3);v.put_u32(0x675fc0,640);v.put_u32(0x675fc4,640)
v.call(0x56d400,(22,boot_cull))
# Actual normal-scene caller, as original413ba1/4149b1. The leaf inherits it.
v.call(0x417d60,(1,1),this=0x65a57c)
for address,method in[(0x56eb30,scene.observe_draw),(0x56d400,scene.observe_state),(0x40a8f0,scene.observe_object)]:v.uc.hook_add(UC_HOOK_CODE,method,begin=address,end=address)
state_pointer=v.allocate(4)
for key in (7,14,19,20,21,22):
 v.call(0x56d4b0,(key,state_pointer));scene.states[key]=v.u32(state_pointer)
print('inherited raster states',scene.states,flush=True)
rows=[];tile_requests=[]
from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EAX
current_tick=0
def observe_tile(uc,*_):
 sp=uc.reg_read(UC_X86_REG_ESP);definition=v.u32(sp+4);tile_requests.append(dict(tick=current_tick,definition_hex=bytes(v.uc.mem_read(definition,52)).hex()))
def tile_return(uc,*_):
 if tile_requests:tile_requests[-1]['result']=uc.reg_read(UC_X86_REG_EAX)
v.uc.hook_add(UC_HOOK_CODE,observe_tile,begin=0x450e40,end=0x450e40)
v.uc.hook_add(UC_HOOK_CODE,tile_return,begin=0x4e9240,end=0x4e9240)
for tick in range(a.frames+1):
 current_tick=tick
 if tick:
  call(0x470cc0,this=pulp);call(0x4de800,this=pulp);call(0x40e460,(0,),this=scene.animator,instruction_limit=100000000)
 scene.draws=[];s.clear();native_owner(scene);call(0x416f70,this=0x65a57c);v.call(0x54de20,(s.screen.address,s.depth.address),this=s.camera);v.call(0x54dd70,(0,1279,0,959,1280),this=s.camera);call(0x4e9400,this=scene.animator,instruction_limit=100000000);call(0x416fb0,this=0x65a57c)
 raw=v.surface_bytes('screen');row=dict(tick=tick,draws=scene.draws,state=state())
 if tick in(0,1,2,3,5,10,20,40,60,80):
  p=out/f'native-{tick:03}.png';save_rgb565_png(p,raw,1280,960);row.update(image=str(p),image_sha256=sha(p),pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',raw)))
 rows.append(row)
 with(out/'trace.jsonl').open('a')as h:h.write(json.dumps(row)+'\n')
 if tick%10==0 or 'image'in row:print('render',tick,len(scene.draws),row.get('pixels'),rng_calls,flush=True)
(out/'manifest.json').write_text(json.dumps(dict(status='complete_native_reference',initial=initial,accepted=False,visual_pass=False,records=rows,tile_requests=tile_requests,ground=ground,owner_extension=bytes(v.uc.mem_read(pulp+0x184,0x24)).hex(),actor=f.dump(ch),executable_sha256=sha(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe'),boot_image_sha256=sha(a.boot_image),tool_sha256=sha(__file__),scope=__doc__,viewport=[1280,960],center=[640,640],source=list(SOURCE),camera=[*SOURCE[:2],0],lighting=dict(ambient=[38]*3,directional=[1]*3),raster_states=scene.states,clock_ms=v.milliseconds,scene_mode=1),indent=2))
