"""Observe literal Fire texture reads in its original tiny default draw.

The loaded trace supplies actual geometry/matrix/state; source texture bytes are
immutable. This explains blank default raster output, not a visible-effect pass.
"""
import argparse,json,struct,zipfile,hashlib
from pathlib import Path
from lit_software_probe import LitSoftwareFixture
from speed_controller_preflight import ROOT
from software_probe import save_rgb565_png
from unicorn import UC_HOOK_MEM_READ
parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--trace',type=Path,required=True);parser.add_argument('--output',type=Path,required=True);args=parser.parse_args()
B=args.output;B.mkdir(parents=True,exist_ok=False);trace=json.loads(args.trace.read_text());d=trace['records'][0]['draws'][0]
with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/misc/fire.i3d'))
assert hashlib.sha256(data).hexdigest()=='70574ac537f9ec6f6a39df1adb449e969afa15691a9190c5b4450114d0ae3fee'
u=lambda p:struct.unpack_from('<I',data,p)[0];r=lambda p:p+u(p);body=20+u(16);height,width=struct.unpack_from('<2I',data,body+928);assert (width,height)==(64,64);offset=r(r(body+1784));raw=data[offset:offset+8192];f=LitSoftwareFixture(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',1280,960,camera=(3185,3185,0));f.lighting((38,38,38),(1,1,1));f.set_texture(64,64,raw,format='ARGB4444');f.clear();reads=[]
def observe(uc,access,address,size,value,*_):
 if f.texture.address<=address<f.texture.address+8192:reads.append(dict(offset=address-f.texture.address,size=size,data=bytes(uc.mem_read(address,size)).hex()))
f.vm.uc.hook_add(UC_HOOK_MEM_READ,observe,begin=f.texture.address,end=f.texture.address+8191)
for k,v in d['states'].items():f.vm.call(0x56d400,(int(k),v))
v=list(struct.iter_unpack('<8f',bytes.fromhex(d['vertices_hex'])));idx=list(struct.unpack('<6H',bytes.fromhex(d['indices_hex'])));pixels,depth,t=f.draw_authored(v,idx,d['matrix'],cull=1);save_rgb565_png(B/'first-replay.png',pixels,1280,960);r=dict(status='diagnostic',accepted=False,texel_reads=reads,unique_offsets=sorted(set(x['offset']for x in reads)),nonzero_pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',pixels)),transformed=t,asset_sha256=hashlib.sha256(data).hexdigest(),trace_sha256=hashlib.sha256(args.trace.read_bytes()).hexdigest(),probe_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),scope='Original observed Fire firstquad replay withliteralfirstARGB4444 texture/states/matrix. Read-onlytexelmemoryobservation; no forcedanimation/scale/brightness or naturalcallerclaim.');(B/'first-replay.json').write_text(json.dumps(r,indent=2)+'\n');print(len(reads),r['nonzero_pixels'],sorted(set(x['data']for x in reads)))
