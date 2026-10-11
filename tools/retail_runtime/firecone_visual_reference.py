#!/usr/bin/env python3
"""Full original FireCone lifecycle samples for fixed native-domain Metal review."""
import argparse,json,struct,zipfile
from pathlib import Path
from firecone_render_probe import ROOT,FireConeRenderFixture,parse_asset
from firecone_owner_probe import native_owner,native_world,pixels
from firecone_state_probe import sha
from lit_software_probe import LitSoftwareFixture
from software_probe import save_rgb565_png


def run(executable, output):
    output.mkdir(parents=True,exist_ok=False)
    with zipfile.ZipFile(ROOT/'data/imagery.rvi')as archive:asset=archive.read('Imagery/Magic/firecone.i3d')
    parts=parse_asset(asset);f=FireConeRenderFixture(executable)
    # Same declared preview origin0; native Initialize has already elevated
    # this owner by100. No local particle or authored vertex is modified.
    f.vm.write(f.owner+0x10,struct.pack('<3i',0,0,100))
    software=LitSoftwareFixture(executable,640,340);software.lighting((38,38,38),(1,1,1));v=software.vm
    for address,value in ((0x5d7a28,1),(0x66818c,0),(0x5e91c0,0),(0x5e8740,0),(0x5c61ac,1),(0x5e8790,0)):v.put_u32(address,value)
    v.call(0x417d60,(8,1),this=0x65a57c)
    selected=(1,2,5,6,15,20,29,33,45,59,80,92,93,100);cases=[];lifecycle=[]
    # FrameSnap skips one complete rendered update. Retained frame001 is
    # therefore tick2; filenames and these capture numbers are one-based.
    f.step()
    for capture in range(1,101):
        before=f.state();rng_before=before['random_count']
        if before['alive']:f.step()
        state=f.state();lifecycle.append(dict(capture=capture,tick=capture+1,alive=state['alive'],rng_before=rng_before,rng_after=state['random_count'],used={name:sum(p['used']!=0 for p in pool)for name,pool in state['pools'].items()}))
        if capture not in selected:continue
        owner=native_owner(f);packets=f.render(parts)
        for packet in packets:packet['matrix']=native_world(f,packet)
        image,depth,colors=pixels(software,packets,parts,asset);path=output/f'native-capture{capture:03d}.png';save_rgb565_png(path,image,640,340)
        cases.append(dict(capture=capture,tick=capture+1,alive=state['alive'],native_rng_draws_before_capture=rng_before,packets=len(packets),owner_matrix=owner,path=str(path),sha256=sha(path.read_bytes()),rgb5_colors=sorted(set(tuple(c)for c in colors)),nonzero_pixels=sum(p!=(0,)for p in struct.iter_unpack('<H',image))))
    report=dict(status='reference_complete',original_executable_sha256=sha(executable.read_bytes()),asset_sha256=sha(asset),fixture_sha256=sha((ROOT/'tools/retail_runtime/lit_software_probe.py').read_bytes()),probe_sha256=sha(Path(__file__).read_bytes()),configuration=dict(origin=[0,0,0],initialized_owner=[0,0,100],facing=0,camera=[0,0,0],viewport=[640,340],source_ambient=[38]*3,directional=[1]*3,point_lights=[],source_blend=8,fps=24,seed=1),cases=cases,lifecycle=lifecycle,accepted=False,scope='Original FireCone Initialize/Animate/Render, native owner and relative RenderObject matrix, full authored normals/UV/RGB565 textures, original Illuminate/projector/raster. Null spell, explicit light descriptors. No production vertices or fitted inputs. One-based retained capture i follows tick i+1 after the declared one-frame warmup.')
    (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();r=run(a.executable,a.output);print(json.dumps({k:v for k,v in r.items()if k not in ('cases','lifecycle')},indent=2))
