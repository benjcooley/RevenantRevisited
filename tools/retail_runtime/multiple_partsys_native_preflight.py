#!/usr/bin/env python3
"""Opt-in whole original cfire/sgeyser multiple-tag initialization audit."""
import argparse,json,struct,zipfile
from pathlib import Path
from generic_mesh_visual_reference import NativeGenericMeshScene
from scroll_water_visual_reference import NativeScrollScene
from essencedrain_native_preflight import NativeEssence,ROOT
from static_particles_probe import punctuation
from speed_profile_contract import sha
from preset_file_access import install as install_file_access
from unicorn.x86_const import UC_X86_REG_EIP,UC_X86_REG_ESP,UC_X86_REG_ESI
class NativeMultiple(NativeEssence):
 def __init__(self,exe,data,name):
  NativeScrollScene.__init__(self,exe,data,name,1280,960,particle_required=None,native_binding=(0x40dc00,0x5e8508,0x5a370c),eager_pose=False);v=self.vm;self.tick=0;self.raster_enabled=True;self.object=[0];self.file_requests=[]
  count,original=v.handlers['CreateFileA']
  def opened(args):
   result=original(args);self.file_requests.append(dict(path=v.string(args[0]),disposition=args[4],result=hex(result)));return result
  v.handlers['CreateFileA']=(count,opened)
  v.put_u32(self.owner,0x5a85ac);v.write(self.owner+0x7c,struct.pack('<h',-1));v.put_u32(self.owner+0x4c,self.imagery);v.put_u32(self.owner+0x54,self.imagery);v.put_u32(self.owner+0x58,self.animator);v.write(self.owner+0x5c,struct.pack('<hh',0,1));vt=v.u32(self.imagery);v.put_u32(0x5d79e4,0)
  for offset,value in((0x8c,struct.unpack_from('<H',data,68)[0]),(0x90,self.asset['state0_frames'])):
   p=v.allocate(16);v.write_code(p,b'\xb8'+struct.pack('<I',value)+b'\xc2\x04\x00');v.put_u32(vt+offset,p)
 def initialize(self,startup=False,data_root=None):
  v=self.vm
  if startup:
   v.call(0x41e2de,stop_address=0x41e535,instruction_limit=20000000)
   # Original WinMain creates its unnamed file-manager mutex here.
   v.uc.reg_write(UC_X86_REG_ESI,v.api_address('kernel32.dll','CreateMutexA'));v.call(0x4865c5,stop_address=0x4865d7)
  if data_root is not None:self.filesystem,self.mounted_files=install_file_access(v,data_root)
  punctuation(self);v.call(0x401610);v.call(0x40dd60,this=self.animator,instruction_limit=5000000);self.animobjs=[v.call(0x40eef0,(i,),this=self.animator)for i in range(len(self.asset['objects']))];self.controllers=[v.u32(v.u32(self.animator+0x68)+i*4)for i in range(v.u32(self.animator+0x58))];return[dict(vtable=hex(v.u32(c)),frame=v.u32(c+8),objects=v.u32(c+0x18),capacity=v.u32(c+0xf8))for c in self.controllers]
 def setup_resources(self):
  v=self.vm;a=bytes(v.uc.mem_read(self.animator+0x30,40));NativeGenericMeshScene.setup_resources(self);v.uc.mem_write(self.animator+0x30,a)
def run(output,startup=False,data_root=None):
 output.mkdir(parents=True,exist_ok=False);rows=[]
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:
  for name,path in [('cfire','imagery/misc/cfire.i3d'),('sgeyser','imagery/cave/cavsgeyser.i3d')]:
   data=z.read(next(n for n in z.namelist()if n.lower()==path));f=None
   try:
    f=NativeMultiple(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data,name);init=f.initialize(startup,data_root);rows.append(dict(name=name,status='initialized',asset_sha256=sha(data),controllers=init,constructor=f.constructor,file_requests=f.file_requests))
   except Exception as e:
    r=dict(name=name,status='failed',error=str(e),asset_sha256=sha(data))
    if f:
     v=f.vm;r.update(eip=hex(v.uc.reg_read(UC_X86_REG_EIP)),stack=[hex(v.u32(v.uc.reg_read(UC_X86_REG_ESP)+i*4))for i in range(10)])
    rows.append(r)
   print(rows[-1],flush=True)
 report=dict(status='bounded_feasibility',accepted=False,startup=startup,data_root=str(data_root) if data_root else None,cases=rows,source_sha256=sha((ROOT/'tools/retail_runtime/speed_controller_preflight.py').read_bytes()),scope='Explicit opt-in skips only fixture single-prototype precondition. Full original default constructor/Initialize and all immutable tags/resources retained, no synthesized controllers. Optional actual41e2de..41e535 object-service startup is separately declared. Whole rendering/source/acceptance remain separate.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);p.add_argument('--startup',action='store_true');p.add_argument('--data-root',type=Path);a=p.parse_args();run(a.output,a.startup,a.data_root)
