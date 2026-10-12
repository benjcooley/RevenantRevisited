"""Observe nullifier's complete original RGB5 and integer raster packets.

Original full loaded owner/Animate/Render and pixel kernel execute unchanged.
Read-only observers prove byte>>3, integer screen XYZ and UV times the actual
64x64 RGB565 texture. Existing full-reference states/PNG pixels must repeat.
"""
import argparse,hashlib,json,struct
from pathlib import Path
from PIL import Image
from dust_loaded_context_probe import DustLoadedScene
from character_native_preflight import ROOT,sha
from software_probe import save_rgb565_png
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP

def run(exe,boot,reference,out):
 out.mkdir(parents=True,exist_ok=False);expected=json.loads(reference.read_text());f=DustLoadedScene(exe,boot,0,38,(0,0,16),name='nullifier');v=f.vm;transformed=[];authored=[];records=[];tick=0;colors=0
 def authored_packet(uc,*_):
  sp=uc.reg_read(UC_X86_REG_ESP);primitive,fvf,pointer,count=struct.unpack('<4I',uc.mem_read(sp+4,16));assert (primitive,fvf,count)==(4,0x1e2,4)
  raw=bytes(uc.mem_read(pointer,count*32));authored[:]=[struct.unpack_from('<I',raw,i*32+16)[0]for i in range(count)]
 def transform(uc,*_):
  nonlocal colors
  sp=uc.reg_read(UC_X86_REG_ESP);count=v.u32(sp+4);transformed[:]=list(struct.iter_unpack('<3f4I2f',v.uc.mem_read(v.u32(0x675fb4),count*36)))
  for word,q in zip(authored,transformed):assert list(q[3:7])==[(word>>shift)&31 for shift in(19,11,3,27)];colors+=4
 def packet(uc,*_):
  sp=uc.reg_read(UC_X86_REG_ESP);a=list(struct.unpack('<29i',uc.mem_read(sp+4,116)));actual=[a[1+i*8:9+i*8]for i in range(3)]
  wanted=[[int(q[0]),int(-q[1]),int(q[2]),int(q[7]*64),int(q[8]*64),*q[3:6]]for q in transformed]
  assert all(q in wanted for q in actual),(tick,actual,wanted)
  records.append(dict(tick=tick,floating_vertices=transformed.copy(),integer_vertices=actual))
 for address,callback in((0x56eb30,authored_packet),(0x56d960,transform),(0x54e3a0,packet)):v.uc.hook_add(UC_HOOK_CODE,callback,begin=address,end=address)
 rows=[]
 for tick,e in enumerate(expected['frames']):
  if tick:f.step(tick)
  raw=f.render();state=f.state();assert state==e['state'];images={}
  if 'software'in e['images']:
   path=out/f'software-{tick:03}.png';save_rgb565_png(path,raw,640,480);assert Image.open(path).tobytes()==Image.open(e['images']['software']['path']).tobytes();images['software']=dict(path=str(path),sha256=sha(path))
  rows.append(dict(tick=tick,state=state,images=images,rgb565_sha256=hashlib.sha256(raw).hexdigest()))
 assert len(rows)==77 and len(records)==5546 and colors==44368,(len(rows),len(records),colors)
 report=dict(status='pass',accepted=False,scope=__doc__,rows=rows,triangles=len(records),rgba5_channels=colors,records=records,executable_sha256=sha(exe),boot_sha256=sha(boot),reference_sha256=sha(reference),tool_sha256=sha(__file__))
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(rows),len(records),colors);return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__)
 for name in('boot-image','reference','output'):p.add_argument('--'+name,type=Path,required=True)
 a=p.parse_args();run(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',a.boot_image,a.reference,a.output)
