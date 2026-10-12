"""Execute original MakeIndex clock/RNG controls; never alter modern map IDs."""
import argparse,json,hashlib
from pathlib import Path
from character_native_preflight import AuthenticCharacters,ROOT,sha
from unicorn import UC_HOOK_CODE

def run(boot,out):
 out.mkdir(parents=True,exist_ok=False);f=AuthenticCharacters(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',boot);v=f.vm;call=f.world.call;calls=[]
 v.uc.hook_add(UC_HOOK_CODE,lambda *_:calls.append(1),begin=0x58c582,end=0x58c582);tls=call(0x58fbf1);cases=[]
 for clock in(0,1,1000):
  v.sleep_ms=clock;v.ticks=0;call(0x58c575,(1,));calls.clear();identifier=call(0x44ce30,this=0x6668d8)
  first=((1*214013+2531011)>>16)&32767;expected=((first+clock)*214013+2531011)&0xffffffff
  assert len(calls)==2 and v.u32(tls+0x14)==expected,(clock,len(calls),v.u32(tls+0x14),expected)
  cases.append(dict(clock_ms=v.milliseconds,index=identifier,rand_calls=len(calls),first_rand=first,seed_after=v.u32(tls+0x14),predicted_seed=expected))
 assert len({x['seed_after']for x in cases})==3
 report=dict(status='pass',accepted=False,visual_pass=False,imported_clock=v.stubs[v.u32(0x5a30fc)],cases=cases,executable_sha256=sha(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe'),boot_sha256=sha(boot),tool_sha256=sha(__file__),scope=__doc__)
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--boot-image',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.boot_image,a.output)
