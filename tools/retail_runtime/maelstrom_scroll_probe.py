"""Original full Render scroll controllers versus compiled production UV offsets."""
import argparse,json,struct,zipfile
from pathlib import Path
from maelstrom_visual_reference import MaelstromScene
from speed_profile_contract import ROOT,sha
import scrolltex_water_probe as source

def run(exe,out):
 out.mkdir(parents=True,exist_ok=True)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read('Imagery/Magic/maelstrom.I3D')
 f=MaelstromScene(exe,data);f.step();f.raster_enabled=False
 ticks=(0,1,2,7,31,130,244,1024);source.TICKS=ticks
 metadata={str(i):dict(object_name=f.asset['objects'][i]['name'],tag_params=t['parameters'],frames=245)for i,t in zip((4,2,3),f.asset['tags'][:3])}
 compiled,build=source.build_controller(out,metadata)
 f32=lambda x:struct.unpack('<f',struct.pack('<f',x))[0]
 fields=0;maximum=0.;cases=[]
 for tick in ticks:
  f.vm.put_u32(0x65cb38,tick);f.animate();f.render();draws={d['object']:d for d in f.draws}
  for i in (4,2,3):
   row=compiled[(str(i),tick)];assert row['active']and row['tracks']==1
   original=list(struct.iter_unpack('<8f',bytes.fromhex(draws[i]['vertices_hex'])));literal=list(struct.iter_unpack('<8f',bytes.fromhex(f.asset['objects'][i]['vertex_hex'])))
   assert len(original)==len(literal)
   error=max(abs(v[k+6]-f32(p[k+6]+f32(row['offset'][k])))for v,p in zip(original,literal)for k in (0,1));fields+=len(literal)*2;maximum=max(maximum,error)
   assert error<=8e-6,(tick,i,error)
   cases.append(dict(global_tick=tick,owner_frame=f.state()['frame'],object=i,source_offset=row['offset'],vertices=len(literal),max_uv_error=error))
 r=dict(status='pass',uv_fields=fields,max_uv_error=maximum,cases=cases,controllers=f.controller_state(),compiled_source=build,asset_sha256=sha(data),fixture_source_sha256=sha((ROOT/'tools/retail_runtime/maelstrom_visual_reference.py').read_bytes()),source_sha256=sha(Path(__file__).read_bytes()),scope='Original default Render executes all three registered scrolltex controllers on independently selected global ticks while owner frame remains1. Production parser and ScrollTexOffset are compiled verbatim. GPU vertex addition uses the same float offset; original x87 final-store precision differences are bounded, not claimed identical.')
 (out/'proof.json').write_text(json.dumps(r,indent=2)+'\n');print('pass',fields,maximum);return r
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.executable,a.output)
