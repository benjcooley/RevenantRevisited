"""Fixed supported four-view coverage of Nakrnoth's late expansion."""
import argparse,json,hashlib,struct
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EFLAGS
from pathlib import Path
from nakrnoth_visual_reference import NakrnothScene,ROOT,sha,save_rgb565_png
VIEWS=((1440,920),(480,920),(1440,120),(480,120))
def run(executable,boot,baseline,output,views=VIEWS,guard_control=False):
 output.mkdir(parents=True,exist_ok=False);f=NakrnothScene(executable,boot,1920,1080,views[0],True);v,call=f.vm,f.world.call
 original={r['tick']:r for r in json.loads(baseline.read_text())['records']};rows=[];guards=[];negative=[]
 def observe(uc,address,*unused):guards.append(dict(object=f.current_object,address=hex(address),reject=bool(uc.reg_read(UC_X86_REG_EFLAGS)&64)))
 if guard_control:
  for address in(0x56e006,0x56e036,0x56e05c,0x56e082,0x56e36a,0x56e395,0x56e3bb,0x56e3e1,0x56e6f1,0x56e71c,0x56e742,0x56e768):v.uc.hook_add(UC_HOOK_CODE,observe,begin=address,end=address)
 for tick in range(1,702):
  v.put_u32(0x65cb38,tick);call(0x470cc0,this=f.owner);call(0x4de800,this=f.owner);f.raster_enabled=tick in(601,621,641,661,681,701)
  images=[];control=None
  for view,center in enumerate(views if f.raster_enabled else views[:1]):
   f.center=center;guards.clear();pixels=f.render();state=f.state();pool=f.controller_state()
   identity=(state['seed'],state['rng_draws'],state['flags'],state['frame'],pool)
   if control is None:control=identity
   else:assert control==identity,'A repeated Render changed native controller/RNG/lifetime'
   if f.raster_enabled:
    name=f'native-{tick:03}-view{view}.png';save_rgb565_png(output/name,pixels,1920,1080);images.append(dict(view=view,center=list(center),image=name,sha256=sha(output/name),bounds=f.draw_bounds(),draws=f.draws))
  if guard_control and tick==661:
   original_draws=list(f.draws);original_guards=list(guards);original_state=f.state();original_pool=f.controller_state()
   name='guard-original.png';save_rgb565_png(output/name,pixels,1920,1080);negative.append(dict(name=name,sha256=sha(output/name),guards=original_guards))
   v.write(0x5b9cc0,struct.pack('<f',6400));v.write(0x5b9cbc,struct.pack('<f',4800));guards.clear();wide=f.render()
   assert f.state()==original_state and f.controller_state()==original_pool and f.draws==original_draws
   assert any(g['reject']for g in original_guards) and not any(g['reject']for g in guards)
   name='guard-widened-negative.png';save_rgb565_png(output/name,wide,1920,1080);negative.append(dict(name=name,sha256=sha(output/name),guards=list(guards)))
   v.write(0x5b9cc0,struct.pack('<f',640));v.write(0x5b9cbc,struct.pack('<f',480))
  expected=original[tick]['state'];assert all(state[k]==expected[k]for k in('seed','rng_draws','flags','frame'))
  rows.append(dict(tick=tick,seed=state['seed'],rng_draws=state['rng_draws'],flags=state['flags'],frame=state['frame'],views=images))
  if tick%50==0 or images:print(tick,len(images),flush=True)
 report=dict(type_id='0xad92bd21',records=rows,views=[list(v)for v in views],guard_negative_control=negative,viewport=[1920,1080],source=[3185,3185,16],camera=[3185,3185,0],baseline_sha256=sha(baseline),executable_sha256=sha(executable),boot_sha256=sha(boot),scope='Same actual loaded owner/Pulse/Animate/fullRender. Fixed 2x2 view origins cover the late expansion. Repeated same-tick view renders prove identical particle pool/RNG/flags/frame; each tick RNG equals independent original whole run. Raw windows retained, no overlap/equality claim. Terminal unsampled raster omitted.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--boot-image',type=Path,required=True);p.add_argument('--baseline',type=Path,required=True);p.add_argument('--output',type=Path,required=True);p.add_argument('--middle',action='store_true');p.add_argument('--guard-control',action='store_true');a=p.parse_args();run(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',a.boot_image,a.baseline,a.output,((1440,520),(480,520))if a.middle else VIEWS,a.guard_control)
