"""Actual sector residency distinguishes missing Sandswirl child type from map absence."""
import argparse,json,struct
from pathlib import Path
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX,UC_X86_REG_ESP
from character_native_preflight import AuthenticCharacters,ROOT,sha
from lit_software_probe import LitSoftwareFixture

def run(executable,boot,output):
 output.mkdir(parents=True,exist_ok=False);f=AuthenticCharacters(executable,boot);f.select_screen_prefix();v,call=f.vm,f.world.call
 sw=LitSoftwareFixture(executable,640,480,camera=(3185,3185,0),vm=v)
 assert v.u32(0x668578)==0
 sector=call(0x4982b0,(0,3,3,0),instruction_limit=150000000)
 assert sector and v.u32(0x668578)==1 and call(0x499e10,(0,3,3))==sector
 registry=[]
 for i in range(v.u32(0x659d30)):
  p=v.u32(v.u32(0x659d40)+i*4);name=v.string(v.u32(p));kind=v.uc.mem_read(p+0x22,1)[0]
  if 'sand' in name.lower():registry.append(dict(name=name,kind=kind,type=struct.unpack('<H',v.uc.mem_read(p+0x20,2))[0]))
 events=[]
 def observe(uc,address,*unused):
  sp=uc.reg_read(UC_X86_REG_ESP)
  if address==0x450e40:
   definition=v.u32(sp+4);events.append(dict(function='MapPane.NewObject',definition=bytes(v.uc.mem_read(definition,52)).hex(),kind=struct.unpack('<h',v.uc.mem_read(definition,2))[0],type=struct.unpack('<h',v.uc.mem_read(definition+2,2))[0]))
  elif address==0x499e10:events.append(dict(function='FindLoadedSector',args=[v.u32(sp+4+i*4)for i in range(3)]))
  else:events.append(dict(address=hex(address),result=hex(uc.reg_read(UC_X86_REG_EAX))))
 for a in(0x450e40,0x499e10,0x4ede62,0x4edeb1,0x4edebc):v.uc.hook_add(UC_HOOK_CODE,observe,begin=a,end=a)
 owner=f.new_object('Sandswirl',(3185,3185,16),25,0x5ab72c);assert call(0x46e8b0,this=owner)==1;animator=v.u32(owner+0x58)
 assert v.u32(animator+0x104)==0
 requested=v.string(0x5e1004);assert requested.lower()=='sand'
 assert not any(row['name'].lower()=='sand' and row['kind']==25 for row in registry)
 assert events==[dict(address='0x4ede62',result='0xffffffff'),dict(function='MapPane.NewObject',definition=events[1]['definition'],kind=25,type=-1),dict(address='0x4edeb1',result='0xffffffff'),dict(address='0x4edebc',result='0x0')]
 try:v.put_u32(0x65cb38,1);call(0x470cc0,this=owner);call(0x4edd80,this=owner)
 except RuntimeError as error:
  assert f.world.last_fault[:2]==(0x4ee0bc,0);failure=str(error)
 else:raise AssertionError('Missing child unexpectedly advanced')
 report=dict(status='missing-authored-child-type',accepted=False,type_id='0xbecefeda',executable_sha256=sha(executable),boot_image_sha256=sha(boot),probe_sha256=sha(Path(__file__)),requested_child=requested,related_registry=registry,sector=dict(public_load='0x4982b0',args=[0,3,3,0],pointer=hex(sector),public_query='0x499e10',query_matches=True,resident_count=v.u32(0x668578),object_count=v.u32(sector+0xb8)),events=events,child=0,fault=failure,scope='Actual original LoadSector creates/loads and registers an empty sector, and original FindLoadedSector returns it. Original Init child-name lookup fails before MapPane can query residency: class25/type−1, no shipped effect named sand. No pointer/return/type/resource mutation, no child alias, no visual acceptance. This supersedes missing-residency as the immediate Sandswirl blocker in this shipped registry.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report));return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--executable',type=Path,default=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe');p.add_argument('--boot-image',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.executable,a.boot_image,a.output)
