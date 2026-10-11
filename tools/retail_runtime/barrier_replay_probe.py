"""Fresh complete barrier replay with independent selected original rasters."""
import argparse
import json
import zipfile
from pathlib import Path
from barrier_visual_reference import NativeBarrierWall, PROFILES, ROOT
from barrier_state_probe import snapshot
from speed_profile_contract import sha
from software_probe import save_rgb565_png


def run(first,output):
    output.mkdir(parents=True,exist_ok=False);reference=json.loads(first.read_text());cases=[]
    with zipfile.ZipFile(ROOT/'data/imagery.rvi') as archive:
        for profile in PROFILES:
            name=profile[0];expected=next(c for c in reference['cases']if c['name']==name)
            data=archive.read(next(n for n in archive.namelist()
                                  if n.lower()=='imagery/misc/'+name.lower()+'.i3d'))
            f=NativeBarrierWall(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data,profile)
            folder=output/name;folder.mkdir();images=[]
            for row in expected['rows']:
                f.advance();f.raster_enabled=row['tick'] in (1,31,91)
                pixels,record=f.render();assert snapshot(f)==row['state'],(name,row['tick'])
                # Serialized render-state keys are strings; compare the same
                # representation without altering any observed values.
                draws=json.loads(json.dumps(record['draws']))
                if f.raster_enabled:
                    assert draws==row['draws'],(name,row['tick'])
                else:
                    assert len(draws)==len(row['draws'])
                if f.raster_enabled:
                    p=folder/f"native-{row['tick']:03d}.png";save_rgb565_png(p,pixels,1280,960)
                    h=sha(p.read_bytes());assert h==row['image_sha256'],(name,row['tick'])
                    images.append(dict(tick=row['tick'],path=str(p),sha256=h))
            cases.append(dict(name=name,all_state_rows_exact=len(expected['rows']),images=images))
    report=dict(status='pass',first_sha256=sha(first.read_bytes()),cases=cases,
                scope='Fresh original constructor/Animate/Render at every tick; all91 state snapshots exact per resource, and three selected whole PNGs byte-identical. Independent framebuffer/backend acceptance remains separate.')
    (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS both91state replays/6PNGs')


if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('first',type=Path);p.add_argument('--output',type=Path,required=True)
    a=p.parse_args();run(a.first,a.output)
