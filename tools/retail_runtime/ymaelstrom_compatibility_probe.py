"""Establish exact shared Maelstrom control layout without aliasing Y geometry."""
import argparse,hashlib,json,struct,zipfile
from pathlib import Path
from speed_controller_preflight import read_asset
from speed_profile_contract import ROOT
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,required=True);a=p.parse_args()
with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:raw=[z.read('Imagery/Magic/'+name+'.I3D')for name in ('maelstrom','ymaelstrom')]
base,y=map(read_asset,raw)
for k in ('vertices','faces','materials','textures','states','state0_frames','tags','face_hex','state_header_hex'):assert base[k]==y[k],k
changed=[0]*8;maximum=[0.0]*8
for b,v in zip(base['objects'],y['objects']):
 for key in b:
  if key!='vertex_hex':assert b[key]==v[key],(b['index'],key)
 for original,current in zip(struct.iter_unpack('<8f',bytes.fromhex(b['vertex_hex'])),struct.iter_unpack('<8f',bytes.fromhex(v['vertex_hex']))):
  for i,(x,z)in enumerate(zip(original,current)):changed[i]+=x!=z;maximum[i]=max(maximum[i],abs(x-z))
assert changed[:6]==[663,709,496,410,408,173]and changed[6:]==[0,0]
r=dict(status='pass',base_asset_sha256=base['sha256'],y_asset_sha256=y['sha256'],shared=['245-END state/header','51 names/material slots','all inclusive keys/parents','faces/texture-major order','five tags/three scroll selectors and rates','UV fields'],independent=['vertex XYZ and normals','actual texture pixels','strict asset profile/type ID'],changed_vertex_channels=changed,max_vertex_channel_difference=maximum,scope='Only control layout compatibility. Y geometry and textures are retained and require their own original full Render and actual Metal appearance.')
a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(r,indent=2)+'\n');print('pass',changed,maximum)
