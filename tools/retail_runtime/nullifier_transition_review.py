"""Freeze nullifier's original five-frame emitter transition in both policies."""
import argparse,csv,json
from pathlib import Path
from PIL import Image,ImageDraw
from fspray_device_review import sha,error

def review(base,modern,software,output,source_comparison):
 output.mkdir(parents=True,exist_ok=False);pins={}
 def pin(p):pins[str(Path(p).resolve())]=sha(p)
 def load(p):pin(p);return json.loads(p.read_text())
 n=load(base/'native/manifest.json');s=load(base/'software-native/manifest.json');proof=load(source_comparison);assert proof['status']=='pass'and proof['frames']==75
 def film(p):
  m=load(p);assert m['status']=='pass'and m['exit_code']==0 and not m['forced_owned_shutdown'];assert len(m['frames'])==75
  timing=p.parent/'frame-timing.csv';pin(timing);rows=list(csv.DictReader(timing.open()));result={}
  for row,frame in zip(rows,m['frames']):
   tick=round(float(row['simulation_seconds'])*24);assert abs(tick-float(row['simulation_seconds'])*24)<1e-6
   pin(frame['path']);im=Image.open(frame['path']).convert('RGB');assert im.size==(640,480);result[tick]=im
  assert set(result)==set(range(2,77));return m,result
 bm,b=film(base/'modern-after/manifest.json');am,a=film(modern);sm,sb=film(base/'software-after/manifest.json');tm,sa=film(software)
 rows=[];views=[]
 for f,g in zip(n['frames'],s['frames']):
  assert f['state']==g['state'];tick=f['tick']
  if tick not in a or 'point'not in f['images']:continue
  wanted=[]
  for im in(f['images']['point'],g['images']['software']):pin(im['path']);wanted.append(Image.open(im['path']).convert('RGB'))
  errors=dict(modern_before=error(wanted[0],b[tick]),modern_after=error(wanted[0],a[tick]),software_before=error(wanted[1],sb[tick]),software_after=error(wanted[1],sa[tick]))
  rows.append(dict(tick=tick,**errors));views.append((tick,wanted[0],b[tick],a[tick],wanted[1],sb[tick],sa[tick]))
  whole=Image.new('RGB',(1920,1008));d=ImageDraw.Draw(whole)
  for j,(label,im)in enumerate(zip(('qualified RGB8 original','Metal RGB8 old pose','Metal RGB8 transition','unchanged original software','Metal software old pose','Metal software transition'),views[-1][1:])):
   x=(j%3)*640;y=(j//3)*504;d.text((x+8,y+5),f'{tick}: {label}',fill='white');whole.paste(im,(x,y+24))
  p=output/f'pair-{tick:03}.png';whole.save(p);pin(p)
 for page in range((len(views)+1)//2):
  panel=Image.new('RGB',(1800,4*668));d=ImageDraw.Draw(panel)
  for row,entry in enumerate(views[page*2:page*2+2]):
   tick=entry[0]
   for policy in range(2):
    y=(2*row+policy)*668;d.text((8,y+5),f'tick {tick}; {"qualified hardware" if policy==0 else "original software"} / prior Metal / corrected Metal; identical 4x nearest crop',fill='white')
    for col,im in enumerate(entry[1+3*policy:4+3*policy]):panel.paste(im.crop((250,180,400,340)).resize((600,640),Image.Resampling.NEAREST),(col*600,y+28))
  p=output/f'detail-{page+1}.png';panel.save(p);pin(p)
 report=dict(status='ready-for-independent-review',accepted=False,type_id='0xad92bd38',new_effect_credit=False,scope='Exact nullifier cached emitter samples actual GetAniKey transition with the real previous state/frame. Both policies change geometry; prior SW appearance approval requires re-review. Hardware retains independently qualified RGB8 semantic device scope.',source_corner_comparison=proof,old_binary=bm['binary_sha256'],new_binary=am['binary_sha256'],software_changed=sum(sb[t].tobytes()!=sa[t].tobytes()for t in sb),pairs=rows,totals={k:sum(r[k]for r in rows)for k in('modern_before','modern_after','software_before','software_after')},limits=['No natural map/actor or period-GPU claim','Original software sampling versus GPU edge/texture/target quantization remain non-identical','Other profiles, renderer, textures, blend, geometry topology and color policy are unchanged'],pins=pins)
 (output/'pair-manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['totals'],sha(output/'pair-manifest.json'));return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__)
 for x in('base','modern','software','output','source-comparison'):p.add_argument('--'+x,type=Path,required=True)
 a=p.parse_args();review(a.base,a.modern,a.software,a.output,a.source_comparison)
