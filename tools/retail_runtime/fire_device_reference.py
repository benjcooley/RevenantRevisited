"""Qualified hardware Fire: unchanged quad, material, texture0 and four facings.

Actual cached factory/default lifecycle, original hardware DrawIndexedPrimitive
and selected COM material/texture. Exact white emissive material saturates fixed
function diffuse; no normal/light substitution is needed. Original transform
with zero indices supplies explicit source-domain screen coordinates. Qualified
RGB8 linear target, explicit legacy-integer/modern-half-center coverage and
point/linear sampler controls;
not an unchanged retail software renderer or period GPU.
"""
import argparse,json,struct
from pathlib import Path
from PIL import Image
from fire_cardinal_native_probe import CardinalFire,ROOT,sha
from argb4444_device_raster import raster,emissive_saturation
from unicorn.x86_const import UC_X86_REG_ESI,UC_X86_REG_EDI
class FireDevice(CardinalFire):
 def new_object(self,*args,**kwargs):
  self.materials={};v=self.vm;old=self.world.ddraw._device_method
  def material(interface,method,argv):
   if interface=='IDirect3DMaterial3'and method=='SetMaterial':self.materials[argv[0]]=bytes(v.uc.mem_read(argv[1],80))
   return old(interface,method,argv)
  self.world.ddraw._device_method=material
  return super().new_object(*args,**kwargs)
 def __init__(self,exe,boot,facing):
  super().__init__(exe,boot,facing);v=self.vm;old=self.world.ddraw._device_method
  self.packets=[];self.device_states={};self.stage_states={};self.bound_textures={};self.matrix=None
  def device(interface,method,args):
   if method=='SetRenderState':self.device_states[args[1]]=args[2]
   if method=='SetTextureStageState':self.stage_states[(args[1],args[2])]=args[3]
   if method=='SetTransform'and args[1]==1:self.matrix=bytes(v.uc.mem_read(args[2],64))
   if method=='SetTexture':self.bound_textures[args[1]]=args[2]
   if method=='GetTexture':v.put_u32(args[2],self.bound_textures.get(args[1],0));return 0
   if method=='GetClipStatus':v.write(args[1],struct.pack('<I6f',2,0,0,0,0,0,0));return 0
   if method=='DrawIndexedPrimitive':
    _,primitive,fvf,vertices,count,indices,ni,flags=args;assert(primitive,fvf,count,ni,flags)==(4,0x112,4,6,0)
    assert all(self.device_states.get(k)==value for k,value in((27,1),(19,5),(20,6),(22,1),(14,0),(7,1)))
    assert all(self.stage_states.get((0,k))==value for k,value in((1,4),(2,2),(3,0),(4,4),(5,2),(6,0)))
    d=self.world.ddraw;surface=d.surface(d.textures[self.bound_textures[0]])
    assert struct.unpack('<8I',surface.pixel_format)==(32,65,0,16,0xf00,0xf0,15,0xf000)
    material=self.materials[self.material_state];emissive_saturation(material)
    texture=b''.join(bytes(v.uc.mem_read(surface.memory+y*surface.pitch,surface.width*2))for y in range(surface.height))
    self.packets.append(dict(fvf=fvf,vertices_hex=bytes(v.uc.mem_read(vertices,count*32)).hex(),indices=list(struct.unpack('<6H',v.uc.mem_read(indices,12))),world_matrix_hex=self.matrix.hex(),material_handle=self.material_state,material_hex=material.hex(),texture_pointer=self.bound_textures[0],texture_hex=texture.hex(),texture_width=surface.width,texture_height=surface.height,states=dict(self.device_states),stages={str(k):value for k,value in self.stage_states.items()}));return 0
   return old(interface,method,args)
  self.world.ddraw._device_method=device;v.put_u32(0x5d7a28,0)
  v.uc.reg_write(UC_X86_REG_ESI,0x65a57c);v.uc.reg_write(UC_X86_REG_EDI,0);v.call(0x4132c7,stop_address=0x413342);assert self.device_states[27]==1
  self.matrix_buffer=v.allocate(64)
 def render_device(self):
  self.packets=[];self.raster_enabled=True;super().render();v=self.vm;s=self.software
  assert len(self.packets)==1
  for p in self.packets:
   v.write(s.authored,bytes.fromhex(p['vertices_hex']));v.write(self.matrix_buffer,bytes.fromhex(p['world_matrix_hex']));v.call(0x56d5f0,(1,self.matrix_buffer));v.put_u32(0x675fb8,s.vertices+4*36)
   v.call(0x56eb30,(4,0x112,s.authored,4,s.indices,0,0));transformed=list(struct.iter_unpack('<3f4I2f',v.uc.mem_read(s.vertices,4*36)))
   p['projected']=[[row[0],-row[1],row[2]]for row in transformed];p['original_transformed']=transformed
  return self.packets

def run(exe,boot,out):
 out.mkdir(parents=True,exist_ok=False);records=[]
 for face in(0,64,128,192):
  f=FireDevice(exe,boot,face)
  for tick in range(67):
   f.world.call(0x470cc0,this=f.owner);f.world.call(0x4de800,this=f.owner);f.vm.put_u32(0x65cb38,tick);packets=f.render_device();state=f.state()
   assert state['frame']==state['animator_frame']==state['texture_frame']==0 and state['rng_draws']==0
   if tick in(0,33,66):
    images={}
    for lattice,center in(('legacy',0.),('modern',.5)):
     for filtering in('point','linear'):
      mode=lattice+'-'+filtering;raw=raster(packets,filtering,1280,960,pixel_center=center);path=out/f'face-{face:03}-{mode}-{tick:03}.png';Image.frombytes('RGB',(1280,960),raw).save(path)
      pixels=[dict(x=i//3%1280,y=i//3//1280,rgb=list(raw[i:i+3]))for i in range(0,len(raw),3)if any(raw[i:i+3])];images[mode]=dict(path=str(path),sha256=sha(path),pixels=pixels,sample_center=center,filtering=filtering)
    records.append(dict(facing=face,tick=tick,state=state,definition=f.definition,packets=packets,images=images));print(face,tick,{mode:image['pixels']for mode,image in images.items()},flush=True)
  del f
  import gc;gc.collect()
 report=dict(scope=__doc__,qualified=True,unchanged_original_device=False,visual_pass=False,executable_sha256=sha(exe),boot_sha256=sha(boot),tool_sha256=sha(__file__),raster_sha256=sha(Path(__file__).with_name('argb4444_device_raster.py')),dimensions=[1280,960],center=[640,480],source=[3185,3185,16],camera=[3185,3185,0],hardware_packet_frames=268,records=records)
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--boot-image',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',a.boot_image,a.output)
