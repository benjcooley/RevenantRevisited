"""Execute exact JhagaAttack complete registration and owner factory."""
import argparse,json,zipfile
from pathlib import Path
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ECX
from static_mesh_probe import StaticFixture,ROOT
from simple_effect_preflight import native_dispatch,asset_summary
from speed_controller_preflight import NativeSpeed,sha

def run(exe,out):
 out.mkdir(parents=True,exist_ok=True);f=StaticFixture(exe,64,64,5000000);v=f.vm;registrations=native_dispatch(v);base=[]
 def boundary(uc,*_):base.append(hex(uc.reg_read(UC_X86_REG_ECX)));NativeSpeed.ret(f,0,8)
 hook=v.uc.hook_add(UC_HOOK_CODE,boundary,begin=0x46e1f0,end=0x46e1f0)
 v.call(0x505800);assert v.u32(v.u32(0x66d168))==0x509c90
 owner=v.call(0x509c90,(v.allocate(0x100),0),this=0x66d168);v.uc.hook_del(hook)
 assert v.u32(owner)==0x5b14ac and v.u32(owner+8)==0x4c009 and len(base)==1
 name=v.allocate(20);v.write(name,b'JhagaAttack\0');assert v.call(0x40dca0,(name,))==0x5e8508;assert v.u32(v.u32(0x5e8508))==0x40dc00
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:
  data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/jattack.i3d'));text=z.read('class.def').decode('latin1');assert '0xaeaeeb32' in text
 assert sha(data)=='dd85944e6e587e9647746f7bfb2cd5505e1f46d3344cbe53a82a0dcd7d46522d'
 report=dict(status='pass',effect='JhagaAttack',type_id='0xaeaeeb32',owner_factory='0x509c90',owner_vtable=hex(v.u32(owner)),owner_flags=hex(v.u32(owner+8)),animator_factory='0x40dc00',animator_vtable='0x5a370c',asset_sha256=sha(data),asset=asset_summary(data),retail_sha256=sha(v.image),scope='Complete named registry and entire custom owner factory/Effect constructor; TObjectInstance resource definition constructor46e1f0 is the explicit interface. Full animator Init/Animate/Render proof is separate.')
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('exe',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();print(run(a.exe,a.output)['status'])
