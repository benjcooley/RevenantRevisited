#!/usr/bin/env python3
"""Existing bounded real map lifecycle protocol for literal water controllers."""
import argparse,json
from pathlib import Path
from authored_water_map_probe import run
def capture(binary,data,output):
 results=[]
 for name,id in [('Waterfall','0xa907dabf'),('Water','0x1903abcd')]:
  folder=output/name;r=run(binary,data,folder,dict(name=name,id=id));r['scope']='Actual literal typed NewObject/controller owner, stationary visible100/25drop evolution, MOVE/position/DELETE/absence and restored synthetic floor. Exact quad normal-lit RGB565 additive helper, ordinary common-world owner/camera, source ambient32white and ground16. Full natural scene/device/exactpixel gates separate.';(folder/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');results.append(r);print(name,r['status'],r['errors'],flush=True)
 return results
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--binary',type=Path,required=True);p.add_argument('--data-root',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();capture(a.binary,a.data_root,a.output)
