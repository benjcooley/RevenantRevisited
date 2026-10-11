"""Actual YLose FVF1e2 byte-to-RGB5 conversion before the original raster."""
import sys,struct,zipfile,json,argparse
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parent))
from ylose_visual_reference import YLoseScene,ROOT,sha
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP
p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path);p.add_argument('--output',type=Path,required=True);args=p.parse_args()
with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read('Imagery/Magic/ylose.I3D')
f=YLoseScene(args.executable,data);v=f.vm;records=[]
def observe(uc,address,*_):
 sp=uc.reg_read(UC_X86_REG_ESP);n=v.u32(sp+4);base=v.u32(0x675fb4);obj=f.current_object
 if obj not in(0,1,12):
  color=v.u32(v.u32(f.animobjs[obj]+0xa4)+16);rgb=[(color>>shift)&255 for shift in(16,8,0)]
  colors=[struct.unpack('<4I',v.uc.mem_read(base+i*36+12,16))for i in range(n)]
  assert all(list(c[:3])==[x>>3 for x in rgb]for c in colors),(obj,rgb,colors)
  records.append(dict(frame=f.state()['frame'],object=obj,vertices=n,color=hex(color),carrier=list(colors[0])))
 f.ret(0,0)
v.uc.hook_add(UC_HOOK_CODE,observe,begin=0x56d960,end=0x56d960)
for tick in range(14):f.step();f.animate();f.render()
p=args.output;p.mkdir(parents=True,exist_ok=True);(p/'proof.json').write_text(json.dumps(dict(status='pass',fields=sum(r['vertices']*3 for r in records),records=records,executable_sha256=sha(args.executable.read_bytes()),asset_sha256=sha(data),source_sha256=sha(Path(__file__).read_bytes()),scope='Actual full owner/controller Render and56eb30 FVF1e2 transform; observe all RGB5 carriers at56d960 entry and skip only raster. Complete independent original raster films remain separate.'),indent=2)+'\n');print('pass',len(records),'draws',sum(r['vertices']*3 for r in records),'RGB5 fields')
