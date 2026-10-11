#!/usr/bin/env python3
"""Bounded rendered-Might readiness check; never promote pose-only data to pixels."""
import argparse
import hashlib
import json
from pathlib import Path
import zipfile
from unicorn.x86_const import UC_X86_REG_EIP
from effect_probe import PartsysProbe,RETAIL_SHA

ROOT=Path(__file__).resolve().parents[2]
ASSET_SHA='ac23bbe4b0ba283af5758b571e9106c691a21d9e1616a67aff3a9da76c10101a'


def digest(data):return hashlib.sha256(data).hexdigest()


def preflight(executable,archive):
    with zipfile.ZipFile(archive)as assets:
        member=next(n for n in assets.namelist()if n.lower()=='imagery/magic/might.i3d')
        asset=assets.read(member)
    if digest(asset)!=ASSET_SHA:raise ValueError('Might asset revision changed')
    # This probe deliberately tests the current reusable adapter, not a newly
    # invented authored Might controller. Its synthetic state scope stays named.
    fixture=PartsysProbe(executable);fixture.configure(shape=2,face=0,render=True)
    fixture.step(1)
    if fixture.vm.u32(fixture.particles+0xe4)!=1:raise AssertionError('Native synthetic control did not spawn')
    boundary=None
    try:fixture.vm.call(0x4026d0,this=fixture.particles)
    except Exception as error:
        boundary=dict(type=type(error).__name__,message=str(error),
            original_pc=hex(fixture.vm.uc.reg_read(UC_X86_REG_EIP)),
            owner_vtable=hex(fixture.vm.u32(fixture.owner)))
    if boundary is None or boundary['original_pc']!='0x4028e2' or boundary['owner_vtable']!='0x0':
        raise AssertionError('Known missing owner-to-animator boundary changed; reassess readiness')
    return dict(status='blocked_render_adapter',retail_sha256=RETAIL_SHA,
        probe_sha256=digest(Path(__file__).read_bytes()),
        asset_member=member,asset_sha256=ASSET_SHA,type_id='0x5be39ae0',
        current_adapter_control='Synthetic ten-slot PartsysProbe; original Pulse and pose sample execute',
        original_full_render_attempt=boundary,
        blockers=[
            'Reusable PartsysProbe does not install the literal Might curves/controller: authored capacity31 or measured quality1 capacity7 is a separate adapter.',
            'Original Render4026d0 reaches4028e2 owner virtual+24 without a mapped animator; RenderObject40a8f0 and native prototype CalcObjectMatrix do not execute.',
            'Native Render writes prototype+348, requiring at least34c bytes; current synthetic prototype allocation is200 bytes and must not be extended into a full-render claim unchanged.',
            'Compiled SubmitPartSys raw-center/local-model Z bridge and original software projection require a declared common-domain comparison; historical state/pose proofs omit this boundary.'
        ],next_might_step='Allocate the real prototype layout and mapped owner/animator/imagery interfaces; run original RenderObject/CalcObjectMatrix for exact #rect before compiling production SubmitPartSys packets. Keep native curve/controller inputs explicit.',
        alternate_candidate=dict(name='Water',type_id='0x1903abcd',asset='Misc/Water.i3d',
            registration='0x4f32aa',builder='0x66ce98',builder_vtable='0x5ad4e0',
            route='Reuse the literal Waterfall software-fixture structure for the distinct native Water leaf; inspect real Init/Animate/Render and full drop cardinality before credit.',
            reason='Existing port has a small delay/move/respawn drop machine; no authored curve parser or character-spell context in its basic drop path.'),
        source_sha256={name:digest((ROOT/name).read_bytes())for name in
            ('tools/retail_runtime/effect_probe.py','src/3dimage.cpp','src/authoredpartsys.cpp','src/partsysdefinition.cpp')},
        new_rendered_ab_cases=0,production_changes=False,dosbox_used=False,full_acceptance=False)


if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('executable',type=Path)
    parser.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi')
    parser.add_argument('--output',type=Path,required=True);args=parser.parse_args()
    report=preflight(args.executable,args.archive)
    args.output.parent.mkdir(parents=True,exist_ok=True)
    args.output.write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({k:v for k,v in report.items()if k!='source_sha256'},indent=2))
