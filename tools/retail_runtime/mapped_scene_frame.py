"""Original full screen ticks on a resident mapped scene.

Optional cache warming is disabled as a declared diagnostic boundary; original
actor updates, drawing, lighting and on-demand decompression remain native.
"""
import argparse,json,struct,traceback
from pathlib import Path
from character_native_preflight import ROOT,sha
from hudworld import HudWorld
from unicorn import UC_PROT_READ,UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP
from software_probe import save_rgb565_png
MAP=0x6668d8;PLAY=0x65caf0
def run(checkpoint,out,ticks=1,draw=False):
 out.mkdir(parents=True,exist_ok=False);w=HudWorld(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',image_path=checkpoint);v=w.vm;pointer_checks=[]
 from native_store_loop import install
 stores=install(v)
 precache=[]
 def skip_cache(uc,*_):
  sp=uc.reg_read(UC_X86_REG_ESP);precache.append(hex(sp));uc.reg_write(UC_X86_REG_EIP,v.u32(sp));uc.reg_write(UC_X86_REG_ESP,sp+4)
 from unicorn.x86_const import UC_X86_REG_EIP
 v.uc.hook_add(UC_HOOK_CODE,skip_cache,begin=0x418890,end=0x418890)
 def readable(args):
  start,size=args;end=start+size
  if size==0:result=0
  elif not start or end>0x100000000:result=1
  else:
   covered=start
   for lo,hi,permissions in sorted(v.uc.mem_regions()):
    if hi<covered:continue
    if lo>covered or not permissions&UC_PROT_READ:break
    covered=min(end,hi+1)
    if covered==end:break
   result=int(covered!=end)
  pointer_checks.append(dict(address=hex(start),bytes=size,bad=result));return result
 assert readable((0,4))==1 and readable((0,0))==0 and readable((0xfffffff0,32))==1 and readable((MAP,4))==0
 v.handlers['IsBadReadPtr']=(2,readable)
 def cursor(args):v.write(args[0],struct.pack('<2i',-32,-32));return 1
 v.handlers['GetCursorPos']=(1,cursor);v.api_dlls['GetCursorPos']={'user32.dll'}
 def client_screen(args):
  hwnd,point=args;window=next(row for row in w.windows.created if row[0]==hwnd);assert window[4:6]==(0,0),window
  return 1
 v.handlers['ClientToScreen']=(2,client_screen);v.api_dlls['ClientToScreen']={'user32.dll'}
 def keyboard(args):v.write(args[0],bytes(256));return 1
 v.handlers['GetKeyboardState']=(1,keyboard);v.api_dlls['GetKeyboardState']={'user32.dll'}
 def translate(args):
  message=v.u32(args[0]+4);assert message not in(0x100,0x101,0x104,0x105),hex(message);return 0
 v.handlers['TranslateMessage']=(1,translate);v.api_dlls['TranslateMessage']={'user32.dll'}
 original_device=w.ddraw._device_method;bg=[0];material=[0]
 for method,args in w.ddraw.device_calls:
  if method=='IDirect3DViewport3::SetBackground':bg[0]=args[0]
  if method=='IDirect3DDevice3::SetLightState'and args[0]==1:material[0]=args[1]
 def device(interface,method,args):
  if interface=='IDirect3DViewport3'and method=='GetBackground':v.put_u32(args[1],bg[0]);v.put_u32(args[2],int(bg[0]!=0));return 0
  if interface=='IDirect3DViewport3'and method=='SetBackground':bg[0]=args[1]
  if interface=='IDirect3DDevice3'and method in('GetLightState','SetLightState')and args[1]==1:
   if method=='GetLightState':v.put_u32(args[2],material[0]);return 0
   material[0]=args[2]
  return original_device(interface,method,args)
 w.ddraw._device_method=device
 draws=[];current=[0];current_controller=[0]
 def object_call(uc,*_):current[0]=v.u32(uc.reg_read(UC_X86_REG_ESP)+4)
 def draw_call(uc,*_):
  sp=uc.reg_read(UC_X86_REG_ESP);args=list(struct.unpack('<7I',uc.mem_read(sp+4,28)));draws.append(dict(controller=hex(current_controller[0]),object=hex(current[0]),args=args,active=bytes(v.uc.mem_read(0x676055,1)).hex(),center=[v.u32(0x675fc0),v.u32(0x675fc4)],world=list(struct.unpack('<16f',v.uc.mem_read(0x675ed0,64)))))
 v.uc.hook_add(UC_HOOK_CODE,object_call,begin=0x40a8f0,end=0x40a8f0);v.uc.hook_add(UC_HOOK_CODE,draw_call,begin=0x56eb30,end=0x56eb30)
 controller_calls=[]
 def controller_call(uc,address,*_):
  from unicorn.x86_const import UC_X86_REG_ECX
  obj=uc.reg_read(UC_X86_REG_ECX);controller_calls.append(dict(entry=hex(address),this=hex(obj)))
  if address==0x404270:
   current_controller[0]=obj;ret=v.u32(uc.reg_read(UC_X86_REG_ESP))
   def returned(uc,*_):current_controller[0]=0
   v.uc.hook_add(UC_HOOK_CODE,returned,begin=ret,end=ret)
 for address in(0x403760,0x404270,0x40e8d0):v.uc.hook_add(UC_HOOK_CODE,controller_call,begin=address,end=address)
 calls={}
 def observed(uc,address,*_):calls[hex(address)]=calls.get(hex(address),0)+1
 for address in(0x454390,0x4541c0,0x454450,0x48fda0,0x48f180,0x47b4d0,0x48f450):v.uc.hook_add(UC_HOOK_CODE,observed,begin=address,end=address)
 hwnd=v.u32(0x65b8ec);v.post_message(hwnd,0x1c,1,0);v.post_message(hwnd,0x6,1,0)
 frames=[];failure=None
 try:
  for tick in range(1,ticks+1):
   for timer,(callback,period,user)in w.system.timers.items():
    assert callback==0x49e2f0 and period==41;v.sleep_ms+=period;w.call(callback,(int(timer),0,user,0,0));print('delivered original WinMM',hex(callback),period,'clock',float(v.elapsed_ms),flush=True)
   draws.clear()
   print('TICK',tick,'screenframe',v.u32(PLAY+0x48),'timer',hex(v.u32(v.u32(PLAY)+0x44)),flush=True);w.call(0x4911b0,(int(draw),),this=PLAY,instruction_limit=1000000000)
   display=v.u32(0x5d79e0);surfaces=[]
   for name,p in [('display',display),('back',v.u32(display+0x8c))]:
    surface=w.ddraw.surface(v.u32(p+0x68));raw=b''.join(bytes(v.uc.mem_read(surface.memory+y*surface.pitch,surface.width*2))for y in range(surface.height));image=out/f'{name}-{v.u32(PLAY+0x48):03d}.png';save_rgb565_png(image,raw,surface.width,surface.height);surfaces.append(dict(name=name,width=surface.width,height=surface.height,pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',raw)),image=str(image),sha256=sha(image)))
   rows=[]
   for i in range(v.u32(0x668578)):
    s=v.u32(v.u32(0x668588)+4*i)
    for slot in range(v.u32(s+0xb8)):
     o=v.u32(v.u32(s+0xc8)+4*slot)
     if o and struct.unpack('<H',v.uc.mem_read(o+4,2))[0]in(11,12,17,25):
      state=struct.unpack('<H',v.uc.mem_read(o+12,2))[0];im=v.u32(o+0x54)
      animator=v.u32(o+0x58);animator_vtable=v.u32(animator)if animator else 0
      rows.append(dict(index=v.u32(o+0x40),owner=hex(o),position=list(struct.unpack('<3i',v.uc.mem_read(o+0x10,12))),state=state,frame=struct.unpack('<h',v.uc.mem_read(o+0x5c,2))[0],frame_increment=struct.unpack('<h',v.uc.mem_read(o+0x5e,2))[0],asset=v.string(v.u32(im+4)),clip_frames=w.call(v.u32(v.u32(im)+0x90),(state,),this=im),animator_vtable=hex(animator_vtable),flame_internal_frame=v.u32(animator+0xfc)if animator_vtable==0x5a9dec else None))
   frames.append(dict(tick=tick,draws=list(draws),screen_frame=v.u32(PLAY+0x48),surfaces=surfaces,targets=rows));print('PASS',tick,'screen',frames[-1]['screen_frame'],'draws',len(draws),'pixels',[x['pixels']for x in surfaces],'actors',len(rows),flush=True)
 except Exception as e:failure=dict(error=str(e),fault=w.last_fault,trace=traceback.format_exc());print('FAIL',failure,flush=True)
 w.save(out/('screen-gate.image'if failure else'after-frame.image'))
 r=dict(controller_calls=controller_calls,function_calls=calls,map_core_hex=bytes(v.uc.mem_read(MAP,0x160)).hex(),multimedia_timers=w.system.timers,thread_waits={tid:dict(state=t.state,wait=t.wait,wake=str(t.wake))for tid,t in v.scheduler.threads.items()},ambient_hex=bytes(v.uc.mem_read(MAP+0x1b0,256)).hex(),scheduler_trace=v.scheduler.trace[-20:],cursor_control=[-32,-32],status='genuine_full_map_frame_gate'if failure else'genuine_full_map_frames',accepted=False,failure=failure,frames=frames,pointer_checks=pointer_checks,checkpoint_sha256=sha(checkpoint),probe_sha256=sha(__file__),device_calls=w.ddraw.device_calls,draw=draw,skipped_bitmap_precache=True,store_acceleration={k:value for k,value in stores.items()if not k.endswith('hook')},scope=__doc__);(out/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');return r
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--checkpoint',type=Path,required=True);p.add_argument('--output',type=Path,required=True);p.add_argument('--ticks',type=int,default=1);p.add_argument('--draw',action='store_true');a=p.parse_args()
 if a.ticks<1:p.error('--ticks must be positive')
 result=run(a.checkpoint,a.output,a.ticks,a.draw);raise SystemExit(1 if result['failure'] else 0)
