#!/usr/bin/env python3
"""Genuine CHARACTER Blood EffectBurst caller feasibility; no actor/map replacements."""
import argparse,json
from pathlib import Path
from character_native_preflight import AuthenticCharacters,ROOT,SOURCE,sha
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ECX,UC_X86_REG_ESP
BOOT=Path('/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra/character-authentic/hud-boot-09c83243d43516b6.image')
def run(output):
 output.mkdir(parents=True,exist_ok=False);exe=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe';f=AuthenticCharacters(exe,BOOT);f.select_screen_prefix();v=f.vm;actor=f.new_character('Acolyte',SOURCE);before=f.dump(actor);entries=[]
 def observe(uc,a,n,u):entries.append(dict(address=hex(a),receiver=hex(uc.reg_read(UC_X86_REG_ECX)),stack=list(__import__('struct').unpack('<8I',uc.mem_read(uc.reg_read(UC_X86_REG_ESP),32)))))
 for a in(0x4c85d0,0x450e40,0x452690,0x474bb0,0x4f9550):v.uc.hook_add(UC_HOOK_CODE,observe,begin=a,end=a)
 p=v.allocate(6);v.write(p,b'Blood\0');error=None;result=None
 try:result=f.world.call(0x4c85d0,(p,40),this=actor,instruction_limit=100000000)
 except Exception as e:error=dict(message=str(e),fault=f.world.last_fault)
 r=dict(status='completed_caller_prefix_with_map_insertion_open'if error is None else'declared_caller_gate',accepted=False,visual_pass=False,type_id='0xddc4042e',executable_sha256=sha(exe),boot_image_sha256=sha(BOOT),actor_before=before,actor_after=f.dump(actor),result=result,error=error,entries=entries,inherited_boundaries=f.world.boundary_calls,added_actor_boundaries=[],scope='Genuine original NewObject/Acolyte class/character constructor/stats, originalShowScreen prefix and actualEffectBurst Blood40 caller. No fake character/bone/stat/damage, no map or factory success substitution. Full animator/render and naturalmap/device separate.');(output/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');print(r['status'],error,len(entries));return r
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.output)
