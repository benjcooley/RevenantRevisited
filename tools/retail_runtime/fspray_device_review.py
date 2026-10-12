"""Freeze a narrow fspray hardware policy correction and unchanged SW film."""
import argparse,csv,hashlib,json
from pathlib import Path
from PIL import Image,ImageChops,ImageDraw,ImageStat

def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def error(a,b):return int(sum(ImageStat.Stat(ImageChops.difference(a,b)).sum))
def review(native,repeat,software,before,after,sw_before,sw_after,out):
 out.mkdir(parents=True,exist_ok=False);pins={}
 def pin(p):p=Path(p);pins[str(p.resolve())]=sha(p)
 def load(p):pin(p);return json.loads(p.read_text())
 n,r,s=(load(p)for p in(native,repeat,software))
 assert len(n['frames'])==len(r['frames'])==len(s['frames'])==61
 for a,b,c in zip(n['frames'],r['frames'],s['frames']):
  assert a['tick']==b['tick']==c['tick'];assert a['state']==b['state']==c['state'];assert a['packet_sha256']==b['packet_sha256'];assert a['packets']==b['packets']
  for key,image in a['images'].items():
   pin(image['path']);pin(b['images'][key]['path']);assert Image.open(image['path']).tobytes()==Image.open(b['images'][key]['path']).tobytes()
  for image in c['images'].values():pin(image['path'])
 def film(p):
  m=load(p);assert m['status']=='pass'and m['exit_code']==0 and not m['forced_owned_shutdown'];assert len(m['frames'])==60
  timing=p.parent/'frame-timing.csv';pin(timing);rows=list(csv.DictReader(timing.open()));assert len(rows)==60;result={}
  for row,frame in zip(rows,m['frames']):
   tick=round(float(row['simulation_seconds'])*24);assert abs(tick-float(row['simulation_seconds'])*24)<1e-6
   pin(frame['path']);im=Image.open(frame['path']).convert('RGB');assert im.size==(640,480);result[tick]=im
  return m,result
 bm,b=film(before);am,a=film(after);sbm,sb=film(sw_before);sam,sa=film(sw_after)
 assert set(sb)==set(sa)==set(b)==set(a)
 for tick in sb:assert sb[tick].tobytes()==sa[tick].tobytes()
 rows=[];views=[]
 for f in n['frames']:
  tick=f['tick']
  if tick not in a or 'point'not in f['images']:continue
  wanted=Image.open(f['images']['point']['path']).convert('RGB');old=Image.open(f['images']['point-rgb5']['path']).convert('RGB')
  eb,ea=error(wanted,b[tick]),error(wanted,a[tick]);assert ea<eb
  rows.append(dict(tick=tick,reference=f['images']['point']['path'],before_error=eb,after_error=ea,before_vs_old_rgb5_error=error(old,b[tick]),after_vs_old_rgb5_error=error(old,a[tick])))
  pair=Image.new('RGB',(1920,504));draw=ImageDraw.Draw(pair)
  for i,(label,im)in enumerate((('qualified original RGB8',wanted),('Metal before RGB5',b[tick]),('Metal corrected RGB8',a[tick]))):draw.text((640*i+8,5),f'tick {tick}: {label}',fill='white');pair.paste(im,(640*i,24))
  path=out/f'pair-{tick:03}.png';pair.save(path);pin(path);views.append((tick,wanted,b[tick],a[tick]))
 crop=(280,250,370,390)
 for page in range((len(views)+2)//3):
  panel=Image.new('RGB',(1080,3*588));draw=ImageDraw.Draw(panel)
  for row,(tick,wanted,old,new)in enumerate(views[page*3:page*3+3]):
   y=row*588;draw.text((8,y+5),f'tick {tick}: qualified RGB8 / old Metal RGB5 / corrected Metal RGB8; fixed crop 4x nearest',fill='white')
   for col,im in enumerate((wanted,old,new)):panel.paste(im.crop(crop).resize((360,560),Image.Resampling.NEAREST),(col*360,y+28))
  path=out/f'detail-{page+1}.png';panel.save(path);pin(path)
 report=dict(status='ready-for-independent-review',accepted=False,new_effect_credit=False,qualified_device=True,type_id='0x0c052638',scope='Exact admitted fspray modern/hardware colors retain original packed RGB8. Genuine loaded factory/controller/fullRender; declared RGB8 target, point sampler, modern half-pixel centers. Software policy remains exactly unchanged.',phase_rule='recorded simulation_seconds*24; no fitted offset',native_repeat_exact_frames=61,native_software_hardware_state_pool_rng_exact=61,software_metal_before_after_exact_images=60,before_binary_sha256=bm['binary_sha256'],after_binary_sha256=am['binary_sha256'],before_total_error=sum(x['before_error']for x in rows),after_total_error=sum(x['after_error']for x in rows),pairs=rows,limits=['Independent qualified device; not a period-GPU claim','RGB565 texture realization, interpolation and per-fragment target quantization leave small nonzero residuals','Other admitted PartSys hardware policies remain unaudited and unchanged','Existing original-software appearance acceptance is preserved, hardware revision requires separate review'],pins=pins)
 (out/'pair-manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['before_total_error'],report['after_total_error'],sha(out/'pair-manifest.json'))
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__)
 for name in('native','repeat','software','before','after','sw-before','sw-after','output'):p.add_argument('--'+name,type=Path,required=True)
 a=p.parse_args();review(a.native,a.repeat,a.software,a.before,a.after,a.sw_before,a.sw_after,a.output)
