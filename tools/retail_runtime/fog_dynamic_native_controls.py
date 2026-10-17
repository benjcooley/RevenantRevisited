#!/usr/bin/env python3
"""Literal original varying RGB5/A5 and sloped-triangle flat-depth controls."""
import argparse,json,struct,math
from pathlib import Path
from lit_software_probe import LitSoftwareFixture
from speed_controller_preflight import sha

def run(exe,out):
 out.mkdir(parents=True,exist_ok=False);f=LitSoftwareFixture(exe,640,480);v=f.vm
 for a,x in((0x5d7a28,1),(0x66818c,0),(0x5e8790,0)):v.put_u32(a,x)
 v.call(0x417d60,(2,1),this=0x65a57c)
 def triangle(x,y,rgb,alphas,depths):
  points=[(x,y),(x+96,y),(x,y+96)]
  f.draw([(p[0],p[1],z,*c,a,0,0)for p,c,a,z in zip(points,rgb,alphas,depths)],[0,1,2],z_enabled=True,z_write=False)
 def value(x,y):
  color=struct.unpack_from('<H',v.surface_bytes('screen'),(y*640+x)*2)[0];depth=struct.unpack_from('<H',v.surface_bytes('depth'),(y*640+x)*2)[0];return color,depth
 cases=[]
 specs=[dict(name='alpha_gradient',rgb=[[22]*3]*3,alpha=[0,31,0]),dict(name='rgb_gradient',rgb=[[0,22,31],[31,0,0],[0,22,31]],alpha=[3]*3),dict(name='combined_gradient',rgb=[[0,22,31],[31,0,0],[0,22,31]],alpha=[0,31,0])]
 for i,s in enumerate(specs):
  f.clear();x=40+180*i;y=40;triangle(x,y,s['rgb'],s['alpha'],[2750]*3);samples=[]
  for dx in(8,16,24,32,40,48,56,64):
   rgb565,z=value(x+dx,y+16)
   # 54eba0 scanline setup: (delta << 8) / edge length, signed truncation.
   # This axis-aligned control isolates the native 8-bit slope accumulator.
   values=[(start*256+math.trunc((end-start)*256/96)*dx)//256
           for start,end in zip(s['rgb'][0]+[s['alpha'][0]],s['rgb'][1]+[s['alpha'][1]])]
   r,g,b,a=values;channels=[r*(a+1)//32,2*g*(a+1)//32,b*(a+1)//32]
   assert rgb565==(channels[0]<<11|channels[1]<<5|channels[2]),(s['name'],dx,values,rgb565)
   samples.append(dict(pixel=[x+dx,y+16],rgb565=rgb565,depth=z,fixed8_interpolated_rgba5=values))
  cases.append(dict(**s,origin=[x,y],samples=samples))
 # A sloped triangle uses max vertex depth, not interpolated depth.
 depths=[]
 for i,vertices in enumerate(([2700.25,2800.75,2700.5],[2700.25,2769.75,2700.5],[2769.1,2769.8,2769.5])):
  f.clear();x=40+180*i;y=200;triangle(x,y,[[31,0,0]]*3,[31]*3,[2770]*3);triangle(x,y,[[0,31,0]]*3,[31]*3,vertices);pixel,z=value(x+16,y+16);expected=(31<<11,2770)if i==0 else(31<<6,2769);assert(pixel,z)==expected,(i,pixel,z,expected);depths.append(dict(origin=[x,y],background_depth=2770,vertex_depths=vertices,pixel=[x+16,y+16],rgb565=pixel,depth=z))
 report=dict(status='pass',cases=cases,depth=depths,executable_sha256=sha(exe.read_bytes()),probe_sha256=sha(Path(__file__).read_bytes()),scope='Actual Scene2 original untextured raster, varying RGB5/A5, source ZWRITEfalse still writes trunc(max triangle depth); near/behind/fractionalmax controls. All24 gradient samples independently match signed8-bit slope interpolation; precise GPU interpolation remains a separate residual.')
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('executable',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.executable,a.output)
