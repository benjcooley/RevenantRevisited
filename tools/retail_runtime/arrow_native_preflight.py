"""Independent exact ArrowEffect and QueenArrow owner/animator factory proof."""
import argparse,json,struct,zipfile
from pathlib import Path
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ECX
from static_mesh_probe import StaticFixture,ROOT
from simple_effect_preflight import native_dispatch,asset_summary
from speed_controller_preflight import NativeSpeed,sha
BINDINGS=(('ArrowEffect','0xad92bd1f',0x5048d0,0x66d178,0x5097c0,0x5b0d68,0x66d188,0x509870,0x5b0f74,0),('QueenArrow','0xad92bd1e',0x504ce0,0x66d158,0x509940,0x5b0fdc,0x66cfe8,0x509a30,0x5b11e4,2))
def run(exe,out):
 out.mkdir(parents=True,exist_ok=True);f=StaticFixture(exe,64,64,5000000);v=f.vm;rows=[];registrations=native_dispatch(v);base=[]
 def boundary(uc,*_):base.append(hex(uc.reg_read(UC_X86_REG_ECX)));NativeSpeed.ret(f,0,8)
 h=v.uc.hook_add(UC_HOOK_CODE,boundary,begin=0x46e1f0,end=0x46e1f0)
 for name,ident,constructor,owner_builder,owner_factory,owner_vtable,builder,factory,leaf,color in BINDINGS:
  v.call(constructor);assert v.u32(v.u32(owner_builder))==owner_factory
  owner=v.call(owner_factory,(v.allocate(0x100),0),this=owner_builder)
  assert(v.u32(owner),v.u32(owner+0x190),v.u32(owner+0x198),v.u32(owner+0x19c))==(owner_vtable,1,6,color)
  if name=='QueenArrow':assert v.u32(owner+0x1a0)==0xffffffff
  assert v.u32(v.u32(builder))==factory
  ptr=v.allocate(len(name)+1);v.write(ptr,name.encode()+b'\0');assert v.call(0x40dca0,(ptr,))==builder
  rows.append(dict(name=name,type_id=ident,owner_factory=hex(owner_factory),actual_owner_vtable=hex(v.u32(owner)),initial=1,grow=6,color=color,owner_flags=hex(v.u32(owner+8)),animator_factory=hex(factory),expected_animator_vtable=hex(leaf)))
 v.uc.hook_del(h)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:
  data=z.read('Imagery/Misc/arroweffects.i3d');text=z.read('class.def').decode('latin1').lower()
  for _,ident,*_ in BINDINGS:assert ident in text
 report=dict(status='pass',bindings=rows,asset_sha256=sha(data),asset=asset_summary(data),retail_sha256=sha(v.image),base_object_constructor_boundary='46e1f0: definition/imagery interface; actual Effect4de770 and entire custom owner factories execute',base_calls=len(base),scope='Distinct owner and full animator registration identity. Moving/full Render evidence lives in arrow_visual_reference; no stationary visual credit.')
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('exe',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();print(run(a.exe,a.output)['status'])
