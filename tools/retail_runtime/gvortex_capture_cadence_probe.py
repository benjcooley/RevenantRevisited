"""Prove sparse PNG output retains the same full-rate GVortex rendering."""
import argparse,hashlib,json,re
from pathlib import Path
from PIL import Image
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--evidence',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args()
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
def pin(p):return dict(path=str(p.resolve()),sha256=sha(p))
full=a.evidence/'gvortex-metal-final/lower/native-domain';sparse=a.evidence/'gvortex-metal-sparse/lower/native-domain'
f=json.loads((full/'manifest.json').read_text());s=json.loads((sparse/'manifest.json').read_text())
assert f['status']==s['status']=='pass' and f['binary_sha256']==s['binary_sha256']
assert len(f['frames'])==260 and len(s['frames'])==33
assert '--snapstep=0.0416666666666667' in f['command'] and '--snapstep=0.0416666666666667' in s['command']
assert s['sample_simulation_ticks']==list(range(2,259,8))
rows=[]
for i,t in enumerate(s['sample_simulation_ticks']):
 x=Path(s['frames'][i]['path']);y=Path(f['frames'][t-2]['path']);assert Image.open(x).convert('RGB').tobytes()==Image.open(y).convert('RGB').tobytes()
 rows.append(dict(simulation_tick=t,original_capture=t-1,sparse=pin(x),full=pin(y),pixels_identical=True))
def renders(folder):return int(re.search(r'\[vfx\] close \(frames=(\d+)\)',(folder/'run.log').read_text())[1])
assert renders(full)==261 and renders(sparse)==258
native=[]
for view in ('lower','upper'):
 paths=[a.evidence/f'gvortex-native-{view}'/'trace.json',a.evidence/f'gvortex-native-{view}-repeat'/'trace.json',a.evidence/f'gvortex-native-sparse-{view}'/'trace.json']
 traces=[json.loads(p.read_text()) for p in paths]
 assert traces[0]['records']==traces[1]['records']
 assert all(len(t['records'])==260 and t['render_stride']==1 for t in traces)
 for r,q in zip(traces[0]['records'],traces[2]['records']):
  assert all(r[k]==q[k] for k in ('tick','before','after','rendered'))
 native.append(dict(view=view,traces=[pin(p)for p in paths],all_260_states_and_render_schedule_identical=True))
a.output.parent.mkdir(parents=True,exist_ok=True)
a.output.write_text(json.dumps(dict(status='pass',full_render_count=261,sparse_render_count=258,sparse_capture_count=33,pixel_controls=rows,native_controls=native,scope='PNG interval only changes output sampling; simulation and rendering remain24Hz. Native capture is simulation tick minus1, derived from recorded timestamps and full-film identical pixels.'),indent=2)+'\n')
print('PASS:33 exact GPU frame controls; 260 native state/render controls per view')
