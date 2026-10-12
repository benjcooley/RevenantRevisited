"""Actual Metal films of immutable supported Dragonattack viewport origins."""
import argparse,json
from pathlib import Path
from port_capture import capture
from dragonattack_tiled_reference import ORIGINS

def run(scenario,output,views):
 output.mkdir(parents=True,exist_ok=False);base=json.loads(scenario.read_text());results=[]
 assert base['effect']=='Dragonattack_ORIGINAL' and base['native_domain']
 for view in views:
  x,y=ORIGINS[view];case=dict(base,resolution=[1920,1080],rectangle=[0,0,1920,1080],camera=[640-x,480-y]);out=output/f'view-{view:02}'
  (output/f'view-{view:02}-scenario.json').write_text(json.dumps(case,indent=2)+'\n')
  result=capture(case,out);results.append(dict(view=view,origin=[x,y],manifest=str(out/'manifest.json')))
  print(view,result.get('status'),flush=True)
 (output/'manifest.json').write_text(json.dumps(dict(views=results,scope='Independent actual process per immutable viewport; every24Hz tick renders, sparse PNG interval only.'),indent=2)+'\n')
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--scenario',type=Path,required=True);p.add_argument('--output',type=Path,required=True);p.add_argument('--views',type=int,nargs='+',default=[0,3,6,7,10,13]);a=p.parse_args();run(a.scenario,a.output,a.views)
