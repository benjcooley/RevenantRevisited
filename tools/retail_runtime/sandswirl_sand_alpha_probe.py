"""Own Sand ARGB4444 literal source-term controls in the original Scene2 kernel."""
import argparse,collections,json,struct,zipfile
from pathlib import Path
from lit_software_probe import LitSoftwareFixture
from test_argb4444 import QUAD,INDICES,blend565,source565
from speed_controller_preflight import ROOT,sha

def run(executable,archive,output):
 output.mkdir(parents=True,exist_ok=False)
 with zipfile.ZipFile(archive)as z:data=z.read('Imagery/Magic/sandswirl.i3d')
 assert sha(data)=='4ffcc6ab4e0f519bbfbfebc4713691eb92d14b792f27222130c35ce5502af85f'
 u=lambda p:struct.unpack_from('<I',data,p)[0];r=lambda p:p+u(p);body=20+u(16);tex=r(body+40);pixels=r(r(tex+108));counts=collections.Counter(x[0]for x in struct.iter_unpack('<H',data[pixels:pixels+8192]))
 f=LitSoftwareFixture(executable,32,32);v=f.vm
 for a,x in((0x5d7a28,1),(0x66818c,0),(0x5e8790,0)):v.put_u32(a,x)
 v.call(0x417d60,(2,1),this=0x65a57c);rows=[]
 for word in sorted(set(counts)|{0x0fff,0xffff,0x0000,0x0204}):
  f.set_texture(2,2,struct.pack('<4H',*[word]*4),format='ARGB4444');actual=[]
  for light in(0,31):
   quad=[(x,y,z,light,light,light,a,u,w)for x,y,z,_,__,___,a,u,w in QUAD];f.clear();rgb,_=f.draw(quad,INDICES,z_enabled=True,z_write=False)
   pixel=struct.unpack_from('<H',rgb,2*(16*32+16))[0];assert pixel==blend565(word,0),(hex(word),hex(pixel));actual.append(pixel)
  source=source565(word);alpha=(1+(word>>12))/16;channels=[(source>>s)&mask for s,mask in((11,31),(5,63),(0,31))]
  exact=[int(x*alpha)for x in channels];got=[(actual[0]>>s)&mask for s,mask in((11,31),(5,63),(0,31))];assert got==exact
  rows.append(dict(texel=hex(word),authored_count=counts[word],native=hex(actual[0]),incoming_color_ignored=actual[0]==actual[1],source_channels=channels,alpha=alpha,float_source_contribution=[x*alpha for x in channels],quantized_source_contribution=exact))
 assert sum(r['authored_count']for r in rows)==4096
 assert next(r for r in rows if r['texel']=='0xfff')['native']!='0x0'
 report=dict(status='pass',accepted=False,visual_pass=False,cases=len(rows)*2,unique_authored_texels=len(counts),authored_texels=4096,asset_sha256=sha(data),probe_sha256=sha(Path(__file__).read_bytes()),rows=rows,scope=__doc__,limits='Original Scene2 ARGB4444 contribution on black only. Every own texture word tested with incoming RGB5 zero/31; no artificial color or alpha threshold. Existing helper retail_alpha_source_quantize selects these literal source-channel floors. GPU destination rounding, affine interpolation and sampling remain open.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report['cases'],'literal controls',len(counts),'own texels');return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--executable',type=Path,default=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe');p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.executable,a.archive,a.output)
