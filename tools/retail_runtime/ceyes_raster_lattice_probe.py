"""Independent integer-packet reconstruction and wrong sample-centre control."""
import argparse,json,struct,zipfile,hashlib
from pathlib import Path
from PIL import Image
from speed_profile_contract import ROOT
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--packets',type=Path,required=True);p.add_argument('--reference',type=Path,required=True);p.add_argument('--output',type=Path,required=True);args=p.parse_args()
records=json.loads(args.packets.read_text())['records'];trace=json.loads((args.reference/'trace.json').read_text())
with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read('Imagery/Magic/ceyes.I3D')
u=lambda o:struct.unpack_from('<I',data,o)[0];rp=lambda o:o+u(o);t=rp(712+40);pixels=rp(rp(t+108));texture=struct.unpack_from('<1024H',data,pixels)
def render(tri,offset=0):
 im=Image.new('RGB',(640,480));written=set()
 for q in tri:
  a,b,c=q;den=(b[1]-c[1])*(a[0]-c[0])+(c[0]-b[0])*(a[1]-c[1])
  assert all(v[5:8]==a[5:8]for v in q)
  for y in range(min(v[1]for v in q),max(v[1]for v in q)):
   for x in range(min(v[0]for v in q),max(v[0]for v in q)):
    px,py=x+offset,y+offset;w0=((b[1]-c[1])*(px-c[0])+(c[0]-b[0])*(py-c[1]))/den;w1=((c[1]-a[1])*(px-c[0])+(a[0]-c[0])*(py-c[1]))/den;w2=1-w0-w1
    if min(w0,w1,w2)<-1e-9 or (x,y)in written:continue
    written.add((x,y));uv=[int(sum(w*v[k]for w,v in zip([w0,w1,w2],q))+1e-9)&31 for k in(3,4)]
    tex=texture[uv[1]*32+uv[0]];raw=[(tex>>11)&31,(tex>>6)&31,tex&31];m=[int(v*light/31)for v,light in zip(raw,a[5:8])];m=[(m[0]//2)*2,m[1]*2,(m[2]//2)*2];packed=(m[0]<<11)|(m[1]<<5)|m[2];R=(packed>>11)&31;G=(packed>>5)&63;B=packed&31;im.putpixel((x,y),((R<<3)|(R>>2),(G<<2)|(G>>4),(B<<3)|(B>>2)))
 return im
cases=[];args.output.mkdir(parents=True,exist_ok=True)
for image in sorted(args.reference.glob('native-*.png')):
 tick=int(image.stem.split('-')[-1]);frame=trace['records'][tick]['after']['frame'];tri=[a['integer']for a in records if a['frame']==frame][:2];original=Image.open(image).convert('RGB');predicted=render(tri);wrong=render(tri,.5)
 diff=lambda im:sum(a!=b for a,b in zip(original.getdata(),im.getdata()))
 cases.append(dict(capture=tick,frame=frame,integer_lattice_pixel_differences=diff(predicted),wrong_half_integer_pixel_differences=diff(wrong),native_image_sha256=sha(image)))
 predicted.save(args.output/f'integer-{tick:03}.png')
r=dict(status='pass_integer_center_controls'if all(c['integer_lattice_pixel_differences']==0 and c['wrong_half_integer_pixel_differences']==15 for c in cases if c['capture']in(59,79,179))and len(cases)==20 else'fail',cases=cases,packets_sha256=sha(args.packets),native_trace_sha256=sha(args.reference/'trace.json'),source_sha256=sha(Path(__file__)),scope='Independent affine interpolation of executed integer packets at integer pixel coordinates, literal texture and actual constant RGB5 normal lighting, exactly reproduces the three peak frames. Other phases retain 3-4 pixel fixed-point interpolation differences; no complete raster equivalence is claimed. GPU sample centres x+.5/y+.5 therefore need integer vertices shifted +.5 for the same interpolation coordinates. Wrong half-integer interpolation is an actual negative control; no per-effect registration or scale fit.',accepted=False)
(args.output/'proof.json').write_text(json.dumps(r,indent=2)+'\n');print(r['status'],[(c['capture'],c['integer_lattice_pixel_differences'],c['wrong_half_integer_pixel_differences'])for c in cases])
