"""Full original FireBall impact state versus actual StepAnimate and quad helpers."""
import argparse,json,subprocess,struct
from pathlib import Path
from fireball_spark_probe import compile_prefix,function,ROOT
from speed_controller_preflight import sha

def compile_full(output,negative_control=False):
 _,meta=compile_prefix(output);src=output/'port-spark.cpp';text=src.read_text();cpp=(ROOT/'src/effect.cpp').read_text()
 text=text.replace(function(text,'void TFireBallEffect::StepAnimate()'),function(cpp,'void TFireBallEffect::StepAnimate()'))
 text=text.replace('bool status_{};','bool status_{},ring_active_{},ring_done_{true},alive_{true};float ring_scale_=25;hmm_vec3 ring_pos_{};SFireBallData burst_[10];bool IsTrailDraining()const;')
 text=text.replace('void card(const SFireBallData&p)',function(cpp,'bool TFireBallEffect::IsTrailDraining() const')+'\nvoid card(const SFireBallData&p)')
 text=text.replace('effect.fireball_.scale=.6f','effect.fireball_.scale=.3f')
 text=text.replace('std::printf("],\\\"sparks\\\":[");','std::printf("],\\\"burst\\\":[");for(int i=0;i<10;++i){if(i)std::printf(",");card(effect.burst_[i]);}std::printf("],\\\"ring_scale\\\":%.9g,\\\"ring_done\\\":%s,\\\"sparks\\\":[",effect.ring_scale_,effect.ring_done_?"true":"false");')
 if negative_control:
  text=text.replace('bq.used = false;\n                    bq.glow', '{ bq.used = false; continue; }\n                    bq.glow').replace('ring_done_ = true;\n                }', '{ ring_scale_ = kFireBallRingMaxSize; ring_done_ = true; }\n                }')
 src.write_text(text);q=subprocess.run(meta['command'],capture_output=True,text=True);assert q.returncode==0,q.stderr
 return output/'port-spark'

def equivalent(a,b):
 if isinstance(a,bool)or isinstance(b,bool):return a==b
 return struct.pack('<f',a)==struct.pack('<f',b)

def run(trace,output,negative_control=False):
 output.mkdir(parents=True,exist_ok=True);native=json.loads(trace.read_text());binary=compile_full(output,negative_control)
 motion=output/'motion.txt';motion.write_text(''.join(' '.join(map(str,[r['motion']['state'],*r['motion']['position']]))+'\n'for r in native))
 port=json.loads(subprocess.check_output([str(binary),str(motion),'1'],text=True));errors=[];checks=0
 assert len(port)==len(native) and native
 for tick,(row,b)in enumerate(zip(native,port)):
  a=row['state'];checks+=1
  if a['seed']!=b['seed']:errors.append(dict(tick=tick,field='seed',native=a['seed'],port=b['seed']))
  for group in('head','trail','sparks','burst'):
   left=[a[group]]if group=='head'else a[group];right=[b[group]]if group=='head'else b[group]
   assert len(left)==len(right)
   for index,(x,y)in enumerate(zip(left,right)):
    for key in y:
     # Native factory zero-initializes unused burst slots; the port's default
     # glow1 is irrelevant until used. Compare active/expired authored slots.
     if group=='burst'and row['motion']['state']!=2:continue
     for u,v in zip(x[key]if isinstance(x[key],list)else[x[key]],y[key]if isinstance(y[key],list)else[y[key]]):
      checks+=1
      if not equivalent(u,v):errors.append(dict(tick=tick,group=group,index=index,field=key,native=u,port=v))
  if row['motion']['state']==2:
   for key in('ring_scale','ring_done'):
    checks+=1
    if not equivalent(a[key],b[key]):errors.append(dict(tick=tick,field=key,native=a[key],port=b[key]))
 report=dict(negative_control=negative_control,status='pass'if not errors else'differences_found',checks=checks,errors=errors,frames=len(native),native_trace_sha256=sha(trace.read_bytes()),source_sha256={f:sha((ROOT/f).read_bytes())for f in('src/effect.cpp','src/fireballcore.h','src/fireballspark.h')})
 (output/'state-report.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items()if k!='errors'},indent=2));print('errors',len(errors));print(errors[:5]);return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('trace',type=Path);p.add_argument('--output',required=True,type=Path);p.add_argument('--negative-control',action='store_true');a=p.parse_args();r=run(a.trace,a.output,a.negative_control);raise SystemExit(bool(r['errors']))
