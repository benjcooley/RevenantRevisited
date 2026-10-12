"""Original ArrowEffect following an original weightless Move caller.

The declared physical source moves at20 units/tick until original Move detects
our terrain plane. The fixture then supplies the actual source-deletion event;
original Notify decides the follower's kill. No host per-tick pose or timer.
"""
from arrow_visual_reference import *
class ArrowScene(QueenArrowScene):
 owner_vtable,owner_init,registry_name,builder_vtable=0x5b0d68,0x5048f0,'ArrowEffect',0x5b0f70
 owner_pulse,anim_factory,anim_vtable,anim_init,anim_pulse,anim_render,builder=(0x504920,0x509870,0x5b0f74,0x504a50,0x40e2e0,0x504a70,0x66d188)
 def setup_resources(self):
  FireBallScene.setup_resources(self);v=self.vm;v.put_u32(self.imagery+0x10,0);self.events=[];self.deleted=False
  v.write(self.missile.source+0x10,struct.pack('<3i',-180,0,78));v.write(self.missile.source+0x36,bytes([64]));v.put_u32(self.owner+0xe4,self.missile.source);v.put_u32(self.owner+0xe8,0xffffffff)
  v.write(self.missile.source+0x1c,struct.pack('<3i',20*65536,0,0));v.put_u32(self.missile.source+8,65536)
 def step(self):
  if not self.deleted:
   self.missile.wall_x=180
   moved=self.vm.call(0x470920,this=self.missile.source,instruction_limit=5000000)
   if moved&2:
    self.deleted=True;self.events.append(dict(event='source-deleted-after-original-move-blocked'))
    self.vm.call(0x4df0f0,(1,self.missile.source),this=self.owner,instruction_limit=5000000)
  super().step()
 def state(self):
  s=super().state();s['source_position']=list(struct.unpack('<3i',self.vm.uc.mem_read(self.missile.source+0x10,12)));s['deleted']=self.deleted;s['attached']=bool(self.vm.u32(self.owner+0xe4));return s
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('exe',type=Path);p.add_argument('--output',type=Path,required=True);p.add_argument('--frames',type=int,default=26);p.add_argument('--state-only',action='store_true');p.add_argument('--render-stride',type=int,default=1);p.add_argument('--gallery-direction',action='store_true',help='Actual review path delta(-170,-219), sourceZ144; original facing tables and physical Move');p.add_argument('--samples',type=int,nargs='+',default=list(range(19))+[22,25]);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read('Imagery/Misc/arroweffects.i3d')
 f=ArrowScene(a.exe,data,width=640,height=480,center=(320,320));rows=[]
 source=[-180,0,78];target=[180,0,78];facing=64;velocity=[20*65536,0,0]
 if a.gallery_direction:
  # Algebraic XY relocation of the logged gallery endpoints; actual Z stays144.
  source=[0,0,144];target=[-170,-219,144];v=f.vm
  angle_table=bytes(v.uc.mem_read(0x63da64,65536));facing=256-(64-angle_table[170*256+219])
  dx,dy=struct.unpack('<256h',f.missile.lookup[:512]),struct.unpack('<256h',f.missile.lookup[512:])
  velocity=[dx[facing]*20*256,dy[facing-128 if facing>128 else 128-facing]*20*256,0]
  v.write(f.missile.source+0x10,struct.pack('<3i',*source));v.write(f.missile.source+0x36,bytes([facing]));v.write(f.missile.source+0x1c,struct.pack('<3i',*velocity));f.missile.terrain_plane=(source,target)
 before=f.state();f.vm.call(0x4df0f0,(1,f.missile.target),this=f.owner);assert f.state()==before # Nonmatching deletion must not kill this attachment.
 for tick in range(a.frames):
  f.step()
  if not(f.vm.u32(f.owner+8)&4096):f.animate()
  f.raster_enabled=not a.state_only and tick in a.samples
  if tick%a.render_stride==0:pixels=f.render()
  else:f.draws=[];f.software.clear();pixels=f.vm.surface_bytes('screen')
  if f.raster_enabled:save_rgb565_png(a.output/f'native-{tick:03}.png',pixels,640,480)
  rows.append(dict(tick=tick,state=f.state(),draws=f.draws));print(tick,f.state(),len(f.draws),flush=True)
 (a.output/'trace.json').write_text(json.dumps(dict(effect='ArrowEffect',records=rows,asset_sha256=sha(data),retail_sha256=sha(a.exe.read_bytes()),viewport=[640,480],center=[320,320],source=source,target=target,facing=facing,velocity_fixed=velocity,terrain_plane=bool(a.gallery_direction),ambient=[38]*3,directional=[1]*3,seed=1,render_hz=24,render_stride=a.render_stride,scope='Original weightless source Move470920, actual follower504920/base4de800/customRender504a70, normal lighting and software raster. Declared source velocity20 from original initialized facing tables; terrain plane or legacywallX180, then explicit N_DELETINGOBJECT event to original4df0f0. Attachment +e4 and indefinite+e8 are caller inputs. Gallery direction uses algebraicXY relocation of logged endpoints, not a pose/geometry fit. No natural physical-arrow/bow/actor or map-deletion-dispatch claim.'),indent=2)+'\n')
