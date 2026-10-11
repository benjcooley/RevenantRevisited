#!/usr/bin/env python3
"""Owned real-map lifecycle captures for the exact three static particle rows."""
import argparse,hashlib,json,os,re,shutil,subprocess
from pathlib import Path
from PIL import Image
from static_particles_profile_contract import PROFILES,Y_PROFILES,CAMERA_PROFILES,ROOT


def sha(path):return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def run(binary,fixture,output,profiles=PROFILES):
 output.mkdir(parents=True,exist_ok=True);cases=[];binary_sha=sha(binary)
 for profile in profiles:
  p=output/profile['name'];p.mkdir(exist_ok=True);save=p/'save';save.mkdir(exist_ok=True);shutil.copyfile(fixture/'revenant.ini',save/'Revenant.ini')
  name=profile['name'];ident=profile['id'];new=name in {p['name']for p in Y_PROFILES+CAMERA_PROFILES}
  emitters,capacity=({'Ogrestrength':(3,46),'trollblood':(3,41),'YEnergy':(1,112),'YEnergyLose':(1,112),'YAbsorb':(24,41),'nullifier':(8,125)}[name]if new else (1,80)if name=='fspray'else (32,37))
  alive,quads=(1,1)if new else(5,10)
  script=f'3 | - | addat 10000 10000 {name}\n3 | {name} | @expect {ident} 10000 10000 16 unused 0\n9 | {name} | @partsys 1 {emitters} {capacity} 1 3 1\n24 | {name} | move 16 -8 4\n25 | {name} | @expect {ident} 10016 9992 20 unused 0\n25 | {name} | @partsys 1 {emitters} {capacity} {alive} 15 {quads}\n35 | {name} | delete\n36 | {name} | @absent\n';(p/'commands.txt').write_text(script)
  command=[str(binary),'--headless','--test=sector','--scene-module=FontRuntimeLab','--scene-camera=110,10000,10000,16','--scene-ambient=32,255,255,255','--partsys-quality=0','--scene-command-file='+str(p/'commands.txt'),'--filmstrip=48,0.0416666666666667','--snapstep=0.0416666666666667','--snapseed=1','--snapwarmup=1','--snaprect=0,0,640,340','--snapprefix='+str(p/'frame-')]
  env=os.environ.copy();env.update(REVENANT_DATA_PATH=str(fixture),REVENANT_ASSETS_PATH=str(ROOT/'assets'),REVENANT_SAVE_PATH=str(save))
  with(p/'run.log').open('wb')as log:process=subprocess.run(command,cwd=p,env=env,stdout=log,stderr=log,timeout=90)
  log=(p/'run.log').read_text();rows=re.findall(r'\[scene-command\] row .* status=(\w+)',log);frames=sorted(p.glob('frame-[0-9][0-9][0-9]-*.png'));pixels=[]
  for frame in frames:
   with Image.open(frame)as im:pixels.append(hashlib.sha256(im.convert('RGB').tobytes()).hexdigest())
  errors=[];opaque_black=[]
  if process.returncode:errors.append('process_exit')
  if rows!=['PASS']*8 or '[scene-command] COMPLETE rows=8 failures=0'not in log:errors.append('typed_lifecycle')
  if len(frames)!=48:errors.append('frame_count')
  if len(pixels)==48:
   if pixels[0]!=pixels[1]or any(v!=pixels[0]for v in pixels[34:]):errors.append('floor_restore')
   if name in {p['name']for p in CAMERA_PROFILES}:
    # Native RGB565 particle16 and inherited base16 are additive. Hidden
    # path triangles must not replace lit floor pixels with opaque black.
    base=Image.open(frames[0]).convert('RGB');source=list(base.getdata())
    opaque_black=[]
    for i,frame in enumerate(frames[2:34],3):
     current=Image.open(frame).convert('RGB');count=sum(a!=(0,0,0)and b==(0,0,0)for a,b in zip(source,current.getdata()))
     if count:opaque_black.append(dict(frame=i,new_black_pixels=count))
    if opaque_black:errors.append('hidden_mesh_opaque_floor')

   # Creation precedes the first Pulse; that Pulse's newborn scale is0.
   # The native/source scale gate deliberately leaves frames3/4 empty.
   if new:
    # Check four declared early/mid-life frames without requiring a
    # authored black/zero-scale tail to remain visible. Record all hashes.
    if any(pixels[i-1]==pixels[0]for i in (7,12,20,28))or len(set(pixels[4:34]))<4:errors.append('active_visibility')
   elif any(v!=pixels[0]for v in pixels[2:4])or any(v==pixels[0]for v in pixels[4:34]):errors.append('active_visibility')
  if '[shutdown] complete'not in log:errors.append('shutdown')
  report=dict(name=name,type_id=ident,status='fail'if errors else'pass',errors=errors,exit_code=process.returncode,observations=len(rows),frames=len(frames),emitters=emitters,capacity=capacity,opaque_black_floor=[]if name not in {p['name']for p in CAMERA_PROFILES}else opaque_black,distinct_active=len(set(pixels[4:34])),initial_empty_frames=[i+1 for i,h in enumerate(pixels[:4])if h==pixels[0]],first_visible_frame=next((i+1 for i,h in enumerate(pixels)if h!=pixels[0]),None),floor_restored='floor_restore'not in errors,pixel_sha256=pixels,command=command,binary_sha256=binary_sha,commands_sha256=sha(p/'commands.txt'),log_sha256=sha(p/'run.log'),fixture=str(fixture),accepted=False,scope='Exact typed create/controller/emitter/capacity/live draws/MOVE/DELETE/absence on synthetic FontRuntimeLab module. Real Metal image changes and clean restored floor, not native scene/device/character/caller visual acceptance.')
  (p/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');cases.append(dict(name=name,status=report['status'],errors=errors,manifest=str(p/'manifest.json')));print(json.dumps(cases[-1]),flush=True)
 if sha(binary)!=binary_sha:raise AssertionError('Binary changed during capture')
 report=dict(status='pass'if all(c['status']=='pass'for c in cases)else'fail',cases=cases,binary_sha256=binary_sha,probe_sha256=sha(__file__),accepted=False);(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report

if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--binary',type=Path,default=ROOT/'build-merge/Revenant');p.add_argument('--fixture',type=Path,default=Path('/Users/benjamincooley/RevenantRetailLab/captures/runtime-fixtures/fountain-command-20261004/data'));p.add_argument('--output',type=Path,required=True);p.add_argument('--y-family',action='store_true');p.add_argument('--camera-particles',action='store_true');a=p.parse_args();r=run(a.binary.resolve(),a.fixture.resolve(),a.output.resolve(),CAMERA_PROFILES if a.camera_particles else Y_PROFILES if a.y_family else PROFILES)
 if r['status']!='pass':raise SystemExit(1)
