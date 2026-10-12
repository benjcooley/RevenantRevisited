"""Full watcher rendering with actual pack-loaded texture surfaces."""
import argparse
import json
import struct
from pathlib import Path
from character_native_preflight import AuthenticCharacters,ROOT,sha
from lit_software_probe import LitSoftwareFixture
from software_probe import save_rgb565_png
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP


def run(boot,out):
    out.mkdir(parents=True,exist_ok=False)
    f=AuthenticCharacters(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',boot)
    f.select_screen_prefix();v=f.vm;world=f.world
    original=world.ddraw._device_method;material=[0]
    for name,args in world.ddraw.device_calls:
        if name=='IDirect3DDevice3::SetLightState' and args[0]==1:material[0]=args[1]
    def device(interface,method,args):
        if interface=='IDirect3DDevice3' and method in ('GetLightState','SetLightState') and args[1]==1:
            if method=='GetLightState':v.put_u32(args[2],material[0])
            else:material[0]=args[2]
            return 0
        return original(interface,method,args)
    world.ddraw._device_method=device
    sw=LitSoftwareFixture(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',640,480,vm=v)
    sw.lighting((38,38,38),(1,1,1));draws=[]
    def observe(uc,*unused):
        sp=uc.reg_read(UC_X86_REG_ESP)
        draws.append([v.u32(sp+4+i*4)for i in range(7)])
    v.uc.hook_add(UC_HOOK_CODE,observe,begin=0x56eb30,end=0x56eb30)
    cases=[]
    for name in ['ogrokwatcher1','ogrokwatcher2','ogrokwatcher3']:
        owner=f.new_object(name,(0,0,0),25,0x5a85ac)
        assert world.call(0x46e8b0,this=owner,instruction_limit=100000000)==1
        animator=v.u32(owner+0x58);imagery=v.u32(owner+0x54)
        folder=out/name;folder.mkdir();rows=[]
        for tick in range(1,9):
            world.call(0x470cc0,this=owner)
            world.call(0x40e460,(1,),this=animator)
            sw.clear();draws.clear()
            world.call(0x416f70,this=0x65a57c)
            world.call(0x40e740,this=animator)
            # PreRender can bind the game's real display again. The isolated
            # target and fixed camera are explicit scene inputs, not fitting.
            v.call(0x54de20,(sw.screen.address,sw.depth.address),this=sw.camera)
            v.call(0x54dd70,(0,639,0,479,640),this=sw.camera)
            world.call(0x40e8d0,this=animator,instruction_limit=100000000)
            world.call(0x40eb00,this=animator)
            world.call(0x416fb0,this=0x65a57c)
            pixels=v.surface_bytes('screen');p=folder/f'native-{tick:03d}.png'
            save_rgb565_png(p,pixels,640,480)
            node=v.u32(v.u32(imagery+0x5c));surface=v.u32(node+0x90)
            rows.append(dict(tick=tick,owner_frame=struct.unpack('<h',v.uc.mem_read(owner+0x5c,2))[0],
                             surface=hex(surface),draws=list(draws),image=str(p),image_sha256=sha(p),
                             nonblack_pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',pixels))))
            print(name,tick,rows[-1]['owner_frame'],len(draws),rows[-1]['nonblack_pixels'],flush=True)
        cases.append(dict(name=name,rows=rows))
    report=dict(status='native_loaded_render_complete',accepted=False,cases=cases,
                boot_image_sha256=sha(boot),scope='Actual class/resource/texture/animator creation and complete default Animate/PreRender/Render/PostRender plus software raster. No raw texel/descriptor/geometry injection. Declared camera0/640x480/ambient38/directional1 and isolated RAM target; generic owner world/map Pulse remains outside this frame-selection fixture. Format correctness/port/visual acceptance separate.')
    (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')


if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--boot-image',type=Path,required=True)
    p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.boot_image,a.output)
