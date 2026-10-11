"""Original ArrowEffect/QueenArrow moving owner and complete custom Render.

Declared moving native caller fixture; appearance acceptance is separate.
"""
from cataclysm_visual_reference import *
from unicorn.x86_const import UC_X86_REG_ESP

class QueenArrowScene(CataclysmScene):
 owner_vtable,owner_init,registry_name,builder_vtable=0x5b0fdc,0x5048f0,'QueenArrow',0x5b11e0
 owner_pulse,anim_factory,anim_vtable,anim_init,anim_pulse,anim_render,builder=(0x504d20,0x509a30,0x5b11e4,0x504fe0,0x40e2e0,0x505000,0x66cfe8)
 def setup_resources(self):
  FireBallScene.setup_resources(self);v=self.vm;v.put_u32(self.imagery+0x10,0)
  v.call(0x504d00,this=self.owner)
  self.events=[]
  v.write(self.missile.source+0x10,struct.pack('<3i',-180,0,38));v.write(self.missile.source+0x36,bytes([64]));v.put_u32(self.owner+0xe4,self.missile.source)
  # Native collision query result is an explicit stationary fixture character.
  # Actor reaction/damage are observed API boundaries, not a simulated actor.
  self.target_x=180
  target=self.missile.target;vt=v.allocate(0x300);v.put_u32(target,vt)
  method=v.api_address('kernel32.dll','ArrowTargetDamage');v.handlers['ArrowTargetDamage']=(5,lambda args:self.events.append(dict(event='damage',args=args)) or 0);v.put_u32(vt+0x228,method)
  v.write(target+0x10,struct.pack('<3i',180,0,78))
  for address in [0x4d4db0,0x4d3750]:v.uc.hook_add(UC_HOOK_CODE,self.world,begin=address,end=address)
 def world(self,uc,address,*_):
  v=self.vm
  if address==0x4d4db0:
   hit=struct.unpack('<i',v.uc.mem_read(self.owner+0x10,4))[0]>=self.target_x
   self.ret(self.missile.target if hit else 0,0)
  else:self.events.append(dict(event='reaction'));self.ret(0,8)
 def animate(self):
  self.vm.call(0x40e460,(0,),this=self.animator,instruction_limit=5000000);native_owner(self)
 def state(self):
  v=self.vm;o=self.owner
  result=dict(position=list(struct.unpack('<3i',v.uc.mem_read(o+0x10,12))),velocity=list(struct.unpack('<3f',v.uc.mem_read(o+0x184,12))),initial=v.u32(o+0x190),angle=v.u32(o+0x194),grow=v.u32(o+0x198),color=v.u32(o+0x19c),flags=v.u32(o+8),rng_draws=self.rng_draws,seed=v.rng_seed,events=list(self.events))
  if self.registry_name=='QueenArrow':result['countdown']=struct.unpack('<i',v.uc.mem_read(o+0x1a0,4))[0]
  return result

if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('exe',type=Path);p.add_argument('--output',type=Path,required=True);p.add_argument('--frames',type=int,default=40);p.add_argument('--state-only',action='store_true');p.add_argument('--render-stride',type=int,default=1);p.add_argument('--resolution',type=int,nargs=2,default=[1280,960]);p.add_argument('--center',type=int,nargs=2,default=[640,640]);p.add_argument('--samples',type=int,nargs='+',default=[0,1,3,5,10,15,20,25,30,35,39]);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read('Imagery/Misc/arroweffects.i3d')
 f=QueenArrowScene(a.exe,data,width=a.resolution[0],height=a.resolution[1],center=a.center);rows=[]
 for tick in range(a.frames):
  f.step()
  if not(f.vm.u32(f.owner+8)&0x1000):f.animate()
  f.raster_enabled=not a.state_only and tick in a.samples
  if tick%a.render_stride==0:pixels=f.render()
  else:f.draws=[];f.software.clear();pixels=f.vm.surface_bytes("screen")
  if f.raster_enabled:save_rgb565_png(a.output/f'native-{tick:03}.png',pixels,*a.resolution)
  row=dict(tick=tick,state=f.state(),draws=f.draws);rows.append(row);print(tick,f.state(),len(f.draws),flush=True)
 (a.output/'trace.json').write_text(json.dumps(dict(records=rows,asset_sha256=sha(data),retail_sha256=sha(a.exe.read_bytes()),viewport=a.resolution,center=a.center,source=[-180,0,38],target=[180,0,78],ambient=[38,38,38],directional=[1,1,1],seed=1,render_hz=24,render_stride=a.render_stride,scope='Original QueenArrow complete owner Pulse and custom Render/normal lighting/software raster. Declared non-character launch face64 and stationary target-plane query; reaction/damage, resource/COM, base animator Initialize and map/script/audio are explicit boundaries.'),indent=2)+'\n')
