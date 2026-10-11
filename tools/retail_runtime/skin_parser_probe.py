#!/usr/bin/env python3
"""Execute original malformed-key and numtrails default/zero controls."""
import argparse,hashlib,json,struct,zipfile
from pathlib import Path
from skin_visual_reference import NativeSkin,PROFILES,ROOT

def run(exe,archive,output):
 output.mkdir(parents=True,exist_ok=False);reports=[]
 with zipfile.ZipFile(archive)as z:
  for name,profile in PROFILES.items():
   data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/'+profile['asset']+'.i3d'));cases={}
   for label in ('authored','explicit_zero','omitted','colon_control'):
    f=NativeSkin(exe,data,name);v=f.vm;tag=f.asset['tags'][1]['parameters']
    if label=='explicit_zero':tag=tag.replace('numtrails=3:3','numtrails=0:0')
    if label=='omitted':tag=tag.replace(',numtrails=3:3','')
    if label=='colon_control':tag=tag.replace('100(0,0,0)','100:(0,0,0)')
    resource=v.u32(v.u32(f.imagery+0x8c)+4);v.put_u32(resource+12,f.put_string(tag));init=f.initialize_scene();frames=[];keys=None
    for tick in range(21):
     if tick:f.advance()
     f.raster_enabled=tick in(0,1,2,5,10,20);raw,record=f.render();c=f.controllers[1];pool=v.u32(c+0x2c);live=[]
     for slot in range(v.u32(c+0xf8)):
      p=pool+slot*252
      if not v.u32(p+0xe4):continue
      row=[slot,v.u32(p+0xd0),v.u32(p+0xd4),*struct.unpack('<6f',v.uc.mem_read(p+0xb8,24)),struct.unpack('<f',v.uc.mem_read(p+0x64,4))[0],v.u32(p+0xf8)]
      live.append(row)
      tracks=v.u32(p+0xb4)
      if keys is None:
       for i in range(v.u32(tracks)):
        q=v.u32(v.u32(tracks+0x10)+i*4)
        if v.u32(q)==11:keys=[list(struct.unpack('<9f',v.uc.mem_read(v.u32(v.u32(q+0x20)+j*4),36)))for j in range(v.u32(q+0x10))]
     frames.append(dict(tick=tick,seed=v.rng_seed,live=live,draw_count=len(record['draws']),raster=f.raster_enabled,pixels=hashlib.sha256(raw).hexdigest()if f.raster_enabled else None))
    cases[label]=dict(localrotation_keys=keys,frames=frames)
   assert cases['authored']['localrotation_keys']==[[0.0]*9,[0.0]*9]
   assert cases['colon_control']['localrotation_keys'][1][0]==100
   assert json.dumps(cases['explicit_zero'],sort_keys=True)==json.dumps(cases['omitted'],sort_keys=True)
   assert all(f['draw_count']==1+sum(row[-2]>9.999999747378752e-6 for row in f['live'])*4 for f in cases['authored']['frames'])
   assert all(f['draw_count']==1+sum(row[-2]>9.999999747378752e-6 for row in f['live']) for f in cases['explicit_zero']['frames'])
   reports.append(dict(name=name,id=profile['id'],cases=cases,zero_omitted_byte_identical=True,rotation_curve_presence_preserved=True))
 report=dict(status='pass',cases=reports,source_policy='Exact authored tag remains untouched. Named original-only variants prove malformed key semantics and absent/zero trails; colon variant is a negative control, never a replacement asset.',original_sha256=hashlib.sha256(exe.read_bytes()).hexdigest(),probe_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),accepted=False)
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print('pass',len(reports),'types, original zero/omitted and trail draw controls');return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path);p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.executable,a.archive,a.output)
