#!/usr/bin/env python3
"""Reproduce the original shipped Regeneration generic-owner control path."""
from pathlib import Path
import argparse,json,struct,hashlib
from character_native_preflight import AuthenticCharacters,ROOT,SOURCE,TARGET
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ECX
parser=argparse.ArgumentParser(description='Genuine shipped Regeneration Strike creator, Follow binding and full owner lifetime; no renderer substitution.');parser.add_argument('--output',type=Path,required=True);parser.add_argument('--ticks',type=int,default=2895);parser.add_argument('--boot-image',type=Path,required=True);parser.add_argument('--origin',type=int,nargs=3,default=list(SOURCE));args=parser.parse_args();SOURCE=tuple(args.origin);TARGET=(SOURCE[0]+96,SOURCE[1],SOURCE[2]);OUT=args.output;OUT.mkdir(parents=True,exist_ok=False)
f=AuthenticCharacters(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',args.boot_image);f.select_screen_prefix();v=f.vm;call=f.world.call
a=f.new_character('Acolyte',SOURCE);b=f.new_character('Acolyte',TARGET);call(0x4d3b90,(b,3),this=a);assert call(0x53ead0,this=0x667c38,instruction_limit=100000000)==1
matches=[];allvariants=[]
for i in range(v.u32(0x667c3c)):
 sd=v.u32(v.u32(0x667c4c)+4*i)
 for j in range(v.u32(sd)):
  va=v.u32(v.u32(sd+0x10)+4*j);cd=v.u32(va+0x70);row=dict(name=v.string(va+4),effect=v.string(va+0x2a),control=v.string(cd)if cd else None,control_hex=bytes(v.uc.mem_read(cd,0xc4)).hex()if cd else None)
  allvariants.append(row)
  if row['name']=='Regeneration':matches.append((sd,va))
assert len(matches)==1;sd,va=matches[0];targets=v.allocate(4);v.put_u32(targets,b);source=v.allocate(12);v.write(source,struct.pack('<3i',0,0,0));spell=call(0x542290,(a,targets,1,source,sd,va,0),this=0x670628);assert v.u32(spell)==0x5b9bdc
camera=v.allocate(12);v.write(camera,struct.pack('<3i',*SOURCE));call(0x450d20,(camera,),this=0x6668d8);call(0x459220,this=0x6668d8,instruction_limit=100000000)
visited=set();v.uc.hook_add(UC_HOOK_CODE,lambda uc,addr,size,user:visited.add(addr),begin=0x4de400,end=0x4df900);v.uc.hook_add(UC_HOOK_CODE,lambda uc,addr,size,user:visited.add(addr),begin=0x540d70,end=0x541d00)
rows=[];owners=[]
def snapshot(o):return dict(address=hex(o),flags=hex(v.u32(o+8)),position=list(struct.unpack('<3i',v.uc.mem_read(o+0x10,12))),fields={hex(i):v.u32(o+i)for i in range(0xd8,0x184,4)})
for tick in range(args.ticks):
 call(0x540d70,this=spell);done=call(0x540eb0,this=spell,instruction_limit=100000000)
 owners=[v.u32(v.u32(spell+0x14c)+4*i)for i in range(v.u32(spell+0x13c))]
 if tick==8:
  point=v.allocate(12);v.write(point,struct.pack('<3i',SOURCE[0]+115,SOURCE[1]+5,SOURCE[2]+8));call(v.u32(v.u32(a)+8),(point,0xffffffff,0),this=a,instruction_limit=100000000)
 before=[snapshot(o)for o in owners]
 for o in owners:call(0x470cc0,this=o);call(0x4de800,this=o,instruction_limit=100000000)
 rows.append(dict(tick=tick,done=done,target=f.dump(b),before=before,after=[snapshot(o)for o in owners]))
report=dict(status='original_owner_reference_complete',declared_origin=list(SOURCE),accepted=False,original_executable_sha256=hashlib.sha256((ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe').read_bytes()).hexdigest(),boot_sha256=hashlib.sha256(args.boot_image.read_bytes()).hexdigest(),variants=allvariants,rows=rows,visited=[hex(x)for x in sorted(visited)],scope='Actual74-block/99-variant SpellList, original Regeneration Strike creator542290, Pulse540d70/Timer540eb0, real empty resident sectors, publicFollow4def90 and originalowner4de800. Real invoker SetPos at tick8, unchanged target. Map/character scheduling is explicit; ownerNextFrame/Pulse once per iteration, no renderer/appearance/controller or naturalcast claim. Literal2880duration/blink24/light160/5fadein unchanged. Source behavior not yet compared.');(OUT/'regeneration-public-full.json').write_text(json.dumps(report,indent=2));print('PASS',len(rows), 'active',sum(bool(r['after'])for r in rows))
