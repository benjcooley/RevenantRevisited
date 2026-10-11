"""Freeze exact-phase JhagaAttack full-viewport original/Metal evidence."""
import argparse, hashlib, json, re, struct
from pathlib import Path
from PIL import Image, ImageDraw
p=argparse.ArgumentParser();p.add_argument('--native',type=Path,required=True);p.add_argument('--metal',type=Path,required=True);p.add_argument('--repeat',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True)
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
ref=lambda p:dict(path=str(p.resolve()),sha256=sha(p))
trace=json.loads((a.native/'trace.json').read_text());samples=list(range(1,98,4));manifests={};controls=[]
assert trace['viewport']==[1920,1080]and trace['center']==[960,485]
# This includes every actually submitted triangle on all100 ticks, not just nonzero sampled pixels.
bounds=[r['draw_bounds']for r in trace['records']if r['draw_bounds']]
union=[min(b[i]for b in bounds)if i<2 else max(b[i]for b in bounds)for i in range(4)]
assert union[0]>2 and union[1]>2 and union[2]<1918 and union[3]<1078,union
repeat=json.loads((a.repeat/'trace.json').read_text())
# Two16-bit surfaces move resource allocation addresses by this exact size delta.
# The three per-slot variable references at216/220/224 relocate; every other
# byte, controller field, owner field, matrix, and RNG value must remain exact.
allocation_delta=4*(1920*1080-repeat['viewport'][0]*repeat['viewport'][1]);relocated=0
for x,y in zip(trace['records'],repeat['records']):
 m,n=x['after'],y['after'];assert {k:v for k,v in m.items()if k!='controllers'}=={k:v for k,v in n.items()if k!='controllers'}
 assert len(m['controllers'])==len(n['controllers'])
 for c,d in zip(m['controllers'],n['controllers']):
  assert {k:v for k,v in c.items()if k!='pool_hex'}=={k:v for k,v in d.items()if k!='pool_hex'}
  raw=bytearray.fromhex(c['pool_hex']);old=bytes.fromhex(d['pool_hex']);assert len(raw)==len(old)
  for slot in range(c['capacity']):
   for offset in(216,220,224):
    index=slot*252+offset;u=struct.unpack_from('<I',raw,index)[0];v=struct.unpack_from('<I',old,index)[0]
    assert (u==v==0)or(u-v==allocation_delta and 0x10000000<=v<0x20000000),(slot,offset,u,v)
    if u:struct.pack_into('<I',raw,index,v);relocated+=1
  assert bytes(raw)==old

for domain,depth in [('native',[0,65535]),('common',[2238,3262])]:
 path=a.metal/(domain+'-domain')/'manifest.json';m=json.loads(path.read_text());assert m['status']=='pass'and m['scenario']['resolution']==[1920,1080]and m['depth_range']==depth
 assert m['actual_viewport']==[1920,1080]and m['actual_camera']==[960,485]
 assert len(m['timing'])==25
 for t in m['timing']:assert abs(float(t['simulation_seconds'])-((int(t['frame'])-1)*4+2)/24)<1e-8
 rows=re.findall(r'\[jhagaattack-phase\] tick=(\d+) state=(\d+) frame=(\d+) position=(-?\d+),(-?\d+),(-?\d+) alive=(\d+) random=(\d+)',(path.parent/'run.log').read_text());assert len(rows)==87,len(rows)
 for row in rows:
  tick,state,frame,x,y,z,alive,rng=map(int,row);n=trace['records'][tick-1]['after'];assert(state,frame,[x,y,z],rng)==(n['state'],n['frame'],n['position'],n['rng_draws']),(domain,tick,row,n)
  controls.append(dict(domain=domain,tick=tick,state=state,frame=frame,position=[x,y,z],particles=alive,rng=rng))
 manifests[domain]=m
