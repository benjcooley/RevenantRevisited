"""Exact loaded fspray hardware RGB8 packet / qualified RGB565 device audit.

Original factory, controller, owner cadence and complete Render execute. Device
semantics are explicitly qualified RGB8 UNORM MODULATE/ONE-ONE with point/linear
sampling and modern half-integer coverage. Legacy integer coverage and the
previous source RGB5 carrier interpretation remain controls, not fitted inputs.
"""
import argparse, hashlib, json
from pathlib import Path
from PIL import Image
from dust_device_reference import DustDevice, raster
from dust_loaded_context_probe import DustLoadedScene
from character_native_preflight import ROOT, sha
from software_probe import save_rgb565_png

SAMPLES=(0,1,5,10,15,20,25,30,39,45,59)
def packed(p):return hashlib.sha256(json.dumps(p,sort_keys=True).encode()).hexdigest()
def run(exe,boot,out,software=False):
 out.mkdir(parents=True,exist_ok=False)
 f=(DustLoadedScene(exe,boot,0,38,(0,0,16),name='fspray') if software else DustDevice(exe,boot,name='fspray',source=(0,0,16)))
 rows=[]
 for tick in range(61):
  if tick:f.step(tick)
  images={};packets=[]
  if software:
   raw=f.render()
   if tick in SAMPLES:
    path=out/f'software-{tick:03}.png';save_rgb565_png(path,raw,640,480);images['software']=dict(path=str(path),sha256=sha(path))
  else:
   packets=f.render_device()
   if tick in SAMPLES:
    for mode,filtering,old,center in [('point','point',False,.5),('linear','linear',False,.5),('point-rgb5','point',True,.5),('linear-rgb5','linear',True,.5),('legacy-linear','linear',False,0)]:
     raw=raster(packets,filtering,legacy_color=old,sample_center=center);path=out/f'{mode}-{tick:03}.png';Image.frombytes('RGB',(640,480),raw).save(path);images[mode]=dict(path=str(path),sha256=sha(path),nonzero_pixels=sum(any(raw[i:i+3])for i in range(0,len(raw),3)))
  rows.append(dict(tick=tick,state=f.state(),draws=len(f.draws)if software else len(packets),packet_sha256=None if software else packed(packets),packets=packets if tick in SAMPLES else [],images=images))
  if tick in SAMPLES:print(tick,rows[-1]['draws'],flush=True)
 report=dict(scope=__doc__,status='reference-complete',accepted=False,qualified_device=not software,original_software=software,type_id='0x0c052638',name='fspray',dimensions=[640,480],source=[0,0,16],camera=[0,0,0],center=[320,320],frames=rows,executable_sha256=sha(exe),boot_sha256=sha(boot),tool_sha256=sha(__file__),device_tool_sha256=sha(Path(__file__).with_name('dust_device_reference.py')),loaded_tool_sha256=sha(Path(__file__).with_name('dust_loaded_context_probe.py')))
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--boot-image',type=Path,required=True);p.add_argument('--output',type=Path,required=True);p.add_argument('--software',action='store_true');a=p.parse_args();run(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',a.boot_image,a.output,a.software)
