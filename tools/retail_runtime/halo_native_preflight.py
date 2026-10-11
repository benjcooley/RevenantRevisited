#!/usr/bin/env python3
"""Actual shipped Halo class/owner/animator and public-parameter feasibility."""
import argparse,json,struct
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from character_native_preflight import AuthenticCharacters,ROOT,sha
from blood_actor_preflight import BOOT

def run(output):
 output.mkdir(parents=True,exist_ok=False);exe=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe';f=AuthenticCharacters(exe,BOOT);f.select_screen_prefix();v=f.vm;call=f.world.call
 found=[]
 for index in range(v.u32(0x659d30)):
  r=v.u32(v.u32(0x659d40)+4*index)
  if v.uc.mem_read(r+0x22,1)[0]==25 and v.string(v.u32(r)).lower()=='halo':found.append(r)
 assert len(found)==1;r=found[0];typ=struct.unpack('<H',v.uc.mem_read(r+0x20,2))[0];d=v.allocate(0x34);v.write(d,struct.pack('<HH',25,typ));v.write(d+12,struct.pack('<3i',0,0,16));o=call(0x474bb0,(d,0xffffffff,1),this=v.u32(0x65a148+4*25),instruction_limit=100000000);assert o;vt=v.u32(o);owner_methods=[hex(v.u32(vt+i*4))for i in range(0x1f0//4,0x218//4)];creator=call(0x46e8b0,this=o,instruction_limit=100000000);a=v.u32(o+0x58);av=v.u32(a)if a else 0
 md=Cs(CS_ARCH_X86,CS_MODE_32);addresses=list(dict.fromkeys([v.u32(vt+x)for x in(0x200,0x204,0x208)]+([v.u32(av+i*4)for i in(6,9,10,11)]if a else[])));asm={hex(addr):[dict(address=hex(i.address),mnemonic=i.mnemonic,operands=i.op_str)for i in md.disasm(bytes(v.uc.mem_read(addr,512)),addr)]for addr in addresses}
 im=v.u32(a+8)if a else 0;entry=v.u32(im+4)if im else 0;report=dict(status='binding_preflight',accepted=False,visual_pass=False,type_name='Halo',owner=hex(o),owner_vtable=hex(vt),owner_extra_methods=owner_methods,owner_tail=bytes(v.uc.mem_read(o+0xfc,80)).hex(),create_animator=creator,animator=hex(a),animator_vtable=hex(av),animator_methods=[hex(v.u32(av+i*4))for i in range(6,18)]if a else[],asset=v.string(entry)if entry else None,disassembly=asm,executable_sha256=sha(exe),boot_image_sha256=sha(BOOT),inherited_boundaries=f.world.boundary_calls,scope='Whole genuine startup/classNewObject and CreateAnimator; no fabricated parameter or caller. Public interface values/callers and render remain audit gates.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:report[k]for k in('owner_vtable','owner_extra_methods','animator_vtable','animator_methods','asset')}));return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.output)
