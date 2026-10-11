#!/usr/bin/env python3
"""Bound original untextured Fog alpha interpolation and RGB565 truncation."""
import argparse,json,struct
from pathlib import Path
from lit_software_probe import LitSoftwareFixture
from fizzle_probe import sha

def run(executable,output):
 f=LitSoftwareFixture(executable,64,64);v=f.vm
 for a,x in ((0x5d7a28,1),(0x66818c,0),(0x5e91c0,0),(0x5e8740,0),(0x5c61ac,1),(0x5e8790,0)):v.put_u32(a,x)
 v.call(0x417d60,(2,1),this=0x65a57c);f.clear()
 f.draw([(8,8,2750,22,22,22,0,0,0),(56,8,2750,22,22,22,31,0,0),(8,56,2750,22,22,22,0,0,0)],
        [0,1,2],z_enabled=True,z_write=False)
 pixels=v.surface_bytes('screen');rows=[]
 for x in (12,16,20,24,28,32,36,40):
  value=struct.unpack_from('<H',pixels,(16*64+x)*2)[0];rgb=[value>>11,(value>>5)&63,value&31]
  alpha5=int(31*(x-8)/48)
  expected=[int(22*(alpha5+1)/32),int(44*(alpha5+1)/32),int(22*(alpha5+1)/32)]
  assert rgb==expected
  rows.append(dict(x=x,y=16,rgb565=value,channels=rgb,interpolated_alpha5=alpha5))
 assert len({r['rgb565']for r in rows})==8
 report=dict(status='pass',executable_sha256=sha(executable.read_bytes()),probe_sha256=sha(Path(__file__).read_bytes()),
  vertex_rgb5=22,vertex_alpha5=[0,31,0],samples=rows,
  low_brightness_control=dict(rgb5=22,alpha5=3,native_rgb565_channels=[2,5,2],gpu_unquantized_channels=[2.75,5.5,2.75]),
  conclusion='Alpha interpolates within one triangle and truncates to A5; lookup result truncates to RGB565. Current GPU standard blend retains fractional channels. No fitted multiplier or renderer change; appearance credit held.')
 output.write_text(json.dumps(report,indent=2)+'\n');print('PASS8 varying-alpha samples, source post-blend truncation confirmed')

if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.executable,a.output)
