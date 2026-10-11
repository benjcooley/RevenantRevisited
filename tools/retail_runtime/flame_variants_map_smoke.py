#!/usr/bin/env python3
"""Actual exact-ID MapPane factory + review-hook liveness; not natural-map A/B."""
import argparse,json,os,subprocess,time
from pathlib import Path
from PIL import Image
from speed_profile_contract import sha
from flame_variants_visual_reference import PROFILES,ROOT

def run(binary,data,output):
 output.mkdir(parents=True,exist_ok=False);pinned=sha(binary.read_bytes());cases=[]
 for name,id,_ in PROFILES:
  folder=output/name;folder.mkdir();save=folder/'save';save.mkdir();(save/'Revenant.ini').write_bytes((data/'revenant.ini').read_bytes());command=[str(binary),'--headless','--test=vfx-review','--vfx-review-effects='+id,'--vfx-review-first','--vfx-review-speed=0.1','--resolution=640x480','--vfx-no-ui','--filmstrip=40,0.0416666666666667','--snapstep=0.0416666666666667','--snapwarmup=1','--snapseed=1','--snaprect=0,0,640,480','--snapprefix='+str(folder/'frame-')];start=time.monotonic()
  with(folder/'run.log').open('wb')as log:q=subprocess.run(command,cwd=ROOT,env=dict(os.environ,REVENANT_DATA_PATH=str(data),REVENANT_SAVE_PATH=str(save),REVENANT_ASSETS_PATH=str(ROOT/'assets')),stdout=log,stderr=log,timeout=60)
  frames=sorted(folder.glob('frame-[0-9][0-9][0-9]-*.png'));log=(folder/'run.log').read_text();pixels=[Image.open(f).convert('RGB')for f in frames];hashes=[sha(im.tobytes())for im in pixels];visible=sum(im.getbbox()is not None for im in pixels);ok=q.returncode==0 and len(frames)==40 and 'type='+id[2:]in log and 'status=Current map factory; normal authored lifecycle'in log and '[flame-component] attached flipbook component tex=128x128'in log and visible==40 and len(set(hashes))>=8 and '[shutdown] complete'in log and sha(binary.read_bytes())==pinned
  cases.append(dict(name=name,id=id,status='pass'if ok else'fail',command=command,exit_code=q.returncode,elapsed_seconds=time.monotonic()-start,frames=[dict(path=str(f),sha256=sha(f.read_bytes()),pixel_sha256=h)for f,h in zip(frames,hashes)],visible_frames=visible,distinct_frames=len(set(hashes)),log_sha256=sha((folder/'run.log').read_bytes())));print(name,cases[-1]['status'],visible,len(set(hashes)),flush=True)
 report=dict(status='pass'if all(c['status']=='pass'for c in cases)else'fail',binary_sha256=pinned,cases=cases,accepted=False,scope='Actual dedicated map review-mode MapPane.NewObject by exact colored Flame IDs and real normal-map TFlameEffect/quad component, centered40frame liveness + clean mode/map shutdown. Duplicate literal CLASS names prohibit named console addat selecting variants; exact-ID review API used. Actual normal-map renderer/component draws both own typed variants; no gallery helper is used on this path. Natural scene/placement/movement/delete differential A/B remains separate, not claimed.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--binary',type=Path,required=True);p.add_argument('--data-root',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.binary.resolve(),a.data_root.resolve(),a.output.resolve())
