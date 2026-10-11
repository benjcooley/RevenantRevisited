"""Own original YCataclysm registration, factory, owner, custom pools and full renderer."""
from cataclysm_visual_reference import *
class YCataclysmScene(CataclysmScene):
 owner_vtable,owner_init,registry_name,builder_vtable=0x5b1d8c,0x507510,"YCataclysm",0x5b1f88
 owner_pulse,anim_factory,anim_vtable,anim_init,anim_pulse,anim_render,builder=(0x507530,0x50a220,0x5b1f8c,0x5077b0,0x507a40,0x507ca0,0x66d018)

if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('exe',type=Path);p.add_argument('--output',type=Path,required=True);p.add_argument('--frames',type=int,default=90);p.add_argument('--state-only',action='store_true');p.add_argument('--render-stride',type=int,default=1);p.add_argument('--samples',type=int,nargs='+',default=[0,10,20,30,40,60,79,80,89]);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read('Imagery/Magic/YCataclysm.I3D')
 f=YCataclysmScene(a.exe,data);initial=f.state();records=[];print('initial',initial['phase'],initial['rng_draws'],flush=True)
 for tick in range(a.frames):
  f.step();active=not(f.vm.u32(f.owner+8)&0x1000)
  if active:f.animate()
  f.raster_enabled=not a.state_only and tick in a.samples
  if tick%a.render_stride==0:pixels=f.render()
  else:f.draws=[];f.software.clear();pixels=f.vm.surface_bytes('screen')
  if f.raster_enabled:save_rgb565_png(a.output/f'native-{tick:03}.png',pixels,1280,960)
  records.append(dict(tick=tick,state=f.state(),draws=f.draws if tick in a.samples else []));print(tick,f.state()['phase'],f.state()['life'],len(f.draws),f.rng_draws,flush=True)
 (a.output/'trace.json').write_text(json.dumps(dict(initial=initial,records=records,asset_sha256=sha(data),executable_sha256=sha(a.exe.read_bytes()),dispatch=f.dispatch,render_stride=a.render_stride,scope='Null-spell variant0, clean original allocation state, empty character world; actual NextFrame/owner Pulse/custom Animate/full Render/normal lighting and software raster. Resource construction, base animator Initialize, silent audio and map/script notification services remain explicit fixture boundaries.',viewport=[1280,960],center=[640,640],lighting=dict(ambient=[38,38,38],directional=[1,1,1]),owner_variant=0),indent=2)+'\n')
