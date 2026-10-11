"""Observe original ceyes RGB565 integer triangle packets after full normal lighting."""
import sys,struct,zipfile,json,argparse
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parent))
from ceyes_visual_reference import CEyesScene,ROOT,sha
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP
p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path);p.add_argument('--output',type=Path,required=True);args=p.parse_args()
with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read('Imagery/Magic/ceyes.I3D')
f=CEyesScene(args.executable,data);v=f.vm;records=[];transformed=[]
def transform(uc,*_):
 sp=uc.reg_read(UC_X86_REG_ESP);n=v.u32(sp+4);transformed[:]=list(struct.iter_unpack('<3f4I2f',v.uc.mem_read(v.u32(0x675fb4),n*36)))
def packet(uc,*_):
 sp=uc.reg_read(UC_X86_REG_ESP);a=list(struct.unpack('<29i',uc.mem_read(sp+4,116)));actual=[a[1+i*8:9+i*8]for i in range(3)]
 expected=[[int(q[0]),int(-q[1]),int(q[2]),int(q[7]*32),int(q[8]*32),*q[3:6]]for q in transformed]
 assert all(q in expected for q in actual),(actual,expected)
 records.append(dict(frame=f.state()['frame'],floating=transformed.copy(),integer=actual))
 # Only the already-observed leaf pixel kernel is omitted in this packet-only
 # proof; independently executed full raster films are the appearance reference.
 f.ret(0,0)
v.uc.hook_add(UC_HOOK_CODE,transform,begin=0x56d960,end=0x56d960)
v.uc.hook_add(UC_HOOK_CODE,packet,begin=0x54e3a0,end=0x54e3a0)
for t in range(245):f.step();f.animate();f.render()
args.output.mkdir(parents=True,exist_ok=True)
r=dict(status='pass',triangles=len(records),integer_fields=len(records)*24,records=records,asset_sha256=sha(data),executable_sha256=sha(args.executable.read_bytes()),source_sha256=sha(Path(__file__).read_bytes()),executed=['56eb30 transform/Illuminate','56e3e7..56e4f5 integer triangle construction','58a840 CRT truncation'],scope='Actual four-corner FVF112 normal-lit output and original RGB565 Gouraud triangle packets. XY/Z and texture UV*32 truncate toward zero, not round-to-nearest. Pixel kernel54e3a0 omitted only after packet observation; complete unmodified original raster films separately retained.',accepted=False)
(args.output/'proof.json').write_text(json.dumps(r,indent=2)+'\n');print('pass',len(records),'triangles',r['integer_fields'],'fields')
