"""Original named Essence Drain spell's ManaDrain owner, actor APIs and pixels.

The separate Essencedrain/Worm prototype is never used or credited here.
"""
import argparse,json,struct,hashlib
from pathlib import Path
from manadrain_target_visual_reference import ManaDrainTargetScene
from character_native_preflight import ROOT,sha
from blood_actor_preflight import BOOT
from software_probe import save_rgb565_png
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EIP,UC_X86_REG_EAX

def run(out,selected=(1,20,21,40,70,90,91,100)):
 out.mkdir(parents=True,exist_ok=False)
 exe=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe'
 f=ManaDrainTargetScene(exe,BOOT,spell_name='Essence Drain');v=f.vm;rows=[];tick=[0]
 def handoff(uc,*_):
  if tick[0]in selected:return
  sp=uc.reg_read(UC_X86_REG_ESP);uc.reg_write(UC_X86_REG_EIP,v.u32(sp));uc.reg_write(UC_X86_REG_ESP,sp+32);uc.reg_write(UC_X86_REG_EAX,0)
 v.uc.hook_add(UC_HOOK_CODE,handoff,begin=0x56eb30,end=0x56eb30)
 for t in range(1,101):
  tick[0]=t;v.put_u32(0x65cb38,t);f.step();raw=f.render();row={'tick':t,'state':f.state(),'draws':f.draws}
  if t in selected:
   image=out/f'native-{t:03d}.png';save_rgb565_png(image,raw,1280,960);row.update(image=str(image),image_sha256=sha(image),pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',raw)));print(t,row['state']['stage'],len(f.draws),row['pixels'],flush=True)
  rows.append(row)
 report={'status':'complete_named_spell_reference','accepted':False,'type_id':'0xad92bc13','spell':'Essence Drain','variant':f.spell_variant,'executable_sha256':sha(exe),'boot_image_sha256':sha(BOOT),'probe_sha256':sha(__file__),'records':rows,'source':[3185,3185,16],'target':[3281,3185,16],'viewport':[1280,960],'center':[640,640],'camera':[3185,3185,0],'lighting':{'ambient':[38]*3,'directional':[1]*3},'scope':'Actual shipped Essence Drain variant110..115 selects ManaDrain, genuine Acolytes/fighting target, original caster/mana APIs/full owner/animator/Render/RNG. Selected complete original software raster phases; other ticks preserve full draw packets/device handoff. Same existing declared isolated camera/light/material-handle interfaces; no fake Worm motion, actor pose, damage, geometry or original instruction patches. Natural map/audio/device/exact pixels remain separate.'}
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.output)
