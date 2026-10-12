"""Pulp public Set with a genuine actor/animator and original full render.

Bounded feasibility only: declared velocity(0,0,8),5/5/5counts, unresident
Acolyte and genuine empty terrain. Original collision tile calls are observed,
not replaced. Unselected Render calls are absent in this preflight; this is
not a complete render-cadence or Metal appearance claim.
"""
import sys,json,struct,argparse,hashlib
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parent))
from character_native_preflight import *
parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--boot-image',type=Path,required=True);parser.add_argument('--output',type=Path,required=True);a=parser.parse_args();out=a.output;out.mkdir(parents=True,exist_ok=False);f=AuthenticCharacters(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',a.boot_image);f.select_screen_prefix();v=f.vm;call=f.world.call
ch=f.new_character('Acolyte',SOURCE);print('actor',f.dump(ch),flush=True);print('animator create',call(0x46e8b0,this=ch,instruction_limit=100000000),hex(v.u32(ch+0x58)),flush=True)
pulp=f.new_object('Pulp',SOURCE,25,0x5aa89c);print('pulp',hex(pulp),'anim',call(0x46e8b0,this=pulp,instruction_limit=100000000),flush=True)
vel=v.allocate(12);v.write(vel,struct.pack('<3i',0,0,8));print('beforeSet',bytes(v.uc.mem_read(pulp+0x184,0x24)).hex(),flush=True)
call(0x58c575,(1,));result=call(0x4e96e0,(vel,ch,5,5,5),this=pulp,instruction_limit=100000000)
print('set',result,'after',bytes(v.uc.mem_read(pulp+0x184,0x24)).hex(),'actor',f.dump(ch),flush=True)
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
s=scene.software=LitSoftwareFixture(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',1280,960,camera=(*SOURCE[:2],0),vm=v);s.lighting((38,)*3,(1,)*3);v.put_u32(0x675fc0,640);v.put_u32(0x675fc4,640)
for address,method in[(0x56eb30,scene.observe_draw),(0x56d400,scene.observe_state),(0x40a8f0,scene.observe_object)]:v.uc.hook_add(UC_HOOK_CODE,method,begin=address,end=address)
rows=[];tile_requests=[];ground=[]
from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EAX
current_tick=0
def observe_tile(uc,*_):
 sp=uc.reg_read(UC_X86_REG_ESP);definition=v.u32(sp+4);tile_requests.append(dict(tick=current_tick,definition_hex=bytes(v.uc.mem_read(definition,52)).hex()))
def tile_return(uc,*_):
 if tile_requests:tile_requests[-1]['result']=uc.reg_read(UC_X86_REG_EAX)
def ground_return(uc,*_):ground.append(dict(tick=current_tick,result=uc.reg_read(UC_X86_REG_EAX)))
v.uc.hook_add(UC_HOOK_CODE,observe_tile,begin=0x450e40,end=0x450e40)
v.uc.hook_add(UC_HOOK_CODE,tile_return,begin=0x4e9240,end=0x4e9240)
for address in(0x4e8fb7,0x4e9037,0x4e90b3):v.uc.hook_add(UC_HOOK_CODE,ground_return,begin=address,end=address)
for tick in range(21):
 current_tick=tick
 if tick:
  call(0x470cc0,this=pulp);call(0x4de800,this=pulp);call(0x40e460,(0,),this=scene.animator,instruction_limit=100000000)
 if tick not in(0,1,2,5,10,20):continue
 scene.draws=[];s.clear();native_owner(scene);call(0x416f70,this=0x65a57c);v.call(0x54de20,(s.screen.address,s.depth.address),this=s.camera);v.call(0x54dd70,(0,1279,0,959,1280),this=s.camera);call(0x4e9400,this=scene.animator,instruction_limit=100000000);call(0x416fb0,this=0x65a57c)
 raw=v.surface_bytes('screen');p=out/f'native-{tick:03}.png';save_rgb565_png(p,raw,1280,960);row=dict(tick=tick,draws=scene.draws,image=str(p),pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',raw)),body_hex=bytes(v.uc.mem_read(v.u32(pulp+0x190),5*72)).hex());rows.append(row);print('render',tick,len(scene.draws),row['pixels'],flush=True)
(out/'manifest.json').write_text(json.dumps(dict(status='feasible_public_api',accepted=False,visual_pass=False,records=rows,tile_requests=tile_requests,ground=ground,owner_extension=bytes(v.uc.mem_read(pulp+0x184,0x24)).hex(),actor=f.dump(ch),executable_sha256=sha(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe'),boot_image_sha256=sha(a.boot_image),tool_sha256=sha(__file__),scope=__doc__),indent=2))
