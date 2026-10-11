#!/usr/bin/env python3
"""All genuine Acolyte constructor-selected bone matrices for Burn source API audit."""
import argparse,json,struct
from pathlib import Path
from character_native_preflight import AuthenticCharacters,ROOT,sha
from blood_actor_preflight import BOOT

def run(output,interpolate=True):
 output.mkdir(parents=True,exist_ok=False);exe=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe';f=AuthenticCharacters(exe,BOOT);f.select_screen_prefix();v=f.vm;call=f.world.call;v.put_u32(0x5d7a14,int(interpolate));actor=f.new_character('Acolyte',(0,0,16));assert call(0x46e8b0,this=actor,instruction_limit=100000000)==1;a=v.u32(actor+0x58);state=struct.unpack('<H',v.uc.mem_read(actor+0xc,2))[0];frame=struct.unpack('<h',v.uc.mem_read(actor+0x5c,2))[0];call(v.u32(v.u32(a)+11*4),(1,),this=a,instruction_limit=100000000);call(v.u32(v.u32(a)+12*4),this=a,instruction_limit=100000000);root=v.allocate(64);call(0x40e670,(root,),this=a);rows=[];keybuf=v.allocate(36)
 for i in range(22):
  obj=call(0x40eef0,(i,),this=a);assert obj;result=call(0x40a420,(obj,state,frame,root,1),this=v.u32(a+8),instruction_limit=100000000);key=call(0x409430,(i,state,frame,keybuf,keybuf+12,keybuf+24),this=v.u32(a+8));rows.append(dict(key_result=key,key_trs=list(struct.unpack('<9f',v.uc.mem_read(keybuf,36))),index=i,object_index=v.u32(obj+4),result=result,matrix=list(struct.unpack('<16f',v.uc.mem_read(obj+0x58,64)))))
 im=v.u32(a+8);entry=v.u32(im+4);r=dict(status='bounded_genuine_actor_pose',accepted=False,visual_pass=False,interpolate=interpolate,actor=f.dump(actor),state=state,frame=frame,asset=v.string(entry),root_matrix=list(struct.unpack('<16f',v.uc.mem_read(root,64))),rows=rows,executable_sha256=sha(exe),boot_image_sha256=sha(BOOT),scope='Genuine Acolyte/class/complex/animator/wholePreRender and all22 realGetObject→originalCalcObjectMatrix calls at constructor-selected14/0. No injected bone/state/pose/cache/stat values. ActualsourceGetObjectMatrix, controller and rendering separate.');(output/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');print(r['asset'],state,frame,len(rows));return r
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);p.add_argument('--no-interpolation',action='store_true');a=p.parse_args();run(a.output,not a.no_interpolation)
