#!/usr/bin/env python3
"""Literal EnergySpray ARGB4444 sub-quantum controls in the original software kernel."""
import argparse,collections,json,struct,zipfile
from pathlib import Path
from lit_software_probe import LitSoftwareFixture
from test_argb4444 import QUAD,INDICES,blend565,source565
from speed_controller_preflight import ROOT,sha

def run(exe,archive,out):
 out.mkdir(parents=True,exist_ok=False)
 with zipfile.ZipFile(archive)as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/energyspray.i3d'))
 u=lambda p:struct.unpack_from('<I',data,p)[0];r=lambda p:p+u(p);b=20+u(16);counts=collections.Counter()
 for i in range(2):
  t=r(b+40)+120*i;h,w=struct.unpack_from('<2I',data,t+8);pixels=r(r(t+108));counts.update(x[0]for x in struct.iter_unpack('<H',data[pixels:pixels+w*h*2]))
 candidates=[(word,n)for word,n in counts.most_common()if word&0xfff and blend565(word,0)==0];assert candidates
 f=LitSoftwareFixture(exe,32,32);v=f.vm
 for a,x in((0x5d7a28,1),(0x66818c,0),(0x5e8790,0)):v.put_u32(a,x)
 v.call(0x417d60,(2,1),this=0x65a57c);rows=[]
 for word,n in [(word,counts[word])for word in(0x0323,0x0322,0x1433,0x0755,0x0856,0x0fff,0xffff)]:
  f.set_texture(2,2,struct.pack('<4H',*[word]*4),format='ARGB4444');results=[]
  for light in(0,31):
   q=[(x,y,z,light,light,light,a,u,w)for x,y,z,_,__,___,a,u,w in QUAD];f.clear();pixels,_=f.draw(q,INDICES,z_enabled=True,z_write=False);got=struct.unpack_from('<H',pixels,2*(16*32+16))[0];assert got==blend565(word,0);results.append(got)
  packed=source565(word);alpha=((word>>12)+1)/16;floats=[((packed>>s)&mask)/mask*alpha for s,mask in((11,31),(5,63),(0,31))]
  rows.append(dict(texel=hex(word),occurrences=n,native_rgb565=hex(results[0]),vertex_color_negative_control=results[0]==results[1],float_black_contribution=floats))
 assert rows[-2]['native_rgb565']!='0x0', 'Alpha nibble0 must not become a blanket black-key'
 report=dict(status='pass',asset_sha256=sha(data),cases=rows,subquantum_authored_texels=sum(n for _,n in candidates),total_authored_texels=sum(counts.values()),scope='Actual original Scene2 ARGB4444 software draw on black with literal shipped texels; two incoming vertex RGB controls give identical pixels. Sub-quantum source contributions truncate perRGB565 channel tozero; floating blending retains positive contribution. Alpha0 white is a required nonzero negative control, prohibiting guessed transparent-key replacement. Destination rounding and general overlapped GPU framebuffer parity remain open.',probe_sha256=sha(Path(__file__).read_bytes()))
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],len(rows),report['subquantum_authored_texels'],rows[:2]);return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('executable',type=Path);p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.executable,a.archive,a.output)
