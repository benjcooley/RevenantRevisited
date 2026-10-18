"""Compare actual Dragonfire pool matrix consumer with every original full draw."""
import argparse,json,struct,subprocess,hashlib
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
def run(native,out):
 out.mkdir(parents=True,exist_ok=False);ref=json.loads(native.read_text());cpp=out/'pose.cpp';cpp.write_text(r'''
#include "dragonfirepose.h"
#include "retailsoftwaretransform.h"
#include <cstdio>
int main(){retail_dragonfire::Particle p;hmm_mat4 owner;BuildRetailSoftwareOwner(owner,3185,3185,16,0);while(fread(&p,1,88,stdin)==88){hmm_mat4 local=retail_dragonfire::LocalMatrix(p),world;MtxMultiply(&world,&local,&owner);fwrite(&world,1,64,stdout);}}
''');binary=out/'pose';subprocess.run(['clang++','-std=c++17','-ffp-contract=off','-iquote',str(ROOT/'src'),'-I',str(ROOT/'thirdparty/handmademath'),str(cpp),str(ROOT/'src/engine/core/math3d.cpp'),'-o',str(binary)],check=True)
 inputs=[];draws=[];identities=[]
 for row in ref['records']:
  if row['state']['flags']&0x1000:assert not row['draws'];continue
  current=[]
  for index,name in enumerate(('fire','smoke')):
   raw=bytes.fromhex(row['state']['pools'][name])
   for slot in range(300):
    p=raw[slot*88:(slot+1)*88]
    if struct.unpack_from('<i',p,84)[0]:inputs.append(p);current.append((index,slot))
  assert len(current)==len(row['draws']),(row['tick'],len(current),len(row['draws']))
  for (index,slot),draw in zip(current,row['draws']):assert draw['object']==index;draws.append(draw);identities.append((row['tick'],index,slot))
 result=subprocess.check_output([str(binary)],input=b''.join(inputs));assert len(result)==len(draws)*64
 errors=[];maximum=0
 for i,draw in enumerate(draws):
  values=struct.unpack_from('<16f',result,i*64)
  for j,(a,b)in enumerate(zip(draw['matrix'],values)):
   e=abs(a-b);maximum=max(maximum,e)
   if e>1e-4:errors.append((identities[i],j,a,b))
 assert not errors,errors[:10]
 report=dict(status='pass',draws=len(draws),fields=len(draws)*16,max_float_error=maximum,native_manifest_sha256=hashlib.sha256(native.read_bytes()).hexdigest(),pose_sha256=hashlib.sha256((ROOT/'src/game/effects/dragonfirepose.h').read_bytes()).hexdigest(),scope=__doc__)
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--native',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.native,a.output)
