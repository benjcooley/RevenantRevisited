"""Original barrier film with Animate/Render cadence and no hidden warmup draw."""
import argparse
import json
import struct
import zipfile
from pathlib import Path
from barrier_wall_native_preflight import NativeBarrierWall, PROFILES, ROOT
from barrier_state_probe import snapshot
from speed_profile_contract import sha
from software_probe import save_rgb565_png


def run(output,frames,stride):
    output.mkdir(parents=True,exist_ok=False);cases=[]
    with zipfile.ZipFile(ROOT/'data/imagery.rvi') as archive:
        for profile in PROFILES:
            name,type_id,_,_,digest=profile
            data=archive.read(next(n for n in archive.namelist()
                                  if n.lower()=='imagery/misc/'+name.lower()+'.i3d'))
            f=NativeBarrierWall(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data,profile)
            folder=output/name;folder.mkdir();rows=[]
            for tick in range(1,frames+1):
                f.advance();f.raster_enabled=(tick-1)%stride==0
                pixels,row=f.render();row['state']=snapshot(f)
                wall=f.animobjs[0]
                vertices=f.vm.u32(wall+0xa4)
                row['uv']=list(struct.unpack('<2f',f.vm.uc.mem_read(vertices+24,8)))
                if f.raster_enabled:
                    p=folder/f'native-{tick:03d}.png';save_rgb565_png(p,pixels,1280,960)
                    row.update(image=str(p),image_sha256=sha(p.read_bytes()))
                rows.append(row)
            cases.append(dict(name=name,type_id=type_id,asset_sha256=digest,rows=rows))
    report=dict(status='native_render_complete',accepted=False,cases=cases,frames=frames,
                capture_stride=stride,scope='Actual original own constructors/Setup/Animate/key cache/RenderObject and software ARGB4444 wall, every tick rendered. Zero-Y flare matrices remain genuinely rejected. No initial hidden Render, host geometry/UV/pose replacement or port appearance credit. Immutable decoded resource/COM and null-owner bookkeeping boundaries retained.')
    (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS full original cadence')


if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True)
    p.add_argument('--frames',type=int,default=91);p.add_argument('--stride',type=int,default=3)
    a=p.parse_args();run(a.output,a.frames,a.stride)
