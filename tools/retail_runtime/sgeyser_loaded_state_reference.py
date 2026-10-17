"""Genuine loaded sgeyser inline controller, with failed optional vapor filename.

Full original owner/controller/render runs in a private startup VM. Coordinates
are explicit inputs; sector/script/trap initialization is a separate boundary.
"""
import argparse,json,struct,hashlib
from types import SimpleNamespace
from pathlib import Path
from character_native_preflight import AuthenticCharacters,ROOT
from lit_software_probe import LitSoftwareFixture
from unicorn.x86_const import UC_X86_REG_EIP,UC_X86_REG_ESP,UC_X86_REG_ECX,UC_X86_REG_EAX
from unicorn import UC_HOOK_CODE
from software_probe import save_rgb565_png
def run(boot_image, output, ticks, source, samples, center=(640,480)):
    B=output;B.mkdir(parents=True,exist_ok=False);boot=boot_image;source=tuple(source);args=SimpleNamespace(ticks=ticks,samples=samples);assert len(source)==3 and args.ticks>0;f=AuthenticCharacters(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',boot);f.select_screen_prefix();v=f.vm;requests=[];argc,original=v.handlers['CreateFileA']
    def opened(args):
     result=original(args);requests.append(dict(path=v.string(args[0]),result=result));return result
    v.handlers['CreateFileA']=(argc,opened);report=dict(status='genuine_placed_coordinate_initialization',accepted=False,source=list(source),executable_sha256=hashlib.sha256((ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe').read_bytes()).hexdigest(),boot_sha256=hashlib.sha256(boot.read_bytes()).hexdigest(),probe_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),viewport=[1280,960],center=list(center),scope='Actual shipped sgeyser generic class factory at explicit declared position, publicCreateAnimator and originalfilenamepresetloader; no sampledsector/flags or parser replacement. Map residency/script/naturalrender not yet claimed.')
    try:
     software=LitSoftwareFixture(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',1280,960,camera=(*source[:2],0),vm=v);software.lighting((38,)*3,(1,)*3);owner=f.new_object('sgeyser',source,25,0x5a85ac);report['create_result']=f.world.call(0x46e8b0,this=owner,instruction_limit=100000000);a=v.u32(owner+0x58);report['animator']=hex(a);assert a and v.u32(a)==0x5a370c;report['controllers']=[hex(v.u32(v.u32(a+0x68)+i*4))for i in range(v.u32(a+0x58))]
     if a:
      assert len(report['controllers'])==1
      report['controller_fields']=[dict(address=hex(c),vtable=hex(v.u32(c)),objects=v.u32(c+0x18),capacity=v.u32(c+0xf8),header=bytes(v.uc.mem_read(c,0x168)).hex())for c in [v.u32(v.u32(a+0x68)+i*4)for i in range(v.u32(a+0x58))]]
      assert report['controller_fields'][0]['objects']==1 and report['controller_fields'][0]['capacity']==104
      pulse_inputs=[];rng_calls=[0];ground=[];pending=[];returns=set();states={};current_object=[-1]
      def rng(uc,*_):rng_calls[0]+=1
      v.uc.hook_add(UC_HOOK_CODE,rng,begin=0x58c582,end=0x58c582)
      def pulse(uc,*_):
       c=uc.reg_read(UC_X86_REG_ECX);emitters=[]
       for i in range(v.u32(c+0x18)):
        e=v.u32(v.u32(c+0x28)+4*i);emitters.append(dict(position=list(struct.unpack('<3f',uc.mem_read(e+0x10,12))),scale=list(struct.unpack('<3f',uc.mem_read(e+0x40,12))),matrix=list(struct.unpack('<16f',uc.mem_read(e+0x58,64)))))
       pulse_inputs.append(dict(frame=v.u32(a+0x14),position=list(source),origin=list(struct.unpack('<3f',uc.mem_read(c+0x170,12))),emitters=emitters))
      def ground_exit(uc,address,*_):
       if pending and pending[-1][0]==address:
        _,row=pending.pop();row['result']=uc.reg_read(UC_X86_REG_EAX)
      def ground_entry(uc,*_):
       sp=uc.reg_read(UC_X86_REG_ESP);ret=v.u32(sp);point=v.u32(sp+4);row=dict(point=list(struct.unpack('<3i',uc.mem_read(point,12))),caller=hex(ret));ground.append(row);pending.append((ret,row))
       if ret not in returns:returns.add(ret);uc.hook_add(UC_HOOK_CODE,ground_exit,begin=ret,end=ret)
      def state_entry(uc,*_):
       sp=uc.reg_read(UC_X86_REG_ESP);states[v.u32(sp+4)]=v.u32(sp+8)
      def object_entry(uc,*_):
       sp=uc.reg_read(UC_X86_REG_ESP);obj=v.u32(sp+4);current_object[0]=v.u32(obj+4)
      for address,fn in [(0x403760,pulse),(0x452e10,ground_entry),(0x56d400,state_entry),(0x40a8f0,object_entry)]:v.uc.hook_add(UC_HOOK_CODE,fn,begin=address,end=address)
      saved_ecx=v.uc.reg_read(UC_X86_REG_ECX);v.uc.reg_write(UC_X86_REG_ECX,v.u32(v.u32(a+0x68)));pulse(v.uc);v.uc.reg_write(UC_X86_REG_ECX,saved_ecx);report['initial_inputs']=pulse_inputs.copy();pulse_inputs.clear()
      material=[0];origdev=f.world.ddraw._device_method
      def device(interface,method,args):
       if interface=='IDirect3DDevice3'and method in('GetLightState','SetLightState')and args[1]==1:
        if method=='GetLightState':v.put_u32(args[2],material[0])
        else:material[0]=args[2]
        return 0
       return origdev(interface,method,args)
      f.world.ddraw._device_method=device;draws=[];f.world.call(0x58c575,(1,));tls=f.world.call(0x58fbf1);report['seed_after_initialization']=1
      def draw(uc,*_):
       sp=uc.reg_read(UC_X86_REG_ESP);args=[v.u32(sp+4+i*4)for i in range(7)];draws.append(dict(object=current_object[0],scene_mode=v.u32(0x5e8790),requested_mode=v.u32(0x5e88b4),states=states.copy(),arguments=args,vertices_hex=bytes(uc.mem_read(args[2],args[3]*32)).hex(),matrix=list(struct.unpack('<16f',uc.mem_read(0x675ed0,64)))))
      v.uc.hook_add(UC_HOOK_CODE,draw,begin=0x56eb30,end=0x56eb30);rows=[]
      for tick in range(1,args.ticks+1):
       v.put_u32(0x65cb38,tick);pulse_inputs.clear();ground.clear()
       if not v.u32(owner+8)&0x1000:
        f.world.call(0x470cc0,this=owner);f.world.call(0x4de800,this=owner)
       software.clear();draws.clear()
       if not v.u32(owner+8)&0x1000:
        f.world.call(0x40e460,(0,),this=a);f.world.call(0x416f70,this=0x65a57c);f.world.call(0x40e740,this=a);v.call(0x54de20,(software.screen.address,software.depth.address),this=software.camera);v.call(0x54dd70,(0,1279,0,959,1280),this=software.camera);v.put_u32(0x675fc0,center[0]);v.put_u32(0x675fc4,center[1]);f.world.call(0x40e8d0,this=a,instruction_limit=100000000);f.world.call(0x40eb00,this=a);f.world.call(0x416fb0,this=0x65a57c)
       raw=v.surface_bytes('screen');image=B/f'sgeyser-wide-{tick:03d}.png'
       if tick in args.samples:save_rgb565_png(image,raw,1280,960)
       controller=v.u32(v.u32(a+0x68));pool=v.u32(controller+0x2c);capacity=v.u32(controller+0xf8);rows.append(dict(rng_calls=rng_calls[0],pulse_inputs=pulse_inputs.copy(),ground=ground.copy(),seed=v.u32(tls+0x14),pool_hex=bytes(v.uc.mem_read(pool,capacity*252)).hex(),emission_credit=struct.unpack('<f',v.uc.mem_read(controller+0x16c,4))[0],tick=tick,frame=struct.unpack('<h',v.uc.mem_read(owner+0x5c,2))[0],flags=v.u32(owner+8),draws=draws.copy(),nonblack=sum(x!=(0,)for x in struct.iter_unpack('<H',raw)),image=str(image),image_sha256=hashlib.sha256(image.read_bytes()).hexdigest()if image.exists()else None))
      report['rows']=rows;print(tick,len(draws),flush=True);report['status']='genuine_loaded_state_reference_complete';report['type_id']='0xad92bd30'
    except Exception as e:report.update(status='genuine_initialization_failed',error=str(e),eip=hex(v.uc.reg_read(UC_X86_REG_EIP)))
    report['file_requests']=requests;(B/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],len(report.get('rows',[])),[r['nonblack']for r in report.get('rows',[])if r['image_sha256']])
    return report

if __name__=="__main__":
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--boot-image',type=Path,required=True);parser.add_argument('--output',type=Path,required=True);parser.add_argument('--ticks',type=int,default=120);parser.add_argument('--center',default='640,480');parser.add_argument('--source',default='1019,11935,289');parser.add_argument('--samples',type=int,nargs='*',default=[1,2,3,6,12,20,30,44,45,46,60,89,90,120]);args=parser.parse_args()
    run(args.boot_image,args.output,args.ticks,tuple(map(int,args.source.split(','))),args.samples,tuple(map(int,args.center.split(','))))
