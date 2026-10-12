"""Actual translated Pulp public-Set gallery, persistent chunks and owned cleanup."""
import argparse,json,os,subprocess,time,re
from pathlib import Path
from PIL import Image
from character_native_preflight import ROOT,sha

def run(binary,data,out):
 out.mkdir(parents=True,exist_ok=False);save=out/'save';save.mkdir();(save/'Revenant.ini').write_bytes((data/'revenant.ini').read_bytes());pinned=sha(binary)
 command=[str(binary),'--headless','--test=vfx-review','--vfx-review-effects=0x152dafdd','--vfx-review-first','--vfx-review-speed=0.1','--resolution=640x480','--vfx-no-ui','--filmstrip=20,1','--snapstep=0.0416666666666667','--snapwarmup=1','--snapseed=1','--snaprect=0,0,640,480','--snapprefix='+str(out/'frame-')];start=time.monotonic()
 with(out/'run.log').open('wb')as log:q=subprocess.run(command,cwd=ROOT,env=dict(os.environ,REVENANT_DATA_PATH=str(data),REVENANT_SAVE_PATH=str(save),REVENANT_ASSETS_PATH=str(ROOT/'assets')),stdout=log,stderr=log,timeout=120)
 frames=sorted(out.glob('frame-[0-9][0-9][0-9]-*.png'));log=(out/'run.log').read_text();images=[Image.open(p).convert('RGB')for p in frames];visible=sum(im.getbbox()is not None for im in images)
 spawns=re.findall(r'\[pulp-rng\] seeded=(\d+)',log);cleanup=re.findall(r'\[pulp-cleanup\] tick=(\d+) flags=([0-9a-f]+) health=(\d+) decals=(\d+)',log);decals=re.findall(r'\[pulp-decal\][^\n]+',log)
 ok=q.returncode==0 and len(frames)==20 and visible==20 and spawns==['0'] and len(cleanup)==1 and int(cleanup[0][0])>=456 and cleanup[0][1:3]==('406c028','102') and len(decals)==10 and all('result=-1 intrinsic_reseed=1 clock_ms=0'in x for x in decals) and 'implementation=TPulp_CONFIGURED_ACTOR'in log and '[shutdown] complete'in log and sha(binary)==pinned
 report=dict(status='pass'if ok else'fail',accepted=False,visual_pass=False,type_id='0x152dafdd',binary_sha256=pinned,command=command,exit_code=q.returncode,elapsed_seconds=time.monotonic()-start,frames=[dict(path=str(p),sha256=sha(p),bounds=im.getbbox())for p,im in zip(frames,images)],visible_frames=visible,spawns=spawns,cleanup=cleanup,intrinsic_reseed_count=len(decals),log_sha256=sha(out/'run.log'),scope='Actual translated common-world review station; genuine private Acolyte animator/public-Set source; persistent settled bodies never retriggered. Factory does not reseed. Ten original failed-decal MakeIndex intrinsic reseeds use declared clock0. Genuine terrain queried and genuine NewObject called/required to fail; actor invisibility restored on owned teardown. Natural mapped blood tiles/live-clock/actor death caller remain open.')
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],visible,cleanup);assert ok
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--binary',type=Path,required=True);p.add_argument('--data-root',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.binary.resolve(),a.data_root.resolve(),a.output.resolve())
