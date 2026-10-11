"""Fixed supported viewports covering Dragon's complete genuine-world trajectory.

Every simulation tick executes the full original caller. Extra sampled views
replay the same pre-Render VM/host/CRT checkpoint and must produce identical
post-Render owner/controller/cache/random state. Canonical Render resumes the
master trajectory. No pixels are produced by host drawing.
"""
import argparse,json
from pathlib import Path
from dragonattack_visual_reference import DragonAttackScene,ROOT,sha
from software_probe import save_rgb565_png

ORIGINS=[(-256,-832*i) for i in range(16)]

def run(executable,boot_image,output,samples):
 output.mkdir(parents=True,exist_ok=False)
 f=DragonAttackScene(executable,boot_image,1920,1080,(896,480));records=[];checks=0;missing=[];viewport_cache_differences=[]
 for tick in range(110):
  f.step();before=f.state();f.center=(896,480);f.raster_enabled=tick in samples
  if tick in samples:
   f.world.checkpoint();saved_rng=f.rng_draws;saved_material=f.material_state;saved_states=dict(f.render_states)
  pixels=f.render();after=f.state();points=[(x-256,y)for x,y in f.points]
  for i in range(0,len(points),3):
   tri=points[i:i+3]
   if not any(all(x<=a<=x+1919 and y<=b<=y+1079 for a,b in tri)for x,y in ORIGINS):missing.append(dict(tick=tick,triangle=i//3,points=tri))
  images=[];empty_views=[]
  if tick in samples:
   for view,(x,y)in enumerate(ORIGINS):
    intersects=any(max(a for a,b in points[i:i+3])>=x and min(a for a,b in points[i:i+3])<=x+1919 and max(b for a,b in points[i:i+3])>=y and min(b for a,b in points[i:i+3])<=y+1079 for i in range(0,len(points),3))
    if not intersects:
     empty_views.append(view);continue
    if view:
     f.world.restore();f.rng_draws=saved_rng;f.material_state=saved_material;f.render_states=dict(saved_states)
     assert f.state()==before,(tick,view,'checkpoint did not restore caller')
     f.center=(640-x,480-y);pixels=f.render();observed=f.state();changed=[k for k in after if observed[k]!=after[k]]
     if changed:
      (output/'unexpected-state.json').write_text(json.dumps(dict(tick=tick,view=view,changed=changed,canonical=after,replay=observed),indent=2)+'\n')
      raise AssertionError((tick,view,changed))
     checks+=1
    name=f'view-{view:02}-native-{tick:03}.png';save_rgb565_png(output/name,pixels,f.width,f.height)
    images.append(dict(view=view,path=name,sha256=sha(output/name)))
   f.world.restore();f.rng_draws=saved_rng;f.material_state=saved_material;f.render_states=dict(saved_states)
   assert f.state()==before,(tick,'final checkpoint restore')
   f.center=(896,480);f.raster_enabled=False;f.render();assert f.state()==after,(tick,'canonical replay')
  records.append(dict(tick=tick,before=before,after=after,images=images,empty_views=empty_views))
  print(tick,after['state'],after['rng_draws'],flush=True)
 result=dict(effect='dragonattack',type_id='0xba09fead',viewport=[1920,1080],origins=ORIGINS,camera=[3185,3185,0],samples=samples,records=records,coverage_missing=missing,extra_render_state_checks=checks,viewport_cache_differences=viewport_cache_differences,executable_sha256=sha(executable),boot_image_sha256=sha(boot_image),scope='Fixed1x16 supported viewport grid, immutable world camera/lighting/geometry. All110 ticks checked for complete triangle containment in at least one view. Additional views replay a canonical pre-Render checkpoint and restore full VM/host state before further simulation. Every replay must match the canonical complete owner/controller/cache/CRT state, and canonical Render is executed again before simulation resumes. Natural camera/caller integration remains open. Views proven disjoint from every submitted triangle are recorded empty without writing redundant PNGs. Raw overlap differences remain open.')
 (output/'trace.json').write_text(json.dumps(result,indent=2)+'\n');assert not missing,missing[:3];return result
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--executable',type=Path,default=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe');p.add_argument('--boot-image',type=Path,required=True);p.add_argument('--output',type=Path,required=True);p.add_argument('--samples',type=int,nargs='+',default=list(range(1,98,4)));a=p.parse_args();run(a.executable,a.boot_image,a.output,a.samples)
