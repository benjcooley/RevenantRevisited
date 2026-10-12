#!/usr/bin/env python3
"""Unmodified Aura whole/ROI panels paired by logged simulation ticks."""
import argparse,json,re,struct
from pathlib import Path
from PIL import Image,ImageDraw
from speed_profile_contract import sha

def run(native,metal,initial,output):
 output.mkdir(parents=True,exist_ok=False);n=json.loads((native/'manifest.json').read_text());m=json.loads((metal/'manifest.json').read_text());i=json.loads((initial/'manifest.json').read_text())if initial else None;assert m['status']=='pass'and len(m['frames'])==750 and m['sample_simulation_ticks']in(list(range(1,751)),list(range(2,752)));by_tick={t:f for t,f in zip(m['sample_simulation_ticks'],m['frames'])};
 if 1 not in by_tick:
  assert i and i['status']=='pass'and i['sample_simulation_ticks']==[1,2,3]and m['binary_sha256']==i['binary_sha256'];by_tick[1]=i['frames'][0]
 selected=[r for r in n['rows']if r['image']];whole=Image.new('RGB',(1280,len(selected)*505));crop=Image.new('RGB',(180,len(selected)*190));detail=Image.new('RGB',(720,len(selected)*685));pairs=[];roi=(280,170,370,335)
 for row,r in enumerate(selected):
  t=r['tick'];f=by_tick[t];paths=[Path(r['image']),Path(f['path'])];hashes=[r['image_sha256'],f['sha256']];assert[sha(p.read_bytes())for p in paths]==hashes;images=[Image.open(p).convert('RGB')for p in paths];boxes=[im.getbbox()for im in images];assert all(b is None or b[0]>0 and b[1]>0 and b[2]<640 and b[3]<480 for b in boxes);pairs.append(dict(tick=t,images=list(map(str,paths)),sha256=hashes,visible_bbox=boxes,native_draws=len(r['draws']),flags=r['flags'],owner_position=r['owner_position'],spell_defense=r['spell_defense'],spell_offense=r['spell_offense']))
  for col,(im,label)in enumerate(zip(images,('Original','Actual Metal'))):
   whole.paste(im,(col*640,row*505+25));ImageDraw.Draw(whole).text((col*640+4,row*505+5),f'{label} Aura tick{t}',fill='white');piece=im.crop(roi);crop.paste(piece,(col*90,row*190+25));ImageDraw.Draw(crop).text((col*90+2,row*190+5),f'{label} {t}',fill='white');detail.paste(piece.resize((360,660),Image.Resampling.NEAREST),(col*360,row*685+25));ImageDraw.Draw(detail).text((col*360+2,row*685+5),f'{label} tick{t} 4x nearest',fill='white')
 for name,im in [('whole.png',whole),('fixed-crop.png',crop),('nearest4x-details.png',detail)]:im.save(output/name)
 log=(metal/'run.log').read_text();errors=[];fields=0;max_error=0
 for tick,slot,x,y,z,scale,life,span in re.findall(r'\[aura-slot\] tick=(\d+) slot=(\d+) p=([0-9.eE+-]+),([0-9.eE+-]+),([0-9.eE+-]+) scale=([0-9.eE+-]+) life=(\d+) span=(\d+)',log):
  t,s=int(tick),int(slot);v=struct.unpack_from('<18f4i',bytes.fromhex(n['rows'][t-1]['fire_hex']),s*88);d=max(abs(a-b)for a,b in zip([v[0],v[1],v[2],v[3]],map(float,(x,y,z,scale))));max_error=max(max_error,d);fields+=7
  if not v[21]or d>2e-6 or [v[19],v[20]]!=[int(life),int(span)]:errors.append(dict(tick=t,slot=s,error=d))
 report=dict(status='awaiting_independent_review',accepted=False,type_id='0x00ab1d1d',binary_sha256=m['binary_sha256'],panels={name:sha((output/name).read_bytes())for name in('whole.png','fixed-crop.png','nearest4x-details.png')},pairs=pairs,actual_source_slot_fields=fields,max_source_slot_error=max_error,slot_errors=errors,native_manifest_sha256=sha((native/'manifest.json').read_bytes()),metal_manifest_sha256=sha((metal/'manifest.json').read_bytes()),initial_manifest_sha256=sha((initial/'manifest.json').read_bytes())if initial else None,configuration=m['scenario'],appearance_fitted=False,scope='Exact own Aura modern9484-byte body,4vertices/2faces/RGB565 normal-lit blue sprite. Full original named licensed Aura TSpell/genuine Acolyte SetSpell owner/controller/render through750 against actual Metal; real ctor size0 gives random(0,-1), skips negative objects and uses genuine object0 bones. Original assigns spread-only birthZ and owner-relative Render(false,false); snapshot alternatives not used. Integer/no-transition actor14/0/facing96; finite emission720→721/kill744/blanktail745/750. Pairs use absolute logged source ticks from the frozen manifests; final film warmup0 is1..750. Earlier adjacent-state first-whole-four.png is superseded. FixedROI280,170,370,335/4xnearest display only; bboxes interior. Original genericworldPulse explicitlyupstream; no natural bone-limit initialization/caller/movingactor/map/audio/device/exactpixel certification.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],len(pairs),fields,max_error,report['binary_sha256']);return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--native',type=Path,required=True);p.add_argument('--metal',type=Path,required=True);p.add_argument('--initial',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.native,a.metal,a.initial,a.output)
