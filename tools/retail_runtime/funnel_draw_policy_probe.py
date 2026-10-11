"""Original full-leaf base winding and additive scene-state control."""
import sys,struct,zipfile,json,hashlib,argparse
from pathlib import Path
sys.path.insert(0,'tools/retail_runtime')
from funnel_visual_reference import FunnelScene
from software_probe import save_rgb565_png
class Filter(FunnelScene):
 def observe_draw(self,uc,*args):
  enabled=self.raster_enabled;self.raster_enabled=enabled and self.current_object<4
  super().observe_draw(uc,*args);self.raster_enabled=enabled
p=argparse.ArgumentParser();p.add_argument('exe',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True)
with zipfile.ZipFile('data/imagery.rvi')as z:data=z.read('Imagery/Magic/funnel.i3d')
f=Filter(a.exe,data);f.raster_enabled=False
for tick in range(21):f.step();f.animate();f.render()
v=f.vm;p=v.u32(f.imagery+0x30);raw=bytes(v.uc.mem_read(p,f.asset['faces']*6));reverse=b''.join(struct.pack('<3H',a,c,b)for a,b,c in struct.iter_unpack('<3H',raw))
f.raster_enabled=True;results=[]
for name,faces in[('original',raw),('reverse',reverse)]:
 v.write(p,faces);pixels=f.render();save_rgb565_png(a.output/('base-'+name+'.png'),pixels,1280,960);results.append((name,sum(x!=0 for x in struct.unpack('<'+'H'*(len(pixels)//2),pixels)),hashlib.sha256(pixels).hexdigest()))
states=[d['states']for d in f.draws if d['object']<4];assert all(s[22]==1 and s[14]==0 and s[19]==2 and s[20]==2 for s in states)
r=dict(status='pass'if results[0][1]>0 and results[0][1:]==results[1][1:]else'fail',images=results,scene_states=states,scope='Original full Render with only the four base draws rasterized; reverse every original triangle as a negative control. Original CULL_NONE, ONE/ONE and no depth writes execute. Both windings produce identical nonempty pixels; no shader brightness fitting.')
(a.output/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');print(r['status'],results)
