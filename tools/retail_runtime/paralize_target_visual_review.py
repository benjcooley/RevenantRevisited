"""Freeze same-tick full-frame target Paralize evidence without phase fitting."""
import argparse,csv,hashlib,json,re,struct
from pathlib import Path
from PIL import Image,ImageDraw
from software_probe import SoftwareFixture
from speed_profile_contract import ROOT

def digest(path):return hashlib.sha256(path.read_bytes()).hexdigest()

def run(native,metal,state,output):
 output.mkdir(parents=True,exist_ok=False)
 n=json.loads((native/'manifest.json').read_text());m=json.loads((metal/'manifest.json').read_text());s=json.loads((state/'manifest.json').read_text())
 assert m['status']=='pass' and s['status']in('pass','bounded_float_residual') and s['exact_integer_rng_and_draw_count'] and m['exit_code']==0
 assert len(m['frames'])==m['scenario']['frames']==280
 timing=list(csv.DictReader((metal/'frame-timing.csv').open()))
 by_tick={round(float(row['simulation_seconds'])*24):frame for row,frame in zip(timing,m['frames'])}
 assert list(by_tick)==m['sample_simulation_ticks']==list(range(2,282))
 log=(metal/'run.log').read_text();phases=re.findall(r'\[paralize-phase\] tick=(\d+) variant=(\d+) phase=(\d+) count=(-?\d+) scale=([^ ]+) knot=([^ ]+) first=\(([^)]+)\) alive=(\d+)',log)
 assert len(phases)==260 and phases[-1][-1]=='0'
 # Independently compiled actual production core is compared with actual GPU
 # process logs, in addition to its complete original owner-field comparison.
 source=(state/'source.bin').read_bytes();phase_error=0
 for row in phases:
  tick=int(row[0]);data=source[(tick-1)*972+8:tick*972]
  expected=[struct.unpack_from('<i',data,a-0x184)[0]for a in(0x404,0x3ac,0x444)]
  assert expected==list(map(int,row[1:4]))
  expected=[struct.unpack_from('<f',data,a-0x184)[0]for a in(0x504,0x500,0x448,0x44c,0x450)]
  observed=list(map(float,row[4:6]))+list(map(float,row[6].split(',')))
  phase_error=max(phase_error,max(abs(a-b)for a,b in zip(expected,observed)))
 assert phase_error<.00002,phase_error
 selected=[row for row in n['records']if 'image'in row and row['tick']in by_tick]
 whole=Image.new('RGB',(1280,500*len(selected)),(18,18,18))
 detail=Image.new('RGB',(800,224*len(selected)),(18,18,18))
 wd=ImageDraw.Draw(whole);dd=ImageDraw.Draw(detail);cases=[]
 projection=SoftwareFixture(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',1280,960)
 for i,row in enumerate(selected):
  tick=row['tick'];pair=[Path(row['image']),Path(by_tick[tick]['path'])];bounds=[]
  for draw in row['draws']:
   vertices=[v[:3]for v in struct.iter_unpack('<8f',bytes.fromhex(draw['vertices_hex']))]
   points=projection.project(vertices,draw['matrix'],camera=n['camera'])
   xs=[p[0]for p in points];ys=[p[1]+160 for p in points]
   bound=[min(xs),min(ys),max(xs),max(ys)];bounds.append(bound)
   assert 0<bound[0]<=bound[2]<1279 and 0<bound[1]<=bound[3]<959,(tick,bound)
  images=[Image.open(p).convert('RGB')for p in pair]
  assert all(im.size==(1280,960)for im in images)
  for col,im in enumerate(images):
   whole.paste(im.resize((640,480)),(col*640,i*500+20))
   detail.paste(im.crop((520,500,920,700)),(col*400,i*224+24))
   label=f'tick {tick} '+('original'if col==0 else'Metal')
   wd.text((col*640+8,i*500+3),label,fill='white');dd.text((col*400+8,i*224+4),label,fill='white')
  cases.append(dict(tick=tick,native_image=str(pair[0]),native_sha256=digest(pair[0]),metal_image=str(pair[1]),metal_sha256=digest(pair[1]),whole_projected_geometry_bounds=bounds,draw_count=row['draw_count'],pixel_bounds=[im.getbbox()for im in images]))
 whole.save(output/'contact.png');detail.save(output/'detail.png')
 report=dict(status='awaiting_independent_review',accepted=False,type_id='0xad92bc14',fixture='Physical Paralysis variant1, genuine stationary Acolyte invoker/target',binary_sha256=m['binary_sha256'],native_executable_sha256=n['executable_sha256'],inputs={str(p):digest(p)for p in(native/'manifest.json',metal/'manifest.json',state/'manifest.json',metal/'frame-timing.csv')},cases=cases,actual_gpu_phase_maximum_error=phase_error,native_source_numerical_status=s['status'],native_source_numerical_exceptions=s['errors'],capture_timing='Exactly280 numbered images, CSV simulationticks2..281; frame-filmstrip.png is a generated contact sheet, not a simulation sample.',scope='Actual loaded actors/spell/resource/typed owner/animator Init and complete custom Render/RenderObject/original raster. Executed base owner/PreRender matrix fragments; explicit isolated camera/RAM framebuffer/source38+dir1 and material-handle COM get/set boundary. No character, effect, resource, rand or raster instruction replacement. Port uses real Acolytes with explicit one-target variant fixture; natural spell scheduling, animated actors, damage/audio/device and exact pixels remain open. Existing null-invoker route remains partial. UV accumulation and RGB565/framebuffer precision limits are reported separately.',contact_sha256=digest(output/'contact.png'),detail_sha256=digest(output/'detail.png'))
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],len(cases),phase_error);return report

if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--native',type=Path,required=True);p.add_argument('--metal',type=Path,required=True);p.add_argument('--state',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.native,a.metal,a.state,a.output)
