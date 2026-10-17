"""Actual preferred exact Labyrinth gallery route, persistent live RNG stream."""
import argparse,json,os,subprocess,time
from pathlib import Path
from PIL import Image
from speed_controller_preflight import ROOT
from speed_profile_contract import sha

def run(binary,data,out):
 out.mkdir(parents=True,exist_ok=False);save=out/'save';save.mkdir();(save/'Revenant.ini').write_bytes((data/'revenant.ini').read_bytes());pinned=sha(binary.read_bytes());command=[str(binary),'--headless','--test=vfx-review','--vfx-review-effects=0xad92bd20','--vfx-review-first','--vfx-review-speed=0.1','--vfx-review-source-lighting','--resolution=640x480','--vfx-no-ui','--filmstrip=80,0.08','--snapstep=0.0416666666666667','--snapwarmup=1','--snapseed=1','--snaprect=0,0,640,480','--snapprefix='+str(out/'frame-')];start=time.monotonic()
 with(out/'run.log').open('wb')as log:q=subprocess.run(command,cwd=ROOT,env=dict(os.environ,REVENANT_DATA_PATH=str(data),REVENANT_SAVE_PATH=str(save),REVENANT_ASSETS_PATH=str(ROOT/'assets')),stdout=log,stderr=log,timeout=120)
 frames=sorted(out.glob('frame-[0-9][0-9][0-9]-*.png'));text=(out/'run.log').read_text();rows=[]
 for f in frames:
  im=Image.open(f).convert('RGB');rows.append(dict(path=str(f),sha256=sha(f.read_bytes()),pixel_sha256=sha(im.tobytes()),visible=im.getbbox()is not None))
 logs=[line for line in text.splitlines()if'[labyrinth-qualified] typed_id=ad92bd20'in line];ok=q.returncode==0 and len(frames)==80 and logs and all('source_seed=none'in line for line in logs)and'implementation=Labyrinth_QUALIFIED_BOUND partial=0'in text and'[shutdown] complete'in text and sha(binary.read_bytes())==pinned
 r=dict(status='pass'if ok else'fail',accepted=False,type_id='0xad92bd20',binary_sha256=pinned,command=command,elapsed_seconds=time.monotonic()-start,exit_code=q.returncode,frames=rows,visible_frames=sum(r['visible']for r in rows),distinct_frames=len(set(r['pixel_sha256']for r in rows)),spawn_logs=logs,log_sha256=sha((out/'run.log').read_bytes()),scope='ActualexactLabyrinth qualifiedinitializer1..3 preferredgallery route/realcamera-follow/100frame loop andpersistentunreseeded150-starcontroller. Ordinarybackground retained; explicitnativeSW extentgateonlytest-domain. Visibleframes aregallery liveness, notnaturalmap appearance acceptance. No changedasset/caller/fifthprototype.');(out/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');print(r['status'],r['visible_frames'],r['distinct_frames']);return r
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--binary',type=Path,required=True);p.add_argument('--data-root',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.binary.resolve(),a.data_root.resolve(),a.output.resolve())
