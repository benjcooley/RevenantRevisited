"""Check own YFireWind original controller and null-spell visual compatibility."""
import argparse,json
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from runtime import Runtime
from speed_controller_preflight import sha
p=argparse.ArgumentParser(description=__doc__)
p.add_argument('executable',type=Path);p.add_argument('--base',type=Path,required=True);p.add_argument('--variant',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args()
v=Runtime(a.executable);cs=Cs(CS_ARCH_X86,CS_MODE_32)
def instructions(address):
 result=[]
 for i in cs.disasm(bytes(v.uc.mem_read(address,0x658)),address):
  operand=hex(int(i.op_str,16)-address)if i.mnemonic.startswith('j')else i.op_str
  result.append((i.address-address,i.mnemonic,operand))
 return result
base=instructions(0x4e2620);variant=instructions(0x5069a0);assert base==variant
b=json.loads(a.base.read_text());y=json.loads(a.variant.read_text());assert b['effect']=='FireWind'and y['effect']=='YFireWind';assert b['initial']==y['initial'];assert len(b['records'])==len(y['records'])
packets=0
for x,z in zip(b['records'],y['records']):
 assert x['state']==z['state'];assert len(x['draws'])==len(z['draws'])
 for left,right in zip(x['draws'],z['draws']):
  for key in ('object','matrix','states'):assert left[key]==right[key],(x['tick'],key)
  packets+=1
r=dict(status='pass',original_executable_sha256=sha(a.executable.read_bytes()),base_trace=dict(path=str(a.base.resolve()),sha256=sha(a.base.read_bytes())),y_trace=dict(path=str(a.variant.resolve()),sha256=sha(a.variant.read_bytes())),pulse_instructions=len(base),initial_and_states_exact=len(y['records'])+1,ordered_object_matrix_scene_packets_exact=packets,own_asset_sha256=y['asset_sha256'],scope='Own Y factory50a050/vtable5b1d28, Init5067d0/Pulse5069a0/Render507000 fully executed. Shared visual core justified only under controlled null-spell facing0; own authored Y material/texture/vertices remain separately loaded.',limits=['Y Init gets facing through4df070 instead of base spell-target selection; natural facing remains open.','Y owner radius grows5 without base320cap and damages every40 instead of80; null-spell captures do not exercise damage or target cleanup.','Distinct original sound resource is a silent boundary; full audio/game caller equivalence not claimed.'])
a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(r,indent=2)+'\n');print('pass',len(base),'instructions',len(y['records'])+1,'states',packets,'packets')
