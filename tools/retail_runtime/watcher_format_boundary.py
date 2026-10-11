#!/usr/bin/env python3
"""Bounded fresh watcher ARGB1555/software-format diagnostic; no visual credit."""
import argparse,json,struct,zipfile
from pathlib import Path
from scroll_water_visual_reference import NativeScrollScene
from static_texture_probe import parse_frames
from speed_controller_preflight import ROOT,read_asset
from firecone_state_probe import sha
from software_probe import save_rgb565_png

def run(executable,archive,output):
 output.mkdir(parents=True,exist_ok=False);rows=[];profiles=json.loads((ROOT/'tools/retail_runtime/static_additional_profiles.json').read_text())['profiles'][1:]
 with zipfile.ZipFile(archive)as z:
  members={n.lower():n for n in z.namelist()}
  for p in profiles:
   data=z.read(members['imagery/'+p['asset'].replace('\\','/').lower()]);m=parse_frames(data,p);assert m['pixel_format']==[32,65,0,16,0x7c00,0x3e0,0x1f,0x8000]
   f=NativeScrollScene(executable,data,p['name'],640,480);init=f.initialize_scene();assert not init['controllers'];v=f.vm;bo=read_asset(data)['body_offset'];tex=bo+40+struct.unpack_from('<I',data,bo+40)[0]
   descriptor=bytearray(data[tex:tex+108]+bytes(16));struct.pack_into('<I',descriptor,16,128)
   src=f.bytes(bytes(descriptor));dst=f.bytes(bytes(descriptor));v.put_u32(0x65a57c,1);v.call(0x4164c0,(src,dst),this=0x65a57c);assert bytes(v.uc.mem_read(dst,124))==descriptor
   original_pixels=f.bytes(m['textures'][0]);converted=v.allocate(len(m['textures'][0]));v.call(0x416790,(src,original_pixels,0,dst,converted,0),this=0x65a57c);transferred=bytes(v.uc.mem_read(converted,len(m['textures'][0])));assert transferred==m['textures'][0]
   # Explicit resident-resource boundary: source descriptor and raw frame0
   # transferred above. Full original LoadTexture/CreateSurface is not claimed.
   f.software.pixel_format=struct.pack('<8I',*m['pixel_format']);v.write(f.software.texture.address,transferred);v.call(0x56d3a0,(1,f.software.texture_object));raw,depth,draw=f.render(1,0,cull=1);repeat=f.render(1,0,cull=1);assert raw==repeat[0]and depth==repeat[1]
   path=output/(p['name']+'-native1555-frame0.png');save_rgb565_png(path,raw,640,480);assert any(raw);rows.append(dict(name=p['name'],type_id=p['id'],asset_sha256=p['asset_sha256'],pixel_format=m['pixel_format'],active_owner_frames=4,stored_texture_frames=m['texture_frame_count'],initialization=init,closest_format_bytes_unchanged=True,original_convert_texture_raw_equal=True,raw_frame0_sha256=sha(transferred),native_image=str(path),native_image_sha256=sha(path.read_bytes()),rgb565_sha256=sha(raw),depth_sha256=sha(depth),geometry_bounds=draw['projected_geometry_bounds'],draw=draw['draws']))
 report=dict(status='diagnostic_deferred',cases=rows,accepted=False,accurate_software_reference=False,primary_visual_pass=False,full_LoadTexture_executed=False,viewport=[640,480],origin=[0,0,0],camera=[0,0,0],cull=1,source_ambient=[38]*3,directional=[1]*3,original_executable_sha256=sha(executable.read_bytes()),probe_sha256=sha(Path(__file__).read_bytes()),shared_fixture_sha256=sha(Path(__file__).with_name('scroll_water_visual_reference.py').read_bytes()),scope='Three exact shipped descriptors and fresh frame0 whole default/owner/key/RenderObject/software diagnostics. Original GetClosestTextureFormat4164c0 leaves descriptor unchanged in declaredsoftwaremode; ConvertTexture416790 copies identical-format frame bytes exactly. Explicit resident texture/COM resource boundaries; fullLoadTexture/CreateSurface/texture-frame selection/natural placement remain unproved. Original Blue alpha interpretation produces neon/rainbow, unlike correct authored1555 colors. Preserve correct portable decoder; noformatretagging or graphicschanges and noacceptancecredit.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--executable',type=Path,default=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe');p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');p.add_argument('--output',type=Path,required=True);a=p.parse_args();r=run(a.executable,a.archive,a.output);print(r['status'],len(r['cases']))
