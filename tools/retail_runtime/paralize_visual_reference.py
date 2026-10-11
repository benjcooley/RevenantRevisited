import sys,zipfile,json,struct
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parent))
from cataclysm_visual_reference import *
class ParalizeScene(CataclysmScene):
 owner_vtable,owner_init,registry_name,builder_vtable=0x5adca4,0x4fa530,'Paralize1',0x5adeb0
 owner_pulse,anim_factory,anim_vtable,anim_init,anim_pulse,anim_render,builder=(0x4fadb0,0x500050,0x5adeb4,0x4fb600,0x40e2e0,0x4fb650,0x66cf78)
 def setup_resources(self):
  FireBallScene.setup_resources(self);v=self.vm
  spell=v.allocate(0x200);variant=v.allocate(0x100);v.write(variant+4,b'Physical Paralysis\0');v.put_u32(spell+0x120,variant);v.put_u32(self.owner+0xd8,spell)
 def state(self):
  v=self.vm;return dict(flags=v.u32(self.owner+8),owner_hex=bytes(v.uc.mem_read(self.owner+0x184,0x548-0x184)).hex(),phase=v.u32(self.owner+0x3ac),variant=v.u32(self.owner+0x404),seed=v.rng_seed,rng_draws=self.rng_draws)
def run(output,frames=32,state_only=False):
 output.mkdir(parents=True,exist_ok=False)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/paralize.i3d'))
 f=ParalizeScene(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data);initial=f.state();rows=[]
 for t in range(frames):
  f.step();f.animate();f.raster_enabled=not state_only and t in(0,1,4,8,9,10,12,15,18,19,20,21,24,29,31);raw=f.render();state=f.state();row=dict(tick=t,state=state,draw_count=len(f.draws))
  if f.raster_enabled:
   p=output/f'native-{t:03}.png';save_rgb565_png(p,raw,1280,960);row.update(pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',raw)),image=str(p.resolve()),image_sha256=sha(p.read_bytes()),draws=f.draws)
  rows.append(row);print(t,len(f.draws),state['variant'],state['phase'],hex(state['flags']),flush=True)
 report=dict(initial=initial,records=rows,asset_sha256=sha(data),type_id='0xad92bc14',declared_spell='Physical Paralysis',invoker=None,accepted=False,executable_sha256=sha((ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe').read_bytes()),scope='Actual typed owner Initialize4fa530/first-Pulse Initialize4fa540/completePulse4fadb0, customanimatorInit4fb600/completeRender4fb650/RenderObject/originalsoftware. Named Physical Paralysis spell descriptor with nullinvoker and zero actualtargets; no fabricated actors/bones. Immutable decodedresource/COM, baseanimatorInitialize and script/map notifications, camera0/lights38dir1 are explicit boundaries. No target-bound egg/actor/damage/audio/device/exactpixels claim.')
 (output/'trace.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);p.add_argument('--frames',type=int,default=32);p.add_argument('--state-only',action='store_true');a=p.parse_args();run(a.output,a.frames,a.state_only)
