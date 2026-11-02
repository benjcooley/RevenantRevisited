"""Independent compiled Pulp public-Set pool/RNG and ordered full-render proof.

The declared original empty map rejects all ten blood tiles, but each real
MakeIndex still does rand/srand(rand+clock)/rand. Clock0 is an explicit cached-VM
input, not a replacement lifetime or random draw count. No natural caller claim.
"""
import argparse,json,struct,subprocess,hashlib,zipfile
from pathlib import Path
from speed_controller_preflight import read_asset
ROOT=Path(__file__).resolve().parents[2]
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
DRIVER=r'''
#include "pulppose.h"
#include <cstdio>
int main(){pulp_core::State s;unsigned seed=1,calls=0,decals=0;auto rng=[&](int a,int b){++calls;seed=seed*214013u+2531011u;return a+int((seed>>16)&32767u)%(b-a+1);};int current_tick=0;FILE* queries=fopen("ground.txt","w");auto ground=[&](auto p){fprintf(queries,"%d %d %d %d\n",current_tick,p.x,p.y,p.z);return 0;};auto decal=[&](bool,int,auto){++decals;unsigned first=rng(0,32767);seed=first+CLOCK;rng(0,32767);};s.Initialize({0,0,8},{3185,3185,16},22,rng,[](int){return pulp_core::Vec{};},ground);FILE* data=fopen("pools.bin","wb");FILE* poses=fopen("poses.txt","w");for(int t=0;t<=FRAMES;++t){current_tick=t;if(t){s.Advance({3185,3185,16},rng,ground,decal);EXTRA}printf("%d %u %u %u\n",t,seed,calls,decals);fwrite(s.bodies.data(),sizeof(s.bodies),1,data);fwrite(s.blood.data(),sizeof(s.blood),1,data);pulp_core::ForEachDraw(s,[&](int obj,const hmm_mat4&m){fprintf(poses,"%d %d",t,obj);for(int c=0;c<4;++c)for(int r=0;r<4;++r)fprintf(poses," %.17g",m.Elements[c][r]);fputc(10,poses);});}fclose(data);fclose(poses);fclose(queries);}
'''
def run(trace,out):
 out.mkdir(parents=True,exist_ok=False);native=json.loads(trace.read_text());records=native['records']
 def compile_run(folder,driver):
  folder.mkdir(exist_ok=True);p=folder/'driver.cpp';p.write_text(driver.replace('FRAMES',str(len(records)-1)).replace('3185,3185,16',','.join(map(str,native.get('source',[3185,3185,16])))));exe=folder/'driver'
  subprocess.run(['clang++','-std=c++17','-O1','-ffp-contract=off','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(p),str(ROOT/'src/engine/core/math3d.cpp'),'-o',str(exe)],check=True)
  result=subprocess.run([str(exe)],cwd=folder,capture_output=True,text=True);assert result.returncode==0,result.stderr;(folder/'states.txt').write_text(result.stdout);return result.stdout,(folder/'pools.bin').read_bytes()
 driver=DRIVER.replace('CLOCK','0').replace('EXTRA','');text,raw=compile_run(out,driver);states=[list(map(int,x.split()))for x in text.splitlines()];assert len(states)==len(records)
 fields=0
 for row,actual in zip(records,states):
  t=row['tick'];n=row['state'];assert actual[:3]==[t,n['seed'],n['rng_calls']],(t,actual,n['seed'],n['rng_calls'])
  expected=bytes.fromhex(n['body_hex']+n['blood_hex']);source=raw[t*560:(t+1)*560];assert source==expected,(t,'pool bytes');fields+=140
 assert states[-1][3]==len(native['tile_requests'])==10 and all(x['result']==0xffffffff for x in native['tile_requests'])
 source_ground=[list(map(int,line.split()))for line in(out/'ground.txt').read_text().splitlines()]
 native_ground=[[r['tick'],*r['point']]for r in native['ground']]
 # Original452e10 consumes X/Y only. The decal caller leaves the unused
 # point.z stack word stale; record it rather than fabricating its contents.
 assert len(source_ground)==len(native_ground)
 assert [r[:3]for r in source_ground]==[r[:3]for r in native_ground]
 unused_z_differences=sum(a[3]!=b[3]for a,b in zip(source_ground,native_ground))
 assert all(a==b or row['caller']=='0x4e91f9' for a,b,row in zip(source_ground,native_ground,native['ground']))
 assert all(r['result']==0 for r in native['ground'])
 poses={}
 for line in(out/'poses.txt').read_text().splitlines():
  a=line.split();poses.setdefault(int(a[0]),[]).append((int(a[1]),list(map(float,a[2:]))))
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as archive:asset=read_asset(archive.read(next(n for n in archive.namelist()if n.lower()=='imagery/misc/pulp.i3d')))
 pose_fields=0;maximum=0;vertex_bytes=0;indices=0
 for row in records:
  source=poses[row['tick']];assert len(source)==len(row['draws']),(row['tick'],len(source),len(row['draws']))
  for(obj,m),d in zip(source,row['draws']):
   assert obj==d['object'];mesh=asset['objects'][obj];assert d['fvf']==0x112 and d['vertices_hex']==mesh['vertex_hex'];assert len([c for c in mesh['texture_runs'][1::2]if c])==1
   first,count=next((mesh['texture_runs'][i],mesh['texture_runs'][i+1])for i in range(0,len(mesh['texture_runs']),2)if mesh['texture_runs'][i+1]);assert bytes.fromhex(d['indices_hex'])==bytes.fromhex(asset['face_hex'])[first*6:(first+count)*6]
   error=max(abs(a-b)for a,b in zip(m,d['matrix']));assert error<2e-5,(row['tick'],obj,error);maximum=max(maximum,error);pose_fields+=16;vertex_bytes+=len(bytes.fromhex(d['vertices_hex']));indices+=count*3
 negatives={}
 for name,variant in [('wrong_clock1',DRIVER.replace('CLOCK','1').replace('EXTRA','')),('missing_intrinsic_reseed',driver.replace('seed=first+0;','')),('double_pulse',driver.replace('s.Advance({3185,3185,16},rng,ground,decal);','s.Advance({3185,3185,16},rng,ground,decal);s.Advance({3185,3185,16},rng,ground,decal);'))]:
  txt,pool=compile_run(out/name,variant);changed=sum(a!=b for a,b in zip(raw,pool));assert changed;negatives[name]={'rejected':True,'changed_pool_bytes':changed,'changed_state_rows':sum(a!=b for a,b in zip(text.splitlines(),txt.splitlines()))}
 report=dict(status='pass',accepted=False,visual_pass=False,exact_pool_fields=fields,exact_rng_rows=len(states),original_failed_tile_requests=10,exact_consumed_ground_queries=len(source_ground),unused_decal_query_z_differences=unused_z_differences,all_original_ground_results_zero=True,declared_clock_ms=0,ordered_pose_fields=pose_fields,maximum_pose_error=maximum,authored_vertex_bytes=vertex_bytes,authored_indices=indices,negative_controls=negatives,native_manifest_sha256=sha(trace),source_hashes={str(p.relative_to(ROOT)):sha(p)for p in(ROOT/'src/game/effects/pulpcore.h',ROOT/'src/game/effects/pulppose.h',Path(__file__))},scope=__doc__)
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('trace',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.trace,a.output)
