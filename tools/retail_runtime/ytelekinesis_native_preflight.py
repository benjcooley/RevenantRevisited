"""Execute exact YTelekinesis complete registration and owner factory."""
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
 v.call(0x505f30);assert v.u32(v.u32(0x66d118))==0x509e20
 owner=v.call(0x509e20,(v.allocate(0x100),0),this=0x66d118);v.uc.hook_del(hook)
 assert v.u32(owner)==0x5b18b4 and v.u32(owner+8)==0x48001 and len(base)==1
 name=v.allocate(20);v.write(name,b'YTelekinesis\0');assert v.call(0x40dca0,(name,))==0x66d140;assert v.u32(v.u32(0x66d140))==0x509eb0
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:
  data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/misc/chunk.i3d'));text=z.read('class.def').decode('latin1');assert '0xaeaeeb28' in text
 assert sha(data)=='eec03fea3caf7a5c70d4f594a5a4406ba0f2acefb8698baf0e029d26f375f306'
 report=dict(status='pass',effect='YTelekinesis',type_id='0xaeaeeb28',owner_factory='0x509e20',owner_vtable=hex(v.u32(owner)),owner_flags=hex(v.u32(owner+8)),animator_factory='0x509eb0',animator_vtable='0x5b1ab8',asset_sha256=sha(data),asset=asset_summary(data),retail_sha256=sha(v.image),scope='Complete named registry and entire custom owner factory/Effect constructor; TObjectInstance resource definition constructor46e1f0 is the explicit interface. Full animator Init/Animate/Render proof is separate.')
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('exe',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();print(run(a.exe,a.output)['status'])
