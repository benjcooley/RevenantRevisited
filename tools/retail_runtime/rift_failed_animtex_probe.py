#!/usr/bin/env python3
"""Reproduce Rift1's unmodified animtex parse failure and bounded native deletion fault.

No repaired tag, rendered/static acceptance, destructor interception or world boot.
"""
import argparse,json,struct,zipfile
from pathlib import Path
from unicorn import UC_HOOK_CODE,UC_HOOK_MEM_READ_UNMAPPED,UcError
from unicorn.x86_const import UC_X86_REG_EAX,UC_X86_REG_ECX,UC_X86_REG_EIP
from scrolltex_water_probe import ScrollFixture,ROOT
from software_probe import RETAIL_SHA
from fizzle_probe import sha
ASSET_SHA='58fbb47a8d293744458363f73996e971223118a53e3c4abc76ddc9de17b1263f'


def setup(executable,asset):
    if sha(asset)!=ASSET_SHA:raise ValueError('Pinned Rift1 asset changed')
    f=ScrollFixture(executable);v=f.vm;r=lambda a:a+struct.unpack_from('<I',asset,a)[0];body=20+struct.unpack_from('<I',asset,16)[0];obj=r(body+48);tag=r(body+56);vertices=r(r(r(body+16)))
    if struct.unpack_from('<2i',asset,tag)!=(0,0)or asset[r(tag+8):].split(b'\0')[0]!=b'animtex':raise ValueError('Native tag contract changed')
    text=asset[r(tag+12):].split(b'\0')[0]
    if text!=b'obj=rift,u=4,d=4':raise ValueError('Do not rewrite the invalid authored parameter')
    image_obj=v.u32(v.u32(f.context+0x74));v.write(image_obj,asset[obj:obj+32]);v.put_u32(f.drawobj+0xa0,4);v.write(f.drawverts,asset[vertices:vertices+128]);pointer=v.allocate(len(text)+1);v.write(pointer,text+b'\0')
    return f,pointer,text.decode()


def single_init(executable,asset):
    f,p,text=setup(executable,asset);v=f.vm;c=v.call(0x405b40,(0,0,f.animator,f.context,f.owner));before=bytes(v.uc.mem_read(f.drawverts,128));result=v.call(0x401b70,(p,),this=c)
    strings={hex(a):bytes(v.uc.mem_read(a,4)).split(b'\0')[0].decode()for a in(0x5c59ac,0x5c59b4,0x5c59bc)}
    if strings!={'0x5c59ac':'u','0x5c59b4':'v','0x5c59bc':'g'}:raise AssertionError('Actual native grammar changed')
    return dict(tag=text,initialize_result=result,selected_objects=v.u32(c+0x18),dimensions_delay=list(struct.unpack('<3i',v.uc.mem_read(c+0x2c,12))),uv_array=v.u32(c+0x40),vertices_unchanged=before==bytes(v.uc.mem_read(f.drawverts,128)),native_parameter_names=strings)


def refresh(executable,asset,stop_at_delete):
    f,p,text=setup(executable,asset);v=f.vm
    name=v.allocate(8);v.write(name,b'animtex\0');tag=v.allocate(16);v.write(tag,struct.pack('<4I',0,0,name,p));rows=v.allocate(4);v.put_u32(rows,tag);v.put_u32(f.context+0x7c,1);v.put_u32(f.context+0x8c,rows);v.put_u32(f.animator+4,f.owner);v.put_u32(f.animator+0x6c,0xffffffff)
    v.call(0x41c7f0,(16,16),this=f.animator+0x58);v.call(0x401a60)
    trace=[];faults=[];objects=[]
    def observe(uc,address,size,user):
        event=dict(address=hex(address),eax=uc.reg_read(UC_X86_REG_EAX))
        if address==0x405bc0:
            c=uc.reg_read(UC_X86_REG_ECX);event.update(selected_objects=v.u32(c+0x18),uv_array=v.u32(c+0x40));objects.append(c)
        trace.append(event)
    def unmapped(uc,access,address,size,value,user):faults.append(dict(instruction=hex(uc.reg_read(UC_X86_REG_EIP)),access_address=hex(address),size=size));return False
    for address in(0x401b70,0x40e0c8,0x405bc0,0x401c90):v.uc.hook_add(UC_HOOK_CODE,observe,begin=address,end=address)
    v.uc.hook_add(UC_HOOK_MEM_READ_UNMAPPED,unmapped)
    error=None
    try:v.call(0x40df90,(0,),this=f.animator,**({'stop_address':0x405bc0}if stop_at_delete else{}))
    except UcError as e:error=str(e)
    if stop_at_delete:
        # Runtime stops before executing the target instruction, so explicitly
        # observe the stop receiver without claiming its destructor executed.
        c=v.uc.reg_read(UC_X86_REG_ECX);receiver=dict(selected_objects=v.u32(c+0x18),uv_array=v.u32(c+0x40))
    else:receiver=None
    return dict(tag=text,stopped_before_delete=stop_at_delete,trace=trace,stop_receiver=receiver,admitted_controllers=v.u32(f.animator+0x58),exception=error,unmapped_reads=faults,final_instruction=hex(v.uc.reg_read(UC_X86_REG_EIP)),controller_render_calls=sum(x['address']=='0x401c90'for x in trace))


def run(executable,asset):
    init=single_init(executable,asset);stopped=refresh(executable,asset,True);fault=refresh(executable,asset,False)
    if init['initialize_result']!=0 or init['dimensions_delay'][:2]!=[4,0]or not init['vertices_unchanged']:raise AssertionError('Unmodified Rift rejection changed')
    if stopped['admitted_controllers']!=0 or stopped['exception']or stopped['final_instruction']!='0x405bc0':raise AssertionError('Actual failed-init delete admission changed')
    if not any(x['address']=='0x40e0c8'and x['eax']==0 for x in stopped['trace']):raise AssertionError('Native admission return path not observed')
    if fault['admitted_controllers']!=0 or fault['controller_render_calls']or fault['final_instruction']!='0x405c14' or not fault['exception']:raise AssertionError('Declared cold-heap destructor fault changed')
    if not fault['unmapped_reads']or fault['unmapped_reads'][0]['access_address']!='0x0':raise AssertionError('Fault memory boundary changed')
    return dict(single_initialize=init,actual_refresh_stopped_at_delete=stopped,actual_refresh_cold_heap_fault=fault)


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path);p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');p.add_argument('--output',type=Path,required=True);args=p.parse_args();args.output.mkdir(parents=True,exist_ok=True)
    with zipfile.ZipFile(args.archive)as z:asset=z.read(next(n for n in z.namelist()if n.lower()=='imagery/misc/rift1.i3d'))
    first=run(args.executable,asset);second=run(args.executable,asset)
    if first!=second:raise AssertionError('Fresh isolated preflight replay changed')
    report=dict(status='expected_native_failure_reproduced',effect='Rift1',type_id='0xadbcef18',asset_sha256=ASSET_SHA,retail_sha256=RETAIL_SHA,probe_sha256=sha(Path(__file__).read_bytes()),observations=first,exact_isolated_repeats=2,scope='Exact raw tag through original factory/full parser/Init and actual RefreshControllers admission/delete. Minimal imagery/owner/instance-vertex adapters, original cold CRT heap. Stop proof executes no destructor; separate unchanged full call reaches original null UV-array read. No prediction of every real retail heap state or natural placement.',production_changed=False,tag_repaired=False,animtex_support_granted=False,static_render_acceptance_granted=False,full_game_integration=False,dosbox_used=False)
    (args.output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))

if __name__=='__main__':main()
