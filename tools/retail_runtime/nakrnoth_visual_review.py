"""Freeze same-tick complete Nakrnoth main/tiled comparison without phase fitting."""
import argparse,json,hashlib
from pathlib import Path
from PIL import Image,ImageDraw,ImageChops

def pin(path):
 p=Path(path).resolve();return dict(path=str(p),sha256=hashlib.sha256(p.read_bytes()).hexdigest())
def run(p):
 out=p/'nakrnoth-review-single-light';out.mkdir(exist_ok=False)
 np=p/'nakrnoth-native-main/manifest.json';mp=p/'nakrnoth-metal-final/manifest.json';n=json.loads(np.read_text());m=json.loads(mp.read_text());assert m['status']=='pass';ticks=list(range(1,782,20));assert m['sample_simulation_ticks']==ticks
 ms=dict(zip(ticks,m['frames']));ns={r['tick']:r for r in n['records']if'image'in r};assert list(ns)==ticks
 baseline=json.loads((p/'nakrnoth-native-whole/manifest.json').read_text())['records']
 for a,b in zip(n['records'],baseline):assert all(a['state'][k]==b['state'][k]for k in('flags','frame','seed','rng_draws'))
 pairs=[];panels=[];selected=[1,81,161,201,241,281,321,361,401,441,481,521,561,601,621,641,661,681,701,741,781];whole=Image.new('RGB',(1280,((len(selected)+1)//2)*264),(24,24,24));details=[]
 for tick in ticks:
  a=p/'nakrnoth-native-main'/ns[tick]['image'];b=Path(ms[tick]['path']);assert pin(a)['sha256']==ns[tick]['sha256']and pin(b)['sha256']==ms[tick]['sha256'];aa=Image.open(a).convert('RGB');bb=Image.open(b).convert('RGB');assert aa.size==bb.size==(1280,960)
  pair=Image.new('RGB',(2560,960));pair.paste(aa,(0,0));pair.paste(bb,(1280,0));path=out/f'main-{tick:03}.png';pair.save(path);pairs.append(dict(tick=tick,native=pin(a),metal=pin(b),pair=pin(path)))
  if tick in selected:
   i=selected.index(tick);whole.paste(pair.resize((640,240),Image.Resampling.NEAREST),((i%2)*640,(i//2)*264+24));ImageDraw.Draw(whole).text(((i%2)*640+4,(i//2)*264+4),f'tick{tick}: original / Metal',fill='white')
   if tick<=601:
    crop=(512,272,800,576);im=Image.new('RGB',(1152,632),(24,24,24));im.paste(aa.crop(crop).resize((576,608),Image.Resampling.NEAREST),(0,24));im.paste(bb.crop(crop).resize((576,608),Image.Resampling.NEAREST),(576,24));ImageDraw.Draw(im).text((4,4),f'tick{tick}: identical crop,2x nearest; original / Metal',fill='white');details.append(im)
 path=out/'whole-life.png';whole.save(path);panels.append(pin(path))
 for start in range(0,len(details),3):
  page=Image.new('RGB',(1152,632*min(3,len(details)-start)))
  for i,im in enumerate(details[start:start+3]):page.paste(im,(0,632*i))
  path=out/f'detail-{start//3+1}.png';page.save(path);panels.append(pin(path))
 native_tiles={};tile_manifests=[]
 for folder in('nakrnoth-native-tiles-single-light','nakrnoth-native-middle-single-light'):
  path=p/folder/'manifest.json';tile_manifests.append(pin(path));data=json.loads(path.read_text())
  for row in data['records']:
   for v in row['views']:native_tiles[(row['tick'],tuple(v['center']))]=(p/folder/v['image'],v)
 centers=((1440,920),(480,920),(1440,520),(480,520),(1440,120),(480,120));late=[601,621,641,661,681,701];metal_tiles={}
 for i,center in enumerate(centers):
  path=p/f'nakrnoth-metal-view{i}/manifest.json';tile_manifests.append(pin(path));data=json.loads(path.read_text());assert data['status']=='pass' and data['binary_sha256']==m['binary_sha256'] and data['sample_simulation_ticks']==late and data['actual_viewport']==[1920,1080]
  for tick,frame in zip(late,data['frames']):metal_tiles[(tick,center)]=(Path(frame['path']),frame)
 tile_pairs=[];unions=[]
 for tick in late:
  merged=[Image.new('RGB',(2880,1880)),Image.new('RGB',(2880,1880))]
  for i,center in enumerate(centers):
   a,ar=native_tiles[(tick,center)];b,br=metal_tiles[(tick,center)];assert pin(a)['sha256']==ar['sha256']and pin(b)['sha256']==br['sha256'];aa=Image.open(a).convert('RGB');bb=Image.open(b).convert('RGB');assert aa.size==bb.size==(1920,1080)
   pair=Image.new('RGB',(3840,1080));pair.paste(aa,(0,0));pair.paste(bb,(1920,0));path=out/f'late-{tick:03}-view{i}.png';pair.save(path);tile_pairs.append(dict(tick=tick,view=i,center=center,native=pin(a),metal=pin(b),pair=pin(path)))
   for k,im in enumerate((aa,bb)):
    layer=Image.new('RGB',(2880,1880));layer.paste(im,(1440-center[0],920-center[1]));merged[k]=ImageChops.lighter(merged[k],layer)
  pair=Image.new('RGB',(5760,1880));pair.paste(merged[0],(0,0));pair.paste(merged[1],(2880,0));path=out/f'late-{tick:03}-union.png';pair.save(path);unions.append(pin(path))
 contact=Image.new('RGB',(1440,6*494),(24,24,24))
 for i,row in enumerate(unions):contact.paste(Image.open(row['path']).resize((1440,470),Image.Resampling.NEAREST),(0,i*494+24));ImageDraw.Draw(contact).text((4,i*494+4),f'tick{late[i]}: fixed6-window MAX union; original / Metal; raw overlaps retained',fill='white')
 path=out/'late-whole.png';contact.save(path);panels.append(pin(path))
 report=dict(status='ready-for-independent-review',effect='Nakrnoth',type_id='0xad92bd21',binary_sha256=m['binary_sha256'],native=pin(np),metal=pin(mp),pairs=pairs,panels=panels,late_pairs=tile_pairs,late_unions=unions,tile_manifests=tile_manifests,proofs=[pin(p/'nakrnoth-source-final/manifest.json'),pin(p/'nakrnoth-coverage.json'),pin(p/'nakrnoth-native-whole/manifest.json'),pin(p/'nakrnoth-ground-native/manifest.json'),pin(p/'nakrnoth-lighting-native/manifest.json')],configuration=dict(native_source=[3185,3185,16],native_camera=[3185,3185,0],port_source=[0,0,16],ambient=[38,38,38],directional=[1,1,1],simulation_hz=24,owner_kill=775,main_viewport=[1280,960],main_center=[640,480],late_viewport=[1920,1080],late_centers=centers),scope='Genuine typed loaded null-spell fixed-owner/default renderer and unchanged shipped775-frame resource/controllers versus exact source adapter. Original software triangle-span policy only in native-domain/software mode. Whole lifetime/main film plus immutable six-window coverage of complete late expansion. No fitted phase/color/scale/geometry.',limits=['Natural boss/actor/spell/map/audio/device/exactpixels remain open.','Main late frame is clipped; use raw supported windows and declared MAX-union diagnostics for full expansion.','MAX unions are coverage contacts, not one original giant framebuffer; overlapping original raster differences remain.','Common/hardware retains the full authored outer ring; widened original limits and prior unguarded native-domain Metal are diagnostic only.','Nearest sampling/RGB565 framebuffer accumulation and 0.001953 world-coordinate float drift remain.','Initial larger native fixture registered two directional lights; native-tiles/native-middle and review-final remain rejected lighting controls. Corrected single-light references preserve port binary/films unchanged.','First auxiliary warmup25-frame films were rejected by CSV timing and retained as diagnostics; final auxiliary films assert601..701.'])
 path=out/'pair-manifest.json';path.write_text(json.dumps(report,indent=2)+'\n');print(pin(path));return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--lab',type=Path,required=True);a=p.parse_args();run(a.lab)
