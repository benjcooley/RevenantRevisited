"""Explain Dust's blank native software output from literal texels/draw colors."""
import argparse,json,struct,zipfile
from pathlib import Path
from speed_profile_contract import ROOT,sha
from fireball_visual_reference import read_fireball
p=argparse.ArgumentParser();p.add_argument('draw_trace',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args()
with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read('Imagery/Misc/dustcloud.I3D')
asset=read_fireball(data);u=lambda p:struct.unpack_from('<I',data,p)[0];r=lambda p:p+u(p);tex=r(asset['body_offset']+40);px=r(r(tex+108))
texels=set(struct.unpack_from('<1024H',data,px));trace=json.loads(a.draw_trace.read_text());colors=set()
for draws in trace['draws']:
 for draw in draws:
  assert draw['arguments'][1]==0x1e2
  v=bytes.fromhex(draw['vertices_hex']);colors.update(struct.unpack_from('<I',v,i+16)[0]for i in range(0,len(v),32))
outputs=set()
for t in texels:
 for c in colors:
  texture=((t>>11)&31,(t>>6)&31,t&31);light=((c>>19)&31,(c>>11)&31,(c>>3)&31)
  m=[x*y//31 for x,y in zip(texture,light)];outputs.add(((m[0]&~1)<<11)|(m[1]<<6)|(m[2]&~1))
assert colors and outputs=={0},outputs
report=dict(status='pass',asset_sha256=sha(data),draw_trace_sha256=sha(a.draw_trace.read_bytes()),distinct_texels=len(texels),distinct_colors=len(colors),combinations=len(texels)*len(colors),output_rgb565=sorted(outputs),scope='All literal texels crossed with all actual full original Render packed particle colors. Existing native-proven RGB565 additive modulation, including low-bit clearing; no texture/color change. This explains blank software output, not a visual appearance pass.')
a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(report,indent=2)+'\n');print(report)
