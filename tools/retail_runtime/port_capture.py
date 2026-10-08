#!/usr/bin/env python3
"""Capture the real Revisited renderer from an explicit VFX scenario.

This only runs an owned Mac process. It has no DOSBox/guest/editor-control path.
The original thin-emulator fixture supplies the independent reference images.
"""
import argparse
import csv
import hashlib
import json
import os
from pathlib import Path
import subprocess
import time

ROOT=Path(__file__).resolve().parents[2]


def sha(path):return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def capture(scenario,output):
    binary=Path(scenario.get('binary',ROOT.parent/'build/Revenant')).resolve()
    data=Path(scenario.get('data_root',ROOT/'data')).resolve()
    count=int(scenario['frames']);fps=int(scenario.get('fps',24))
    if count<1 or fps<1:raise ValueError('Positive frame count and FPS required')
    output=output.resolve();output.mkdir(parents=True,exist_ok=False)
    origin=scenario.get('origin',[0,0,0]);camera=scenario.get('camera',[320,170])
    rectangle=scenario.get('rectangle',[272,122,96,96])
    command=[str(binary),'--headless','--test=vfx','--vfx='+scenario['effect'],'--vfx-no-ui',
        '--vfx-bg='+scenario.get('background','black'),
        '--vfx-camera='+','.join(map(str,camera)),'--vfx-origin='+','.join(map(str,origin)),
        f'--filmstrip={count},{1/fps:.16f}',f'--snapstep={1/fps:.16f}',
        '--snapseed='+str(scenario.get('seed',1)),'--snapwarmup='+str(scenario.get('warmup',1)),
        '--snaprect='+','.join(map(str,rectangle)),'--snapprefix='+str(output/'frame-')]
    env=os.environ.copy();env['REVENANT_DATA_PATH']=str(data)
    assets=Path(scenario.get('assets_root',env.get('REVENANT_ASSETS_PATH',ROOT/'assets'))).resolve()
    env['REVENANT_ASSETS_PATH']=str(assets)
    binary_sha=sha(binary)
    # Engine-owned definitions follow the executable's assets lookup, independent
    # of the licensed retail data root (which supplies imagery and maps).
    definitions=assets/'effects.def'
    definitions_sha=sha(definitions);(output/'effects.def').write_bytes(definitions.read_bytes())
    log=output/'run.log';start=time.monotonic();complete=False;terminated=False
    with log.open('wb') as handle:
        process=subprocess.Popen(command,cwd=binary.parent,env=env,stdout=handle,stderr=handle)
        try:
            deadline=start+float(scenario.get('timeout_seconds',60))
            while time.monotonic()<deadline:
                frames=sorted(output.glob('frame-[0-9][0-9][0-9]-*.png'))
                if len(frames)==count and (output/'frame-filmstrip.png').is_file():
                    complete=True;break
                if process.poll() is not None:break
                time.sleep(.1)
        finally:
            try:process.wait(timeout=2)
            except subprocess.TimeoutExpired:
                terminated=True;process.terminate()
                try:process.wait(timeout=2)
                except subprocess.TimeoutExpired:process.kill();process.wait(timeout=2)
    frames=sorted(output.glob('frame-[0-9][0-9][0-9]-*.png'))
    report=dict(status='pass' if complete and process.returncode==0 and not terminated else 'fail',scenario=scenario,command=command,
        binary_sha256=binary_sha,definitions_sha256=definitions_sha,
        elapsed_seconds=time.monotonic()-start,owned_pid=process.pid,exit_code=process.returncode,
        forced_owned_shutdown=terminated,frames=[dict(path=str(p),sha256=sha(p)) for p in frames],
        log_sha256=sha(log),dosbox_used=False,
        scope='Actual Revisited GPU output for the explicit scenario; comparison/acceptance is separate.')
    if terminated:
        report['error']='Owned capture process required forced shutdown; completed images do not establish a clean exit'
    elif process.returncode!=0:
        report['error']=f'Owned capture process exited with code {process.returncode}; completed images do not establish a clean exit'
    elif not complete:
        report['error']='Capture did not complete the requested frames and filmstrip'
    if sha(binary)!=binary_sha or sha(definitions)!=definitions_sha:
        report['status']='fail';report['error']='Binary or runtime definitions changed during capture'
    if complete:
        with (output/'frame-timing.csv').open() as timing_file:
            timing=list(csv.DictReader(timing_file))
        if len(timing)!=count or any(abs(float(row['relative_seconds'])-i/fps)>1e-6 for i,row in enumerate(timing)):
            report['status']='fail';report['error']='Simulation cadence differs from the explicit scenario'
        report['timing']=timing
        if "--vfx='"+scenario['effect']+"' matched" not in log.read_text(errors='replace'):
            report['status']='fail';report['error']='Requested effect did not match the actual catalogue'
    (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
    if report['status']!='pass':raise RuntimeError('Port capture failed; see '+str(output/'manifest.json'))
    return report


if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--scenario',type=Path,required=True)
    parser.add_argument('--output',type=Path,required=True);args=parser.parse_args()
    report=capture(json.loads(args.scenario.read_text()),args.output)
    print(json.dumps({k:v for k,v in report.items() if k not in ('frames','timing')},indent=2))
