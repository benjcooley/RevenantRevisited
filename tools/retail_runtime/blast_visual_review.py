#!/usr/bin/env python3
"""Freeze original/common/native-domain Blast pairs; appearance remains held."""
import argparse,hashlib,json,re
from pathlib import Path
from PIL import Image,ImageDraw
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def review(native,common,domain,output):
 output.mkdir(parents=True,exist_ok=False);ref=json.loads((native/'manifest.json').read_text());captures=[json.loads((p/'manifest.json').read_text())for p in(common,domain)]
 assert ref['warm_replays']==2 and ref['source_mode']==80
 for index,(path,m)in enumerate(zip((common,domain),captures)):
  assert m['status']=='pass'and m['scenario']['native_domain']==bool(index)
  assert m['scenario']['effect']=='TBlastEffect_BESPOKE'and m['scenario']['frames']==120
  log=(path/'run.log').read_text();assert 'ambient=(0.149019614,0.149019614,0.149019614) directional=(1,1,1) point_lights=0'in log
 ticks=(2,6,16,30,46,60,90,119);panel=Image.new('RGB',(960,len(ticks)*265));draw=ImageDraw.Draw(panel);cases=[]
 for row,tick in enumerate(ticks):
  n=next(c for c in ref['cases']if c['tick']==tick);paths=[Path(n['image'])]+[Path(m['frames'][tick-2]['path'])for m in captures]
  hashes=[n['image_sha256']]+[m['frames'][tick-2]['sha256']for m in captures];assert list(map(sha,paths))==hashes
  cases.append(dict(tick=tick,images=list(map(str,paths)),sha256=hashes,native_draws=len(n['draws']),native_rejected_triangles={d['object']:d['oversized_triangles']for d in n['draws']if d.get('oversized_triangles')}))
  for col,(path,label)in enumerate(zip(paths,('Original','Common-world Metal','Native-domain Metal'))):
   panel.paste(Image.open(path).convert('RGB').resize((320,240)),(col*320,row*265+25));draw.text((col*320+2,row*265+5),f'{label} tick{tick}',fill='white')
 panel.save(output/'contact.png')
 log=(domain/'run.log').read_text();matrices={(int(t),int(o)):list(map(float,v.split(',')))for t,o,v in re.findall(r'blast-matrix\] tick=(\d+) object=(\d+) m=\(([^)]+)\)',log)};errors=[]
 for n in ref['cases']:
  if n['tick']not in(16,60):continue
  assert [d['object']for d in n['draws']]==[o for t,o in matrices if t==n['tick']]
  errors.extend(abs(a-b)for d in n['draws']for a,b in zip(d['matrix'],matrices[n['tick'],d['object']]))
 assert max(errors)<3e-5
 report=dict(status='source_progress_appearance_held',accepted=False,retail_name='Blast',retail_type_id='0x0ea34fa3',cases=cases,actual_runtime_matrix_channels=len(errors),maximum_actual_runtime_matrix_error=max(errors),input_manifest_sha256=[sha(p/'manifest.json')for p in(native,common,domain)],binary_sha256=[m['binary_sha256']for m in captures],contact_sha256=sha(output/'contact.png'),appearance_fitted=False,scope='Exact authored56-part integer animation, eight camera-facing parts, scrolltex and immediate mode80 blend. Native completeRender/RenderObject/normal illumination/three exactRGB565 textures. Source actualowner NextFrame/Pulse and separate world callback;120frames per route,118visible and blanktail. Original textured raster rejects oversized triangles, including outer shock8 at60; Metal correctly retains complete geometry and clips to viewport. Appearance credit held; natural caller/scene/audio and exact raster parity remain open.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'])
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__)
 for name in('native','common','domain','output'):p.add_argument('--'+name,type=Path,required=True)
 a=p.parse_args();review(a.native,a.common,a.domain,a.output)
