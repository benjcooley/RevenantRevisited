#!/usr/bin/env python3
"""Frozen no-fit FireCone actual-render appearance review."""
from pathlib import Path
import argparse,json,hashlib,re
from PIL import Image,ImageChops,ImageDraw
parser=argparse.ArgumentParser(description='Pair complete original FireCone samples with actual common/native-domain Metal captures and map cleanup.')
parser.add_argument('--artifacts',type=Path,required=True);parser.add_argument('--output',type=Path,required=True);args=parser.parse_args()
P=args.artifacts;O=args.output;O.mkdir(parents=True,exist_ok=False)
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def load(p):return json.loads(p.read_text())
n=load(P/'firecone-visual-native-final/manifest.json');m={mode:load(P/f'firecone-metal-{mode}/manifest.json')for mode in ['common','native']};logs={mode:(P/f'firecone-metal-{mode}/run.log').read_text()for mode in m};rng={mode:{int(i)-1:int(j)for i,j in re.findall(r'before_render=(\d+) global_random_draws=(\d+)',logs[mode])}for mode in m};light={mode:[l for l in logs[mode].splitlines()if '[vfx-source-light]'in l]for mode in m};cases=[]
assert all(c['status']=='pass'for c in m.values())
assert m['common']['binary_sha256']==m['native']['binary_sha256']
for mode in m:
 assert m[mode]['scenario']['native_domain']==(mode=='native')
 assert any('ambient=(0.149019614,0.149019614,0.149019614) directional=(1,1,1) point_lights=0'in l for l in light[mode])
 assert len(m[mode]['frames'])==100
 for f in m[mode]['frames']:assert sha(f['path'])==f['sha256']
for c in n['cases']:
 i=c['capture'];assert sha(c['path'])==c['sha256'];paths=[c['path']]+[m[mode]['frames'][i-1]['path']for mode in ['common','native']];images=[Image.open(p).convert('RGB')for p in paths];errors=[]
 for im in images[1:]:errors.append(sum(k*v for hist in [ImageChops.difference(images[0],im).histogram()]for k,v in [(i%256,v)for i,v in enumerate(hist)]))
 record=dict(capture=i,tick=c['tick'],paths=paths,sha256=[sha(p)for p in paths],bbox=[im.getbbox()for im in images],absolute_rgb_error=errors,native_rng_before=c['native_rng_draws_before_capture'],metal_rng_before={mode:rng[mode].get(i)for mode in m});cases.append(record)
 if i in rng['native']:assert all(rng[mode][i]==c['native_rng_draws_before_capture']for mode in m),record
selected=[1,5,15,29,45,59];roi=(240,0,540,235);w=(roi[2]-roi[0])*2;h=(roi[3]-roi[1])*2+25;sheet=Image.new('RGB',(w*3,h*len(selected)),(22,22,22));d=ImageDraw.Draw(sheet)
for row,i in enumerate(selected):
 c=next(c for c in cases if c['capture']==i)
 for col,(path,label)in enumerate(zip(c['paths'],['original software','Metal common','Metal native-domain'])):
  d.text((col*w+4,row*h+5),f'capture{i} {label}',fill='white');sheet.paste(Image.open(path).convert('RGB').crop(roi).resize((w,h-25),Image.Resampling.NEAREST),(col*w,row*h+25))
sheet.save(O/'contact.png');report=dict(status='review_ready',accepted=False,strict_pixel_parity=False,source_policy='EXPLICIT_NATIVE_DOMAIN_ISOLATED_PREVIEW',native_manifest_sha256=sha(P/'firecone-visual-native-final/manifest.json'),metal_manifest_sha256={mode:sha(P/f'firecone-metal-{mode}/manifest.json')for mode in m},binary_sha256=m['native']['binary_sha256'],light_logs=light,rng_counts=rng,cases=cases,contact=str(O/'contact.png'),contact_sha256=sha(O/'contact.png'),fit=False,scope='Complete null-spell FireCone lifecycle state;14 fixed original full-normal-lit samples against actual common/native-domain Metal renders. Remaining raster/source/destination precision differences are not exact parity. Ordinary map projection remains unchanged.')
mapdir=P/'firecone-map-current/FireCone';mapmeta=load(mapdir/'manifest.json');assert mapmeta['status']=='pass'
mapimages=[Image.open(f['path']).convert('RGB')for f in mapmeta['frames']];floor=mapimages[-1]
tail=sum(ImageChops.difference(im,floor).getbbox()is None for im in mapimages[-20:]);assert tail==20
assert all('status=PASS'in row for row in mapmeta['rows'])
report['map_lifecycle']=dict(manifest=str(mapdir/'manifest.json'),sha256=sha(mapdir/'manifest.json'),frames=len(mapimages),rows=len(mapmeta['rows']),last20_exact_floor=True,scope='Actual common-domain owner elevation, movement and natural destruction. No map-native pixel equality claimed.')
report['timing_contract']='One-based retained frame001 follows render2/tick2 after warmup1. Logged before_render N belongs to retained frame N-1. This is the capture implementation, not a searched temporal offset.'
report['total_absolute_rgb_error_common_native']=[sum(c['absolute_rgb_error'][i]for c in cases)for i in range(2)]
report['matched_rng_pairs']=sum(c['capture']in rng['native']for c in cases)
(O/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({'rng':rng,'light':light,'cases':[{k:c[k]for k in ['capture','bbox','absolute_rgb_error']}for c in cases]},indent=2))
