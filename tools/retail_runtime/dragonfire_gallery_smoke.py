"""Translated Dragonfire gallery: finite original drain, restart and owned cleanup."""
import argparse,json,os,subprocess,re,time
from pathlib import Path
from PIL import Image
from character_native_preflight import ROOT,sha

def run(binary,data,out):
 out.mkdir(parents=True,exist_ok=False);save=out/'save';save.mkdir();(save/'Revenant.ini').write_bytes((data/'revenant.ini').read_bytes());pinned=sha(binary)
 command=[str(binary),'--headless','--test=vfx-review','--vfx-review-effects=0xad92bc1a','--vfx-review-first','--vfx-review-speed=0.1','--resolution=640x480','--vfx-no-ui','--filmstrip=20,1','--snapstep=0.0416666666666667','--snapwarmup=1','--snapseed=1','--snaprect=0,0,640,480','--snapprefix='+str(out/'frame-')];start=time.monotonic()
 with(out/'run.log').open('wb')as log:q=subprocess.run(command,cwd=ROOT,env=dict(os.environ,REVENANT_DATA_PATH=str(data),REVENANT_SAVE_PATH=str(save),REVENANT_ASSETS_PATH=str(ROOT/'assets')),stdout=log,stderr=log,timeout=120)
 frames=sorted(out.glob('frame-[0-9][0-9][0-9]-*.png'));log=(out/'run.log').read_text();images=[Image.open(p).convert('RGB')for p in frames];visible=sum(im.getbbox()is not None for im in images)
 cleanup=[(int(t),int(a))for t,a in re.findall(r'\[dragonfire-cleanup\] tick=(\d+) alive=(\d+)',log)];complete=[t for t,a in cleanup if not a];spawns=log.count('[dragonfire] exact_id=ad92bc1a');seeds=re.findall(r'\[rng\][^\n]*',log)
 ok=q.returncode==0 and len(frames)==20 and visible>=16 and len(complete)>=4 and all(80<t<100 for t in complete) and spawns==len(cleanup) and 'implementation=TDragonFire_BESPOKE_NULLSPELL'in log and '[shutdown] complete'in log and sha(binary)==pinned
 report=dict(status='pass'if ok else'fail',accepted=False,visual_pass=False,type_id='0xad92bc1a',binary_sha256=pinned,command=command,exit_code=q.returncode,elapsed_seconds=time.monotonic()-start,frames=[dict(path=str(p),sha256=sha(p),bounds=im.getbbox())for p,im in zip(frames,images)],visible_frames=visible,spawns=spawns,cleanup=cleanup,rng_log=seeds,log_sha256=sha(out/'run.log'),scope='Actual common-world translated station, genuine finite two-pool controller and real alive callback. No per-factory RNG reseed. Original random lifetime drain can differ across repeated cycles. Null spell, no natural caster/target claim.')
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],visible,cleanup);assert ok
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--binary',type=Path,required=True);p.add_argument('--data-root',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.binary.resolve(),a.data_root.resolve(),a.output.resolve())
