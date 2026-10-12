"""Freeze matched Dragonattack raw tile pairs and readable review contacts."""
import argparse,json,hashlib,math,shutil
from pathlib import Path
from PIL import Image,ImageDraw
from dragonattack_capture_witness import run as witness

def sha(path):return hashlib.sha256(Path(path).read_bytes()).hexdigest()
def run(native,metal,canonical,output):
 output.mkdir(parents=True,exist_ok=False);reference=json.loads((native/'trace.json').read_text());assert not reference['coverage_missing']
 assert reference['viewport']==[1920,1080]
 canonical_trace=json.loads(canonical.read_text())
 assert len(reference['records'])==len(canonical_trace['records'])==110
 for actual,baseline in zip(reference['records'],canonical_trace['records']):
  assert actual['tick']==baseline['tick']
  assert actual['before']==baseline['before'] and actual['after']==baseline['after'],actual['tick']
 manifests={};frames={};reports={}
 for directory in sorted(metal.glob('view-*')):
  if not directory.is_dir():continue
  view=int(directory.name.split('-')[1]);m=json.loads((directory/'manifest.json').read_text());assert m['status']=='pass' and m['actual_viewport']==[1920,1080]
  assert m['scenario']['origin']==[3185,3185,16] and m['depth_range']==[0,65535]
  manifests[view]=m;frames[view]={tick-1:Path(f['path'])for tick,f in zip(m['sample_simulation_ticks'],m['frames'])}
  reports[view]=witness(canonical,directory/'run.log',directory/'state-witness.json');assert reports[view]['status']=='pass'
 pairs=[];panels=[];main=[]
 for record in reference['records']:
  tick=record['tick']
  if tick not in reference['samples']:continue
  for sample in record['images']:
   view=sample['view'];a=native/sample['path'];b=frames[view][tick]
   with Image.open(a)as src:ia=src.convert('RGB')
   with Image.open(b)as src:ib=src.convert('RGB')
   assert ia.size==ib.size==(1920,1080)
   boxes=[x for x in(ia.getbbox(),ib.getbbox())if x]
   if boxes:
    box=(max(0,min(x[0]for x in boxes)-12),max(0,min(x[1]for x in boxes)-12),min(1920,max(x[2]for x in boxes)+12),min(1080,max(x[3]for x in boxes)+12))
   else:box=(0,0,256,256)
   pa,pb=ia.crop(box),ib.crop(box);panel=Image.new('RGB',(pa.width*2,pa.height+24),(24,24,24));panel.paste(pa,(0,24));panel.paste(pb,(pa.width,24));ImageDraw.Draw(panel).text((6,5),f'record {tick} / view {view}: original | Metal',fill='white')
   name=f'pair-{tick:03}-view-{view:02}.png';panel.save(output/name)
   pairs.append(dict(record=tick,view=view,origin=reference['origins'][view],crop=box,original=dict(path=str(a),sha256=sha(a)),metal=dict(path=str(b),sha256=sha(b)),panel=dict(path=name,sha256=sha(output/name))))
   panels.append((tick,view,panel))
   if view==0:main.append((tick,panel))
 # Actual original/Metal1280 lifetime controls retain empty start and tail.
 baseline_dir=metal.parent/'dragonattack-metal-first';baseline=json.loads((baseline_dir/'manifest.json').read_text());tails=[]
 assert baseline['binary_sha256']==next(iter(manifests.values()))['binary_sha256']
 baseline_frames={tick-1:Path(f['path'])for tick,f in zip(baseline['sample_simulation_ticks'],baseline['frames'])}
 for tick in(1,65,85,97):
  a=canonical.parent/f'native-{tick:03}.png';b=baseline_frames[tick]
  ia=Image.open(a).convert('RGB');ib=Image.open(b).convert('RGB');assert ia.size==ib.size==(1280,960) and ia.getbbox() is None and ib.getbbox() is None
  tails.append(dict(record=tick,viewport=[1280,960],original=dict(path=str(a),sha256=sha(a)),metal=dict(path=str(b),sha256=sha(b))))
  panel=Image.new('RGB',(512,200),(0,0,0));ImageDraw.Draw(panel).text((6,5),f'record {tick}: original | Metal; actual1280 blank control',fill='white');main.append((tick,panel))
 main.sort(key=lambda x:x[0])
 # Main scene life plus separate distant impact tiles; keep raw viewport files.
 for name,items in [('main-life',[(t,p)for t,p in main]),('distant-impact',[(t,p)for t,v,p in panels if v!=0])]:
  columns=3;cellw,cellh=520,260;contact=Image.new('RGB',(columns*cellw,math.ceil(max(1,len(items))/columns)*cellh),(30,30,30))
  for i,(tick,panel)in enumerate(items):
   panel=panel.copy();panel.thumbnail((cellw,cellh),Image.Resampling.NEAREST);contact.paste(panel,((i%columns)*cellw,(i//columns)*cellh))
  contact.save(output/(name+'.png'))
 result=dict(status='ready_for_independent_review',effect='dragonattack',type_id='0xba09fead',native_trace=dict(path=str(native/'trace.json'),sha256=sha(native/'trace.json')),pairs=pairs,lifetime_controls=tails,empty_views=[dict(record=r['tick'],views=r['empty_views'])for r in reference['records']if r['tick']in reference['samples']],binary_sha256=next(iter(manifests.values()))['binary_sha256'],metal_manifests={v:dict(path=str(metal/f'view-{v:02}'/'manifest.json'),sha256=sha(metal/f'view-{v:02}'/'manifest.json'))for v in manifests},witness_fields=sum(r['checks']for r in reports.values()),coverage_missing=reference['coverage_missing'],scope='Genuine Acolyte Fighting/Sid Dragon Attack, source3185,3185,16,target3465,3185,ground0; nonresident actors, original typed owner and loaded resource. Fixed supported viewport snapshots from same pre-Render caller/CRT state; no fitted geometry/color/phase. Natural dragon/bones/map/combat/audio/device, overlap/quantization and exact framebuffer equality remain open.')
 assert len({m['binary_sha256']for m in manifests.values()})==1
 diagnostics={}
 for name,relative in [('common_policy','dragonattack-metal-common/manifest.json'),('head_default_regression','dragonattack-head-regression/pixel-regression.json')]:
  path=metal.parent/relative;data=json.loads(path.read_text());assert data['status']=='pass'
  diagnostics[name]=dict(path=str(path),sha256=sha(path))
 result['diagnostics']=diagnostics
 result['source_commits']=['75189e60','260defc3']
 result['canonical_full_state_replay_records']=110
 result['extra_render_state_checks']=reference['extra_render_state_checks']
 result['contacts']={name:dict(path=name+'.png',sha256=sha(output/(name+'.png')))for name in ('main-life','distant-impact')}
 binary=Path(next(iter(manifests.values()))['scenario']['binary']);assert sha(binary)==result['binary_sha256']
 shutil.copy2(binary,output/'Revenant-captured')
 result['frozen_binary']=dict(path='Revenant-captured',sha256=sha(output/'Revenant-captured'))
 (output/'pair-manifest.json').write_text(json.dumps(result,indent=2)+'\n');print('pairs',len(pairs),'witness',result['witness_fields']);return result
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--native',type=Path,required=True);p.add_argument('--metal',type=Path,required=True);p.add_argument('--canonical',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.native,a.metal,a.canonical,a.output)
