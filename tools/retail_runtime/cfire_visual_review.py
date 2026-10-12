"""Freeze literal CFire installed-fallback pairs without image or phase fitting."""
import argparse,hashlib,json
from pathlib import Path
from PIL import Image,ImageDraw

def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def run(native,repeat,metal,source,output,common=None):
 output.mkdir(parents=True,exist_ok=False);n=json.loads(native.read_text());r=json.loads(repeat.read_text());m=json.loads(metal.read_text());assert n==r and m['status']=='pass';assert m['actual_viewport']==[1280,960];assert m['sample_simulation_ticks']==list(range(1,65));assert m['pixel_summary']['nonblack_frames']==29
 assert n['controllers']==[];opens=[x for x in n['preset_opens']if x.get('result')=='0x0'];assert [x['path'].lower()for x in opens]==['.\\resources\\cfire.txt','.\\resources\\csparks.txt']
 files={};pin=lambda p:files.update({str(p.resolve()):sha(p)})
 for p in(native,repeat,metal,source):pin(p)
 if common:pin(common)
 pairs=[];tiles=[];whole=[]
 crop=(616,448,696,504)
 for row in n['records']:
  tick=row['tick'];assert bool(row['state']['flags']&4096)==(tick>=30);assert row['state']['seed']==1 and row['state']['rng_draws']==0
  if 'image'not in row:continue
  a=native.parent/row['image'];b=Path(m['frames'][tick-1]['path']);assert sha(a)==row['sha256'] and sha(b)==m['frames'][tick-1]['sha256'];pin(a);pin(b)
  x=Image.open(a).convert('RGB');y=Image.open(b).convert('RGB');assert x.size==y.size==(1280,960)
  raw=Image.new('RGB',(2560,960));raw.paste(x,(0,0));raw.paste(y,(1280,0));p=output/f'pair-{tick:03}.png';raw.save(p);pin(p)
  tile=Image.new('RGB',(1280,472));tile.paste(x.crop(crop).resize((640,448),Image.Resampling.NEAREST),(0,24));tile.paste(y.crop(crop).resize((640,448),Image.Resampling.NEAREST),(640,24));ImageDraw.Draw(tile).text((8,6),f'tick {tick}: original installed no presets | Metal native-domain (identical crop x8)',fill='white');tiles.append(tile)
  w=raw.resize((640,240),Image.Resampling.NEAREST);whole.append(w)
  pairs.append(dict(tick=tick,native=str(a),modern=str(b),raw_pair=str(p),native_bounds=x.getbbox(),modern_bounds=y.getbbox()))
 for page in range(2):
  panel=Image.new('RGB',(1280,472*5))
  for i,tile in enumerate(tiles[page*5:(page+1)*5]):panel.paste(tile,(0,i*472))
  p=output/f'detail-{page+1}.png';panel.save(p);pin(p)
 panel=Image.new('RGB',(1280,240*5))
 for i,tile in enumerate(whole):panel.paste(tile,((i%2)*640,(i//2)*240))
 p=output/'whole-life.png';panel.save(p);pin(p)
 report=dict(type_id='0xad92bd27',preview='CFire_INSTALLED_NO_PRESETS',status='ready-for-independent-review',binary_sha256=m['binary_sha256'],native_executable_sha256=n['executable_sha256'],asset_sha256=n['asset']['sha256'],scope=n['scope'],configuration=dict(viewport=n['viewport'],center=n['center'],native_source=n['source'],native_camera=n['camera'],modern=m['scenario'],crop=list(crop),detail_scale=8),failed_opens=opens,controllers=0,rng=0,kill_tick=30,visible_ticks=[1,29],blank_ticks=[30,64],fresh_native_repeat='all64 records, pixels, requests and matrices identical',residuals=['Approximately one-pixel sample-edge displacement; low-alpha RGB565 destination/interpolation precision remains open.','Common owner/projector policy remains a diagnostic.','Absent expected CFire/CSparks definitions are not synthesized; no intended particle effect, natural caller, device or exact-pixel claim.'],pairs=pairs,files=files)
 p=output/'pair-manifest.json';p.write_text(json.dumps(report,indent=2)+'\n');print(str(p),sha(p));return report
if __name__=='__main__':
 p=argparse.ArgumentParser();
 for name in('native','repeat','metal','source','output'):p.add_argument('--'+name,type=Path,required=True)
 p.add_argument('--common',type=Path);a=p.parse_args();run(a.native,a.repeat,a.metal,a.source,a.output,a.common)
