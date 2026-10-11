"""Bound every native-verified authored corner in the two fixed view windows."""
import argparse,json,struct,zipfile
from pathlib import Path
from speed_controller_preflight import ROOT,read_asset,sha
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--pose',type=Path,required=True);p.add_argument('--output',type=Path,required=True);args=p.parse_args()
with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read('Imagery/Misc/gvortex.I3D')
a=read_asset(data);bounds=[]
for line in args.pose.read_text().splitlines():
 q=line.split();state,object,frame=map(int,q[:3]);v=list(map(float,q[3:]))
 if min(v[6:9])<=1e-5:continue
 matrix=v[9:]
 for pt in struct.iter_unpack('<8f',bytes.fromhex(a['objects'][object]['vertex_hex'])):
  w=[sum(matrix[j*4+i]*pt[j]for j in range(3))+matrix[12+i]for i in range(3)]
  bounds.append((960+1.0101525783538818*(w[0]-w[1]),1500+.5050762891769409*(w[0]+w[1])-1.237179160118103*w[2]))
b=[min(x for x,y in bounds),min(y for x,y in bounds),max(x for x,y in bounds),max(y for x,y in bounds)]
assert len(bounds)==30080 and 0<b[0]<b[2]<1920 and 0<b[1]<b[3]<1780
r=dict(status='pass',asset_sha256=sha(data),pose_trace_sha256=sha(args.pose.read_bytes()),source_sha256=sha(Path(__file__).read_bytes()),projection_header_sha256=sha((ROOT/'src/retailsoftwaretransform.h').read_bytes()),corner_frames=len(bounds),combined_extent=b,combined_viewport=[1920,1780],views=[dict(viewport=[1920,1080],screen_center=[960,1500]),dict(viewport=[1920,1080],screen_center=[960,800])],integer_offset=700,overlap_height=380,scope='All source corners at every visible-scale authored object/frame. Independently native-verified local matrices and established original software projection coefficients; owner/camera world0. No transform or geometry fitted to images. Both fixed windows together contain the complete authored geometry.')
args.output.mkdir(parents=True,exist_ok=True);(args.output/'proof.json').write_text(json.dumps(r,indent=2)+'\n');print(r['status'],len(bounds),b)
