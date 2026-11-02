"""Independent compiled Tornado pools, RNG, ordered poses and lifecycle controls."""
import argparse,json,struct,subprocess,hashlib,zipfile
from speed_controller_preflight import read_asset
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
DRIVER=r'''
#include "tornadopose.h"
#include <cstdio>
int main(){tornado_core::State s;unsigned seed=1,calls=0;auto rng=[&](int a,int b){++calls;seed=seed*214013u+2531011u;return a+int((seed>>16)&32767u)%(b-a+1);};s.Initialize(rng);hmm_mat4 owner;BuildRetailSoftwareOwner(owner,3185,3185,16,0);FILE* data=fopen("pools.bin","wb");FILE* poses=fopen("poses.txt","w");auto dump=[&]{printf("%d %u %d %d %d %u\n",s.frame,seed,s.flame_on,s.alive,s.damage_events,calls);fwrite(s.swirls.data(),sizeof(s.swirls),1,data);fwrite(s.ice.data(),sizeof(s.ice),1,data);fwrite(s.flames.data(),sizeof(s.flames),1,data);};dump();for(int t=1;t<=380;++t){s.Advance(rng);dump();if(s.alive)tornado_core::ForEachDraw(s,owner,0,[&](int obj,const hmm_mat4& m){fprintf(poses,"%d %d",t,obj);for(int c=0;c<4;++c)for(int r=0;r<4;++r)fprintf(poses," %.9g",m.Elements[c][r]);fputc(10,poses);});}fclose(data);fclose(poses);}
'''
def run(trace,out):
 out.mkdir(parents=True,exist_ok=False);native=json.loads(trace.read_text());(out/'driver.cpp').write_text(DRIVER)
 command=['clang++','-std=c++17','-O1','-ffp-contract=off','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(out/'driver.cpp'),str(ROOT/'src/engine/core/math3d.cpp'),'-o',str(out/'driver')]
 subprocess.run(command,check=True);p=subprocess.run([str(out/'driver')],cwd=out,capture_output=True,text=True);assert p.returncode==0,p.stderr;(out/'state.txt').write_text(p.stdout)
 states=[list(map(int,x.split()))for x in p.stdout.splitlines()];raw=(out/'pools.bin').read_bytes();records=[native['initial']]+[r['state']for r in native['records']];assert len(records)==len(states)==381
 checks=0;errors=[];maximum=0;activated=set();flames=set()
 for t,(n,s)in enumerate(zip(records,states)):
  expected=[n['stage'],n['seed'],n['flameon'],int(not n['flags']&0x1000),min(4,(min(t,360)+99)//100),n['rng_calls']]
  assert s==expected,(t,s,expected)
  b=raw[t*7320:(t+1)*7320]
  for name,start,count,stride,fmt in [('tornado',0,100,48,'11fi'),('ice',4800,30,68,'15f2i'),('flame',6840,15,32,'8f')]:
   source=b[start:];original=bytes.fromhex(n[name+'_hex'])
   for i in range(count):
    a=struct.unpack_from('<'+fmt,original,i*stride);z=struct.unpack_from('<'+fmt,source,i*stride)
    if name=='ice' and a[15]!=1:activated.add(i)
    if name=='flame' and a[6]!=0:flames.add(i)
    for j,(x,y)in enumerate(zip(a,z)):
     if name=='tornado' and t==0 and j<3:continue
     if name=='ice' and i%6 and ((i not in activated and j<15)or j==16):continue
     if name=='flame' and i not in flames and j<6:continue
     checks+=1;error=abs(x-y);maximum=max(maximum,error)
     if x!=y:errors.append([t,name,i,j,x,y])
 poses={}
 for line in(out/'poses.txt').read_text().splitlines():
  a=line.split();poses.setdefault(int(a[0]),[]).append((int(a[1]),list(map(float,a[2:]))))
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as archive:
  asset=read_asset(archive.read(next(n for n in archive.namelist()if n.lower()=='imagery/magic/tornado.i3d')))
 pose_fields=0;pose_error=0;vertex_bytes=0;index_count=0
 for r in native['records']:
  source=poses.get(r['tick'],[]);assert len(source)==len(r['draws']),(r['tick'],len(source),len(r['draws']))
  for(obj,m),d in zip(source,r['draws']):
   assert obj==d['object'];mesh=asset['objects'][obj];assert d['fvf']==0x112
   assert d['vertices_hex']==mesh['vertex_hex'];slot=[0,1,2,1][obj]+1;first,count=mesh['texture_runs'][slot*2:slot*2+2]
   assert bytes.fromhex(d['indices_hex'])==bytes.fromhex(asset['face_hex'])[first*6:(first+count)*6];vertex_bytes+=len(bytes.fromhex(d['vertices_hex']));index_count+=count*3
   error=max(abs(x-y)for x,y in zip(m,d['matrix']));pose_fields+=16;pose_error=max(pose_error,error);assert error<1e-4,(r['tick'],obj,error)
 negatives={}
 for name,driver in [('double_pulse',DRIVER.replace('s.Advance(rng);dump();','s.Advance(rng);s.Advance(rng);dump();')),('skipped_simulation',DRIVER.replace('s.Advance(rng);dump();','if(t%2)s.Advance(rng);dump();'))]:
  d=out/name;d.mkdir();(d/'driver.cpp').write_text(driver);cmd=command.copy();cmd[cmd.index(str(out/'driver.cpp'))]=str(d/'driver.cpp');cmd[-1]=str(d/'driver');subprocess.run(cmd,check=True);q=subprocess.run([str(d/'driver')],cwd=d,capture_output=True,text=True);assert q.returncode==0,q.stderr
  changed=sum(x!=y for x,y in zip(p.stdout.splitlines(),q.stdout.splitlines()));assert changed;negatives[name]=dict(rejected=True,changed_frames=changed)
 assert states[360][0]==360 and states[360][3]==0 and states[360:]==[states[360]]*21
 report=dict(status='pass'if not errors else'fail',accepted=False,visual_pass=False,defined_pool_fields=checks,maximum_state_error=maximum,errors=errors[:30],rng_and_counters_exact=True,pose_fields=pose_fields,maximum_pose_error=pose_error,authored_vertex_bytes=vertex_bytes,authored_indices=index_count,asset_sha256=asset['sha256'],kill_tick=360,post_kill_ticks_unchanged=20,negative_controls=negatives,native_manifest_sha256=sha(trace),source_hashes={str(p.relative_to(ROOT)):sha(p)for p in(ROOT/'src/game/effects/tornadocore.h',ROOT/'src/game/effects/tornadopose.h',Path(__file__))},scope='All380 owner/customRender submissions compared. Original uninitialized dormant fields excluded; matrices use decimal output tolerance.20 sampled full original raster frames; other lighting/raster submissions skipped under exact first10 controls. No successful damage claim; four genuine empty loaded-sector queries.')
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],checks,maximum,pose_fields,pose_error);assert not errors
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('trace',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.trace,a.output)
