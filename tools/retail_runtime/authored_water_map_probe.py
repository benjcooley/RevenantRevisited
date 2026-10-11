#!/usr/bin/env python3
"""Real map create/frame/move/delete after the authored zero-scale birth phase."""
import argparse,hashlib,json,os,re,subprocess,time
from pathlib import Path
from PIL import Image,ImageDraw
from map_texture_probe import ROOT,digest,image_digest

def run(binary,data,output,profile):
 output=output.resolve();output.mkdir(parents=True,exist_ok=False);name=profile['name'];identity=profile['id'];x=y=10000;z=16
 rows=[f'3 | - | addat {x} {y} {name}',f'3 | {name} | @expect {identity} {x} {y} {z} unused 0']
 rows.extend(f'{tick} | {name} | @frame {tick-3}'for tick in range(4,8))
 rows.extend([f'30 | {name} | move 16 -8 4',f'31 | {name} | @expect {identity} {x+16} {y-8} {z+4} unused 0',f'43 | {name} | delete',f'44 | {name} | @absent'])
 commands=output/'commands.txt';commands.write_text('\n'.join(rows)+'\n');save=output/'save';save.mkdir();(save/'Revenant.ini').write_bytes((data/'revenant.ini').read_bytes())
 env=os.environ.copy();env.update(REVENANT_DATA_PATH=str(data.resolve()),REVENANT_SAVE_PATH=str(save),REVENANT_ASSETS_PATH=str(ROOT/'assets'))
 binary=binary.resolve();pinned=digest(binary);source=digest(__file__);command=[str(binary),'--headless','--test=sector','--scene-module=FontRuntimeLab','--scene-camera=110,10000,10000,16','--scene-ambient=32,255,255,255','--scene-command-file='+str(commands),'--filmstrip=48,0.0416666666666667','--snapstep=0.0416666666666667','--snapseed=1','--snapwarmup=1','--snaprect=0,0,640,340','--snapprefix='+str(output/'frame-')]
 start=time.monotonic();forced=False
 with (output/'run.log').open('wb')as log:
  process=subprocess.Popen(command,cwd=output,env=env,stdout=log,stderr=log)
  try:process.wait(timeout=60)
  except subprocess.TimeoutExpired:
   forced=True;process.terminate()
   try:process.wait(timeout=2)
   except subprocess.TimeoutExpired:process.kill();process.wait(timeout=2)
 log=(output/'run.log').read_text(errors='replace');frames=sorted(output.glob('frame-[0-9][0-9][0-9]-*.png'));hashes=[image_digest(p)for p in frames];errors=[]
 if forced or process.returncode!=0:errors.append('Process did not exit cleanly')
 if len(frames)!=48:errors.append('Incomplete captures')
 if f'COMPLETE rows={len(rows)} failures=0'not in log or 'status=FAIL'in log:errors.append('Map lifecycle observations failed')
 visible=len(hashes)==48 and any(h!=hashes[0]for h in hashes[9:29]);distinct=len(set(hashes[9:29]));restored=len(hashes)==48 and hashes[0]==hashes[-1]
 if not visible:errors.append('No visible authored particles after birth warmup')
 if distinct<4:errors.append('Particle lifecycle did not visibly evolve')
 if not restored:errors.append('Deleted owner did not restore exact floor')
 if digest(binary)!=pinned or digest(__file__)!=source:errors.append('Binary/probe changed during capture')
 panel=Image.new('RGB',(4*640,3*365),(16,16,16));draw=ImageDraw.Draw(panel)
 for i,frame in enumerate([1,4,10,15,20,25,29,30,32,40,44,48]):
  if len(frames)<frame:continue
  panel.paste(Image.open(frames[frame-1]).convert('RGB'),((i%4)*640,(i//4)*365+25));draw.text(((i%4)*640+4,(i//4)*365+4),f'{name} actualmap frame{frame}',fill='white')
 panel.save(output/'map-lifecycle.png')
 report=dict(status='pass'if not errors else'differences_found',errors=errors,name=name,type_id=identity,binary_sha256=pinned,probe_sha256=source,commands_sha256=digest(commands),command=command,exit_code=process.returncode,forced_shutdown=forced,elapsed_seconds=time.monotonic()-start,command_rows=len(rows),completed_rows=len(re.findall(r'row line=.*status=PASS',log)),created_visible=visible,distinct_stationary_frames=distinct,stationary_frame_window=[10,29],floor_restored=restored,frames=[dict(path=str(p),pixel_sha256=h)for p,h in zip(frames,hashes)],log_sha256=digest(output/'run.log'),panel_sha256=digest(output/'map-lifecycle.png'),accepted=False,scope='Actual typed NewObject/default animator/six-emitter PartSys/owner frame observations, stationary visible birth/evolution then realMOVE/position/DELETE/absence and exact restored syntheticfloor. The authored scale starts0; visibility is tested after emission warmup, not immediately at creation. Sourceambient32 white, scene ground16. Natural world/retail-device placement/lighting/depth acceptance separate.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--binary',type=Path,required=True);p.add_argument('--data-root',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args()
 for profile in json.loads((ROOT/'tools/retail_runtime/authored_water_profiles.json').read_text())['profiles']:
  r=run(a.binary,a.data_root,a.output/profile['name'],profile);print(profile['name'],r['status'],r['errors'],flush=True)
