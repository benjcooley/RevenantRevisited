#!/usr/bin/env python3
"""Actual Icedsparks whole Metal/translated map compared to its own native default leaf."""
import argparse,json,shutil,re
from pathlib import Path
from PIL import Image,ImageDraw
from port_capture import capture,sha
from map_texture_probe import run as map_capture
from speed_controller_preflight import ROOT

def run(binary,data,native,output):
 output.mkdir(parents=True,exist_ok=False);save=output/'save';save.mkdir();shutil.copyfile(data/'revenant.ini',save/'Revenant.ini')
 scenario=dict(binary=str(binary.resolve()),data_root=str(data.resolve()),save_root=str(save.resolve()),assets_root=str(ROOT/'assets'),effect='TIcedsparks_AUTHORED_STILL',frames=200,fps=24,warmup=1,seed=1,origin=[0,0,0],camera=[640,480],rectangle=[0,0,1280,960],resolution=[1280,960],background='black',ambient=[32,255,255,255],native_domain=True,timeout_seconds=120)
 metal=capture(scenario,output/'metal');assert metal['pixel_summary']['nonblack_frames']==200 and metal['pixel_summary']['distinct_frames']==1
 log=(output/'metal/run.log').read_text();assert '[generic-static-preview] actual owner id=acabfada' in log
 loops=[(int(frame),int(flags,16))for frame,flags in re.findall(r'\[icedsparks-owner\] frame=(\d+) flags=([0-9a-f]+)',log)]
 assert [frame for frame,flags in loops]==[99,0,99,0] and all(not flags&0x1000 for frame,flags in loops),loops
 mapped=map_capture(binary,data,'FontRuntimeLab',(110,10000,10000,16),output/'map',[dict(name='Icedsparks',id='0xacabfada',owner_frames=100,render_frames=1)])
 assert mapped['status']=='pass'and mapped['completed_rows']==10 and mapped['created_visible']and mapped['floor_restored'],mapped
 n=json.loads(native.read_text());assert n['name']=='Icedsparks'and n['status']=='reference_complete'and n['independent_replays']==2
 contact=Image.new('RGB',(1280,10*500));detail=Image.new('RGB',(1024,10*404));pairs=[]
 for index,c in enumerate(n['cases']):
  f=metal['frames'][c['tick']-2];assert metal['sample_simulation_ticks'][c['tick']-2]==c['tick']
  assert sha(c['image'])==c['image_sha256']and sha(f['path'])==f['sha256'];images=[Image.open(p).convert('RGB')for p in (c['image'],f['path'])]
  assert all(im.size==(1280,960)for im in images)
  for j,im in enumerate(images):
   contact.paste(im.resize((640,480)),(j*640,index*500+20));ImageDraw.Draw(contact).text((j*640+4,index*500+3),f'Icedsparks {"Original"if j==0 else"Metal"} tick{c["tick"]} / owner{c["owner_frame"]}',fill='white')
   detail.paste(im.crop((384,288,896,672)),(j*512,index*404+20));ImageDraw.Draw(detail).text((j*512+4,index*404+3),f'Unscaled center, tick{c["tick"]}',fill='white')
  pairs.append(dict(tick=c['tick'],owner_frame=c['owner_frame'],images=[c['image'],f['path']],sha256=[c['image_sha256'],f['sha256']],bounds=[im.getbbox()for im in images]))
 contact.save(output/'contact.png');detail.save(output/'detail.png')
 report=dict(status='review_ready',accepted=False,name='Icedsparks',type_id='0xacabfada',pairs=pairs,actual_owner_wrap_events=loops,binary_sha256=metal['binary_sha256'],native_manifest=dict(path=str(native.resolve()),sha256=sha(native)),metal_manifest_sha256=sha(output/'metal/manifest.json'),map_manifest_sha256=sha(output/'map/manifest.json'),contact_sha256=sha(output/'contact.png'),detail_sha256=sha(output/'detail.png'),probe_sha256=sha(__file__),scope='Ten own full original/native-domain actualMetal pairs through two100frame loops; constant authored50-root ice geometry, no borrowed Iced shatter/actor effect.200actualvisible identical frames, declaredambient32white/source38+dir1,1280x960,origin0. Own literal default typed map create/frame/MOVE/delete/absence10commands and restoredfloor. Resource/key/owner/light/software fixture boundaries; shareddepth/quantization/naturalcaller/device/exactpixel limits remain open.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],sha(output/'manifest.json'),flush=True);return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__)
 for name in('binary','data','native','output'):p.add_argument('--'+name,type=Path,required=True)
 a=p.parse_args();run(a.binary,a.data,a.native,a.output)
