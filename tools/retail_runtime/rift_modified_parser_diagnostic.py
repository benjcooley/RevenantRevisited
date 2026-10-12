"""Genuine loaded Rift1/Teleporter factory, controller admission and full renderer boundaries."""
import argparse,json,struct,zipfile
from pathlib import Path
from character_native_preflight import AuthenticCharacters,ROOT,sha
from dragonattack_visual_reference import DragonAttackScene
from lit_software_probe import LitSoftwareFixture
from software_probe import save_rgb565_png
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX,UC_X86_REG_ESP

class ModifiedRiftScene(AuthenticCharacters):
 observe_rng=DragonAttackScene.observe_rng
 observe_render_state=DragonAttackScene.observe_render_state
 observe_object=DragonAttackScene.observe_object
 observe_draw=DragonAttackScene.observe_draw
 draw_bounds=DragonAttackScene.draw_bounds
 def __init__(self,executable,boot_image,name,actor=False):
  super().__init__(executable,boot_image);self.select_screen_prefix();assert name=='Rift1';assert bytes(self.vm.uc.mem_read(0x5c59b4,2))==b'v\0';self.vm.uc.mem_write(0x5c59b4,b'd')
  self.name=name;self.controller_events=[];self.stage='NewObject';self.owner=self.new_object(name,(3185,3185,16)if actor else(0,0,0),25,0x5a85ac if name=='Rift1' else 0x5aa200)
  v,call=self.vm,self.world.call;self.width,self.height,self.center=1280,960,(640,480)
  self.software=LitSoftwareFixture(executable,self.width,self.height,camera=(3185,3185,0)if actor else(0,0,0),vm=v)
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
  self.actors=[]
  if actor:
   assert name=='Teleporter';self.stage='actual actors and Teleport spell'
   invoker=self.new_character('Acolyte',(3185,3185,16));target=self.new_character('Acolyte',(3281,3185,16))
   call(0x4d3b90,(target,3),this=invoker)
   assert call(0x53ead0,this=0x667c38,instruction_limit=100000000)==1
   selected=[]
   for i in range(v.u32(0x667c3c)):
    sd=v.u32(v.u32(0x667c4c)+4*i)
    for j in range(v.u32(sd)):
     variant=v.u32(v.u32(sd+0x10)+4*j)
     if v.string(variant+4)=='Teleport':selected.append((sd,variant))
   assert len(selected)==1
   targets,source=v.allocate(4),v.allocate(12);v.put_u32(targets,target);v.write(source,struct.pack('<3i',3185,3185,16))
   spell=call(0x542200,(invoker,targets,1,source,*selected[0],0),this=0x670630);v.put_u32(self.owner+0xd8,spell)
   self.actors=[invoker,target];self.actors_before=[self.dump(x)for x in self.actors]
  self.stage='CreateAnimator';self.controller_events=[]
  for address in(0x401b70,0x40e0c8,0x405bc0,0x405c14,0x401c90):
   v.uc.hook_add(UC_HOOK_CODE,self.observe_controller,begin=address,end=address)
  assert call(0x46e8b0,this=self.owner)==1
  self.animator=v.u32(self.owner+0x58);self.imagery=v.u32(self.owner+0x54)
  assert v.u32(self.animator)==(0x5a370c if name=='Rift1' else 0x5aa400)
  self.animobjs=[v.u32(v.u32(self.animator+0x54)+4*i)for i in range(v.u32(self.animator+0x44))]
  self.render_states={};self.draws=[];self.points=[];self.current_object=-1;self.raster_enabled=True;self.lookups=[]
  for address,observer in((0x40a8f0,self.observe_object),(0x56eb30,self.observe_draw),(0x56d400,self.observe_render_state)):
   v.uc.hook_add(UC_HOOK_CODE,observer,begin=address,end=address)
 def observe_controller(self,uc,address,*unused):
  self.controller_events.append(dict(address=hex(address),eax=hex(uc.reg_read(UC_X86_REG_EAX))))
 def state(self):
  v=self.vm
  return dict(flags=v.u32(self.owner+8),position=list(struct.unpack('<3i',v.uc.mem_read(self.owner+0x10,12))),frame=struct.unpack('<h',v.uc.mem_read(self.owner+0x5c,2))[0],state=struct.unpack('<h',v.uc.mem_read(self.owner+0xc,2))[0],animator_frame=v.u32(self.animator+0x14),custom_state=list(struct.unpack('<28I',v.uc.mem_read(self.animator+0xfc,112)))if self.name=='Teleporter' else [],owner_tail=list(struct.unpack('<5I',v.uc.mem_read(self.owner+0x184,20)))if self.name=='Teleporter' else [],seed=v.u32(self.thread_data+0x14),rng_draws=self.rng_draws,objects=len(self.animobjs),matrices=[list(struct.unpack('<16f',v.uc.mem_read(x+0x58,64)))for x in self.animobjs])
 def render(self):
  s,v,call=self.software,self.vm,self.world.call;s.clear();self.draws=[];self.points=[]
  if not v.u32(self.owner+8)&4096:
   call(0x40e460,(0,),this=self.animator);call(0x416f70,this=0x65a57c);call(0x40e740,this=self.animator)
   call(0x54de20,(s.screen.address,s.depth.address),this=s.camera);call(0x54dd70,(0,self.width-1,0,self.height-1,self.width),this=s.camera)
   v.put_u32(0x675fc0,self.center[0]);v.put_u32(0x675fc4,self.center[1])
   call(0x40e8d0 if self.name=='Rift1' else 0x4e72b0,this=self.animator,instruction_limit=100000000);call(0x40eb00,this=self.animator);call(0x416fb0,this=0x65a57c)
  return v.surface_bytes('screen')

