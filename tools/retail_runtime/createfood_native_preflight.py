#!/usr/bin/env python3
"""Createfood full native feasibility and dynamic angle-table spill boundary; no visual acceptance."""
import argparse,json,struct,zipfile,re
from pathlib import Path
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EIP,UC_X86_REG_ECX,UC_X86_REG_EDX
from fireball_visual_reference import FireBallScene
from speed_controller_preflight import NativeSpeed,read_asset,ROOT,sha
from simple_effect_preflight import native_dispatch
from lit_software_probe import LitSoftwareFixture
from software_probe import save_rgb565_png
from firecone_owner_probe import native_owner


def initialize_color_tables(v):
 # The complete shipped MakeColorTables. Display::BitsPerPixel is the sole
 # display service interface, explicitly16 for the software RGB565 fixture.
 vt=v.allocate(0x100);fn=v.allocate(16);v.write_code(fn,b'\xb8\x10\0\0\0\xc3');v.put_u32(vt+0x18,fn)
 def bpp(uc,*_):v.put_u32(uc.reg_read(UC_X86_REG_ECX),vt);uc.reg_write(UC_X86_REG_EDX,vt)
 h=v.uc.hook_add(UC_HOOK_CODE,bpp,begin=0x41e0e2,end=0x41e0e2)
 v.call(0x41dff0,instruction_limit=100000000);v.uc.hook_del(h)

def read_createfood(data):
 assert sha(data)=='54bcee48d62c0d16995c26f034425dd1c04b355478f83f3a8a726e0c088c043d'
 a=read_asset(data);assert a['version']==1 and a['states']==1 and a['vertices']==8 and a['faces']==4
 u=lambda p:struct.unpack_from('<I',data,p)[0];r=lambda p:p+u(p)
 for o in a['objects']:
  p=r(a['body_offset']+48)+48*o['index'];st=r(p+44);n=a['state0_frames']
  o['states']=[dict(parent=struct.unpack_from('<i',data,st)[0],keys=[],raw_keys=data[r(st+4):r(st+4)+n*8],frames=n)]
 return a

