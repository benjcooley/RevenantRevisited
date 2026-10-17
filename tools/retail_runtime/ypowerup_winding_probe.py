"""Execute YPowerUp's real CULL_NONE caller with both literal index windings."""
import argparse,json,struct,zipfile
from pathlib import Path
from ypowerup_visual_reference import YPowerUpScene,ROOT,sha,save_rgb565_png

def run(executable,output):
 output.mkdir(parents=True,exist_ok=True)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read('Imagery/Magic/ypowerup.I3D')
 f=YPowerUpScene(executable,data);v=f.vm;p=v.u32(f.imagery+0x30)
 original=bytes(v.uc.mem_read(p,f.asset['faces']*6))
 reverse=b''.join(struct.pack('<3H',a,c,b)for a,b,c in struct.iter_unpack('<3H',original))
 assert reverse!=original
 cases=[]
 for tick in range(55):
  f.step();f.animate();f.raster_enabled=tick in(24,54)
  first=f.render()
  if not f.raster_enabled:continue
  states=[d['states']for d in f.draws]
  assert all(s[22]==1 and s[7]==0 and s[14]==0 for s in states)
  v.write(p,reverse);second=f.render();v.write(p,original)
  assert first==second and any(first)
  for name,pixels in [('authored',first),('reversed',second)]:save_rgb565_png(output/f'{name}-{tick:03}.png',pixels,1024,768)
  cases.append(dict(tick=tick,draw_count=len(f.draws),pixels_sha256=sha(first),reversed_pixels_sha256=sha(second),states=states[0]))
 report=dict(status='pass',cases=cases,faces=f.asset['faces'],asset_sha256=sha(data),executable_sha256=sha(executable.read_bytes()),change='Swap corners 1 and 2 of every original face. All vertices, matrices, light, scene calls and textures unchanged.',scope='Original complete default Render is winding-independent for this literal RGB565 mode80 caller; no claim for unrelated effects.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();print(run(a.executable,a.output)['status'])
