"""Maelstrom original primitive-span differential at the expanded authored cones."""
import argparse,json,struct
from pathlib import Path
from speed_profile_contract import ROOT,sha
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EFLAGS
def run_visual(exe,output,variant="maelstrom"):
 import zipfile
 from maelstrom_visual_reference import MaelstromScene
 from software_probe import save_rgb565_png
 output.mkdir(parents=True,exist_ok=True)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read('Imagery/Magic/'+variant+'.I3D')
 f=MaelstromScene(exe,data,registry_name=variant)
 for tick in range(131):
  f.step();f.animate();f.raster_enabled=False
  if tick<130:f.render()
 v=f.vm;v.checkpoint();count=f.rng_draws;guards=[]
 def observe(uc,address,*_):
  guards.append(dict(object=f.current_object,address=hex(address),reject=bool(uc.reg_read(UC_X86_REG_EFLAGS)&64)))
 for address in(0x56e006,0x56e036,0x56e05c,0x56e082,0x56e36a,0x56e395,0x56e3bb,0x56e3e1,0x56e6f1,0x56e71c,0x56e742,0x56e768):
  v.uc.hook_add(UC_HOOK_CODE,observe,begin=address,end=address)
 rows=[]
 for name,limits in [('original',(640,480)),('widened_negative_control',(6400,4800))]:
  v.restore();f.rng_draws=count;guards.clear();v.write(0x5b9cc0,struct.pack('<f',limits[0]));v.write(0x5b9cbc,struct.pack('<f',limits[1]));f.raster_enabled=True;pixels=f.render()
  image=output/(name+'.png');save_rgb565_png(image,pixels,1280,960)
  rows.append(dict(name=name,limits=limits,guards=list(guards),pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',pixels)),state=f.state(),draws=f.draws,image=str(image.resolve()),image_sha256=sha(image.read_bytes())))
 assert rows[0]['state']==rows[1]['state'] and rows[0]['draws']==rows[1]['draws']
 assert not any(g['reject']for g in rows[1]['guards'])
 rejected=any(g['reject']for g in rows[0]['guards'])
 assert rows[1]['pixels']>=rows[0]['pixels']
 if not rejected:assert rows[1]['image_sha256']==rows[0]['image_sha256']
 report=dict(status='pass_rejected_guard_control'if rejected else'pass_unchanged_guard_control',capture=130,controls=rows,source_sha256=sha(Path(__file__).read_bytes()),original_sha256=sha(exe.read_bytes()),scope='Only literal raster guard constants changed in the second branch. Authored vertices, matrices, colors, controller state and RNG are identical. Rejections and image changes are recorded explicitly; an unchanged phase is not a rejection proof.')
 (output/'proof.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],rows[0]['pixels'],rows[1]['pixels']);return report

if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('exe',type=Path);p.add_argument('--output',type=Path,required=True);p.add_argument('--variant',choices=['maelstrom','ymaelstrom'],default='maelstrom');a=p.parse_args();run_visual(a.exe,a.output,a.variant)
