#!/usr/bin/env python3
"""Real typed Flies and independent static Snow map create/move/delete checks."""
import argparse,json
from pathlib import Path
from authored_water_map_probe import run as active
from map_texture_probe import run as still
def run(binary,data,output):
 r=active(binary,data,output/'Flies',dict(name='Flies',id='0xa21d9d00'));r['scope']='Real typed Flies builder/component, exact20drop24Hz random-walk/quad replacement,48 actual map frames,10create/frame/MOVE/delete/absence observations and restored syntheticfloor. Darksprite visibility on declared floor only; natural corpse/lighting/device/exactpixels open.';(output/'Flies/manifest.json').write_text(json.dumps(r,indent=2)+'\n');print('Flies',r['status'],r['errors'],flush=True)
 r=still(binary,data,'FontRuntimeLab',(110,10000,10000,16),output/'Snow',[dict(name='Snow',id='0xfacedbed',owner_frames=100,render_frames=1)]);r['name']='Snow';r['type_id']='0xfacedbed';r['scope']='Real literal Snow default animator/50root authored static mesh/100frame owner loop;16actualmap frames,10create/frame/MOVE/delete/absence observations,static rendering confirmed and exact restored syntheticfloor. Independent of Iced/shatter; naturalcaller/nativehardware/exactpixels remainopen.';(output/'Snow/manifest.json').write_text(json.dumps(r,indent=2)+'\n');print('Snow',r['status'],r['errors'],flush=True)
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--binary',type=Path,required=True);p.add_argument('--data-root',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.binary,a.data_root,a.output)
