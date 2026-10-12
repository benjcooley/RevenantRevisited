#!/usr/bin/env python3
"""Exact Aura with genuine actor and named retail Aura TSpell input."""
import argparse,json,struct
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from character_native_preflight import AuthenticCharacters,ROOT,sha
from blood_actor_preflight import BOOT

def run(output,nullspell=False):
 output.mkdir(parents=True,exist_ok=False);exe=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe';f=AuthenticCharacters(exe,BOOT);f.select_screen_prefix();v=f.vm;call=f.world.call;actor=f.new_character('Acolyte',(0,0,16));before=f.dump(actor);assert call(0x46e8b0,this=actor,instruction_limit=100000000)==1;assert call(0x53ead0,this=0x667c38,instruction_limit=100000000)==1;matches=[]
 for i in range(v.u32(0x667c3c)):
  sd=v.u32(v.u32(0x667c4c)+4*i)
  for j in range(v.u32(sd)):
   variant=v.u32(v.u32(sd+0x10)+4*j)
   if v.string(variant+4)=='Aura':matches.append((sd,variant))
 assert len(matches)==1;targets=v.allocate(4);v.put_u32(targets,actor);source=v.allocate(12);v.write(source,struct.pack('<3i',0,0,16));spell=call(0x542200,(actor,targets,1,source,*matches[0],0),this=0x670630);assert spell;owner=f.new_object('Aura',(0,0,16),25,0x5a96fc);
 if not nullspell:v.put_u32(owner+0xd8,spell)
 error=None;created=None
 try:created=call(0x46e8b0,this=owner,instruction_limit=100000000)
 except Exception as e:error=str(e)
 a=v.u32(owner+0x58);vt=v.u32(a)if a else 0;methods=[hex(v.u32(vt+i*4))for i in range(6,18)]if a else[];md=Cs(CS_ARCH_X86,CS_MODE_32);asm={address:[dict(address=hex(x.address),mnemonic=x.mnemonic,operands=x.op_str)for x in md.disasm(bytes(v.uc.mem_read(int(address,16),256)),int(address,16))]for address in methods if address in(methods[0],methods[5],methods[7])};r=dict(status=('original_null_spell_killed'if nullspell and v.u32(owner+8)&0x1000 else'genuine_named_spell_binding')if not error else'fixture_gate',accepted=False,visual_pass=False,actor_before=before,actor_after=f.dump(actor),spell=hex(spell),variant_name='Aura',variant_definition=hex(matches[0][1]),spell_count=v.u32(0x667c3c),owner=hex(owner),null_spell=nullspell,owner_flags=hex(v.u32(owner+8)),create_animator=created,animator=hex(a),animator_vtable=hex(vt),animator_methods=methods,disassembly=asm,error=error,executable_sha256=sha(exe),boot_image_sha256=sha(BOOT),scope='Actualloaded74spell table/Aura variant/TSpell constructor, realAcolyte/class/complex/animator andexactAura class with inline publicSetSpell binding. No actor/bone/stat/damage/spell data fabricated; owner/controller/render/Metal/naturalposter separate.');(output/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');print(r['status'],hex(vt),methods,error);return r
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);p.add_argument('--null-spell',action='store_true');a=p.parse_args();run(a.output,a.null_spell)
