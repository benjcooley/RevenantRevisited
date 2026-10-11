"""Complete original ManaDrain owner/animator/render with two genuine actors.

The original loaded class factory constructs the exact ManaDrain owner. An
explicit standalone residency boundary performs Timer's reverse association
and calls original SetSpell/list insertion; no map-sector insertion is claimed.
Original named spell, fighting-target selection, mana transfer and Spell::Kill
execute. Camera/lights/RAM targets and the material-handle COM getter are the
same declared interfaces as the authentic Paralize fixture. No actor/effect/
resource/raster/rand instruction is replaced. NOSOUND startup remains inherited.
"""
import argparse
import json
import struct
import zipfile
from pathlib import Path
from character_native_preflight import AuthenticCharacters,ROOT,SOURCE,TARGET,sha
from lit_software_probe import LitSoftwareFixture
from software_probe import save_rgb565_png
from firecone_owner_probe import native_owner
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP

class ManaDrainTargetScene:
 def __init__(self,exe,boot,width=1280,height=960):
  self.actors=AuthenticCharacters(exe,boot);self.world=self.actors.world;self.vm=v=self.world.vm;call=self.world.call
  self.actors.select_screen_prefix()
  self.invoker=self.actors.new_character('Acolyte',SOURCE);self.target=self.actors.new_character('Acolyte',TARGET)
  self.before=[self.actors.dump(x)for x in (self.invoker,self.target)]
  call(0x4d3b90,(self.target,3),this=self.invoker)
  assert call(0x53ead0,this=0x667c38,instruction_limit=100000000)==1
  matches=[]
  for i in range(v.u32(0x667c3c)):
   sd=v.u32(v.u32(0x667c4c)+4*i)
   for j in range(v.u32(sd)):
    variant=v.u32(v.u32(sd+0x10)+4*j)
    if v.string(variant+4)=='Mana Drain':matches.append((sd,variant))
  assert len(matches)==1
  targets=v.allocate(4);v.put_u32(targets,self.target);source=v.allocate(12);v.write(source,struct.pack('<3i',0,0,0))
  spell=call(0x542200,(self.invoker,targets,1,source,*matches[0],0),this=0x670630)
  self.spell_variant=dict(name=v.string(matches[0][1]+4),maximum_damage=v.u32(matches[0][1]+0x58))
  assert self.spell_variant==dict(name='Mana Drain',maximum_damage=62)
  self.owner=self.actors.new_object('ManaDrain',SOURCE,25,0x5adf18)
  call(0x4de7d0,(spell,),this=self.owner);v.put_u32(spell+8,self.owner);call(0x41c840,(self.owner,),this=spell+0x13c);self.spell=spell
  assert call(0x46e8b0,this=self.owner,instruction_limit=100000000)==1
  self.animator=v.u32(self.owner+0x58);assert v.u32(self.animator)==0x5ae118
  # The original device boundary records SetLightState but lacks its getter.
  # Recover the actual booted single device's last material state, then retain
  # get/set semantics without replacing any other device method.
  self.material=0
  for name,args in self.world.ddraw.device_calls:
   if name=='IDirect3DDevice3::SetLightState' and args[0]==1:self.material=args[1]
  original=self.world.ddraw._device_method
  def device(interface,method,args):
   if interface=='IDirect3DDevice3' and method in ('GetLightState','SetLightState') and args[1]==1:
    if method=='GetLightState':v.put_u32(args[2],self.material)
    else:self.material=args[2]
    return 0
   return original(interface,method,args)
  self.world.ddraw._device_method=device
  self.software=LitSoftwareFixture(exe,width,height,camera=(*SOURCE[:2],0),vm=v)
  self.software.lighting((38,38,38),(1,1,1));v.put_u32(0x675fc0,width//2);v.put_u32(0x675fc4,height*2//3)
  self.rng_calls=0;self.draws=[];self.states={};self.current_object=-1
  v.uc.hook_add(UC_HOOK_CODE,self.rand_entry,begin=0x58c582,end=0x58c582)
  v.uc.hook_add(UC_HOOK_CODE,self.observe_draw,begin=0x56eb30,end=0x56eb30)
  v.uc.hook_add(UC_HOOK_CODE,self.observe_state,begin=0x56d400,end=0x56d400)
  v.uc.hook_add(UC_HOOK_CODE,self.observe_object,begin=0x40a8f0,end=0x40a8f0)
  call(0x58c575,(1,)) # Execute actual srand, not a substituted rand handler.
  self.tls=call(0x58fbf1)
 def rand_entry(self,*args):self.rng_calls+=1
 def observe_state(self,uc,*args):
  sp=uc.reg_read(UC_X86_REG_ESP);self.states[self.vm.u32(sp+4)]=self.vm.u32(sp+8)
 def observe_object(self,uc,*args):
  sp=uc.reg_read(UC_X86_REG_ESP);obj=self.vm.u32(sp+4);self.current_object=self.vm.u32(obj+4)
 def observe_draw(self,uc,*args):
  v=self.vm;sp=uc.reg_read(UC_X86_REG_ESP);a=[v.u32(sp+4+i*4)for i in range(7)]
  self.draws.append(dict(object=self.current_object,fvf=a[1],vertex_count=a[3],index_count=a[5],states=dict(self.states),vertices_hex=bytes(v.uc.mem_read(a[2],a[3]*32)).hex(),indices_hex=bytes(v.uc.mem_read(a[4],a[5]*2)).hex(),matrix=list(struct.unpack('<16f',v.uc.mem_read(0x675ed0,64)))))
 def state(self):
  v=self.vm;o=self.owner
  return dict(owner_hex=bytes(v.uc.mem_read(o+0x184,28)).hex(),animator_hex=bytes(v.uc.mem_read(self.animator+0xfc,0x244)).hex(),flags=v.u32(o+8),seed=v.u32(self.tls+0x14),rng_calls=self.rng_calls,stage=v.u32(o+0x198),ticks=v.u32(o+0x19c),timer=v.u32(self.spell+0x110),caster_mana=self.world.call(v.u32(v.u32(self.invoker)+0x1d0),this=self.invoker),victim_mana=self.world.call(v.u32(v.u32(self.target)+0x1d0),this=self.target))
 def step(self):
  if self.vm.u32(self.owner+8)&0x1000:return
  self.world.call(0x470cc0,this=self.owner);self.world.call(0x4fba60,this=self.owner)
  if not self.vm.u32(self.owner+8)&0x1000:self.world.call(0x4fc000,(0,),this=self.animator)
 def render(self):
  self.draws=[];s=self.software;v=self.vm;s.clear()
  if v.u32(self.owner+8)&0x1000:return v.surface_bytes('screen')
  native_owner(self);before=self.rng_calls
  self.world.call(0x416f70,this=0x65a57c)
  # BeginScene correctly binds the real 640x480 display. This isolated view
  # explicitly redirects only camera output buffers/pitch using original APIs.
  v.call(0x54de20,(s.screen.address,s.depth.address),this=s.camera)
  v.call(0x54dd70,(0,s.width-1,0,s.height-1,s.width),this=s.camera)
  self.world.call(0x4fc440,this=self.animator,instruction_limit=100000000)
  self.world.call(0x416fb0,this=0x65a57c)
  self.last_render_rng=self.rng_calls-before
  return v.surface_bytes('screen')

def run(exe,boot,out,frames=100):
 out.mkdir(parents=True,exist_ok=False);f=ManaDrainTargetScene(exe,boot);rows=[]
 samples={1,5,10,20,21,30,40,50,60,70,71,80,90,91,100}
 for tick in range(1,frames+1):
  f.step();raw=f.render();row=dict(tick=tick,state=f.state(),draw_count=len(f.draws),draws=f.draws)
  if tick in samples:
   p=out/f'native-{tick:03}.png';save_rgb565_png(p,raw,1280,960);row.update(image=str(p),image_sha256=sha(p),draws=f.draws,pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',raw)))
  rows.append(row)
  with (out/'trace.jsonl').open('a')as trace:trace.write(json.dumps(row)+'\n')
  if tick in samples or tick%10==0:print(tick,row['state']['stage'],row['state']['ticks'],len(f.draws),row.get('pixels'),flush=True)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as archive:
  asset=archive.read(next(n for n in archive.namelist()if n.lower()=='imagery/magic/manadrain.i3d'))
 report=dict(source_sha256=sha(Path(__file__)),asset_sha256=__import__('hashlib').sha256(asset).hexdigest(),variant=f.spell_variant,type_id='0xad92bc13',accepted=False,visual_pass=False,executable_sha256=sha(exe),boot_image_sha256=sha(boot),source=SOURCE,target=TARGET,actors_before=f.before,actors_after=[f.actors.dump(x)for x in(f.invoker,f.target)],seed=1,records=rows,viewport=[1280,960],center=[640,640],camera=[3185,3185,0],lighting=dict(ambient=[38]*3,directional=[1]*3),scope=__doc__)
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--boot-image',type=Path,required=True);p.add_argument('--output',type=Path,required=True);p.add_argument('--frames',type=int,default=100);a=p.parse_args();run(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',a.boot_image,a.output,a.frames)
