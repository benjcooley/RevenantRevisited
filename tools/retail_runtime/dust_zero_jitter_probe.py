import argparse,json,zipfile
"""Original Dust explicit-zero position jitter versus omitted field."""
from pathlib import Path
from dust_visual_reference import DustScene,ROOT,sha
parser=argparse.ArgumentParser();parser.add_argument('executable',type=Path);parser.add_argument('--output',type=Path,required=True);args=parser.parse_args()
p=args.output;p.mkdir(parents=True,exist_ok=True)
with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read('Imagery/Misc/dustcloud.I3D')
refs=[]
for omit in(False,True):
 f=DustScene(args.executable,data,omit_zero_jitter=omit);states=[f.state()];draws=[];f.raster_enabled=False
 for tick in range(45):f.step();f.animate();f.render();states.append(f.state());draws.append(f.draws)
 r=dict(states=states,draws=draws);(p/('omitted.json'if omit else'explicit.json')).write_text(json.dumps(r,separators=(',',':')));refs.append(r)
print('EXACT',refs[0]==refs[1]);assert refs[0]==refs[1]
(p/'manifest.json').write_text(json.dumps(dict(status='pass',states=46,frames=45,raw_pool_and_draw_packets_identical=True,asset_sha256=sha(data),variants=[dict(path=str(f),sha256=sha(f.read_bytes()))for f in(p/'explicit.json',p/'omitted.json')],scope='Only actual decoded tag replaces posjitter=0:0, with equal-length whitespace; original parser, Init/Pulse/Render execute. Geometry, RNG, scalar jitter, emitter poses unchanged.'),indent=2)+'\n')