class CreatefoodScene(FireBallScene):
 def __init__(self,exe,data):
  self.data=data;self.asset=read_createfood(data);self.software=LitSoftwareFixture(exe,640,480);self.software.lighting((38,38,38),(1,1,1));v=self.vm=self.software.vm
  initialize_color_tables(v)
  self.owner=v.allocate(0x400);self.imagery=v.allocate(0x200);self.put_string=lambda s:self.bytes(s.encode()+b'\0')
  native_dispatch(v);builder=v.call(0x40dca0,(self.put_string('Createfood'),));factory=v.u32(v.u32(builder));assert(builder,factory)==(0x66cdb8,0x4f4d00)
  h=v.uc.hook_add(UC_HOOK_CODE,self.imagery_lookup,begin=0x46e8a0,end=0x46e8a0);self.animator=v.call(factory,(self.owner,),this=builder);v.uc.hook_del(h);assert v.u32(self.animator)==0x5a8aa4
  self.dispatch=dict(builder=hex(builder),factory=hex(factory),animator_vtable=hex(v.u32(self.animator)),owner_init='0x4dfdf0',owner_pulse='0x4dfe30',init='0x4e0040',animate='0x4e0260',render='0x4e0650')
  self.required_pose_indices=set();self.unused_rejected_pose_indices=set();self.eager_pose=False;NativeSpeed.setup_resources(self);self.setup_resources()
  # Original generic owner virtual methods, null spell and non-inventory frame.
  v.put_u32(self.owner,0x5a85ac);v.write(self.owner+0x7c,struct.pack('<h',-1));v.put_u32(self.owner+0x54,self.imagery);v.put_u32(self.owner+0x58,self.animator);v.write(self.owner+0x5c,struct.pack('<hh',0,1))
  method=self.bytes(b'\xb8'+struct.pack('<I',struct.unpack_from('<H',data,68)[0])+b'\xc2\x04\x00');v.put_u32(v.u32(self.imagery)+0x8c,method)
  vt_owner=v.allocate(0x200);v.write(vt_owner,bytes(v.uc.mem_read(0x5a85ac,0x184)));v.put_u32(self.owner,vt_owner)
  def flags(args):v.put_u32(self.owner+8,args[0]);return 0
  v.handlers['CreatefoodFlags']=(1,flags);v.put_u32(vt_owner+0x40,v.api_address('kernel32.dll','CreatefoodFlags'))
  self.boundaries={0x40dd60:0,0x40e2e0:0,0x40c960:0,0x40c9c0:4,0x4708e0:0,0x4713c0:0,0x49c430:4}
  for a in self.boundaries:v.uc.hook_add(UC_HOOK_CODE,self.external,begin=a,end=a)
  self.rng_draws=0;v.uc.hook_add(UC_HOOK_CODE,self.rng,begin=0x58c582,end=0x58c582);v.uc.hook_add(UC_HOOK_CODE,self.texture_frame,begin=0x40c520,end=0x40c520)
  for addr,value in ((0x5d7a28,1),(0x66818c,0),(0x5e91c0,0),(0x5e8740,0),(0x5c61ac,1),(0x5e8790,0),(0x5c619c,1),(0x5e8850,1)):v.put_u32(addr,value)
  device=v.allocate(4);vt=v.allocate(0xa0);v.put_u32(device,vt);v.put_u32(0x668f14,device);self.material_state=0
  def getlight(args):v.put_u32(args[2],self.material_state);return 0
  def setlight(args):self.material_state=args[2];return 0
  for slot,name,method in ((23,'CreatefoodGetLight',getlight),(24,'CreatefoodSetLight',setlight)):v.handlers[name]=(3,method);v.put_u32(vt+slot*4,v.api_address('kernel32.dll',name))
  self.draws=[];self.states={};self.current_object=-1;self.raster_enabled=True;self.tick=0
  v.uc.hook_add(UC_HOOK_CODE,self.observe_object,begin=0x40a8f0,end=0x40a8f0);v.uc.hook_add(UC_HOOK_CODE,self.observe_draw,begin=0x56eb30,end=0x56eb30);v.uc.hook_add(UC_HOOK_CODE,self.observe_state,begin=0x56d400,end=0x56d400)
  # Immutable actual FOOD catalogue; NewObject/text are observed service boundaries.
  with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:cls=z.read('class.def').decode('latin1')
  section=re.search(r'CLASS\s+"FOOD"(.*?)(?=\bCLASS\s+"|\Z)',cls,re.S)[1]
  self.food_names=re.findall(r'^\s*"([^\"]+)"\s+"[^\"]+"\s+0x',section,re.M);assert len(self.food_names)==19
  food=v.allocate(0x80);v.put_u32(food+0x24,len(self.food_names));v.put_u32(food+0x34,self.bytes(struct.pack('<19I',*[self.bytes(struct.pack('<I',self.put_string(n)))for n in self.food_names])))
  v.put_u32(0x65a158,food);v.put_u32(0x65a258,5);self.food_requests=[]
  def new_food(uc,*_):
   sp=uc.reg_read(UC_X86_REG_ESP);definition=v.u32(sp+4);self.food_requests.append(bytes(v.uc.mem_read(definition,52)).hex());self.ret(0,8)
  v.uc.hook_add(UC_HOOK_CODE,new_food,begin=0x450e40,end=0x450e40)
  # All text formatting remains downstream of the real food creation/RNG.
  v.uc.hook_add(UC_HOOK_CODE,lambda uc,*args:uc.reg_write(UC_X86_REG_EIP,0x4e002d),begin=0x4dff01,end=0x4dff01)
  v.put_u32(0x5d79e4,1);v.rng_seed=1;v.call(0x4dfdf0,this=self.owner);v.call(0x4e0040,this=self.animator);native_owner(self)
 def state(self):
  v=self.vm;a=self.animator;n=v.u32(a+0x3034)
  slots=[list(struct.unpack('<20I',v.uc.mem_read(a+0x154+i*0x50,0x50)))for i in range(n)]
  return dict(alive=not bool(v.u32(self.owner+8)&0x1000),life=v.u32(a+0x3044),scale=struct.unpack('<f',v.uc.mem_read(a+0x150,4))[0],capacity=n,owner_counter=v.u32(self.owner+0x188),food_requests=list(self.food_requests),slots=slots,seed=v.rng_seed,rng_draws=self.rng_draws)
 def advance(self):
  self.tick+=1;v=self.vm
  if not self.state()['alive']:return
  v.call(0x470cc0,this=self.owner);v.call(0x4dfe30,this=self.owner)
  if not self.state()['alive']:return
  v.call(0x4e0260,this=self.animator);v.put_u32(self.animator+0x14,struct.unpack('<h',v.uc.mem_read(self.owner+0x5c,2))[0]);native_owner(self)
 def render(self):
  self.draws=[];self.software.clear()
  if self.state()['alive']:self.vm.call(0x4e0650,this=self.animator,instruction_limit=100000000)
  return self.vm.surface_bytes('screen')

