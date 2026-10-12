"""Freeze four-cardinal Fire qualified device/Metal comparisons without fitting."""
import argparse,csv,json,re
from pathlib import Path
from PIL import Image,ImageDraw
from character_native_preflight import sha

def review(native,repeat,films,checkpoint,out):
 out.mkdir(parents=True,exist_ok=False);n=json.loads(native.read_text());r=json.loads(repeat.read_text());cp=json.loads(checkpoint.read_text());assert len(n['records'])==len(r['records'])==12
 rows=[];pins={};matrix_checks=0;maximum=0.
 def pin(p):p=Path(p);pins[str(p.resolve())]=sha(p)
 for p in(native,repeat,checkpoint):pin(p)
 for a,b in zip(n['records'],r['records']):assert(a['facing'],a['tick'],a['state'],a['packets'])==(b['facing'],b['tick'],b['state'],b['packets'])
 for face in(0,64,128,192):
  manifest=films/f'face{face}'/'sparse'/'manifest.json';film=json.loads(manifest.read_text());pin(manifest);pin(manifest.parent/'run.log');pin(manifest.parent/'frame-timing.csv')
  assert film['status']=='pass'and film['exit_code']==0 and not film['forced_owned_shutdown'];assert film['binary_sha256']==cp['binary_sha256']
  timing=list(csv.DictReader((manifest.parent/'frame-timing.csv').open()));assert len(timing)==len(film['frames'])==3
  source=re.findall(r'\[fire-installed-phase\] tick=(\d+) frame=(\d+) face=(\d+) rng=(\d+) matrix=([^\n]+)',(manifest.parent/'run.log').read_text());assert len(source)==67
  original=next(x for x in n['records']if x['facing']==face);import struct;matrix=struct.unpack('<16f',bytes.fromhex(original['packets'][0]['world_matrix_hex']))
  for ordinal,(tick,frame,facing,rng,values)in enumerate(source,1):
   assert(int(tick),int(frame),int(facing),int(rng))==(ordinal,0,face,0)
   values=list(map(float,values.split(',')));delta=max(abs(a-b)for a,b in zip(matrix,values));assert delta<1e-6;maximum=max(maximum,delta);matrix_checks+=16
  for clock,source in zip(timing,film['frames']):
   steps=round(float(clock['simulation_seconds'])*24);assert abs(steps-float(clock['simulation_seconds'])*24)<1e-6
   tick=steps-1 # native record0 follows the first actual NextFrame/Pulse
   row=next(x for x in n['records']if x['facing']==face and x['tick']==tick);actual=Image.open(source['path']).convert('RGB');expected=Image.open(row['images']['modern-point']['path']).convert('RGB');assert actual.size==expected.size==(1280,960);assert actual.tobytes()==expected.tobytes(),(face,tick)
   for item in row['images'].values():pin(item['path'])
   pin(source['path']);pair=Image.new('RGB',(2560,984));pair.paste(expected,(0,24));pair.paste(actual,(1280,24));ImageDraw.Draw(pair).text((8,5),f'Fire facing{face} step{steps}: qualified modern-center/point device | actual unchanged source Metal',fill='white');path=out/f'pair-face{face:03}-tick{tick:03}.png';pair.save(path);pin(path)
   rows.append(dict(facing=face,native_tick=tick,source_steps=steps,exact_rgb_pixels=True,native=row['images']['modern-point']['path'],source=source['path'],pair=str(path),pixels=row['images']['modern-point']['pixels']))
 # Raw40x40 viewport crop enlarged12x nearest, no intensity/geometry changes.
 contact=Image.new('RGB',(1440,4*530));draw=ImageDraw.Draw(contact);crop=(620,450,660,490)
 for index,face in enumerate((0,64,128,192)):
  original=next(x for x in n['records']if x['facing']==face and x['tick']==0);pair=next(x for x in rows if x['facing']==face and x['native_tick']==0);y=index*530
  for column,(label,path)in enumerate((('legacy integer / point',original['images']['legacy-point']['path']),('qualified modern half / point',original['images']['modern-point']['path']),('actual Metal modern policy',pair['source']))):
   draw.text((column*480+6,y+4),f'face{face}: {label}',fill='white');im=Image.open(path).convert('RGB');contact.paste(im.crop(crop).resize((480,480),Image.Resampling.NEAREST),(column*480,y+44))
  draw.text((486,y+23),str(pair['pixels']),fill='white')
 path=out/'detail.png';contact.save(path);pin(path)
 report=dict(status='ready-for-independent-qualified-review',accepted=False,qualified_reference=True,unchanged_original_device=False,binary_sha256=cp['binary_sha256'],native_manifest_sha256=sha(native),original_hardware_frames=268,original_full_packet_replay_cases=12,source_rendered_frames=268,source_matrix_fields=matrix_checks,matrix_max_error=maximum,exact_source_reference_images=12,phase_rule='Original label0 follows first update; match record tick+1 to round(source simulation_seconds*24). No fitted offset.',scope='Exact Fire0x119f01e3 static oldbody quad/one key/texture selector0, genuine factory/default owner and original hardware dispatch/COM material+texture. White emissive material mathematically saturates hardware diffuse; independent ARGB4444 MODULATE/SRCALPHA/INVSRCALPHA device. Modern half-center point interpretation is pixel-exact to current Metal; legacy integer and bilinear controls retained.',limits=['Original RGB565 software/span reference still differs','Modern pixel-center convention is explicitly qualified, not unchanged D3D6 hardware fidelity','Hardware texture filtering was absent from cached software startup; both point/linear controls retained','No invented33-frame clock; actual controller stays frame0 and persistent','Natural caller/map/audio/period-device/general lighting or sampler conformance not certified'],pairs=rows,pins=pins)
 (out/'pair-manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(sha(out/'pair-manifest.json'))
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__)
 for name in('native','repeat','films','checkpoint','output'):p.add_argument('--'+name,type=Path,required=True)
 a=p.parse_args();review(a.native,a.repeat,a.films,a.checkpoint,a.output)
