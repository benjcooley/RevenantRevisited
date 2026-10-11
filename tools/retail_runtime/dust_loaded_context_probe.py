"""Genuine loaded Dust owner: quality/light controls cannot rescue zero source RGB565.

No effect, particle, terrain or renderer replacement. Explicit original software
output buffers and GetLightState material semantics are the device boundary.
"""
import argparse,json,struct,zipfile,hashlib
from pathlib import Path
from character_native_preflight import AuthenticCharacters,ROOT,sha
from tornado_target_visual_reference import TornadoTargetScene
from lit_software_probe import LitSoftwareFixture
from fireball_visual_reference import read_fireball
from unicorn import UC_HOOK_CODE
class DustLoadedScene:
 def __init__(self,exe,boot,quality,light,source):
  self.actors=AuthenticCharacters(exe,boot);self.actors.select_screen_prefix();self.world=self.actors.world;self.vm=v=self.actors.vm;call=self.world.call
  v.put_u32(0x5d79e4,quality);self.owner=self.actors.new_object('Dust',source,25,0x5a85ac);assert v.u32(self.owner+0xe4)==0
  self.rng_calls=0;v.uc.hook_add(UC_HOOK_CODE,self.rand_entry,begin=0x58c582,end=0x58c582);call(0x58c575,(1,));self.tls=call(0x58fbf1)
  assert call(0x46e8b0,this=self.owner,instruction_limit=100000000)==1;self.animator=v.u32(self.owner+0x58);assert v.u32(self.animator)==0x5a370c
  self.material=0
  for name,args in self.world.ddraw.device_calls:
   if name=='IDirect3DDevice3::SetLightState'and args[0]==1:self.material=args[1]
  original=self.world.ddraw._device_method
  def device(interface,method,args):
   if interface=='IDirect3DDevice3'and method in('GetLightState','SetLightState')and args[1]==1:
    if method=='GetLightState':v.put_u32(args[2],self.material)
    else:self.material=args[2]
    return 0
   return original(interface,method,args)
  self.world.ddraw._device_method=device
  self.software=LitSoftwareFixture(exe,640,480,camera=(*source[:2],0),vm=v);self.software.lighting((light,)*3,(light,)*3);v.put_u32(0x675fc0,320);v.put_u32(0x675fc4,320)
  self.draws=[];self.states={};self.current_object=-1
  for address,method in[(0x56eb30,self.observe_draw),(0x56d400,self.observe_state),(0x40a8f0,self.observe_object)]:v.uc.hook_add(UC_HOOK_CODE,method,begin=address,end=address)
  call(0x40e460,(0,),this=self.animator)
 observe_draw=TornadoTargetScene.observe_draw
 observe_state=TornadoTargetScene.observe_state
 observe_object=TornadoTargetScene.observe_object
 def rand_entry(self,*_):self.rng_calls+=1
 def state(self):
  v=self.vm;controllers=[]
  for i in range(v.u32(self.animator+0x58)):
   c=v.u32(v.u32(self.animator+0x68)+4*i);assert v.u32(c)==0x5a3544;capacity=v.u32(c+0xf8);pool=v.u32(c+0x2c);controllers.append(dict(capacity=capacity,pool_sha256=hashlib.sha256(bytes(v.uc.mem_read(pool,252*capacity))).hexdigest()))
  return dict(frame=struct.unpack('<h',v.uc.mem_read(self.owner+0x5c,2))[0],flags=v.u32(self.owner+8),seed=v.u32(self.tls+0x14),rng_calls=self.rng_calls,controllers=controllers)
 def step(self,tick):
  v=self.vm;call=self.world.call
  if v.u32(self.owner+8)&0x1000:return
  v.put_u32(0x65cb38,tick);call(0x470cc0,this=self.owner);call(0x4de800,this=self.owner)
  if not(v.u32(self.owner+8)&0x1000):call(0x40e460,(0,),this=self.animator)
 def render(self):
  self.draws=[];v=self.vm;s=self.software;s.clear();call=self.world.call
  if v.u32(self.owner+8)&0x1000:return v.surface_bytes('screen')
  call(0x416f70,this=0x65a57c);call(0x40e740,this=self.animator);v.call(0x54de20,(s.screen.address,s.depth.address),this=s.camera);v.call(0x54dd70,(0,639,0,479,640),this=s.camera)
  call(0x417d60,(1,1),this=0x65a57c);call(0x40e8d0,this=self.animator,instruction_limit=100000000);call(0x40eb00,this=self.animator);call(0x416fb0,this=0x65a57c);return v.surface_bytes('screen')
def run(exe,boot,out):
 out.mkdir(parents=True,exist_ok=False);cases=[];allcolors=set()
 for quality,light,source in [(0,38,(10000,10000,16)),(0,255,(10000,10000,16)),(3,255,(10000,10000,16)),(0,255,(3185,3185,16))]:
  f=DustLoadedScene(exe,boot,quality,light,source);rows=[]
  for tick in range(25):
   if tick:f.step(tick)
   raw=f.render();colors=[]
   for d in f.draws:
    assert d['fvf']==0x1e2
    colors.extend(struct.unpack_from('<I',bytes.fromhex(d['vertices_hex']),i*32+16)[0]for i in range(d['vertex_count']))
   allcolors.update(colors);row=dict(tick=tick,state=f.state(),draws=len(f.draws),colors=sorted(set(colors)),pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',raw)),pixel_sha256=hashlib.sha256(raw).hexdigest(),draw_sha256=hashlib.sha256(json.dumps(f.draws,sort_keys=True).encode()).hexdigest());rows.append(row)
  case=dict(quality=quality,ambient=light,directional=light,source=source,rows=rows);cases.append(case);print(quality,light,source,sum(r['draws']for r in rows),max(r['pixels']for r in rows),flush=True)
 assert cases[0]['rows']==cases[1]['rows'],'Changing source lighting affected prelit particles'
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read('Imagery/Misc/dustcloud.I3D')
 asset=read_fireball(data);u=lambda p:struct.unpack_from('<I',data,p)[0];r=lambda p:p+u(p);tex=r(asset['body_offset']+40);pixels=r(r(tex+108));texels=set(struct.unpack_from('<1024H',data,pixels));outputs=set()
 # Full authored monotone color envelope, larger than observed packet set:
 # RGB8<=40,30,25 => RGB5<=5,3,3. No normal light multiplies FVF1e2.
 for t in texels:
  for red in range(6):
   for green in range(4):
    for blue in range(4):
     m=[x*y//31 for x,y in zip(((t>>11)&31,(t>>6)&31,t&31),(red,green,blue))];outputs.add(((m[0]&~1)<<11)|(m[1]<<6)|(m[2]&~1))
 assert outputs=={0};assert all(all(r['pixels']==0 for r in c['rows'])for c in cases)
 report=dict(status='pass',accepted=False,visual_pass=False,type_id='0x0c052637',scope=__doc__,cases=cases,distinct_observed_colors=len(allcolors),authored_color_envelope_cases=len(texels)*6*4*4,outputs=sorted(outputs),profile_sha256=sha(ROOT/'src/dustprofile.h'),native_asset_sha256=hashlib.sha256(data).hexdigest(),executable_sha256=sha(exe),boot_image_sha256=sha(boot),source_sha256=sha(__file__),conclusion='Genuine loaded default caller remains blank across quality0/3, source light38/255 and two ordinary/unresident positions. Whole source color envelope quantizes tozero independently of placement. No appearance credit or brightness override.')
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--boot-image',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',a.boot_image,a.output)