def run(exe,archive,output):
 output.mkdir(parents=True,exist_ok=False)
 with zipfile.ZipFile(archive)as z:
  cls=z.read('class.def');binding=re.findall(r'^\s*"Createfood"\s+"([^\"]+)"\s+(0x[0-9a-fA-F]+)',cls.decode('latin1'),re.M)
  assert binding==[('Magic\\Createfood.I3D','0x838cffba')]
  data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/createfood.i3d'))
 f=CreatefoodScene(exe,data);v=f.vm;f.raster_enabled=False;rows=[];reads=[];rendering=False
 def observe_vector(uc,*_):
  if not rendering:return
  sp=uc.reg_read(UC_X86_REG_ESP);angle,distance=struct.unpack('<2i',v.uc.mem_read(sp+4,8));yi=angle-128 if angle>128 else 128-angle
  addresses=[0x634d44+angle*2,0x634f44+yi*2]
  reads.append(dict(tick=f.tick,angle=angle,distance=distance,addresses=list(map(hex,addresses)),spills_to_mmx=any(0x635144<=a<0x635544 for a in addresses)))
 h=v.uc.hook_add(UC_HOOK_CODE,observe_vector,begin=0x46db20,end=0x46db20)
 for tick in range(85):
  if tick:f.advance()
  before=f.state();rendering=True;f.render();rendering=False;assert before==f.state()
  rows.append(dict(tick=tick,alive=before['alive'],owner_counter=before['owner_counter'],animator_tick=before['life'],rng_seed=before['seed'],rng_draws=before['rng_draws'],pool_sha256=sha(struct.pack('<'+str(75*20)+'I',*[w for slot in before['slots']for w in slot])),draws=len(f.draws)))
 v.uc.hook_del(h);assert len(f.food_requests)==1 and rows[79]['alive'] and not rows[80]['alive']
 spills=[r for r in reads if r['spills_to_mmx']];assert spills
 point=v.allocate(12);color=v.allocate(4);v.write(color,bytes([255,255,255,0]));controls=[]
 for ambient in(0,32,128):
  v.put_u32(0x635544,ambient)
  for i in range(4):v.call(0x41da10,(i,color,28),instruction_limit=20000000)
  table=bytes(v.uc.mem_read(0x635144,1024));samples=[]
  for angle in(255,256,383,384,475,512,600,700):
   v.call(0x46db20,(angle,50,point,0));samples.append(dict(angle=angle,position=list(struct.unpack('<3i',v.uc.mem_read(point,12)))))
  controls.append(dict(ambient=ambient,light_colors=[[255]*3]*4,multipliers=[28]*4,mmx_table_sha256=sha(table),samples=samples))
 assert len(set(c['mmx_table_sha256']for c in controls))==3
 assert all(c['samples'][0]['position']==[-1,-50,0]for c in controls)
 assert len(set(tuple(c['samples'][6]['position'])for c in controls))==3
 report=dict(status='native_lifecycle_feasible_dynamic_table_boundary_deferred',accepted=False,visual_pass=False,name='Createfood',type_id='0x838cffba',asset_sha256=sha(data),class_def_sha256=sha(cls),executable_sha256=sha(exe.read_bytes()),probe_sha256=sha(Path(__file__).read_bytes()),dispatch=f.dispatch,full_color_initializer='0x41dff0',table_producer='0x41da10',vector='0x46db20',dist_x='0x634d44',dist_y='0x634f44',adjacent_dynamic_mmx_table=['0x635144','0x635544'],food_catalogue=f.food_names,food_requests=f.food_requests,lifecycle=rows,vector_calls=reads,spill_count=len(spills),first_spill=spills[0],controls=controls,scope='Actual93-constructor registry/leaf factory, complete original owner Init/Pulse and leaf Init/Animate/full RenderObject each live tick through kill80. Exact own decoded resources/COM one-frame textures; silent audio/base animation/owner script+flag notifications and observed FOOD NewObject/text boundaries. Full original MakeColorTables executes with16bpp display facade. Actual SetLightColor changes adjacent MMX lookup words while valid-angle control staysfixed. No actual GPU/source conversion/appearance acceptance; earlier partial-initialization images are diagnostic only. Global ConvertToVector remains unchanged.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],'spill_count',len(spills),'first',spills[0]);print(controls);return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path);p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.executable,a.archive,a.output)
