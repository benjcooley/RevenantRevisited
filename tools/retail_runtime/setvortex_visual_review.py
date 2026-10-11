"""Freeze complete same-tick SetVortex native/Metal pairs without phase fitting."""
import argparse,json,hashlib
from pathlib import Path
from PIL import Image,ImageDraw

def pin(p):
 p=Path(p).resolve();return dict(path=str(p),sha256=hashlib.sha256(p.read_bytes()).hexdigest())
def run(p):
 out=p/'setvortex-review-final';out.mkdir(exist_ok=True)
 np=p/'setvortex-native-final/manifest.json';mp=p/'setvortex-metal-final/manifest.json';n=json.loads(np.read_text());m=json.loads(mp.read_text());assert m['status']=='pass'
 ms=dict(zip(m['sample_simulation_ticks'],m['frames']));rows=[r for r in n['records']if'image'in r];assert len(rows)==27
 whole=Image.new('RGB',(1280,14*264),(24,24,24));detail=Image.new('RGB',(640,27*376),(24,24,24));pairs=[];crop=(600,412,680,500)
 for i,r in enumerate(rows):
  tick=r['tick'];a=p/'setvortex-native-final'/r['image'];b=Path(ms[tick]['path']);assert pin(a)['sha256']==r['sha256'] and pin(b)['sha256']==ms[tick]['sha256']
  aa=Image.open(a).convert('RGB');bb=Image.open(b).convert('RGB');assert aa.size==bb.size==(1280,960)
  pair=Image.new('RGB',(2560,960));pair.paste(aa,(0,0));pair.paste(bb,(1280,0));pp=out/f'pair-{tick:03}.png';pair.save(pp)
  whole.paste(pair.resize((640,240),Image.Resampling.NEAREST),((i%2)*640,(i//2)*264+24));ImageDraw.Draw(whole).text(((i%2)*640+3,(i//2)*264+4),f'tick{tick}: original / Metal',fill='white')
  detail.paste(aa.crop(crop).resize((320,352),Image.Resampling.NEAREST),(0,i*376+24));detail.paste(bb.crop(crop).resize((320,352),Image.Resampling.NEAREST),(320,i*376+24));ImageDraw.Draw(detail).text((3,i*376+4),f'tick{tick}: original / Metal identical 4x nearest crop',fill='white')
  pairs.append(dict(tick=tick,native=pin(a),metal=pin(b),pair=pin(pp),native_bbox=aa.getbbox(),metal_bbox=bb.getbbox()))
 panels=[]
 for name,im in [('whole-life.png',whole),('detail.png',detail)]:im.save(out/name);panels.append(pin(out/name))
 for i in range(0,27,3):
  path=out/f'detail-{i//3+1}.png';detail.crop((0,i*376,640,min(27,i+3)*376)).save(path);panels.append(pin(path))
 report=dict(status='ready-for-independent-review',type_id='0xad92bc15',effect='SetVortex',native=pin(np),metal=pin(mp),binary_sha256=m['binary_sha256'],pairs=pairs,panels=panels,
  proofs=[pin(p/'setvortex-source-final2/manifest.json'),pin(p/'setvortex-native-final/replay.json')],
  configuration=dict(native_source=[0,0,0],native_camera=[0,0,0],port_source=[0,0,0],viewport=[1280,960],center=[640,480],ambient=[38,38,38],directional=[1,1,1],frames=105,simulation_hz=24),
  scope='Genuine loaded persistent null-spell SetVortex owner with custom Animate/full Render versus existing exact source class. Original nonlinear cached height, CULL_NONE complete authored geometry and strict ARGB4444 source decode. All105 ticks render at24Hz;27 PNG snapshots every4ticks. No fitted camera/geometry/color.',
  limits=['Spell teleport marking/previous marker removal and natural map caller remain separate.', 'Ordinary projection/hardware, audio/device/exact pixels remain open.', 'Residual nearest sampling and RGB565 framebuffer/overlap quantization remain.', 'Fixed24Hz Animate/render cadence; arbitrary skipped-render scheduling not certified.'])
 path=out/'pair-manifest.json';path.write_text(json.dumps(report,indent=2)+'\n');print(pin(path))
if __name__=='__main__':
 ap=argparse.ArgumentParser();ap.add_argument('--lab',type=Path,required=True);args=ap.parse_args();run(args.lab)
