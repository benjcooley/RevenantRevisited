#!/usr/bin/env python3
"""Repeat full native authored parser/init/state against compiled production.

Runs only explicitly selected audited state0 resource providers. This does not
admit effects or replace geometry/pixel, map, character or device gates.
"""
import argparse
import hashlib
import json
from pathlib import Path
import zipfile

from speed_controller_preflight import NativeSpeed, read_asset, ROOT
from speed_state_compare import compare

PROFILES = (
    ('Might','Imagery/Magic/might.I3D','ac23bbe4b0ba283af5758b571e9106c691a21d9e1616a67aff3a9da76c10101a'),
    ('Immortalmight','Imagery/Magic/imight.I3D','73cd9faaf6230e903b688473dd8f96cffd0a4351055a2a64c3c9d199c9a8972e'),
    ('Fmastery','Imagery/Magic/fmaster.I3D','9d94bf16e45a88366946811775747063b905be3406d44b0a9a47e26e5b51878d'),
    ('WaterFlft','Imagery/Misc/WFALL.I3D','306a42c1c57a89a568ee67173ffa0de8f1cc6c51c450a10b36e13074e5abfd5a'),
    ('WaterFrt','Imagery/Misc/WFALL2.I3D','e88b8439f16438a23c67642f0f30e38ad8e1a847226e00b167e52c6ca450e9d8'),
    ('WaterClft','Imagery/Misc/WCAP.I3D','583416fb590936cc4f1e9b4d0e04ad13c177ea2d1d4ad5f57d5e73353507ca2e'),
    ('WaterCrt','Imagery/Misc/WCAP2.I3D','e5893cb78c8baa7a8dfffdda8509755bce9a0757e711154d3ad9cbda5e87d87e'),
    ('combatflash','Imagery/Misc/impact.I3D','27f76140ed0c1a8a4bb2dd404c068422feb7d49345ec98e1c9c1b3dd159c7725'),
    ('goldeffect','Imagery/Misc/goldp.I3D','0236d13a7715160d4079cabb7fc6bf9907bb591b0cae3b8e13754ac32dd07d04'),
    ('speed','Imagery/Magic/speed.I3D','65c45c03bcf3cd5cfe549232f88fb18550a30720c5b9500d0cfd5ef8154fce42'),
)


def sha(data):
    return hashlib.sha256(data).hexdigest()


def pins():
    paths = [ROOT/p for p in ('src/partsysdefinition.cpp','src/partsysdefinition.h',
        'src/authoredpartsys.cpp','src/authoredpartsys.h',
        'tools/retail_runtime/speed_controller_preflight.py',
        'tools/retail_runtime/speed_state_compare.py','tools/retail_runtime/admitted_full_parser_probe.py')]
    return {str(p.relative_to(ROOT)):sha(p.read_bytes())for p in paths}


def run(args):
    args.output.mkdir(parents=True,exist_ok=True)
    before=pins();cases=[]
    with zipfile.ZipFile(args.archive)as z:
        for name,member,asset_sha in PROFILES:
            if name not in args.profiles:continue
            data=z.read(member)
            if sha(data)!=asset_sha:raise ValueError(name+' asset revision changed')
            asset=read_asset(data);asset['name']=name
            # Impact's later states remain explicitly outside this provider.
            asset['tags']=[t for t in asset['tags']if t['state']==0]
            output=args.output/(name+'-quality'+str(args.quality));output.mkdir(exist_ok=True)
            (output/'asset-triage.json').write_text(json.dumps([asset],indent=2)+'\n')
            hashes=[]
            for repeat in range(args.repeat):
                fixture=NativeSpeed(args.executable,asset)
                initialization=fixture.initialize(args.quality)
                trace=fixture.state_trace(90)
                hashes.append(sha(json.dumps({'rows':trace['rows'],'poses':trace['poses']},separators=(',',':')).encode()))
                if repeat==0:
                    (output/'native-initialization.json').write_text(json.dumps(initialization,indent=2)+'\n')
                    (output/'retail-state-trace.json').write_text(json.dumps(trace,indent=2)+'\n')
            if len(set(hashes))!=1:raise AssertionError(name+' native fresh replay differs')
            (output/'native-repeat.json').write_text(json.dumps(dict(status='pass',native_replays=args.repeat,
                row_pose_sha256=hashes),indent=2)+'\n')
            comparison=compare(output,output/'comparison')
            cases.append(dict(name=name,quality=args.quality,status=comparison['status'],
                comparisons=comparison['comparisons'],error_count=comparison['error_count'],
                native_repeat_sha256=hashes,manifest=str(output/'comparison/manifest.json')))
            print(name,comparison['status'],comparison['comparisons'],comparison['error_count'],flush=True)
    after=pins()
    if before!=after:raise AssertionError('Production source or probe changed during validation')
    report=dict(status='pass'if all(c['status']=='pass'for c in cases)else'fail',cases=cases,
        comparisons=sum(c['comparisons']for c in cases),source_probe_sha256=after,
        original_executable_sha256=sha(args.executable.read_bytes()),
        no_synthetic_controller_packets=True,renderer_pixels_compared=False,
        new_rendered_ab_cases=0,accepted=False,
        scope='Whole native parser/controller Initialize/Pulse/live sample; exact native-decoded '
            'authored emitter matrices shared as inputs to actual compiled production State. '
            'Real object construction/punctuation/owner-device geometry, independent port pose decoder '
            'and map/Metal/natural character/spell behavior are separate gates.')
    (args.output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
    return report


if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path)
    p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi')
    p.add_argument('--profiles',nargs='+',choices=[x[0]for x in PROFILES],required=True)
    p.add_argument('--quality',type=int,choices=(0,1,2,3),default=0)
    p.add_argument('--repeat',type=int,default=2);p.add_argument('--output',type=Path,required=True)
    args=p.parse_args()
    if args.repeat<2:p.error('At least two fresh native replays required')
    report=run(args)
    if report['status']!='pass':raise SystemExit(1)
