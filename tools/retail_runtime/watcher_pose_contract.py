"""Compare actual fullRender streams to compiled production watcher native submission."""
import argparse,json,subprocess,zipfile,hashlib
from pathlib import Path
from static_mesh_probe import ROOT,build_port
from static_texture_probe import parse_frames

def run(draws_file,output):
 output.mkdir(parents=True,exist_ok=False);profiles=json.loads((ROOT/'tools/retail_runtime/static_additional_profiles.json').read_text())['profiles'][1:];metadata={}
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:
  names={n.lower():n for n in z.namelist()}
  for p in profiles:metadata[p['name']]=parse_frames(z.read(names['imagery/'+p['asset'].replace('\\','/').lower()]),p)
 common,compiled=build_port(output,metadata,frames=(0,1,2,3),texture_frames=True)
 generated=(output/'static-port-components.cpp').read_text();effects=(ROOT/'src/effect.cpp').read_text();method=effects[effects.index('void TAuthoredStaticMeshEffect::SubmitNativeWorldMeshForTest_BESPOKE('):effects.index('void TAuthoredStaticMeshEffect::SubmitWorldMeshForTest_BESPOKE(')]
 generated=generated.replace('#include "math3d.h"','#include "math3d.h"\n#include "retailsoftwaretransform.h"\n#include <cassert>')
 generated=generated.replace('struct TRenderer {SMeshSubmit last;', 'struct SHelperMeshSubmit:SMeshSubmit {bool retail_software_projection=false,retail_source_alpha=false;};\nstruct TRenderer {SMeshSubmit last;void SubmitHelperMesh(const SHelperMeshSubmit& s){assert(s.retail_software_projection&&s.retail_source_alpha&&s.retail_lighting==2);last=s;}')
 generated=generated.replace('void SubmitWorldMeshForTest_BESPOKE(EFxDebugMode);', 'void SubmitWorldMeshForTest_BESPOKE(EFxDebugMode);void SubmitNativeWorldMeshForTest_BESPOKE(EFxDebugMode);S3DPoint Pos()const{return {int(root.m.Elements[3][0]),int(root.m.Elements[3][1]),int(root.m.Elements[3][2])};}int GetFace()const{return 0;}')
 generated=generated.replace('int main(int argc,char**argv){',method+'\nint main(int argc,char**argv){').replace('effect.SubmitWorldMeshForTest_BESPOKE(EFxDebugMode::Normal);','effect.parts_[0].retail_lighting=2;effect.SubmitNativeWorldMeshForTest_BESPOKE(EFxDebugMode::Normal);')
 cpp=output/'native-port.cpp';cpp.write_text(generated);binary=output/'native-port';command=['clang++','-std=c++17','-fsanitize=address,undefined','-I',str(ROOT/'thirdparty/handmademath'),'-iquote',str(ROOT/'src'),str(cpp),str(ROOT/'src/math3d.cpp'),'-o',str(binary)];subprocess.run(command,check=True)
 packets={}
 for p in profiles:
  name=p['name'];trace=subprocess.check_output([str(binary),str(output/(name+'.mesh-input'))],text=True);(output/(name+'-native-port.txt')).write_text(trace);cases={}
  for line in trace.splitlines():
   v=line.split()
   if v[0]=='F':cases[(int(v[1]),int(v[2]))]={'frame_handle':int(v[3]),'vertices':[]}
   elif v[0]=='M':cases[(int(v[1]),int(v[2]))]['matrix']=list(map(float,v[3:]))
   elif v[0]=='V':cases[(int(v[1]),int(v[2]))]['vertices'].append(list(map(float,v[4:])))
  packets[name]=cases
 draws=json.loads(draws_file.read_text());seen={};errors=[];checks=0;maximum=0
 for d in draws:
  name=d['name'];seen[name]=seen.get(name,0)+1;frame=seen[name]%4;packet=packets[name][(frame,0)];m=d['transforms']['1'];meta=metadata[name]
  assert d['args'][1]==0x112 and d['indices']==meta['indices'] and packet['frame_handle']==1000+frame
  assert d['states']=={'22':1,'14':0,'7':1,'19':5,'20':6,'21':2}
  actual=[m[c*4+r]for r in range(4)for c in range(4)]
  comparisons=list(zip(actual,packet['matrix']))
  for raw,authored,submitted in zip(d['vertices'],meta['vertices'],packet['vertices']):
   comparisons.extend(zip(raw,authored))
   transformed=[sum(m[c*4+r]*raw[c]for c in range(3))+m[12+r]for r in range(3)]+raw[3:]
   comparisons.extend(zip(transformed,submitted))
  for a,b in comparisons:
   error=abs(a-b);maximum=max(maximum,error);checks+=1
   if error>3e-5:errors.append((name,frame,a,b))
 assert seen=={'ogrokwatcher1':8,'ogrokwatcher2':8,'ogrokwatcher3':8}and not errors,errors[:10]
 report=dict(status='pass',draws=len(draws),float_checks=checks,maximum_error=maximum,states={'cull':1,'zwrite':0,'ztest':1,'source_blend':5,'destination_blend':6},draws_sha256=hashlib.sha256(draws_file.read_bytes()).hexdigest(),native_submit_sha256=hashlib.sha256(method.encode()).hexdigest(),compiled_common=compiled,scope='Actual loaded fullRender FVF112 streams/world matrix/states vs compiled production keys/extraction/native-helper submission at explicit origin0/facing0, eight ticks. Both texture conversion variants are qualified device references; no device/exactpixel/natural owner Pulse claim.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report));return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--draws',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.draws,a.output)
