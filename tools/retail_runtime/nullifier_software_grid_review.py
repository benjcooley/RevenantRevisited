"""Freeze original SW/nullifier integer-grid appearance and unchanged controls."""
import argparse,csv,json
from pathlib import Path
from PIL import Image,ImageDraw,ImageStat
from fspray_device_review import sha,error

def review(base,software,modern,common,output,proof):
 output.mkdir(parents=True,exist_ok=False);pins={}
 def pin(p):pins[str(Path(p).resolve())]=sha(p)
 def load(p):pin(p);return json.loads(p.read_text())
 n=load(proof);assert n['status']=='pass'and n['triangles']==5546 and n['rgba5_channels']==44368
 def film(p):
  m=load(p);assert m['status']=='pass'and m['exit_code']==0 and not m['forced_owned_shutdown'];assert len(m['frames'])==75
  timing=p.parent/'frame-timing.csv';pin(timing);result={}
  for t,f in zip(csv.DictReader(timing.open()),m['frames']):
   tick=round(float(t['simulation_seconds'])*24);assert abs(tick-float(t['simulation_seconds'])*24)<1e-6;pin(f['path']);im=Image.open(f['path']).convert('RGB');assert im.size==(640,480);result[tick]=im
  assert set(result)==set(range(2,77));return m,result
 before,b=film(base/'software-transition/manifest.json');after,a=film(software)
 _,hm=film(modern);_,ho=film(base/'modern-transition/manifest.json');_,cm=film(common);_,co=film(base/'common-before/manifest.json')
 assert all(hm[t].tobytes()==ho[t].tobytes()for t in hm);assert all(cm[t].tobytes()==co[t].tobytes()for t in cm)
 rows=[];views=[]
 for row in n['rows']:
  tick=row['tick']
  if tick not in a or 'software'not in row['images']:continue
  ref=row['images']['software']['path'];pin(ref);im=Image.open(ref).convert('RGB');eb=error(im,b[tick]);ea=error(im,a[tick]);assert ea<=eb,(tick,eb,ea)
  rows.append(dict(tick=tick,before_error=eb,after_error=ea,channel_sums={label:list(map(int,ImageStat.Stat(image).sum))for label,image in(('original',im),('before',b[tick]),('after',a[tick]))}));views.append((tick,im,b[tick],a[tick]));pair=Image.new('RGB',(1920,504));d=ImageDraw.Draw(pair)
  for c,(label,image)in enumerate((('original software',im),('Metal floating inputs',b[tick]),('Metal native integer inputs',a[tick]))):d.text((640*c+8,5),f'{tick}: {label}',fill='white');pair.paste(image,(640*c,24))
  p=output/f'pair-{tick:03}.png';pair.save(p);pin(p)
 for page in range((len(views)+2)//3):
  panel=Image.new('RGB',(1800,2004));d=ImageDraw.Draw(panel)
  for r,entry in enumerate(views[page*3:page*3+3]):
   d.text((8,r*668+5),f'{entry[0]}: original SW / floating-input Metal / integer-input Metal; same crop 4x NEAREST',fill='white')
   for c,im in enumerate(entry[1:]):panel.paste(im.crop((250,180,400,340)).resize((600,640),Image.Resampling.NEAREST),(600*c,r*668+28))
  p=output/f'detail-{page+1}.png';panel.save(p);pin(p)
 report=dict(status='ready-for-independent-review',accepted=False,new_effect_credit=False,type_id='0xad92bd38',scope='Exact native-domain software nullifier uses original integer XYZ/UV64 raster inputs and integer sample lattice. No brightness/color/texture/timing fit. The existing RGB5 modulation remains unchanged.',hardware_exact_images=75,common_exact_images=75,before_binary=before['binary_sha256'],after_binary=after['binary_sha256'],original_triangles=5546,original_rgba5_channels=44368,pairs=rows,before_total_error=sum(r['before_error']for r in rows),after_total_error=sum(r['after_error']for r in rows),limits=['Original fixed-point scanline texture interpolation and destination RGB565 rounding differ from GPU rasterization','Not exact framebuffer or natural caller/device fidelity','Other profiles and ordinary/common/hardware inputs are unchanged'],pins=pins)
 (output/'pair-manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['before_total_error'],report['after_total_error'],sha(output/'pair-manifest.json'));return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__)
 for name in('base','software','modern','common','output','proof'):p.add_argument('--'+name,type=Path,required=True)
 a=p.parse_args();review(a.base,a.software,a.modern,a.common,a.output,a.proof)
