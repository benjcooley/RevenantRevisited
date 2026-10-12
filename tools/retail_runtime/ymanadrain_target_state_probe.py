"""Compile the actual YManaDrain core/pose against original actor/render records."""
import argparse,hashlib,json,struct,subprocess,zipfile
from speed_controller_preflight import read_asset
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
DRIVER=r"""
#include "manadrainpose.h"
#include "ymanadraincore.h"
#include <algorithm>
#include <cstdio>
int main(){using namespace manadrain_core;ymanadrain_core::State s;unsigned seed=1;auto rng=[&](int a,int b){seed=seed*214013u+2531011u;return a+int((seed>>16)&32767u)%(b-a+1);};auto dist=[](Point a,Point b){int x=std::abs(a.x-b.x),y=std::abs(a.y-b.y),shift=0;while(x>255||y>255){x>>=1;y>>=1;++shift;}return std::min(255,int(std::floor(std::sqrt(double(x)*x+double(y)*y)+.5)))<<shift;};int mana=252,caster=252;
for(int t=1;t<=100;++t){s.Advance({3185,3185,11},{3281,3185,11},64,160,260,rng,dist,[&](int amount){amount=ymanadrain_core::TransferAmount(caster,252,mana,amount);caster+=amount;mana-=amount;return amount;},[]{});float glow=s.alive?s.RenderGlowScale(rng):0;ForEachDraw(s,s.facing,glow,[&](int object,const hmm_mat4& matrix,bool){fprintf(stderr,"%d %d",t,object);for(int c=0;c<4;++c)for(int r=0;r<4;++r)fprintf(stderr," %.9g",matrix.Elements[c][r]);fputc(10,stderr);});printf("%d %u %d %d %d %d %.9g %.9g %.9g",t,seed,int(s.alive),s.stage,s.ticks,mana,s.rotation,s.scale,glow);for(int i=0;i<15;++i)printf(" %d %d %d %d %.9g %.9g %.9g %.9g",s.delay[i],s.position[i].x,s.position[i].y,s.position[i].z,s.velocity[i].x,s.velocity[i].y,s.velocity[i].z,s.stream_scale[i]);printf(" %d\n",caster);}}
"""
def run(trace,out):
 out.mkdir(parents=True,exist_ok=False)
 source=out/'state.cpp';source.write_text(DRIVER);binary=out/'state'
 command=['clang++','-std=c++17','-O1','-ffp-contract=off','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(source),str(ROOT/'src/math3d.cpp'),'-o',str(binary)]
 result=subprocess.run(command,capture_output=True,text=True);assert result.returncode==0,result.stderr
 result=subprocess.run([str(binary)],capture_output=True,text=True);assert result.returncode==0,result.stderr
 (out/'state.txt').write_text(result.stdout);(out/'poses.txt').write_text(result.stderr)
 native=json.loads(trace.read_text());source_rows=[list(map(float,line.split()))for line in result.stdout.splitlines()];poses={}
 for line in result.stderr.splitlines():
  a=line.split();poses.setdefault(int(a[0]),[]).append((int(a[1]),list(map(float,a[2:]))))
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as archive:
  asset=read_asset(archive.read(next(n for n in archive.namelist()if n.lower()=='imagery/magic/ymanadrain.i3d')))
 errors=[];checks=0;maximum=0;pose_checks=0;pose_max=0;vertex_bytes=0;index_count=0
 for row,p in zip(native['records'],source_rows):
  t=row['tick'];r=row['state'];a=bytes.fromhex(r['animator_hex']);owner=bytes.fromhex(r['owner_hex'])
  values=[r['seed'],int(not r['flags']&0x1000),struct.unpack_from('<i',owner,16)[0],struct.unpack_from('<i',owner,20)[0],r['victim_mana'],struct.unpack_from('<f',a,0x20)[0],struct.unpack_from('<f',a,0x24)[0]]
  for j,(x,y)in enumerate(zip(values,p[1:8])):
   error=abs(x-y);maximum=max(maximum,error);checks+=1
   if error>1e-5:errors.append([t,'base',j,x,y])
  assert r['caster_mana']==(252 if t<21 else 453)
  checks+=1
  if r['caster_mana']!=p[-1]:errors.append([t,'caster_mana',r['caster_mana'],p[-1]])
  for i in range(15):
   values=[struct.unpack_from('<i',a,0x28+4*i)[0]]+list(struct.unpack_from('<3i',a,0x64+12*i))+list(struct.unpack_from('<3f',a,0x118+12*i))+[struct.unpack_from('<f',a,0x1cc+4*i)[0]]
   for j,(x,y)in enumerate(zip(values,p[9+8*i:17+8*i])):
    # Uninitialized dormant slots are not source state. Each slot becomes
    # defined at its original first activation,2+3*index.
    if t<2+3*i and j>0:continue
    error=abs(x-y);maximum=max(maximum,error);checks+=1
    if error>max(1e-5,abs(x)*1e-7):errors.append([t,i,j,x,y])
  if 'draws'in row:
   submitted=poses.get(t,[]);assert len(submitted)==len(row['draws']),(t,len(submitted),len(row['draws']))
   for(obj,matrix),draw in zip(submitted,row['draws']):
    assert obj==draw['object']
    mesh=asset['objects'][obj];assert draw['fvf']==0x112 and bytes.fromhex(draw['vertices_hex'])==bytes.fromhex(mesh['vertex_hex'])
    start,count=mesh['texture_runs'][2*(obj+1):2*(obj+1)+2];assert bytes.fromhex(draw['indices_hex'])==bytes.fromhex(asset['face_hex'])[start*6:(start+count)*6]
    vertex_bytes+=len(bytes.fromhex(draw['vertices_hex']));index_count+=count*3
    error=max(abs(x-y)for x,y in zip(matrix,draw['matrix']));pose_checks+=16;pose_max=max(pose_max,error)
    if error>1e-5:errors.append([t,'matrix',obj,error])
 negatives={}
 for name,old,new in [('stale_stream_speed','random(7,13)','random(3,5)'),('missing_initial_damage_rng','amount=random(minimum,maximum)','amount=maximum')]:
  directory=out/name;directory.mkdir();header=(ROOT/'src/manadraincore.h').read_text();assert old in header
  (directory/'manadraincore.h').write_text(header.replace(old,new));(directory/'ymanadraincore.h').write_text((ROOT/'src/ymanadraincore.h').read_text());(directory/'manadrainpose.h').write_text((ROOT/'src/manadrainpose.h').read_text());(directory/'state.cpp').write_text(DRIVER)
  cmd=command.copy();cmd[cmd.index(str(source))]=str(directory/'state.cpp');cmd[-1]=str(directory/'state');compiled=subprocess.run(cmd,capture_output=True,text=True);assert compiled.returncode==0,compiled.stderr
  control=subprocess.run([str(directory/'state')],capture_output=True,text=True);assert control.returncode==0,control.stderr
  changed=sum(a!=b for a,b in zip(result.stdout.splitlines(),control.stdout.splitlines()));assert changed
  negatives[name]=dict(rejected=True,changed_state_frames=changed)
 for name in ('double_custom_pulse','post_owner_animation','incorrect_caster_cap'):
  directory=out/name;directory.mkdir();header=(ROOT/'src/ymanadraincore.h').read_text();driver=DRIVER
  if name=='double_custom_pulse':
   driver=driver.replace('float glow=s.alive?', 'if(s.alive)s.Animate({3185,3185,11},{3281,3185,11},64,rng,dist);float glow=s.alive?')
  elif name=='post_owner_animation':
   old='if(stage>=0){facing=direction;Animate(source,target,direction,random,distance);}'
   header=header.replace(old,'').replace('Pulse(minimum,maximum,random,transfer,kill);','Pulse(minimum,maximum,random,transfer,kill);'+old)
  else:header=header.replace('if(current+amount<=maximum)','if(current+amount>maximum)')
  (directory/'ymanadraincore.h').write_text(header);(directory/'state.cpp').write_text(driver)
  cmd=command.copy();cmd[cmd.index(str(source))]=str(directory/'state.cpp');cmd[-1]=str(directory/'state')
  compiled=subprocess.run(cmd,capture_output=True,text=True);assert compiled.returncode==0,compiled.stderr
  control=subprocess.run([str(directory/'state')],capture_output=True,text=True);assert control.returncode==0,control.stderr
  changed=sum(a!=b for a,b in zip(result.stdout.splitlines(),control.stdout.splitlines()));assert changed
  negatives[name]=dict(rejected=True,changed_state_frames=changed)
 report=dict(status='pass'if not errors else'fail',accepted=False,visual_pass=False,fields=checks,authored_vertex_bytes=vertex_bytes,authored_indices=index_count,asset_sha256=asset['sha256'],maximum_state_error=maximum,pose_fields=pose_checks,maximum_pose_error=pose_max,errors=errors[:50],negative_controls=negatives,native_manifest_sha256=sha(trace),core_sha256=sha(ROOT/'src/manadraincore.h'),pose_sha256=sha(ROOT/'src/manadrainpose.h'),y_core_sha256=sha(ROOT/'src/ymanadraincore.h'),probe_sha256=sha(__file__),sanitizers='ASan/UBSan',scope='Exact integer/RNG/lifecycle and mana; independently compiled float state and sampled whole-render matrices with decimal-output tolerance. Dormant undefined slot fields excluded. No actor pose/damage replacement.')
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],checks,pose_checks,maximum,pose_max);return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('trace',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.trace,a.output)
