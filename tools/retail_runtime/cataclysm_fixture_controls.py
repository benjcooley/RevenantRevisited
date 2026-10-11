"""Native allocator and compressed-v2 fixture controls, with unchanged existing assets."""
import argparse,json,struct,subprocess,zipfile,ast
from pathlib import Path
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EIP,UC_X86_REG_EAX
from lit_software_probe import LitSoftwareFixture
from fireball_visual_reference import read_fireball
from speed_controller_preflight import ROOT,sha,read_asset
p=argparse.ArgumentParser();p.add_argument('exe',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True)
f=LitSoftwareFixture(a.exe,32,32);v=f.vm;block=v.allocate(0x1ac)
def malloc(uc,*_):
 sp=uc.reg_read(UC_X86_REG_ESP);assert v.u32(sp+4)==0x1ac;uc.reg_write(UC_X86_REG_EAX,block);uc.reg_write(UC_X86_REG_EIP,v.u32(sp));uc.reg_write(UC_X86_REG_ESP,sp+4)
h=v.uc.hook_add(UC_HOOK_CODE,malloc,begin=0x58d176,end=0x58d176)
for fill in (0x5a,0xff):
 v.write(block,bytes([fill])*0x1ac);assert v.call(0x482fb0,(0x1ac,))==block;assert bytes(v.uc.mem_read(block,0x1ac))==bytes(0x1ac)
v.uc.hook_del(h)
# Preserve the former raw-v1/known-v3 interpretation as the negative-control oracle.
new=Path(ROOT/'tools/retail_runtime/fireball_visual_reference.py').read_text();node=next(n for n in ast.parse(new).body if isinstance(n,ast.FunctionDef)and n.name=='read_fireball');old=ast.get_source_segment(new,node).replace("if a['version']==3 or (a['version']==2 and a['flags']&0x80):","if a['version']==3:");env=dict(struct=struct,read_asset=read_asset);exec(old,env)
controls=[]
with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:
 for name in ['Imagery/Magic/newfireball.i3d','Imagery/Magic/yfireball.i3d','Imagery/Magic/maelstrom.I3D']:
  actual=next(n for n in z.namelist()if n.lower()==name.lower());data=z.read(actual);assert read_fireball(data)==env['read_fireball'](data);controls.append(dict(asset=actual,sha256=sha(data),unchanged=True))
 name='Imagery/Magic/cataclysm.i3d';data=z.read(name);current=read_fireball(data);assert current['version']==2 and current['flags']&0x80
 try:env['read_fireball'](data)
 except AssertionError:pass
 else:raise AssertionError('Old parser must reject compressed-v2 fixture')
 controls.append(dict(asset=name,sha256=sha(data),compressed_version2=True,old_rejected=True))
r=dict(status='pass',allocator_controls=2,allocator='Original482fb0 executes its own zero initialization over a dirty 0x1ac-byte allocation. Only upstream malloc pointer is supplied.',assets=controls,executable_sha256=sha(a.exe.read_bytes()),source_sha256=sha(Path(__file__).read_bytes()));(a.output/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');print('pass allocator2 assets4')
