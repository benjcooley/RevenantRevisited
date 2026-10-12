"""Translated PunchAndJudy gallery: persistent581-frame authored loop and cleanup."""
import argparse,json,os,subprocess,re,time
from pathlib import Path
from PIL import Image
from character_native_preflight import ROOT,sha

def run(binary,data,out):
 out.mkdir(parents=True,exist_ok=False);save=out/'save';save.mkdir();(save/'Revenant.ini').write_bytes((data/'revenant.ini').read_bytes());pinned=sha(binary)
 command=[str(binary),'--headless','--test=vfx-review','--vfx-review-effects=0x0c05263a','--vfx-review-first','--vfx-review-speed=0.1','--resolution=640x480','--vfx-no-ui','--filmstrip=32,1','--snapstep=0.0416666666666667','--snapwarmup=1','--snapseed=1','--snaprect=0,0,640,480','--snapprefix='+str(out/'frame-')];start=time.monotonic()
 with(out/'run.log').open('wb')as log:q=subprocess.run(command,cwd=ROOT,env=dict(os.environ,REVENANT_DATA_PATH=str(data),REVENANT_SAVE_PATH=str(save),REVENANT_ASSETS_PATH=str(ROOT/'assets')),stdout=log,stderr=log,timeout=120)
 frames=sorted(out.glob('frame-[0-9][0-9][0-9]-*.png'));log=(out/'run.log').read_text();images=[Image.open(p).convert('RGB')for p in frames];visible=sum(im.getbbox()is not None for im in images)
 cleanup=[int(t)for t in re.findall(r'\[punch-cleanup\] tick=(\d+)',log)];spawns=log.count('[punch-preview] actual owner');phases={int(t):int(f)for t,f in re.findall(r'\[punch-phase\] tick=(\d+) frame=(\d+)',log)}
 ok=q.returncode==0 and len(frames)==32 and visible==32 and spawns==1 and len(cleanup)==1 and cleanup[0]>=744 and all(phases[t]==t%581 for t in range(1,cleanup[0]+1)) and 'implementation=TPunchAndJudy_AUTHORED_OWNER'in log and '[shutdown] complete'in log and sha(binary)==pinned
 report=dict(status='pass'if ok else'fail',accepted=False,visual_pass=False,type_id='0x0c05263a',binary_sha256=pinned,command=command,exit_code=q.returncode,elapsed_seconds=time.monotonic()-start,frames=[dict(path=str(p),sha256=sha(p),bounds=im.getbbox())for p,im in zip(frames,images)],visible_frames=visible,spawns=spawns,cleanup=cleanup,owner_phase_count=len(phases),log_sha256=sha(out/'run.log'),scope='Actual common-world translated station, one persistent owner/default animator, full581-frame wrap and owned cleanup. No invented lifetime or reseed. Native-domain parity and natural map caller remain separate.')
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],visible,cleanup);assert ok
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--binary',type=Path,required=True);p.add_argument('--data-root',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.binary.resolve(),a.data_root.resolve(),a.output.resolve())
