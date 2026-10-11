"""Verify Maelstrom full cadence, fresh replay and instruction-budget controls."""
import argparse,hashlib,json
from pathlib import Path
from PIL import Image
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--evidence',type=Path,required=True);p.add_argument('--output',type=Path,required=True);p.add_argument('--variant',choices=['maelstrom','ymaelstrom'],default='maelstrom');a=p.parse_args();prefix=a.variant
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
pin=lambda p:dict(path=str(p.resolve()),sha256=sha(p))
names=['left','right','upper','replay'];paths=[a.evidence/f'{prefix}-native-{v}/trace.json'for v in names];traces=[json.loads(p.read_text())for p in paths]
base=traces[0];controls=[]
for name,t in zip(names,traces):
 assert t['executable_sha256']==base['executable_sha256']and t['asset_sha256']==base['asset_sha256']and t['initial']==base['initial']
 assert len(t['records'])==260 and t['render_stride']==1
 for actual,expected in zip(t['records'],base['records']):
  for key in ('tick','before','after','rendered'):assert actual[key]==expected[key],(name,actual['tick'],key)
  if actual['draws']and expected['draws']:assert actual['draws']==expected['draws'],(name,actual['tick'],'draws')
 controls.append(dict(view=name,identical_full_schedule_records=260))
images=[]
for tick in traces[-1]['sampled_captures']:
 first=a.evidence/(prefix+'-native-left')/f'native-{tick:03}.png';again=a.evidence/(prefix+'-native-replay')/first.name
 assert Image.open(first).convert('RGB').tobytes()==Image.open(again).convert('RGB').tobytes()
 images.append(dict(capture=tick,first=pin(first),fresh=pin(again),identical_rgb=True))
budget=[]
for name in names[:3]:
 old=a.evidence/f'{prefix}-native-budget100m-{name}'
 for first in sorted(old.glob('native-*.png')):
  again=a.evidence/f'{prefix}-native-{name}'/first.name
  if not again.is_file():continue # Earlier attempt saved every8; final reference every16.
  assert Image.open(first).convert('RGB').tobytes()==Image.open(again).convert('RGB').tobytes()
  budget.append(dict(view=name,original100m_prefix=pin(first),completed400m=pin(again),identical_rgb=True))
assert len(images)==6 and len(budget)==(24 if prefix=='maelstrom'else 0),(len(images),len(budget))
r=dict(status='pass',traces=[pin(p)for p in paths],full_schedule_controls=controls,fresh_pixel_controls=images,instruction_budget_controls=budget,scope='260 real Pulse/Animate/Render steps retained in all views and fresh replay; six fresh pixel frames exact.'+(' Original base100M runs failed at phase129; only their24 successful prefix frames are budget-neutral controls, not completed-run acceptance.'if prefix=='maelstrom'else' Own Y reference uses the bounded400M instruction budget; no failed base images are reused.'))
a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(r,indent=2)+'\n');print('pass',len(images),'fresh images',len(budget),'budget prefix images')