def run(executable,boot_image,output,name,frames,actor=False,samples=(1,2,5,10,15,20,24,25,26,30,35,40,45,49,50,51,55,60)):
 output.mkdir(parents=True,exist_ok=False);f=ModifiedRiftScene.__new__(ModifiedRiftScene);rows=[];failure=None
 try:
  f.__init__(executable,boot_image,name,actor);v,call=f.vm,f.world.call
  for tick in range(1,frames+1):
   f.stage=f'tick{tick}/NextFrame';v.put_u32(0x65cb38,tick)
   if not v.u32(f.owner+8)&4096:
    call(0x470cc0,this=f.owner);f.stage=f'tick{tick}/Pulse';call(0x4de800,this=f.owner);call(0x40e2e0,(1,),this=f.animator)
   f.stage=f'tick{tick}/Animate/fullRender';f.raster_enabled=tick in samples
   pixels=f.render();row=dict(tick=tick,state=f.state(),draws=f.draws,draw_bounds=f.draw_bounds())
   if f.raster_enabled:
    image_name=f'native-{tick:03}.png';save_rgb565_png(output/image_name,pixels,f.width,f.height);row.update(image=image_name,sha256=sha(output/image_name),nonzero_pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',pixels)))
   rows.append(row);print(tick,len(f.draws),row.get('nonzero_pixels'),flush=True)
 except RuntimeError as error:
  failure=dict(stage=f.stage,error=str(error),fault=getattr(f.world,'last_fault',None))
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as archive:
  import hashlib
  asset_sha=hashlib.sha256(archive.read('Imagery/Misc/rift1.i3d' if name=='Rift1' else 'Imagery/Misc/irisflare.i3d')).hexdigest()
 report=dict(asset_sha256=asset_sha,status='qualified_modified_original_gate'if failure else'qualified_modified_original_reference',accepted=False,unmodified_visual_pass=False,patched_original=True,patch=dict(address='0x5c59b4',before='76',after='64',reason='Map native V-parameter spelling to unchanged authored d; original full parser/Init/UV allocation andRender remain executed. Other legal v spelling is changed in this isolated diagnostic.'),effect=name,type_id='0xadbcef18' if name=='Rift1' else '0x369a5d2e',actor=actor,actors_before=getattr(f,'actors_before',[]),actors_after=[f.dump(x)for x in getattr(f,'actors',[])],records=rows,failure=failure,controller_events=f.controller_events,executable_sha256=sha(executable),boot_image_sha256=sha(boot_image),probe_sha256=sha(Path(__file__)),scope=('User-authorized qualified modified-original diagnostic, never an unmodified retail appearance pass. Assets/files/callers remain unchanged; one mapped native parser literal byte v→d only. Genuine cached loaded class NewObject/CreateAnimator, whole owner Pulse and original complete animator Animate/PreRender/Render/PostRender under actual BeginScene. Fixed RAM1280x960 center640,480 ambient38/dir1. '+('Real Acolyte invoker/Fighting target offset96, named Teleport actual creator/inlineSetSpell, source3185,3185,16 camera3185,3185,0; original empty-map destination failure branch. 'if actor else'Source/camera0,0,0 null spell. ')+'No asset/tag-file repair, missing prototype, failed-controller destructor interception, invented texture or map residency. Global frame before NextFrame/Pulse; stop advancing real killed owner. Natural caller/device/visual acceptance remain open.'))
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(dict(failure=failure,frames=len(rows))));return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--executable',type=Path,default=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe');p.add_argument('--boot-image',type=Path,required=True);p.add_argument('--output',type=Path,required=True);p.add_argument('--name',choices=['Rift1'],default='Rift1');p.add_argument('--frames',type=int,default=101);p.add_argument('--actor',action='store_true');p.add_argument('--samples',default='1,2,5,10,15,20,24,25,26,30,35,40,45,49,50,51,55,60');a=p.parse_args();run(a.executable,a.boot_image,a.output,a.name,a.frames,a.actor,tuple(map(int,a.samples.split(','))))
