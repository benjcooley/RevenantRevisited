#!/usr/bin/env python3
"""Owned actual Metal map capture for the three independently referenced buffs."""
from pathlib import Path
import os,subprocess,json,hashlib,shutil,time
import argparse
p=argparse.ArgumentParser(description='Capture actual map buff owners with movement and deletion; use buff_visual_review.py to validate retained outputs.')
p.add_argument('--binary',type=Path,required=True);p.add_argument('--data',type=Path,required=True);p.add_argument('--output',type=Path,required=True)
a=p.parse_args();R=Path(__file__).resolve().parents[2];O=a.output.resolve();D=a.data.resolve()
for name,id in [('speed','ad92bd36'),('quicksilver','ad92bd35'),('Fmastery','b0e024df')]:
 out=O/name;out.mkdir(parents=True,exist_ok=False);save=out/'save';save.mkdir();shutil.copy2(D/'revenant.ini',save/'Revenant.ini')
 commands=out/'commands.txt';commands.write_text(f'3 | - | addat 10000 10000 {name}\n3 | {name} | @expect 0x{id} 10000 10000 16 unused 0\n35 | {name} | move 16 -8 4\n36 | {name} | @expect 0x{id} 10016 9992 20 unused 0\n45 | {name} | delete\n46 | {name} | @absent\n')
 binary=a.binary.resolve();command=[str(binary),'--test=sector','--headless','--partsys-quality=0','--scene-module=FontRuntimeLab','--scene-camera=110,10000,10000,16','--scene-ambient=32,255,255,255','--scene-command-file='+str(commands),'--filmstrip=58,0.0416666666666667','--snapstep=0.0416666666666667','--snapseed=1','--snapwarmup=1','--snaprect=0,0,640,340','--snapprefix='+str(out/'frame-')]
 if name=='Fmastery':command+=['--partsys-incoming-blend=16']
 with (out/'run.log').open('wb') as log:p=subprocess.run(command,cwd=R,env=dict(os.environ,REVENANT_DATA_PATH=str(D),REVENANT_SAVE_PATH=str(save),REVENANT_ASSETS_PATH=str(R/'assets')),stdout=log,stderr=log,timeout=60)
 (out/'capture.json').write_text(json.dumps(dict(command=command,exit=p.returncode,binary_sha256=hashlib.sha256(binary.read_bytes()).hexdigest()),indent=2))
 print(name,p.returncode,flush=True)