assert manifests['native']['binary_sha256']==manifests['common']['binary_sha256']
whole=Image.new('RGB',(1440,len(samples)*294));detail=Image.new('RGB',(1536,len(samples)*564));wd=ImageDraw.Draw(whole);dd=ImageDraw.Draw(detail);pairs=[];pages=[]
# Identical full-viewport scale, plus fixed 1440x1080 crop containing all actual triangles.
crop=(64,0,1600,1080)
for row,t in enumerate(samples):
 paths=[a.native/f'native-{t:03}.png',next((a.metal/'common-domain').glob(f'frame-{(t+3)//4:03}-*.png')),next((a.metal/'native-domain').glob(f'frame-{(t+3)//4:03}-*.png'))];visible=[];pair=Image.new('RGB',(3840,1080))
 for col,(path,label)in enumerate(zip(paths,['Original software','Metal common diagnostic','Metal native domain'])):
  im=Image.open(path).convert('RGB');assert im.size==(1920,1080);bbox=im.getbbox();visible.append(bbox)
  if bbox and col!=1:assert bbox[0]>0 and bbox[1]>0 and bbox[2]<1920 and bbox[3]<1080,(t,label,bbox)
  whole.paste(im.resize((480,270)),(col*480,row*294+24));wd.text((col*480+4,row*294+4),f'record {t} / pulse {t+1}: {label}',fill='white')
  if col in(0,2):
   dcol=col//2;detail.paste(im.crop(crop).resize((768,540)),(dcol*768,row*564+24));dd.text((dcol*768+4,row*564+4),f'record {t}: {label}',fill='white');pair.paste(im,(dcol*1920,0))
 if t in(9,21,33,41,57,65,73):pair.save(a.output/f'actual-{t:03}.png')
 pairs.append(dict(record=t,pulse=t+1,state=trace['records'][t]['after']['state'],images=[ref(p)for p in paths],visible_bounds=visible))
whole.save(a.output/'whole-life.png');detail.save(a.output/'detail.png')
for page in range(5):
 path=a.output/f'detail-{page+1}.png';detail.crop((0,page*5*564,1536,(page+1)*5*564)).save(path);pages.append(ref(path))
r=dict(status='ready_for_independent_review',effect='JhagaAttack',type_id='0xaeaeeb32',pairs=pairs,binary_sha256=manifests['native']['binary_sha256'],native_trace=ref(a.native/'trace.json'),repeat_trace=ref(a.repeat/'trace.json'),viewport_replay_control=dict(status='pass',allocation_reference_delta=allocation_delta,relocated_reference_words=relocated,all_other_state_bytes_exact=True),metal_manifests=[ref(a.metal/(d+'-domain')/'manifest.json')for d in('native','common')],phase_rng_controls=controls,panels=[ref(a.output/n)for n in('whole-life.png','detail.png')]+pages,viewport=[1920,1080],center=[960,485],all100tick_submitted_triangle_bounds=union,detail_crop=crop,cadence='24Hz Pulse and Render, sparse every4 screenshots, warmup1. FileN follows pulse4N-2 / zero-based native record4N-3. All174 logged active renders match original state/frame/position/RNG. Actual OF_KILL record87.',source_policy='Own JhagaAttack registration, Magic/Jattack.i3d, nullspell source(0,60,78), face0, actual20/15/55 owner scheduling, velocity(0,-8,0), timed destination(0,-60,78). Actual default full controller/Render/normal-lit RGB565 head/prelit particles/software raster. Native38white/directional1; Metal requested32white yields same native light. No geometry/tint/time fitting.',limits=['Nullinvoker equals empty character query, so timed flight does not certify terrain/actor collision. Natural boss/invoker, damage, audio, generic Effect/script pulse, resource residency/COM and map notifications remain boundaries.','Initial1024x768 captures genuinely clip late sparks and are diagnostic only. This supported1920x1080 primary contains every actual submitted triangle across all100 ticks.','Controller initialization reads the cached outgoing-flight prototype origin. Normal/stride3 and early-only-render controls prove this input; later teardown cached matrices are not claimed equivalent or consumed by another Initialize.','Common-domain retains ordinary projection/depth as diagnostic. Texture sampling, RGB565 destination rounding, interpolation, device backend and exact framebuffer equality remain open.'])
(a.output/'pair-manifest.json').write_text(json.dumps(r,indent=2)+'\n');print('pair',sha(a.output/'pair-manifest.json'),'bounds',union,'phases',len(pairs),'controls',len(controls))
