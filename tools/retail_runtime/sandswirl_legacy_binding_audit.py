"""Audit supplied legacy Sand provenance without copying licensed data to source.

The installed/CD class registry omits Sand. This proves a restoration candidate,
not that the shipped configuration enables the child or a natural spell caller.
"""
import argparse,hashlib,json,re,struct,subprocess,zipfile
from pathlib import Path
from speed_controller_preflight import read_asset

def sha(data):return hashlib.sha256(data).hexdigest()
def run(legacy,archive,output,cd_archive=None):
 output.mkdir(parents=True,exist_ok=False)
 definition=re.compile(r'^\s*"Sand"\s+"Magic\\Sandswirl\.i3d"\s+0xfecaface\s*$',re.I)
 bindings=[]
 for name in ('Class.Def','old.old.Class.Def','cbClass.Def','jared.def','class backup.def'):
  data=(legacy/name).read_bytes();matches=[dict(line=i,text=line.strip())for i,line in enumerate(data.decode('latin1').splitlines(),1)if definition.fullmatch(line)]
  assert len(matches)==1,(name,matches);bindings.append(dict(path=name,sha256=sha(data),matches=matches))
 registrations=[]
 for name,active in (('walkcode/effect.cpp',True),('effect.cpp',False)):
  data=(legacy/name).read_bytes();matches=[dict(line=i,text=line.strip())for i,line in enumerate(data.decode('latin1').splitlines(),1)if 'REGISTER_MULTI_3DANIMATOR_("sand", TSandAnimator, TParticle3DAnimator)'in line]
  assert len(matches)==1
  uncommented=re.sub(r'/\*.*?\*/|//[^\r\n]*','',data.decode('latin1'),flags=re.S)
  assert ('REGISTER_MULTI_3DANIMATOR_("sand", TSandAnimator, TParticle3DAnimator)'in uncommented)==active
  registrations.append(dict(path=name,sha256=sha(data),active=active,matches=matches))
 own=(legacy/'Imagery/Magic/sandswirl.I3D').read_bytes()
 assert sha(own)=='4ffcc6ab4e0f519bbfbfebc4713691eb92d14b792f27222130c35ce5502af85f'
 archives=[]
 for path in [archive]+([cd_archive]if cd_archive else []):
  with zipfile.ZipFile(path)as z:
   assert z.read('Imagery/Magic/sandswirl.i3d')==own
   definition_text=z.read('class.def');assert not any(re.match(r'^\s*"sand"\s',line,re.I)for line in definition_text.decode('latin1').splitlines())
   archives.append(dict(path=str(path),sha256=sha(path.read_bytes()),members=len(z.namelist()),class_sha256=sha(definition_text),sand_class_absent=True,own_asset_identical=True))
 asset=read_asset(own);assert (len(asset['objects']),asset['vertices'],asset['faces'],asset['textures'],asset['states'],asset['state0_frames'])==(1,4,2,1,1,1)
 assert asset['objects'][0]['name']=='photon01' and not asset['tags']
 u=lambda p:struct.unpack_from('<I',own,p)[0];r=lambda p:p+u(p);t=r(asset['body_offset']+40);height,width=struct.unpack_from('<2I',own,t+8);pf=struct.unpack_from('<8I',own,t+72)
 assert (width,height,pf)==(64,64,(32,65,0,16,0xf00,0xf0,0xf,0xf000))
 def fnv(data):
  result=2166136261
  for b in data:result=((result^b)*16777619)&0xffffffff
  return result
 profile=dict(asset_sha256=sha(own),bytes=len(own),object='photon01',objects=1,vertices=4,triangles=2,states=1,frames=1,texture_size=[width,height],pixel_format=list(pf),vertex_fnv32=hex(fnv(bytes.fromhex(asset['objects'][0]['vertex_hex']))),indices_fnv32=hex(fnv(bytes.fromhex(asset['face_hex']))),texture_sha256=sha(own[r(r(t+108)):r(r(t+108))+width*height*2]))
 report=dict(status='pass',accepted=False,visual_pass=False,legacy_head=subprocess.check_output(['git','rev-parse','HEAD'],cwd=legacy,text=True).strip(),legacy_status=subprocess.check_output(['git','status','--short'],cwd=legacy,text=True),class_bindings=bindings,animator_registrations=registrations,archives=archives,profile=profile,scope=__doc__,limits='Supplied legacy backup provenance is distinct from shipped enabled behavior. Restoring the exact class entry and particle builder is an explicit modified-retail configuration. No Sand filename or Sparks alias is invented; the legacy definition names the existing Sandswirl asset. Natural Sandswirl spell variant and stack-derived seekz remain separate limits.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2));return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--legacy',type=Path,required=True);p.add_argument('--archive',type=Path,required=True);p.add_argument('--cd-archive',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.legacy,a.archive,a.output,a.cd_archive)
