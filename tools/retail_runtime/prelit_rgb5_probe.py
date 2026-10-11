#!/usr/bin/env python3
"""Execute original FVF_LVERTEX constant RGB5 and flat-depth controls."""
import argparse,json,struct
from pathlib import Path
from lit_software_probe import LitSoftwareFixture
from speed_controller_preflight import sha

def run(exe,out):
 out.mkdir(parents=True,exist_ok=False);f=LitSoftwareFixture(exe,640,480);v=f.vm
 for a,x in((0x5d7a28,1),(0x66818c,0),(0x5e8790,0)):v.put_u32(a,x)
 v.call(0x417d60,(8,1),this=0x65a57c);matrix=v.allocate(64);v.write(matrix,struct.pack('<16f',*[float(i%5==0)for i in range(16)]));v.call(0x56d5f0,(1,matrix))
 def draw(rgb,z=0,alpha=31):
  word=(alpha<<27)|(rgb[0]*8<<16)|(rgb[1]*8<<8)|(rgb[2]*8);records=[]
  for x,y in((-24,-24),(24,-24),(24,24),(-24,24)):
   diff=x/1.0101525783538818;total=(y+1.237179160118103*z)/.5050762891769409;records.append(struct.pack('<3f3I2f',(total+diff)/2,(total-diff)/2,z,0,word,0,0,0))
  v.write(f.authored,b''.join(records));v.write(f.indices,struct.pack('<6H',0,1,2,0,2,3));v.put_u32(0x675fb8,f.vertices+4*36)
  for k,x in((7,1),(14,0),(22,1)):v.call(0x56d400,(k,x))
  v.call(0x56eb30,(4,0x1e2,f.authored,4,f.indices,6,0));return v.surface_bytes('screen'),v.surface_bytes('depth')
 rows=[]
 for rgb in((8,2,3),(31,31,31),(0,31,0),(31,0,0)):
  source=(rgb[0]<<11)|(rgb[1]<<6)|rgb[2]
  for alpha in range(32):
   for background in(0,0x1234,0xffff):
    f.clear(background);raw,depth=draw(rgb,alpha=alpha);expected=background
    expected=0
    for shift,mask in((11,31),(5,63),(0,31)):
     src=(source>>shift)&mask;dst=(background>>shift)&mask;expected|=(dst-dst*(alpha+1)//32+src*(alpha+1)//32)<<shift
    covered=[color[0]for color,z in zip(struct.iter_unpack('<H',raw),struct.iter_unpack('<H',depth))if z[0]!=65535]
    assert covered and set(covered)=={expected},(rgb,alpha,background,set(covered),expected)
    rows.append(dict(rgb5=rgb,alpha5=alpha,background=background,rgb565=expected,pixels=len(covered)))
 depth_cases=[]
 for first,second,expected in((20,0,31<<11),(0,20,31<<6),(0,0,31<<6)):
  f.clear();one,dep=draw((31,0,0),first);two,dep=draw((0,31,0),second);center=struct.unpack_from('<H',two,2*(240*640+320))[0];nonzero=set(x[0]for x in struct.iter_unpack('<H',two)if x[0]);assert nonzero=={expected},(first,second,nonzero);depth_cases.append(dict(first_z=first,second_z=second,rgb565=expected,center=center,depth_values=sorted(set(x[0]for x in struct.iter_unpack('<H',dep)if x[0]!=65535))))
 report=dict(status='pass',colors=rows,depth=depth_cases,executable_sha256=sha(exe.read_bytes()),probe_sha256=sha(Path(__file__).read_bytes()),scope='Actual prelit FVF_LVERTEX RGBA8→RGBA5 and untextured raster, material/normal lighting bypassed; same projected quad differentworldZ controls nearer/behind/equal-depth overwrite despite ZWRITE0.')
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('executable',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.executable,a.output)
