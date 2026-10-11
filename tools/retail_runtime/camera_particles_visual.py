#!/usr/bin/env python3
"""Unfitted whole native/actual Metal composites for two exact camera emitters."""
import argparse,json,re,struct,zipfile,hashlib,shutil
from pathlib import Path
from PIL import Image,ImageDraw
from static_particles_profile_contract import CAMERA_PROFILES,ROOT
from speed_controller_preflight import NativeSpeed,read_asset
from static_particles_probe import registry,punctuation
from lit_software_probe import LitSoftwareFixture
from software_probe import save_rgb565_png
from port_capture import capture

sha=lambda p:hashlib.sha256(Path(p).read_bytes()).hexdigest()

def reference(executable,output):
 output.mkdir(parents=True,exist_ok=False);cases=[];state=1;rng={1:0}
 for count in range(1,100001):state=(state*214013+2531011)&0xffffffff;rng[state]=count
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:
  for profile in CAMERA_PROFILES:
   folder=output/profile['name'];folder.mkdir();data=z.read(next(n for n in z.namelist()if n.lower()==('imagery/'+profile['asset']).lower()));assert hashlib.sha256(data).hexdigest()==profile['sha256'];asset=read_asset(data);asset['name']=profile['name'];f=NativeSpeed(executable,asset);registry(f);punctuation(f);init=f.initialize(0)
   # Actual source owner NextFrame/Pulse/Animate cadence: Pulse uses the
   # animator's preceding integer frame; base Render uses owner frame+1.
   trace=f.state_trace(61,move_tick=None);(folder/'native-init.json').write_text(json.dumps(init,indent=2)+'\n');(folder/'native-state.json').write_text(json.dumps(trace,indent=2)+'\n')
   sw=LitSoftwareFixture(executable,640,340);sw.lighting((38,38,38),(1,1,1));v=sw.vm
   for address,value in ((0x5d7a28,1),(0x66818c,0),(0x5e91c0,0),(0x5e8740,0),(0x5c61ac,1),(0x5e8790,0)):v.put_u32(address,value)
   u=lambda p:struct.unpack_from('<I',data,p)[0];r=lambda p:p+u(p);td=r(asset['body_offset']+40);height,width=struct.unpack_from('<2I',data,td+8);bits=r(r(td+108));sw.set_texture(width,height,data[bits:bits+width*height*2]);proto=asset['objects'][0];vertices=list(struct.iter_unpack('<8f',bytes.fromhex(proto['vertex_hex'])));nv=f.vm;point=nv.allocate(12);result=nv.allocate(12);object=f.animobjs[0]
   for tick in (1,5,15,29,45,59):
    sw.clear();v.call(0x417d60,(16,1),this=0x65a57c);particles=0
    for row in trace['rows']:
     if row[0]!=tick or not row[3]:continue
     sample=row[20:];nv.write(object+0x10,struct.pack('<3f',*sample[:3]));nv.write(object+0x28,struct.pack('<3f',*sample[3:6]));nv.write(object+0x40,struct.pack('<3f',*[sample[10]]*3))
     if nv.call(0x40a420,(object,0,0,0,0),this=f.imagery)!=1:continue
     positions=[]
     for vertex in vertices:nv.write(point,struct.pack('<3f',*vertex[:3]));nv.call(0x43ad80,(object+0x58,point,result));positions.append(struct.unpack('<3f',nv.uc.mem_read(result,12)))
     projected=sw.project(positions);color=[(int(c)&255)>>3 for c in sample[6:9]];sw.draw([(*p,*color,int(sample[9]*255)>>3,*vertex[6:])for p,vertex in zip(projected,vertices)],[2,3,0,1,2,0],z_enabled=True,z_write=False,instruction_limit=5000000);particles+=1
    frame=(tick+1)%asset['state0_frames'];base_colors=[]
    for base in asset['objects'][1:]:
     if not base['name'].startswith('#'):continue
     if nv.call(0x40a420,(f.animobjs[base['index']],0,frame,0,0),this=f.imagery)!=1:continue
     matrix=struct.unpack('<16f',nv.uc.mem_read(f.animobjs[base['index']]+0x58,64));raw=list(struct.iter_unpack('<8f',bytes.fromhex(base['vertex_hex'])));start,count=base['texture_runs'][2:4];indices=list(struct.unpack_from('<'+'H'*(count*3),bytes.fromhex(asset['face_hex']),start*6));v.call(0x417d60,(16,1),this=0x65a57c);_,_,transformed=sw.draw_authored(raw,indices,matrix,cull=2,z_enabled=True,z_write=False);base_colors.append(dict(object=base['index'],rgba=[list(p[3:7])for p in transformed]))
    image=folder/f'native-capture{tick:03d}.png';raw=sw.vm.surface_bytes('screen');save_rgb565_png(image,raw,640,340);prior=next(row[2]for row in trace['rows']if row[0]==tick-1)
    cases.append(dict(name=profile['name'],capture=tick,emitter_frame=tick%asset['state0_frames'],base_frame=frame,particles=particles,native_rng_before_capture=prior,native_rng_draws_before_capture=rng[prior],nonzero_pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',raw)),image=str(image),sha256=sha(image),base_colors=base_colors))
 report=dict(status='reference_complete',cases=cases,original_executable_sha256=sha(executable),probe_sha256=sha(__file__),configuration=dict(viewport=[640,340],center=[320,170],seed=1,quality=0,origin=[0,0,0],ambient_rgb=[38]*3,directional_rgb=[1]*3,incoming_blend=16,particle_mode=16,base_mode=16),accepted=False,scope='Whole original parser/Initialize/Pulse/sample/camera matrices/x87 transform plus native normal-light base and full software raster. Three live authored # bases and exact prototype/texture, no source geometry/color inputs. Explicit preceding animator frame for emission and owner next frame for base Render; render1 warmup. Resource-provider, ground0, nullspell and explicit incoming16 bounds, not original map/device/caller acceptance.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report

def metal(binary,fixture,output):
 output.mkdir(parents=True,exist_ok=False);cases=[]
 for p in CAMERA_PROFILES:
  save=output/(p['name']+'-save');save.mkdir();shutil.copyfile(fixture/'revenant.ini',save/'Revenant.ini')
  scenario=dict(save_root=str(save.resolve()),binary=str(binary.resolve()),data_root=str(fixture.resolve()),assets_root=str(ROOT/'assets'),effect='TOgrestrength_AUTHORED_TAGS'if p['name']=='Ogrestrength'else'TTrollblood_AUTHORED_TAGS',frames=60,fps=24,warmup=1,seed=1,origin=[0,0,0],camera=[320,170],rectangle=[0,0,640,340],background='black',native_domain=True,partsys_quality=0,partsys_incoming_blend=16,ambient=[32,255,255,255]);cases.append(dict(name=p['name'],manifest=capture(scenario,output/p['name'])))
 return cases

def review(native,metal,output):
 output.mkdir(parents=True,exist_ok=False);reference=json.load(open(native/'manifest.json'));cases=[];binary=None
 for profile in CAMERA_PROFILES:
  name=profile['name'];capture=json.load(open(metal/name/'manifest.json'));assert capture['status']=='pass';binary=capture['binary_sha256']if binary is None else binary;assert binary==capture['binary_sha256'];frames=[Path(f['path'])for f in capture['frames']];log=(metal/name/'run.log').read_text();rng={int(t):int(n)for t,n in re.findall(r'before_render=(\d+) global_random_draws=(\d+)',log)};panel=Image.new('RGB',(1280,6*365),(22,22,22));labels=ImageDraw.Draw(panel)
  for index,row in enumerate(c for c in reference['cases']if c['name']==name):
   number=row['capture'];original=Path(row['image']);modern=frames[number-1];assert sha(original)==row['sha256'];images=[Image.open(p).convert('RGB')for p in(original,modern)];diff=sum(a!=b for a,b in zip(images[0].getdata(),images[1].getdata()));errors=sum(sum(abs(a-b)for a,b in zip(x,y))for x,y in zip(images[0].getdata(),images[1].getdata()));match=rng.get(number+1)==row['native_rng_draws_before_capture']
   for col,(im,label)in enumerate(zip(images,('Original software','Actual Metal'))):panel.paste(im,(col*640,index*365+25));labels.text((col*640+5,index*365+5),f'{name} {label} capture{number} emitter{row["emitter_frame"]} base{row["base_frame"]}',fill='white')
   cases.append(dict(name=name,capture=number,native=str(original),metal=str(modern),sha256=[sha(original),sha(modern)],bbox=[im.getbbox()for im in images],differing_pixels=diff,absolute_rgb_error=errors,phase_rng_equal=match,native_rng_draws=row['native_rng_draws_before_capture'],metal_rng_draws=rng.get(number+1)))
  panel.save(output/(name+'-native-metal.png'))
 report=dict(status='review_ready'if all(c['phase_rng_equal']for c in cases)else'phase_mismatch',cases=cases,binary_sha256=binary,native_manifest_sha256=sha(native/'manifest.json'),probe_sha256=sha(__file__),accepted=False,strict_pixel_parity=all(c['differing_pixels']==0 for c in cases),scope='Actual current Metal against independent whole native composites. Full640x340 raw panels, no crops/registration/fitting. Same asset, seed, cadence, fixed origin, native software projection, incoming16 and source lighting. Phase/RNG counts are mandatory; metrics and human appearance review remain distinct from shared-raster frontend evidence. No map/caster/collision/full acceptance.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report

if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--mode',choices=['reference','metal','review'],required=True);p.add_argument('--output',type=Path,required=True);p.add_argument('--executable',type=Path,default=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe');p.add_argument('--binary',type=Path,default=ROOT/'build-merge/Revenant');p.add_argument('--fixture',type=Path,default=Path('/Users/benjamincooley/RevenantRetailLab/captures/runtime-fixtures/fountain-command-20261004/data'));p.add_argument('--native',type=Path);p.add_argument('--metal',type=Path);a=p.parse_args();r=reference(a.executable,a.output)if a.mode=='reference'else metal(a.binary,a.fixture,a.output)if a.mode=='metal'else review(a.native,a.metal,a.output);print(json.dumps(r,indent=2))
