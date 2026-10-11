"""Compare the shipped terminal-vector omission with original parser controls."""
import zipfile,copy,json,struct,argparse
from pathlib import Path
from speed_controller_preflight import NativeSpeed,read_asset,ROOT
parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--output',type=Path,required=True);args=parser.parse_args()
with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/nakrnoth.i3d'))
base=read_asset(data);base['name']='Nakrnoth';cases=[]
for name,fix in [('shipped',False),('closed_final_vector',True)]:
 a=copy.deepcopy(base)
 if fix:a['tags'][1]['parameters']=a['tags'][1]['parameters'].replace('(180,180,255]','(180,180,255)]')
 f=NativeSpeed(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',a,eager_pose=False);init=f.initialize(0);v=f.vm;c=f.controller;expressions=[];dest=v.allocate(12)
 for i in range(v.u32(c+0x30)):
  e=v.u32(v.u32(c+0x40)+i*4);keys=[bytes(v.uc.mem_read(v.u32(v.u32(e+0x20)+j*4),24)).hex()for j in range(v.u32(e+0x10))];samples=[]
  for t in(0,1,24,25,26,50,99,100):v.call(0x401390,(t,dest,dest+4,dest+8),this=e);samples.append(list(struct.unpack('<3f',v.uc.mem_read(dest,12))))
  expressions.append(dict(dimensions=v.u32(e+8),keys=keys,samples=samples))
 row=dict(name=name,initialization=init,expressions=expressions,parameters=a['tags'][1]['parameters'],controller_constants=bytes(v.uc.mem_read(c+0x4c,0x118)).hex());cases.append(row);print(json.dumps(row),flush=True)
assert cases[0]['expressions']==cases[1]['expressions']; print('EXPR_EQUAL True',flush=True)
args.output.write_text(json.dumps(cases,indent=2)+'\n')
