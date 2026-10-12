"""Genuine cfire failed-preset path and complete installed base render."""
import argparse,json,struct,zipfile
from pathlib import Path
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EAX
from character_native_preflight import ROOT,sha
from fire_authentic_preflight import FireScene
from software_probe import save_rgb565_png
from speed_controller_preflight import read_asset

class CFireScene(FireScene):
 def new_object(self,name,*args,**kwargs):
  v=self.vm;self.file_requests=[];self.preset_opens=[];pending={}
  argc,original=v.handlers['CreateFileA']
  def opened(args):
   result=original(args);self.file_requests.append(dict(path=v.string(args[0]),disposition=args[4],result=hex(result&0xffffffff)));return result
  v.handlers['CreateFileA']=(argc,opened)
  def fopen(uc,*unused):
   sp=uc.reg_read(UC_X86_REG_ESP);path=v.string(v.u32(sp+4));ret=v.u32(sp);row=dict(path=path,mode=v.string(v.u32(sp+8)));self.preset_opens.append(row);pending.setdefault(ret,[]).append(row)
   if len(pending[ret])==1:
    def returned(uc,*unused,ret=ret):
     if pending[ret]:pending[ret].pop()['result']=hex(uc.reg_read(UC_X86_REG_EAX))
    v.uc.hook_add(UC_HOOK_CODE,returned,begin=ret,end=ret)
  v.uc.hook_add(UC_HOOK_CODE,fopen,begin=0x4a1240,end=0x4a1240)
  return super().new_object('cfire',*args,**kwargs)

def run(boot,output,frames=64):
 output.mkdir(parents=True,exist_ok=False);f=CFireScene(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',boot);v=f.vm;call=f.world.call;rows=[]
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/misc/cfire.i3d'))
 asset=read_asset(data);controllers=[hex(v.u32(v.u32(v.u32(f.animator+0x68)+i*4)))for i in range(v.u32(f.animator+0x58))];print('CONTROLLERS',controllers,'FILES',f.preset_opens,flush=True)
 for tick in range(1,frames+1):
  v.put_u32(0x65cb38,tick);call(0x470cc0,this=f.owner);call(0x4de800,this=f.owner);f.raster_enabled=True;pixels=f.render();row=dict(tick=tick,state=f.state(),draws=f.draws,bounds=f.draw_bounds(),nonzero_pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',pixels)))
  if tick in(1,2,9,15,29,30,31,45,60,64):
   name=f'native-{tick:03}.png';save_rgb565_png(output/name,pixels,f.width,f.height);row.update(image=name,sha256=sha(output/name))
  rows.append(row);print(tick,row['state']['frame'],row['state']['flags'],len(f.draws),row['nonzero_pixels'],flush=True)
 report=dict(effect='cfire',type_id='0xad92bd27',asset=asset,controllers=controllers,file_requests=f.file_requests,preset_opens=f.preset_opens,records=rows,viewport=[1280,960],center=[640,480],source=[3185,3185,16],camera=[3185,3185,0],ambient=[38,38,38],directional=[1,1,1],executable_sha256=sha(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe'),boot_sha256=sha(boot),scope='Actual cached loaded class factory/default CreateAnimator and exact Misc/cfire.I3D. Original filename opens and file API outcomes observed without replacing parser/controller/geometry. Complete NextFrame/whole owner Pulse/Animate/BeginScene/PreRender/Render/PostRender/EndScene everytick. Declared fixed null-spell owner/empty-map prefix/RAM viewport/light. Expected absentCFire/CSparks presets, natural callers/map/device/exactpixels remain open.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--boot-image',type=Path,required=True);p.add_argument('--output',type=Path,required=True);p.add_argument('--frames',type=int,default=64);a=p.parse_args();run(a.boot_image,a.output,a.frames)
