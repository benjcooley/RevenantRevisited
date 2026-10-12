#!/usr/bin/env python3
"""Actual translated-owner Paralize1 gallery proxy liveness, not natural-map proof."""
import argparse,json,os,subprocess,time
from pathlib import Path
from PIL import Image
from paralize_visual_reference import ROOT
from speed_profile_contract import sha

def run(binary,data,output):
 output.mkdir(parents=True,exist_ok=False);save=output/'save';save.mkdir();(save/'Revenant.ini').write_bytes((data/'revenant.ini').read_bytes());pinned=sha(binary.read_bytes());command=[str(binary),'--headless','--test=vfx-review','--vfx-review-effects=0xad92bc14','--vfx-review-first','--partsys-quality=0','--vfx-review-speed=0.1','--resolution=640x480','--vfx-no-ui','--filmstrip=120,0.0416666666666667','--snapstep=0.0416666666666667','--snapwarmup=1','--snapseed=1','--snaprect=0,0,640,480','--snapprefix='+str(output/'frame-')];start=time.monotonic()
 with(output/'run.log').open('wb')as log:q=subprocess.run(command,cwd=ROOT,env=dict(os.environ,REVENANT_DATA_PATH=str(data),REVENANT_SAVE_PATH=str(save),REVENANT_ASSETS_PATH=str(ROOT/'assets')),stdout=log,stderr=log,timeout=90)
 frames=sorted(output.glob('frame-[0-9][0-9][0-9]-*.png'));log=(output/'run.log').read_text();pixels=[Image.open(f).convert('RGB')for f in frames];hashes=[sha(im.tobytes())for im in pixels];visible=sum(im.getbbox()is not None for im in pixels);ok=q.returncode==0 and len(frames)==120 and 'type=ad92bc14'in log and log.count('[paralize] typed_id=ad92bc14')>=2 and 'implementation=TParalize1_NAMED_NO_TARGET'in log  and visible>0 and len(set(hashes))>1 and '[shutdown] complete'in log and sha(binary.read_bytes())==pinned
 report=dict(status='pass'if ok else'fail',type_id='0xad92bc14',binary_sha256=pinned,command=command,exit_code=q.returncode,elapsed_seconds=time.monotonic()-start,frames=[dict(path=str(f),sha256=sha(f.read_bytes()),pixel_sha256=h,visible=im.getbbox()is not None)for f,h,im in zip(frames,hashes,pixels)],visible_frames=visible,distinct_frames=len(set(hashes)),log_sha256=sha((output/'run.log').read_bytes()),accepted=False,scope='Real scrolling review map with exact Paralize1 typed preview, nonzero owner station, ordinary projection, 120frame liveness at declared quality0 and clean shutdown. Gallery proxy uses actual typed imagery and controller; This is only a partial named-spell/no-target branch; whole trajectory coverage, target-bound behavior and natural caller/device are not certified. Original21-tick completion drives factory restart; no invented lifetime or global policy change.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],visible,len(set(hashes)),pinned);return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--binary',type=Path,required=True);p.add_argument('--data-root',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.binary.resolve(),a.data_root.resolve(),a.output.resolve())
