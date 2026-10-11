"""Observe the original Sandswirl child-residency boundary without substituting it."""
import argparse,json
from pathlib import Path
from character_native_preflight import AuthenticCharacters,ROOT,sha
from lit_software_probe import LitSoftwareFixture
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX

def run(executable,boot,output):
 output.mkdir(parents=True,exist_ok=False);f=AuthenticCharacters(executable,boot);f.select_screen_prefix();v,call=f.vm,f.world.call
 software=LitSoftwareFixture(executable,640,480,camera=(3185,3185,0),vm=v)
 returns=[]
 def observe(uc,address,*unused):returns.append(dict(address=hex(address),result=hex(uc.reg_read(UC_X86_REG_EAX))))
 for address in (0x4edeb1,0x4edebc):v.uc.hook_add(UC_HOOK_CODE,observe,begin=address,end=address)
 owner=f.new_object('Sandswirl',(3185,3185,16),25,0x5ab72c);assert call(0x46e8b0,this=owner)==1
 animator=v.u32(owner+0x58);assert v.u32(animator)==0x5ab930 and v.u32(animator+0x104)==0
 assert returns==[dict(address='0x4edeb1',result='0xffffffff'),dict(address='0x4edebc',result='0x0')]
 try:
  v.put_u32(0x65cb38,1);call(0x470cc0,this=owner);call(0x4edd80,this=owner)
 except RuntimeError as error:
  assert f.world.last_fault[:2]==(0x4ee0bc,0)
  blocked=str(error)
 else:raise AssertionError('Missing child unexpectedly advanced')
 report=dict(status='bounded-caller-blocker',accepted=False,type_id='0xbecefeda',executable_sha256=sha(executable),boot_image_sha256=sha(boot),owner_vtable='0x5ab72c',animator_vtable='0x5ab930',actual_map_returns=returns,child_pointer=0,fault=dict(instruction='0x4ee0bc',address=0,error=blocked),scope='Original class constructor, child sand MapPane.NewObject/GetInstance and CreateAnimator; actual owner Pulse. Cached screen prefix has no sectors. No substituted child/pool, return handler or visual claim. Genuine child residency/attachment is required before whole lifecycle.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--executable',type=Path,default=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe');p.add_argument('--boot-image',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();print(json.dumps(run(a.executable,a.boot_image,a.output)))
