"""Original YTelekinesis, declared stationary player position and null spell."""
from cataclysm_visual_reference import *
class YTelekinesisScene(CataclysmScene):
 owner_vtable,owner_init,registry_name,builder_vtable=0x5b18b4,0x505f50,'YTelekinesis',0x5b1ab4
 owner_pulse,anim_factory,anim_vtable,anim_init,anim_pulse,anim_render,builder=(0x505f60,0x509eb0,0x5b1ab8,0x505f90,0x506240,0x506510,0x66d140)
 def setup_resources(self):
  FireBallScene.setup_resources(self);v=self.vm
  self.player=self.missile.target;v.write(self.player+0x10,struct.pack('<3i',0,0,0));v.put_u32(0x667fcc,self.player)
 def render(self):
  self.vm.call(0x417d60,(1,1),this=0x65a57c) # Original normal-scene caller state, including its cache.
  return super().render()
 def state(self):
  v=self.vm;a=self.animator
  rocks=[]
  for i in range(v.u32(a+0x3b0)):
   vector=lambda off:list(struct.unpack('<3f',v.uc.mem_read(a+off+i*32,12)))
   rocks.append(dict(position=vector(0xfc),velocity=vector(0x17c),rotation=vector(0x1fc),rotation_velocity=vector(0x27c),scale=vector(0x2fc),dead=v.u32(a+0x37c+4*i),hit=v.u32(a+0x38c+4*i),mesh=v.uc.mem_read(a+0x39c+i,1)[0],angle=v.u32(a+0x3a0+4*i),life=v.u32(a+0x3b4+4*i)))
  return dict(flags=v.u32(self.owner+8),owner_position=list(struct.unpack('<3i',v.uc.mem_read(self.owner+0x10,12))),rocks=rocks,rng_draws=self.rng_draws,seed=v.rng_seed)
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('exe',type=Path);p.add_argument('--output',type=Path,required=True);p.add_argument('--frames',type=int,default=180);p.add_argument('--state-only',action='store_true');p.add_argument('--samples',type=int,nargs='+',default=[0,10,20,30,40,50,60,75,90,110,130,150,170,179]);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/misc/chunk.i3d'))
 f=YTelekinesisScene(a.exe,data);initial=f.state();records=[];print('initial',initial,flush=True)
 for tick in range(a.frames):
  f.step()
  if not(f.vm.u32(f.owner+8)&0x1000):f.animate()
  f.raster_enabled=not a.state_only and tick in a.samples;pixels=f.render()
  if f.raster_enabled:save_rgb565_png(a.output/f'native-{tick:03}.png',pixels,1280,960)
  records.append(dict(tick=tick,state=f.state(),draws=f.draws if tick in a.samples else []));print(tick,[r['life']for r in f.state()['rocks']],len(f.draws),f.rng_draws,flush=True)
 (a.output/'trace.json').write_text(json.dumps(dict(initial=initial,records=records,asset_sha256=sha(data),executable_sha256=sha(a.exe.read_bytes()),dispatch=f.dispatch,scope='Stationary global player-position fixture at origin and null spell. Actual owner Pulse, custom Init/Animate/full Render/normal lighting/software raster. No natural player/actor/damage caller claim; resource/base initialization/map notification remain boundaries.',viewport=[1280,960],center=[640,640]),indent=2)+'\n')
