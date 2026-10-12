#!/usr/bin/env python3
"""Literal branch controls on a genuinely created Regeneration owner.

Overrides are declared field/configuration diagnostics, not shipped variants.
The ordinary public creator/lifetime witness remains a separate file.
"""
import argparse,json,runpy,sys
from pathlib import Path
p=argparse.ArgumentParser();p.add_argument('--boot-image',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args()
sys.argv=[str(Path(__file__).with_name('generic_effect_owner_probe.py')),'--boot-image',str(a.boot_image),'--output',str(a.output),'--ticks','6']
f=runpy.run_path(sys.argv[0],run_name='__main__');v=f['v'];call=f['call'];owner=f['owners'][0];spell=f['spell'];snap=f['snapshot'];rows=[]
cases=[('delay3',{0x138:3}),('delay1',{0x138:1}),('timed_end',{0x12c:1}),('follow_end',{0xe8:1}),('fadeout2',{0x140:2,0x148:160}),('lights_off',{}),('lights_on',{})]
for name,changes in cases:
 call(0x4de7d0,(spell,),this=owner)
 call(0x472e90,((v.u32(owner+8)&~0x9080)|0x4000,),this=owner)
 baseline={0xe8:100,0x12c:100,0x130:1,0x134:24,0x138:0}
 for offset,value in {**baseline,**changes}.items():v.put_u32(owner+offset,value)
 v.put_u32(0x5d7a18,0 if name=='lights_off' else 1)
 before=snap(owner);call(0x4de800,this=owner,instruction_limit=100000000);after=snap(owner)
 rows.append(dict(name=name,real_time_light=name!='lights_off',before=before,after=after))
report=dict(status='original_branch_controls_complete',accepted=False,rows=rows,scope='Actual Regeneration creator and owner Pulse. Explicit field overrides exercise delay, timed/follow expiry and fadeout; RealTimeLight is the actual option global. These are diagnostic controls, not additional shipped variants or natural casts.')
(a.output/'branch-controls.json').write_text(json.dumps(report,indent=2));print('PASS',len(rows),'original branch controls')
