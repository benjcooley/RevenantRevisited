"""Compile YLose's exact Render-time color cache and compare original callers."""
import argparse,json,struct,subprocess,zipfile
from pathlib import Path
from ylose_visual_reference import YLoseScene,ROOT,sha

def run(exe,output):
 output.mkdir(parents=True,exist_ok=True)
 source=output/'fade.cpp';source.write_text(r'''
#include "ylosefade.h"
#include <cstdio>
int main(){for(int i=0;i<17;++i)printf("%d ",retail_ylose::FadedObject(i));puts("");
 unsigned color=retail_ylose::initial_color;int frame,render;
 while(scanf("%d%d",&frame,&render)==2){printf("%u ",color);if(render)retail_ylose::RenderColor(frame,color);printf("%u\n",color);}}
''');binary=output/'fade';cmd=['clang++','-std=c++17','-O1','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),str(source),'-o',str(binary)]
 q=subprocess.run(cmd,capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr);assert q.returncode==0,q.stderr
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read('Imagery/Magic/ylose.I3D')
 cases=[];fields=0
 for stride in(1,3,5,13,0):
  f=YLoseScene(exe,data);f.raster_enabled=False;v=f.vm
  expected=[2,3,4,5,6,7,8,9,10,11,13,14,15,16]
  assert [r['object']for r in f.fade_objects()]==expected
  assert all(r['color']==0xfffffff8 for r in f.fade_objects())
  records=[];inputs=[]
  for tick in range(30):
   f.step();before=f.state();draw=stride>0 and (tick+1)%stride==0
   if draw:f.animate();f.render()
   after=f.state();inputs.append(f"{before['frame']} {int(draw and not(before['flags']&0x1000))}");records.append(dict(tick=tick,frame=before['frame'],rendered=draw,before=before['fade_objects'],after=after['fade_objects']))
  q=subprocess.run([str(binary)],input='\n'.join(inputs)+'\n',capture_output=True,text=True);assert q.returncode==0,q.stderr
  lines=q.stdout.splitlines();membership=list(map(int,lines[0].split()));assert [i for i,b in enumerate(membership)if b]==expected
  for row,line in zip(records,lines[1:]):
   values=list(map(int,line.split()))
   for stage,color in zip(('before','after'),values):
    assert all(obj['color']==color for obj in row[stage]),(stride,row,values);fields+=len(row[stage])
  cases.append(dict(stride=stride,frames=30,records=records,rng=f.rng_draws))
 report=dict(status='pass',fields=fields,cases=cases,asset_sha256=sha(data),source_sha256=sha((ROOT/'src/ylosefade.h').read_bytes()),driver_sha256=sha(source.read_bytes()),executable_sha256=sha(exe.read_bytes()),scope='Original401d40 registration,4021a0 FVF clone/malformed target parsing,4022b0 Render-time curve/cache, whole default Animate/Render and real owner lifecycle; no replacement vertex colors in original.',limits=['No natural Yhagoro caller or complete scene/device acceptance'])
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print('pass',fields);return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.executable,a.output)
