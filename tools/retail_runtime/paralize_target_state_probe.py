"""Independent compiled target-bound state against real original actor owner."""
import argparse,json,math,struct,subprocess,zipfile
from pathlib import Path
from speed_profile_contract import ROOT,sha
from speed_controller_preflight import read_asset
DRIVER=r'''
#include "paralizecore.h"
#include <cstdio>
int main(){paralize_core::State state;float uv=0;unsigned seed=1;auto rng=[&](int a,int b){seed=seed*214013u+2531011u;return a+int((seed>>16)&32767u)%(b-a+1);};
for(int t=0;t<280;++t){state.TickTarget(rng,{3185,3185,11},{3281,3185,11},16,1);unsigned alive=state.alive;fwrite(&seed,4,1,stdout);fwrite(&alive,4,1,stdout);fwrite(&state,0x3c4,1,stdout);if(state.alive)uv-=state.uv;paralize_core::ForEachDraw(state,uv,[&](const paralize_core::Draw& d){fprintf(stderr,"D %d %d %.9g %.9g %.9g %.9g %.9g\n",t+1,d.object,d.position.x,d.position.y,d.position.z,d.scale,d.uv);});}}
'''
def run(trace,out):
 out.mkdir(parents=True,exist_ok=True);src=out/'state.cpp';src.write_text(DRIVER);exe=out/'state'
 p=subprocess.run(['clang++','-std=c++17','-O1','-ffp-contract=off','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),str(src),'-o',str(exe)],capture_output=True,text=True);assert p.returncode==0,p.stderr
 p=subprocess.run([str(exe)],capture_output=True);assert p.returncode==0,p.stderr;(out/'source.bin').write_bytes(p.stdout);(out/'source.draws').write_bytes(p.stderr)
 native=json.loads(trace.read_text());assert len(native['records'])==280;source_draws={}
 for line in p.stderr.decode().splitlines():
  row=line.split()
  if row and row[0]=='D':source_draws.setdefault(int(row[1]),[]).append([int(row[2])]+list(map(float,row[3:])))
 with zipfile.ZipFile(ROOT/'data/imagery.rvi') as archive:
  asset_bytes=archive.read(next(n for n in archive.namelist() if n.lower()=='imagery/magic/paralize.i3d'))
 asset=read_asset(asset_bytes)
 pose_checks=0;pose_max=0;uv_max=0;vertex_checks=0;index_checks=0
 errors=[];maximum=0;checks=0;floats=[(0x184,0x3ac),(0x448,0x508)]
 for i,row in enumerate(native['records']):
  ref=row['state'];off=i*(0x3c4+8);seed,alive=struct.unpack_from('<II',p.stdout,off)
  assert seed==ref['seed'],(i,seed,ref['seed']);assert alive==int(not(ref['flags']&0x1000)),i
  expected=bytearray.fromhex(ref['owner_hex']);struct.pack_into('<I',expected,0x3b0-0x184,1)
  current=p.stdout[off+8:off+8+0x3c4]
  draws=source_draws.get(i+1,[]);assert len(draws)==row['draw_count'],(i,len(draws),row['draw_count'])
  if 'draws'in row:
   for d,n in zip(draws,row['draws']):
    obj,x,y,z,scale,uv=d;assert obj==n['object']
    matrix=[scale,0,0,0,0,scale,0,0,0,0,scale,0,x,y,z,1]
    error=max(abs(a-b)for a,b in zip(matrix,n['matrix']));pose_max=max(pose_max,error);pose_checks+=16
    if error>.003:errors.append(dict(category='float_accumulation',tick=i+1,draw_object=obj,matrix_error=error))
    mesh=asset['objects'][obj];original=bytes.fromhex(mesh['vertex_hex']);packet=bytes.fromhex(n['vertices_hex'])
    assert n['vertex_count']==mesh['vertex_count']
    run=mesh['texture_runs'];start,count=run[2*(obj+1):2*(obj+1)+2]
    expected_indices=bytes.fromhex(asset['face_hex'])[start*6:(start+count)*6]
    assert bytes.fromhex(n['indices_hex'])==expected_indices,(i,obj,'indices')
    index_checks+=count*3
    for k in range(n['vertex_count']):
     old=struct.unpack_from('<8f',original,k*32);new=struct.unpack_from('<8f',packet,k*32)
     assert old[:3]==new[:3],(i,obj,k,'vertexXYZ')
     vertex_checks+=3
     if obj==1:
      assert struct.unpack_from('<I',packet,k*32+16)[0]==0xfffffff8
      uv_error=max(abs(new[j]-(old[j]+uv))for j in(6,7));uv_max=max(uv_max,uv_error)
      if uv_error>.001:errors.append(dict(tick=i+1,vertex=k,uv_error=uv_error))
  for j in range(0,0x3c4,4):
   address=j+0x184;floating=any(lo<=address<hi for lo,hi in floats);fmt='<f'if floating else'<I';a=struct.unpack_from(fmt,expected,j)[0];b=struct.unpack_from(fmt,current,j)[0];checks+=1
   error=abs(a-b)
   if floating:maximum=max(maximum,error);okay=error<=max(.003,abs(a)*2e-6)
   else:okay=a==b
   if not okay:errors.append(dict(category='float_accumulation'if floating else'integer_mismatch',tick=i+1,field=hex(address),native=a,source=b,error=error))
 # Compile deliberately wrong target/render variants against the same original
 # records. These reject the stale disabled-source egg offsets and a missing
 # target-center offset; neither control changes the native trace.
 negatives={}
 for label,old,new in [
  ('stale_egg_offsets','float(t[0]),float(t[1]),float(t[2]+40)','float(t[0]-10),float(t[1]-20),float(t[2]+20)'),
  ('missing_target_center','target[0]+20,target[1]+20,target[2]','target[0],target[1],target[2]')]:
  directory=out/label;directory.mkdir(exist_ok=True)
  header=(ROOT/'src/paralizecore.h').read_text();assert old in header
  (directory/'paralizecore.h').write_text(header.replace(old,new))
  source=directory/'state.cpp';source.write_text(DRIVER);binary=directory/'state'
  result=subprocess.run(['clang++','-std=c++17','-O1','-ffp-contract=off',str(source),'-o',str(binary)],capture_output=True);assert result.returncode==0,result.stderr
  result=subprocess.run([str(binary)],capture_output=True);assert result.returncode==0,result.stderr
  mismatches=sum(a!=b for a,b in zip(p.stdout,result.stdout))
  draw_changes=sum(a!=b for a,b in zip(p.stderr.splitlines(),result.stderr.splitlines()))
  assert mismatches or draw_changes,label
  negatives[label]=dict(rejected=True,state_bytes=mismatches,draw_packets=draw_changes)
 status='pass' if not errors else ('bounded_float_residual' if all(e.get('category')=='float_accumulation'for e in errors) else 'fail')
 report=dict(status=status,exact_integer_rng_and_draw_count=True,diagnostic_float_tolerance='.003 absolute or2e-6 relative, retained without widening; accumulated exceptions remain explicit',negative_controls=negatives,fields=checks,pose_fields=pose_checks,maximum_pose_error=pose_max,authored_vertex_fields=vertex_checks,authored_indices=index_checks,maximum_accumulated_uv_error=uv_max,asset_sha256=sha(asset_bytes),error_count=len(errors),errors=errors[:100],maximum_float_error=maximum,native_trace_sha256=sha(trace.read_bytes()),source_sha256=sha((ROOT/'src/paralizecore.h').read_bytes()),probe_sha256=sha(Path(__file__).read_bytes()),target_identity_normalization='genuine original pointer to token1; no simulated field substitution',sanitizers='ASan/UBSan',accepted=False)
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],len(errors),maximum);return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('trace',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.trace,a.output)
