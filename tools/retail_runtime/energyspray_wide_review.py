#!/usr/bin/env python3
"""Four overlapping supported1920x1080 views: complete unscaled fan at70/90/110."""
import argparse,importlib.util,json,re,zipfile
from pathlib import Path
from PIL import Image,ImageDraw
from energyspray_visual_reference import EnergySprayScene,ROOT,sha
from software_probe import save_rgb565_png
TILES={'upper_left':(1280,1440),'upper_right':(640,1440),'lower_left':(1280,540),'lower_right':(640,540)}
TICKS=(70,90,110)
def semantic_draws(rows):
 rows=json.loads(json.dumps(rows))
 for d in rows:
  d['arguments'][2]=0;d['arguments'][4]=0
 return rows
def run(exe,base,out,capture_tool):
 out.mkdir(parents=True,exist_ok=False);ref=json.loads((base/'energyspray-native-final/manifest.json').read_text());scenario=json.loads((base/'energy-metal-domain-final/manifest.json').read_text())['scenario']
 spec=importlib.util.spec_from_file_location('wide_capture',capture_tool);mod=importlib.util.module_from_spec(spec);spec.loader.exec_module(mod)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/energyspray.i3d'))
 records=[]
 for name,origin in TILES.items():
  folder=out/name;folder.mkdir();f=EnergySprayScene(exe,data,1920,1080,origin);native=[]
  for tick in range(1,111):
   f.advance();f.raster_enabled=tick in TICKS;raw=f.render();assert f.state()==ref['cases'][tick]['state'];assert semantic_draws(f.draws)==semantic_draws(ref['cases'][tick]['draws'])
   if tick in TICKS:
    p=folder/f'native-{tick:03d}.png';save_rgb565_png(p,raw,1920,1080);native.append(dict(tick=tick,path=str(p),sha256=sha(p.read_bytes()),rgb565_sha256=sha(raw)))
  config=dict(scenario,frames=3,capture_stride=20,warmup=69,resolution=[1920,1080],rectangle=[0,0,1920,1080],camera=list(origin),timeout_seconds=120)
  metal=mod.capture(config,folder/'metal');assert metal['status']=='pass',metal
  rows=re.findall(r'energyspray-phase\] tick=(\d+) phase=(\d+) countdown=(\d+) core=([^ ]+) rendered_scale=([^ ]+) draws=(\d+) rng=(\d+)',(folder/'metal/run.log').read_text())
  for tick,phase,countdown,core,scale,draws,rng in rows:
   n=ref['cases'][int(tick)];assert(int(phase),int(countdown),int(rng),int(draws))==(n['state']['phase'],n['state']['countdown'],n['state']['rng_draws'],len(n['draws']))
  assert len(rows)>=110
  for n,m in zip(native,metal['frames']):
   assert Image.open(m['path']).size==(1920,1080)
  records.append(dict(tile=name,screen_origin=origin,native=native,metal_manifest=str(folder/'metal/manifest.json'),metal_sha256=sha((folder/'metal/manifest.json').read_bytes()),binary_sha256=metal['binary_sha256']))
  print(name,'pass',flush=True)
 # Place fixed overlapping viewports into a2560x1980 coverage canvas, no scale.
 panels=[];comparison=[]
 for index,tick in enumerate(TICKS):
  canvases=[Image.new('RGB',(2560,1980))for _ in range(2)]
  for r in records:
   m=json.loads(Path(r['metal_manifest']).read_text());paths=[r['native'][index]['path'],m['frames'][index]['path']];x,y=r['screen_origin'];dest=(1280-x,1440-y)
   for canvas,p in zip(canvases,paths):canvas.paste(Image.open(p).convert('RGB'),dest)
  paths=[]
  for label,canvas in zip(('native','metal'),canvases):
   p=out/f'{label}-{tick:03d}-whole.png';canvas.save(p);paths.append(str(p))
  p=Image.new('RGB',(1280,520));d=ImageDraw.Draw(p)
  for i,(label,im)in enumerate(zip(('Original','Native-domain Metal'),canvases)):
   p.paste(im.resize((640,495)),(i*640,25));d.text((i*640+4,4),f'{label} tick{tick}, fixed tiled coverage',fill='white')
  panels.append(p);comparison.append(dict(tick=tick,images=paths,sha256=[sha(Path(p).read_bytes())for p in paths],bounds=[im.getbbox()for im in canvases]))
 contact=Image.new('RGB',(1280,1560))
 for i,p in enumerate(panels):contact.paste(p,(0,i*520))
 contact.save(out/'contact.png');report=dict(status='awaiting_independent_review',accepted=False,tiles=records,cases=comparison,contact_sha256=sha((out/'contact.png').read_bytes()),original_manifest_sha256=sha((base/'energyspray-native-final/manifest.json').read_bytes()),probe_sha256=sha(Path(__file__).read_bytes()),scope='Supported1920x1080 viewports with four fixed screen-origin offsets, covering projected x[-1280,1280],y[-1440,540]. No geometry/scale/worldcamera changes. Native constant640/480 triangle-span guard unchanged. Each selected phase full original software raster; intervening original Render executes with raster boundary disabled, every state/RNG/full draw packet asserted identical to complete640 replay. Actual Metal renders all24Hz ticks, capture stride20 saves70/90/110. Four unscaled overlapping viewports assembled by integer translation only; native projection and original Render-RNG unchanged. General destination rounding/edge interpolation remain open.')
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(sha((out/'manifest.json').read_bytes()),flush=True)
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('executable',type=Path)
 for k in('base','output','capture-tool'):p.add_argument('--'+k,type=Path,required=True)
 a=p.parse_args();run(a.executable,a.base,a.output,a.capture_tool)
