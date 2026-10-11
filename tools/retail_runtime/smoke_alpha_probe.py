#!/usr/bin/env python3
"""All eight literal Smoke texels: actual ARGB4444 kernel and overlap rounding."""
import argparse,collections,json,struct,zipfile
from pathlib import Path
from smoke_visual_reference import ROOT,ASSET_SHA,sha,read_smoke
from lit_software_probe import LitSoftwareFixture
from test_argb4444 import QUAD,INDICES,blend565,source565

def run(out):
 out.mkdir(parents=True,exist_ok=False)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/misc/smoke.i3d'))
 counts=collections.Counter(x[0]for x in struct.iter_unpack('<H',bytes.fromhex(read_smoke(data)['texture_data'][0]['raw'])));assert len(counts)==8
 f=LitSoftwareFixture(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',32,32);v=f.vm
 for a,x in((0x5d7a28,1),(0x66818c,0),(0x5e8790,0)):v.put_u32(a,x)
 v.call(0x417d60,(2,1),this=0x65a57c);rows=[]
 for word,count in sorted(counts.items()):
  f.set_texture(2,2,struct.pack('<4H',*[word]*4),format='ARGB4444');source=source565(word);alpha=((word>>12)+1)/16;channels=[(source>>s)&mask for s,mask in((11,31),(5,63),(0,31))]
  for background in(0,0x0821,0x7bef,0xffff):
   for light in(0,31):
    f.clear(color=background);expected=background;quad=[(x,y,z,light,light,light,a,u,w)for x,y,z,_,__,___,a,u,w in QUAD];pixels=[]
    for layer in range(8):
     raw,_=f.draw(quad,INDICES,z_enabled=True,z_write=False);got=struct.unpack_from('<H',raw,2*(16*32+16))[0];expected=blend565(word,expected);assert got==expected;pixels.append(hex(got))
    rows.append(dict(texel=hex(word),occurrences=count,background=hex(background),vertex_rgb5=light,source_contribution_rgb565=[int(c*alpha)for c in channels],float_contribution_rgb565=[c*alpha for c in channels],eight_layer_original=pixels))
 report=dict(status='pass',asset_sha256=ASSET_SHA,literal_texels=8,authored_pixels=sum(counts.values()),actual_draws=len(rows)*8,cases=rows,scope='All own eight literal ARGB4444 texels, actual Scene2 renderer, four RGB565 destinations, two ignored incoming RGB controls, eight ordered overlapping draws. Native contribution floor and destination dst-floor(dst*alpha) independently agree. Existing retail_alpha_source_quantize can reproduce the source contribution; float GPU destination lacks the original retained fractional remainder. No fitted tint or black key.');(out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],report['actual_draws']);return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);run(p.parse_args().output)
