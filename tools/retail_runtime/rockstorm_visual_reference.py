"""Original RockStorm with an explicit named spell and stationary character target.

Only target position/radius and invoker Fighting identity are supplied. Original
owner/animator code runs; damage and reaction callbacks are observed boundaries.
"""
from cataclysm_visual_reference import *
class RockStormScene(CataclysmScene):
 owner_vtable,owner_init,registry_name,builder_vtable=0x5b033c,0x501770,'RockStorm',0x5b0538
 owner_pulse,anim_factory,anim_vtable,anim_init,anim_pulse,anim_render,builder=(0x501790,0x509020,0x5b053c,0x501810,0x501ab0,0x501ed0,0x66d148)
 def setup_resources(self):
  FireBallScene.setup_resources(self);v=self.vm
  self.target_radius=24;self.damage_events=[]
  spell=self.missile.spell;source=self.missile.source;target=self.missile.target
  v.put_u32(self.owner+0xd8,spell);v.put_u32(spell+4,source)
  action=v.allocate(0x50);v.put_u32(action,3);v.put_u32(action+0x44,target);v.put_u32(source+0xe0,action)
  variant=v.allocate(0x100);v.write(variant+4,b'Rock Storm\0');v.put_u32(spell+0x120,variant)
  target_vt=v.allocate(0x300);v.put_u32(target,target_vt);v.write(target+0x10,struct.pack('<3i',0,0,0))
  v.put_u32(target_vt+0x258,self.bytes(b'\xb8'+struct.pack('<I',self.target_radius)+b'\xc3'))
  for address,cleanup in[(0x53f560,4),(0x4d3750,8)]:
   def boundary(uc,address,size,user,cleanup=cleanup):self.damage_events.append(hex(address));self.missile.ret(cleanup)
   v.uc.hook_add(UC_HOOK_CODE,boundary,begin=address,end=address)
 def state(self):
  v=self.vm;o=self.owner;a=self.animator
  return dict(flags=v.u32(o+8),phase=v.u32(a+0x100),owner_phase=v.u32(o+0x184),level=v.u32(a+0xfc),rocks=v.u32(a+0x104),target_radius=self.target_radius,animator_hex=bytes(v.uc.mem_read(a+0xfc,0x210-0xfc)).hex(),rng_draws=self.rng_draws,seed=v.rng_seed,clock=v.u32(0x65cb38),damage_events=list(self.damage_events))
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('exe',type=Path);p.add_argument('--output',type=Path,required=True);p.add_argument('--frames',type=int,default=180);p.add_argument('--state-only',action='store_true');p.add_argument('--samples',type=int,nargs='+',default=[0,10,20,40,60,80,100,120,140,160,179]);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower().endswith('/rocks.i3d')))
 f=RockStormScene(a.exe,data);initial=f.state();records=[];print('initial',initial,flush=True)
 for tick in range(a.frames):
  f.step()
  if not(f.vm.u32(f.owner+8)&0x1000):f.animate()
  f.raster_enabled=not a.state_only and tick in a.samples;pixels=f.render()
  if f.raster_enabled:save_rgb565_png(a.output/f'native-{tick:03}.png',pixels,1280,960)
  records.append(dict(tick=tick,state=f.state(),draws=f.draws if tick in a.samples else []));print(tick,f.state()['phase'],len(f.draws),f.rng_draws,flush=True)
 (a.output/'trace.json').write_text(json.dumps(dict(initial=initial,records=records,asset_sha256=sha(data),executable_sha256=sha(a.exe.read_bytes()),dispatch=f.dispatch,scope='Declared Rock Storm level-one spell, stationary target origin/radius24 and invoker Fighting identity. Actual owner Pulse, custom Animate/full Render/normal lighting and software raster; damage/reaction, base animator Initialize, resource/COM and audio boundaries.'),indent=2)+'\n')
