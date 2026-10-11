"""Actual normal-lit untextured kernel RGB5 packing; white-texture negative control."""
import argparse,json,struct
from pathlib import Path
from lit_software_probe import LitSoftwareFixture
from speed_controller_preflight import sha
p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True)
f=LitSoftwareFixture(a.executable,64,64);vertices=[(-10,10,0,0,0,1,0,0),(10,10,0,0,0,1,0,0),(10,-10,0,0,0,1,0,0),(-10,-10,0,0,0,1,0,0)];matrix=[float(i%5==0)for i in range(16)];cases=[];errors=[]
for color in [(v,v,v)for v in range(0,256,8)]+[(38,38,38),(95,63,31),(255,255,255),(1,0,255)]:
 f.lighting(color);f.clear(0x1234);screen,depth,t=f.draw_authored(vertices,[0,1,2,0,2,3],matrix,cull=1,z_enabled=False)
 q=t[0][3:6];assert all(v[3:6]==q for v in t);expected=(q[0]<<11)|(q[1]<<6)|q[2];pixels=[x[0]for x in struct.iter_unpack('<H',screen)];covered=[x for x in pixels if x!=0x1234]
 if not covered or any(x!=expected for x in covered):errors.append(dict(color=color,expected=expected,observed=sorted(set(covered))))
 cases.append(dict(ambient=color,original_vertex_rgb5=q,expected_rgb565=expected,pixels=len(covered),actual_rgb565=sorted(set(covered))))
# Treating a white RGB565 texture as untextured changes the original path:
# the modulation kernel restores green's sixth bit, whereas untextured doesn't.
f.lighting((255,255,255));f.set_texture(2,2,struct.pack('<4H',*[65535]*4));f.clear(0x1234);screen,_,t=f.draw_authored(vertices,[0,1,2,0,2,3],matrix,cull=1,z_enabled=False);negative=sorted({x[0]for x in struct.iter_unpack('<H',screen)if x[0]!=0x1234});assert negative!=cases[-2]['actual_rgb565']
# Original untextured kernel ignores ZENABLE/ZWRITE: all four scene-state
# combinations still update depth; a same-depth second draw overwrites.
depth_cases=[]
for enabled in (False,True):
 for write in (False,True):
  g=LitSoftwareFixture(a.executable,64,64);g.lighting((100,100,100));g.clear();before=g.vm.surface_bytes('depth')
  first,after,_=g.draw_authored(vertices,[0,1,2,0,2,3],matrix,cull=1,z_enabled=enabled,z_write=write)
  g.lighting((200,200,200));second,_,_=g.draw_authored(vertices,[0,1,2,0,2,3],matrix,cull=1,z_enabled=enabled,z_write=write)
  changed=sum(x!=y for x,y in zip(before,after));overwrite=sum(x!=y for x,y in zip(first,second))
  assert changed>0 and overwrite>0
  depth_cases.append(dict(z_enabled=enabled,z_write=write,depth_bytes_changed=changed,equal_depth_color_bytes_changed=overwrite))
# Independently varied triangle shape/depth and cyclic index order establish
# the kernel's flat trunc(max(vertex Z)) rule rather than interpolated depth.
flat_depth=[]
g=LitSoftwareFixture(a.executable,128,128);g.lighting((100,100,100))
for zs in ((0,0,0),(0,0,20),(0,20,0),(20,0,0),(0,-20,0),(-20,0,20),(30,-5,-15)):
 for order in ((0,1,2),(1,2,0),(2,0,1)):
  triangle=[(*p,0,0,1,0,0)for p in ((-10,10,zs[0]),(20,10,zs[1]),(10,-20,zs[2]))]
  g.clear();_,depth,transformed=g.draw_authored(triangle,order,matrix,cull=1,z_write=False)
  actual=sorted({p[0]for p in struct.iter_unpack('<H',depth)if p[0]!=65535});expected=int(max(p[2]for p in transformed))
  assert actual==[expected],(zs,order,actual,expected)
  flat_depth.append(dict(z_values=zs,indices=order,projected_depths=[p[2]for p in transformed],actual_depth=actual,expected_depth=expected))
# ARGB4444 is a negative control: it interpolates depth and does not inherit
# the untextured rule. Intermediate clear depths admit only part of the face.
g=LitSoftwareFixture(a.executable,128,128);g.lighting((100,100,100));g.set_texture(2,2,struct.pack('<4H',*[65535]*4),format='ARGB4444')
triangle=[(*p,0,0,1,0,0)for p in ((-10,10,0),(20,10,0),(10,-20,0))];alpha_depth=[]
for clear_depth in (2720,2730,2745,2750,2758,2760,65535):
 g.clear(depth=clear_depth);screen,depth,_=g.draw_authored(triangle,[0,1,2],matrix,cull=1,z_write=False)
 count=sum(p[0]!=0 for p in struct.iter_unpack('<H',screen));assert all(p[0]==clear_depth for p in struct.iter_unpack('<H',depth))
 alpha_depth.append(dict(clear_depth=clear_depth,covered_pixels=count))
assert 0<alpha_depth[1]['covered_pixels']<alpha_depth[2]['covered_pixels']<alpha_depth[-1]['covered_pixels']
# Actual four distinct authored cylinder materials still share Illuminate RGB5.
import zipfile
from icebolt_visual_reference import IceScene
from unicorn import UC_HOOK_CODE
with zipfile.ZipFile('data/imagery.rvi') as archive:data=archive.read('Imagery/Magic/icebolt.i3d')
g=IceScene(a.executable,data);g.step();g.pick=0
hook=g.vm.uc.hook_add(UC_HOOK_CODE,lambda *args:g.ret(0,28)if g.current_object!=g.pick else None,begin=0x56eb30,end=0x56eb30)
materials=[]
for i in range(4):
 g.pick=i;g.render();v=g.vm;rows=list(struct.iter_unpack('<3f4I2f',v.uc.mem_read(v.u32(0x675fb4),24*36)))
 materials.append(dict(object=i,original_rgb5_alpha=sorted(set(row[3:7]for row in rows))))
assert all(m['original_rgb5_alpha']==materials[0]['original_rgb5_alpha']for m in materials)
r=dict(status='pass'if not errors else'differences',cases=cases,errors=errors,depth_controls=depth_cases,flat_depth_controls=flat_depth,argb_interpolated_depth_negative_control=alpha_depth,source_sha256=sha(Path(__file__).read_bytes()),authored_material_controls=materials,white_texture_negative_control=negative,untextured_white=cases[-2]['actual_rgb565'],executable_sha256=sha(a.executable.read_bytes()),scope='Actual56eb30 authored D3DVERTEX transform/Illuminate and untextured raster. Constant normals, independently varied actual scene ambient, no fabricated vertex colors. RGB5 output packs R<<11 | G<<6 | B; opaque alpha. White RGB565 modulation is an actual negative control.')
(a.output/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');print(r['status'],len(cases),'native kernels',negative,'versus',r['untextured_white']);assert not errors
