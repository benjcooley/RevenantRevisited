#!/usr/bin/env python3
"""Genuine actor/public setter feasibility for shipped Burn and Aura; no bones substituted."""
import argparse,json,struct
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from character_native_preflight import AuthenticCharacters,ROOT,sha
from blood_actor_preflight import BOOT

def run(output,name):
 output.mkdir(parents=True,exist_ok=False);exe=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe';f=AuthenticCharacters(exe,BOOT);f.select_screen_prefix();v=f.vm;call=f.world.call;actor=f.new_character('Acolyte',(0,0,16));before=f.dump(actor);actor_create=call(0x46e8b0,this=actor,instruction_limit=100000000);actor_anim=v.u32(actor+0x58)
 records=[]
 for index in range(v.u32(0x659d30)):
  r=v.u32(v.u32(0x659d40)+4*index)
  if v.uc.mem_read(r+0x22,1)[0]==25 and v.string(v.u32(r)).lower()==name.lower():records.append(r)
 assert len(records)==1;r=records[0];typ=struct.unpack('<H',v.uc.mem_read(r+0x20,2))[0];d=v.allocate(0x34);v.write(d,struct.pack('<HH',25,typ));v.write(d+12,struct.pack('<3i',0,0,16));o=call(0x474bb0,(d,0xffffffff,1),this=v.u32(0x65a148+4*25),instruction_limit=100000000);assert o;vt=v.u32(o);methods={hex(off):hex(v.u32(vt+off))for off in range(0x1f0,0x218,4)} ;setter_result=None;effect_create=None;effect_anim=0;effect_methods=[]
 if name=='Burn':
  setter_result=call(0x4e2fa0,(actor,),this=o);assert v.u32(o+0x184)==actor;effect_create=call(0x46e8b0,this=o,instruction_limit=100000000);effect_anim=v.u32(o+0x58);effect_methods=[hex(v.u32(v.u32(effect_anim)+i*4))for i in range(6,18)]
 md=Cs(CS_ARCH_X86,CS_MODE_32);asm={addr:[dict(address=hex(x.address),mnemonic=x.mnemonic,operands=x.op_str)for x in md.disasm(bytes(v.uc.mem_read(int(addr,16),256)),int(addr,16))]for addr in dict.fromkeys(methods.values())};report=dict(status='public_setter_audit',accepted=False,visual_pass=False,name=name,actor_before=before,actor_after=f.dump(actor),actor_create=actor_create,actor_animator=hex(actor_anim),actor_animator_vtable=hex(v.u32(actor_anim))if actor_anim else None,owner=hex(o),owner_vtable=hex(vt),owner_methods=methods,setter=hex(0x4e2fa0)if name=='Burn'else None,setter_result=setter_result,effect_create=effect_create,effect_animator=hex(effect_anim),effect_animator_methods=effect_methods,effect_animator_tail=bytes(v.uc.mem_read(effect_anim+0x148,16)).hex()if effect_anim else None,disassembly=asm,owner_tail=bytes(v.uc.mem_read(o+0x180,64)).hex(),executable_sha256=sha(exe),boot_image_sha256=sha(BOOT),inherited_boundaries=f.world.boundary_calls,scope='Genuine complete Acolyte/object/imagery/animator construction and exact effectclassconstructor. Setter signature must be inspected before invocation. No bone/stat/pose/damage/map/factory substitutions. Controller/render/source/appearance remain open.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(name,hex(vt),methods,hex(actor_anim));return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);p.add_argument('--name',choices=['Burn','Aura'],required=True);a=p.parse_args();run(a.output,a.name)
