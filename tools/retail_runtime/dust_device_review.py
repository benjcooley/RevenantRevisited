"""Freeze qualified Dust device/actual Metal pairs by recorded simulation tick."""
import argparse, csv, hashlib, json
from pathlib import Path
from PIL import Image, ImageDraw

def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def review(reference,repeat,metal,software,control,out):
 out.mkdir(parents=True,exist_ok=False);r=json.loads(reference.read_text());again=json.loads(repeat.read_text());m=json.loads(metal.read_text());s=json.loads(software.read_text());c=json.loads(control.read_text());assert m['status']=='pass'
 assert (s['status']=='fail' and s.get('error')=='No visible effect pixels in the black-background capture') or (s['status']=='pass' and s['scenario'].get('expect_visible') is False)
 assert s['exit_code']==0 and not s['forced_owned_shutdown'] and len(s['frames'])==25
 assert s['pixel_summary']['nonblack_frames']==0 and c['states_exact']==25 and c['software_blank']==25
 assert m['scenario']['resolution']==[640,480]
 assert len(r['frames'])==len(again['frames'])==25
 for a,b in zip(r['frames'],again['frames']):
  assert a['tick']==b['tick']and a['state']==b['state']and a['packets']==b['packets']
  for mode in ('point','linear'):assert Image.open(a['images'][mode]['path']).tobytes()==Image.open(b['images'][mode]['path']).tobytes()
 rows=list(csv.DictReader((metal.parent/'frame-timing.csv').open()));assert len(rows)==len(m['frames'])==25
 records=[];pairs=[];pins={}
 def pin(p):p=Path(p);pins[str(p.resolve())]=sha(p)
 for p in(reference,repeat,metal,software,control):pin(p)
 capture_source=reference.parent/'capture-source.py'
 if capture_source.exists():assert sha(capture_source)==r['tool_sha256'];pin(capture_source)
 for frame in s['frames']:pin(frame['path'])
 for row,frame in zip(rows,m['frames']):
  tick=round(float(row['simulation_seconds'])*24);assert abs(tick-float(row['simulation_seconds'])*24)<1e-6
  pin(frame['path'])
  if tick>=len(r['frames']):
   assert not Image.open(frame['path']).convert('RGB').getbbox();continue
  n=r['frames'][tick];left=Image.open(n['images']['linear']['path']).convert('RGB');right=Image.open(frame['path']).convert('RGB');assert left.size==right.size==(640,480)
  for mode in('point','linear'):pin(n['images'][mode]['path'])
  pair=Image.new('RGB',(1280,504));pair.paste(left,(0,24));pair.paste(right,(640,24));ImageDraw.Draw(pair).text((8,5),f'tick {tick} | qualified original hardware semantics, linear sampler / actual Metal modern policy',fill='white');path=out/f'pair-{tick:03}.png';pair.save(path);pin(path)
  records.append(dict(tick=tick,native=n['images']['linear']['path'],point_control=n['images']['point']['path'],metal=frame['path'],pair=str(path),original_owner_frame=n['state']['frame'],flags=n['state']['flags']))
  pairs.append((tick,left,right))
 whole=Image.new('RGB',(1280,math_ceil(len(pairs)/4)*144))
 for i,(tick,left,right)in enumerate(pairs):
  x=(i%4)*320;y=(i//4)*144;whole.paste(left.resize((160,120)),(x,y+24));whole.paste(right.resize((160,120)),(x+160,y+24));ImageDraw.Draw(whole).text((x+4,y+5),f'tick {tick}',fill='white')
 whole.save(out/'whole-life.png');pin(out/'whole-life.png')
 # Identical fixed crop on both sides, integer2x nearest enlargement only.
 crop=(180,200,460,450)
 for page in range(math_ceil(len(pairs)/6)):
  contact=Image.new('RGB',(1120,6*524))
  for j,(tick,left,right)in enumerate(pairs[page*6:page*6+6]):
   y=j*524;ImageDraw.Draw(contact).text((8,y+5),f'tick {tick}: qualified / Metal',fill='white');contact.paste(left.crop(crop).resize((560,500),Image.Resampling.NEAREST),(0,y+24));contact.paste(right.crop(crop).resize((560,500),Image.Resampling.NEAREST),(560,y+24))
  path=out/f'detail-{page+1}.png';contact.save(path);pin(path)
 report=dict(status='ready-for-independent-review',accepted=False,qualified_reference=True,unchanged_original_device=False,binary_sha256=m['binary_sha256'],reference_sha256=sha(reference),source_film_sha256=sha(metal),repeat_exact_frames=25,software_blank_control_frames=25,hardware_vs_software_state_pool_rng_exact=25,phase_rule='round(recorded simulation_seconds *24), no fitted offset',scope='Original loaded Dust owner/controllers/hardware draw packets and RGB565 COM texture. Original software transform supplies native-domain projected coordinates. Independent declared RGB8 UNORM triangle device implements MODULATE and ONE/ONE, point and linear controls; not period hardware or unchanged original software.',limits=['Original software remains mathematically blank','Hardware startup/filter and target precision are explicitly qualified','Integer sample coverage/interpolation/rounding can differ from Metal','Natural caller/map/audio/period device/exact pixels not certified'],pairs=records,pins=pins)
 (out/'pair-manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(sha(out/'pair-manifest.json'))
def math_ceil(x):return int(-(-x//1))
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__)
 for name in('reference','repeat','metal','software','control','output'):p.add_argument('--'+name,type=Path,required=True)
 a=p.parse_args();review(a.reference,a.repeat,a.metal,a.software,a.control,a.output)
