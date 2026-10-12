"""Freeze exact-phase headfireball full-viewport original/Metal evidence."""
import argparse, hashlib, json, re, struct
from pathlib import Path
from PIL import Image, ImageDraw
p=argparse.ArgumentParser();p.add_argument('--native',type=Path,required=True);p.add_argument('--metal',type=Path,required=True);p.add_argument('--repeat',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True)
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
ref=lambda p:dict(path=str(p.resolve()),sha256=sha(p))
trace=json.loads((a.native/'trace.json').read_text());samples=list(range(1,98,4));manifests={};controls=[]
assert trace['viewport']==[1280,960]and trace['center']==[640,560]
# This includes every actually submitted triangle on all110 ticks, not just nonzero sampled pixels.
bounds=[r['draw_bounds']for r in trace['records']if r['draw_bounds']]
union=[min(b[i]for b in bounds)if i<2 else max(b[i]for b in bounds)for i in range(4)]
assert union[0]>2 and union[1]>2 and union[2]<1278 and union[3]<958,union
repeat=json.loads((a.repeat/'trace.json').read_text())
assert [r['after']for r in trace['records']]==[r['after']for r in repeat['records']]
for t in(9,21,33,41,57,89):assert(a.native/f'native-{t:03}.png').read_bytes()==(a.repeat/f'native-{t:03}.png').read_bytes()
for domain,depth in [('native',[0,65535]),('common',[2238,3262])]:
 path=a.metal/(domain+'-domain')/'manifest.json';m=json.loads(path.read_text());assert m['status']=='pass'and m['scenario']['resolution']==[1280,960]and m['depth_range']==depth
 assert m['actual_viewport']==[1280,960]and m['actual_camera']==[640,560]
 assert len(m['timing'])==25
 for t in m['timing']:assert abs(float(t['simulation_seconds'])-((int(t['frame'])-1)*4+2)/24)<1e-8
 rows=re.findall(r'\[headfireball-phase\] tick=(\d+) state=(\d+) frame=(\d+) position=(-?\d+),(-?\d+),(-?\d+) alive=(\d+) random=(\d+)',(path.parent/'run.log').read_text());assert len(rows)==86,len(rows)
 for row in rows:
  tick,state,frame,x,y,z,alive,rng=map(int,row);n=trace['records'][tick-1]['after'];assert(state,frame,[x,y,z],rng)==(n['state'],n['frame'],n['position'],n['rng_draws']),(domain,tick,row,n)
  controls.append(dict(domain=domain,tick=tick,state=state,frame=frame,position=[x,y,z],particles=alive,rng=rng))
 manifests[domain]=m
assert manifests['native']['binary_sha256']==manifests['common']['binary_sha256']
whole=Image.new('RGB',(1536,len(samples)*408));detail=Image.new('RGB',(1664,len(samples)*664));wd=ImageDraw.Draw(whole);dd=ImageDraw.Draw(detail);pairs=[];pages=[]
# Identical full-viewport scale, plus fixed crop containing all actual triangles.
crop=(256,160,1088,800)
for row,t in enumerate(samples):
 paths=[a.native/f'native-{t:03}.png',next((a.metal/'common-domain').glob(f'frame-{(t+3)//4:03}-*.png')),next((a.metal/'native-domain').glob(f'frame-{(t+3)//4:03}-*.png'))];visible=[];pair=Image.new('RGB',(2560,960))
 for col,(path,label)in enumerate(zip(paths,['Original software','Metal common diagnostic','Metal native domain'])):
  im=Image.open(path).convert('RGB');assert im.size==(1280,960);bbox=im.getbbox();visible.append(bbox)
  if bbox and col!=1:assert bbox[0]>0 and bbox[1]>0 and bbox[2]<1280 and bbox[3]<960,(t,label,bbox)
  whole.paste(im.resize((512,384)),(col*512,row*408+24));wd.text((col*512+4,row*408+4),f'record {t} / pulse {t+1}: {label}',fill='white')
  if col in(0,2):
   dcol=col//2;detail.paste(im.crop(crop),(dcol*832,row*664+24));dd.text((dcol*832+4,row*664+4),f'record {t}: {label}',fill='white');pair.paste(im,(dcol*1280,0))
 if t in(9,21,33,41,57,65,73):
  pair.save(a.output/f'actual-{t:03}.png');fixed=Image.new('RGB',(1664,640));fixed.paste(Image.open(paths[0]).crop(crop),(0,0));fixed.paste(Image.open(paths[2]).crop(crop),(832,0));fixed.save(a.output/f'crop-{t:03}.png')
 pairs.append(dict(record=t,pulse=t+1,state=trace['records'][t]['after']['state'],images=[ref(p)for p in paths],visible_bounds=visible))
whole.save(a.output/'whole-life.png');detail.save(a.output/'detail.png')
for page in range(5):
 path=a.output/f'detail-{page+1}.png';detail.crop((0,page*5*664,1664,(page+1)*5*664)).save(path);pages.append(ref(path))
r=dict(status='ready_for_independent_review',effect='headfireball',type_id='0xd000f005',pairs=pairs,binary_sha256=manifests['native']['binary_sha256'],native_trace=ref(a.native/'trace.json'),repeat_trace=ref(a.repeat/'trace.json'),fresh_replay_control=dict(status='pass',all110states_exact=True,six_pngs_exact=True),metal_manifests=[ref(a.metal/(d+'-domain')/'manifest.json')for d in('native','common')],phase_rng_controls=controls,panels=[ref(a.output/n)for n in('whole-life.png','detail.png')]+pages,viewport=[1280,960],center=[640,560],all110tick_submitted_triangle_bounds=union,detail_crop=crop,cadence='24Hz Pulse and Render, sparse every4 screenshots, warmup1. FileN follows pulse4N-2 / zero-based native record4N-3. All172 logged active renders match original state/frame/position/RNG. Actual OF_KILL record86.',source_policy='Own headfireball default registration, Magic/Hfire.i3d, generic Effect constructor4de770. Actual RunFireHeadN Init526c90/Spawn526f00/Tick527140/Move526fd0; single child source(0,60,78), facing0, explicit character-query targetplaneY=-60. State1 atrecord18, collision state2 atrecord32, endframe54 killrecord86. Actual default full controller/Render/normal-lit RGB565 head/prelit particles/software raster. Native38white/directional1; Metal requested32white yields same native light. No geometry/tint/time fitting.',limits=['Declared single trap child and target query, with observed reaction/damage calls. Natural stat arming/two-child scheduling, character selection/damage/audio, generic Effect/script pulse, resource/COM and map notifications remain interfaces. Not the distinct Dragonattack type.','Every original submitted triangle across110 ticks is inside the supported1280x960 viewport. Identical fixed crop is supplemental.','Cached outgoing-flight controller initialization origin independently verified under normal/stride3/early-only Render; later unconsumed teardown matrices are not claimed equivalent.','Common-domain ordinary projection/depth is diagnostic. RGB565 destination rounding, sampling, raster interpolation, device and exact framebuffer parity remain open.'])
(a.output/'pair-manifest.json').write_text(json.dumps(r,indent=2)+'\n');print('pair',sha(a.output/'pair-manifest.json'),'bounds',union,'phases',len(pairs),'controls',len(controls))
