"""Literal original Fire cardinal UV/ARGB4444 sampling, no raster replacement."""
import argparse,json,struct,zipfile
from pathlib import Path
from lit_software_probe import LitSoftwareFixture
from speed_profile_contract import ROOT,sha
from unicorn import UC_HOOK_MEM_READ

def run(trace,output):
 output.mkdir(parents=True,exist_ok=False);n=json.loads(trace.read_text())
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(x for x in z.namelist()if x.lower()=='imagery/misc/fire.i3d'))
 assert sha(data)=='70574ac537f9ec6f6a39df1adb449e969afa15691a9190c5b4450114d0ae3fee';u=lambda p:struct.unpack_from('<I',data,p)[0];r=lambda p:p+u(p);body=20+u(16);offset=r(r(body+1784));tex=data[offset:offset+8192]
 f=LitSoftwareFixture(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',1280,960,camera=(3185,3185,0));f.lighting((38,38,38),(1,1,1));f.set_texture(64,64,tex,format='ARGB4444');reads=[]
 def observe(uc,access,address,size,value,*_):
  if f.texture.address<=address<f.texture.address+8192:
   raw=bytes(uc.mem_read(address,size));reads.append(dict(offset=address-f.texture.address,size=size,hex=raw.hex()))
 f.vm.uc.hook_add(UC_HOOK_MEM_READ,observe,begin=f.texture.address,end=f.texture.address+8191);cases=[]
 for face in[0,64,128,192]:
  row=next(x for x in n['records']if x['facing']==face);d=row['draws'][0];f.clear();reads.clear()
  for k,v in d['states'].items():f.vm.call(0x56d400,(int(k),v))
  vertices=list(struct.iter_unpack('<8f',bytes.fromhex(d['vertices_hex'])));indices=list(struct.unpack('<6H',bytes.fromhex(d['indices_hex'])));pixels,depth,transformed=f.draw_authored(vertices,indices,d['matrix'],cull=1)
  nonzero=[dict(x=i%1280,y=i//1280,rgb565=v[0])for i,v in enumerate(struct.iter_unpack('<H',pixels))if v[0]];assert nonzero==row['nonzero']
  unique=sorted(set(x['offset']for x in reads));nonzero_tex=[dict(offset=x,xy=[(x//2)%64,x//128],word=hex(struct.unpack_from('<H',tex,x)[0]))for x in unique if struct.unpack_from('<H',tex,x)[0]]
  cases.append(dict(facing=face,actual_pixels=nonzero,reads=list(reads),nonzero_sampled_texels=nonzero_tex,transformed=transformed,integer_vertices=[[int(q[0]),int(-q[1]),int(q[2]),int(q[7]*64),int(q[8]*64)]for q in transformed],literal_source_contribution=[dict(word=x['word'],alpha=(int(x['word'],16)>>12)+1,source565=hex(((int(x['word'],16)>>8&15)<<12)|((int(x['word'],16)>>4&15)<<7)|((int(x['word'],16)&15)<<1)))for x in nonzero_tex]))
 report=dict(status='pass',accepted=False,visual_pass=False,asset_sha256=sha(data),trace_sha256=sha(trace.read_bytes()),probe_sha256=sha(Path(__file__).read_bytes()),cases=cases,scope='Original full software transform/illumination/truncation/UV interpolation/ARGB4444 kernel replay, exact four cardinal packets and literal first texture. Read-only texel observations reproduce every original pixel. No UV/geometry/light/clock or renderer fitting; sourceGPU sampler/raster difference remains independent.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print([(x['facing'],x['nonzero_sampled_texels'],x['actual_pixels'])for x in cases]);return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--trace',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.trace,a.output)
