"""Original full-render light registration controls across supported viewports."""
import argparse,json,struct
from pathlib import Path
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EBP
from nakrnoth_visual_reference import NakrnothScene
from character_native_preflight import ROOT,sha

def run(boot,output):
 output.mkdir(parents=True,exist_ok=False);cases=[]
 for width,height in((1280,960),(1920,1080)):
  f=NakrnothScene(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',boot,width,height);v=f.vm;draws=[]
  def transformed(uc,*unused):
   sp=uc.reg_read(UC_X86_REG_ESP);count=uc.reg_read(UC_X86_REG_EBP)
   if f.current_object in(33,34):
    rows=list(struct.iter_unpack('<3f4I2f',uc.mem_read(v.u32(0x675fb4),count*36)))
    draws.append(dict(object=f.current_object,transformed=rows,draw=f.draws[-1]))
   # Complete authored transform/Illuminate, omit only terminal raster writes.
   v.put_u32(sp+0x84,0)
  v.uc.hook_add(UC_HOOK_CODE,transformed,begin=0x56f10f,end=0x56f10f)
  # Diagnostic selected pose only; complete lifetime has independent790tick proof.
  v.write(f.owner+0x5c,struct.pack('<h',641));v.put_u32(0x65cb38,641);f.raster_enabled=True
  passes=[];extra=v.allocate(80);record=bytearray(80);struct.pack_into('<2I4f',record,0,80,3,1,1,1,1);struct.pack_into('<3f',record,0x24,0,-.78125,-.625);v.write(extra,bytes(record))
  for label,expected in(('single',23),('duplicate-negative',31),('removed-repeat',23)):
   if label=='duplicate-negative':v.call(0x56cf00,(extra,))
   if label=='removed-repeat':v.call(0x56d120,(extra,))
   draws.clear();f.render();state=f.state();assert len(draws)==2
   assert all(row[3:7]==(expected,expected,expected,31)for d in draws for row in d['transformed'])
   passes.append(dict(label=label,expected_rgb5=expected,state=state,draws=list(draws)))
  assert passes[0]['state']==passes[1]['state']==passes[2]['state']
  assert passes[0]['draws']==passes[2]['draws']
  assert [d['draw']for d in passes[0]['draws']]==[d['draw']for d in passes[1]['draws']]
  cases.append(dict(viewport=[width,height],passes=passes));print(width,height,'23 ->31 ->23 PASS',flush=True)
 result=dict(status='pass',type_id='0xad92bd21',cases=cases,executable_sha256=sha(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe'),boot_sha256=sha(boot),scope='Actual loaded typed owner/resource/default Animate/PreRender/Render at declared diagnostic frame641. No normal or color injection; original transform/Illuminate observed at56f10f. Additional original directional registration56cf00 and removal56d120 are explicit negative/recovery controls. All authored draw packets, owner matrices/flags/frame/RNG remain unchanged. Original normalizes scaled ring normals. One registered directional produces RGB5=23 at both supported viewport sizes; a duplicate produces31. Complete lifetime independently retained.')
 (output/'manifest.json').write_text(json.dumps(result,indent=2)+'\n');return result
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--boot-image',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.boot_image,a.output)
