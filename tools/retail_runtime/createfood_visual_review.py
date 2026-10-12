"""Freeze complete same-tick Createfood native/Metal pairs without phase fitting."""
import argparse,json,hashlib
from pathlib import Path
from PIL import Image,ImageDraw

def pin(p):
 p=Path(p).resolve();return dict(path=str(p),sha256=hashlib.sha256(p.read_bytes()).hexdigest())
def run(p):
 out=p/'createfood-review-final';out.mkdir(exist_ok=True)
 np=p/'createfood-native-final/manifest.json';mp=p/'createfood-metal-native/manifest.json';n=json.loads(np.read_text());m=json.loads(mp.read_text());assert m['status']=='pass'
 ms=dict(zip(m['sample_simulation_ticks'],m['frames']));rows=[r for r in n['records']if'image'in r];assert len(rows)==21
 whole=Image.new('RGB',(1280,11*264),(24,24,24));detail=Image.new('RGB',(640,21*264),(24,24,24));pairs=[];crop=(512,260,832,500)
 for i,r in enumerate(rows):
  tick=r['tick'];a=p/'createfood-native-final'/r['image'];b=Path(ms[tick]['path']);assert pin(a)['sha256']==r['sha256'] and pin(b)['sha256']==ms[tick]['sha256']
  aa=Image.open(a).convert('RGB');bb=Image.open(b).convert('RGB');assert aa.size==bb.size==(1280,960)
  pair=Image.new('RGB',(2560,960));pair.paste(aa,(0,0));pair.paste(bb,(1280,0));pp=out/f'pair-{tick:03}.png';pair.save(pp)
  whole.paste(pair.resize((640,240),Image.Resampling.NEAREST),((i%2)*640,(i//2)*264+24));ImageDraw.Draw(whole).text(((i%2)*640+3,(i//2)*264+4),f'tick{tick}: original / Metal',fill='white')
  detail.paste(aa.crop(crop),(0,i*264+24));detail.paste(bb.crop(crop),(320,i*264+24));ImageDraw.Draw(detail).text((3,i*264+4),f'tick{tick}: original / Metal identical 1x crop',fill='white')
  pairs.append(dict(tick=tick,native=pin(a),metal=pin(b),pair=pin(pp),native_bbox=aa.getbbox(),metal_bbox=bb.getbbox()))
 panels=[]
 for name,im in [('whole-life.png',whole),('detail.png',detail)]:im.save(out/name);panels.append(pin(out/name))
 for i in range(0,21,4):
  path=out/f'detail-{i//4+1}.png';detail.crop((0,i*264,640,min(21,i+4)*264)).save(path);panels.append(pin(path))
 report=dict(status='ready-for-independent-review',type_id='0x838cffba',effect='Createfood',native=pin(np),metal=pin(mp),binary_sha256=m['binary_sha256'],pairs=pairs,panels=panels,
  proofs=[pin(p/'createfood-core-final/manifest.json'),pin(p/'createfood-index-proof/manifest.json')],
  configuration=dict(native_source=[3185,3185,16],native_camera=[3185,3185,0],port_source=[0,0,16],viewport=[1280,960],center=[640,480],ambient=[38,38,38],directional=[1,1,1],frames=85,simulation_hz=24),
  scope='Genuine loaded Createfood quality3 full owner/native renderer versus exact standalone source adapter. Declared white base light0 multiplier28, ambient32 generated adjacent MMX table; failed FOOD insertion with original clock0 MakeIndex reseed. Actual kill80/blanktail.',
  limits=['Natural light context changes spilled angle lookups; this fixed context is explicit.', 'FOOD request is observed but no item insertion or map residency is ported.', 'Natural actors/spells/audio/device/exact pixels remain open.', 'Residual edge sampling, overlap and RGB565 framebuffer quantization remain.'])
 path=out/'pair-manifest.json';path.write_text(json.dumps(report,indent=2)+'\n');print(pin(path))
if __name__=='__main__':
 ap=argparse.ArgumentParser();ap.add_argument('--lab',type=Path,required=True);args=ap.parse_args();run(args.lab)
