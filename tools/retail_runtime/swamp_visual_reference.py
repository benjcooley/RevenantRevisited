#!/usr/bin/env python3
"""Own Swamp default owner, controller, complete Render and original software pixels."""
import argparse,json,struct,zipfile
from pathlib import Path
from fgeyser_visual_reference import FGeyserScene
from simple_effect_preflight import native_dispatch
from speed_controller_preflight import ROOT,sha
from software_probe import save_rgb565_png
ASSET_SHA='8fffedd844d739286531d3799b5cc93d8fb6f5fd2bd517f2f713aef6a07cc614'
class SwampScene(FGeyserScene):
 def __init__(self,exe,data):
  assert sha(data)==ASSET_SHA;super().__init__(exe,data);before=self.state();registrations=native_dispatch(self.vm);assert before==self.state();self.dispatch=dict(complete_constructors=93,registrations=registrations);builder=self.vm.call(0x40dca0,(self.put_string('Swamp'),));assert builder==0x5e8508 and self.vm.u32(self.vm.u32(builder))==0x40dc00
  assert len(self.asset['objects'])==30 and self.asset['state0_frames']==56 and self.asset['tags'][0]['name']=='partsys' and self.asset['tags'][1]['parameters']=='litalpha'
def run(out,stride=1):
 out.mkdir(parents=True,exist_ok=False);exe=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe'
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/swamp.i3d'))
 f=SwampScene(exe,data);initial=f.state();rows=[]
 for tick in range(60):
  f.step();before=f.state();draw=(tick+1)%stride==0;f.raster_enabled=draw and stride==1 and tick in(1,3,8,9,15,25,40,53,54,55,59)
  if draw:f.animate();raw=f.render()
  else:f.draws=[];f.software.clear();raw=f.vm.surface_bytes('screen')
  row=dict(tick=tick,before=before,after=f.state(),rendered=draw,draw_count=len(f.draws))
  if f.raster_enabled:
   p=out/f'native-{tick:03}.png';save_rgb565_png(p,raw,1024,768);row.update(image=str(p.resolve()),image_sha256=sha(p.read_bytes()),rgb565_sha256=sha(raw),pixels=sum(v!=(0,)for v in struct.iter_unpack('<H',raw)),draws=f.draws);print(tick,row['pixels'],row['draw_count'],flush=True)
  rows.append(row)
 report=dict(status='reference_complete',accepted=False,name='Swamp',type_id='0xbecffeda',asset_sha256=ASSET_SHA,executable_sha256=sha(exe.read_bytes()),dispatch=f.dispatch,initial=initial,records=rows,render_stride=stride,viewport=[1024,768],screen_center=[512,640],quality=0,ground_height=1,source_ambient=[38]*3,directional=[1]*3,probe_sha256=sha(Path(__file__).read_bytes()),scope='Exact Swamp default registry/factory, own30object/three-texture56frame resource; actualNextFrame/effectPulse/controllers/defaultRender/RenderObject/normal producer/software raster. Single31-slot fgeys/#sparks controller then originaltexture-major ARGB4444 world surfaces and litalpha flags, nullspellkill56. Fullimmutable resource/COM, baseowner/script/mapnotifications/flatground1/declaredlight/camera boundaries. No actor/device/exactpixel or production acceptance.');(out/'trace.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);p.add_argument('--render-stride',type=int,default=1);a=p.parse_args();run(a.output,a.render_stride)
