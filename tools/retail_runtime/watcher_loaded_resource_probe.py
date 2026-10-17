"""Observe genuine watcher texture residency after original pack/mesh loading."""
import argparse
import json
import struct
from pathlib import Path
from character_native_preflight import AuthenticCharacters,ROOT,sha


def run(boot,output):
    output.mkdir(parents=True,exist_ok=False)
    f=AuthenticCharacters(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',boot)
    f.select_screen_prefix();v=f.vm;rows=[]
    for name in ['ogrokwatcher1','ogrokwatcher2','ogrokwatcher3']:
        owner=f.new_object(name,(0,0,0),25,0x5a85ac)
        assert f.world.call(0x46e8b0,this=owner,instruction_limit=100000000)==1
        # Actual CreateAnimator46e8dc reads the primary imagery at +54.
        # +4c in legacy fabricated-owner fixtures is not this live field.
        imagery=v.u32(owner+0x54)
        count=v.u32(imagery+0x4c);nodes=v.u32(imagery+0x5c)
        assert count==1,(name,hex(imagery),count)
        textures=[]
        for index in range(count):
            node=v.u32(nodes+index*4);n=v.u32(node+0x7c);frames=v.u32(node+0x84)
            surfaces=[]
            for frame in range(n):
                pointer=v.u32(frames+frame*4)
                surface=f.world.ddraw.surface(pointer)
                pixels=bytes(v.uc.mem_read(surface.memory,surface.pitch*surface.height))
                surfaces.append(dict(frame=frame,pointer=hex(pointer),width=surface.width,
                                     height=surface.height,pitch=surface.pitch,
                                     pixel_format=list(struct.unpack('<8I',surface.pixel_format)),
                                     pixels_sha256=__import__('hashlib').sha256(pixels).hexdigest()))
            textures.append(dict(index=index,frames=n,surfaces=surfaces))
        rows.append(dict(name=name,owner=hex(owner),imagery=hex(imagery),textures=textures))
        print(name,count,[x['frames']for x in textures],flush=True)
    report=dict(status='loaded_resource_observation',accepted=False,cases=rows,
                executable_sha256=sha(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe'),
                boot_image_sha256=sha(boot),
                scope='Original class factory/CreateAnimator and actual pack/resource/mesh/texture load in genuine cached boot. Surface descriptors/bytes observed through the existing DirectDraw COM boundary. No texture-format retagging, resource struct injection, rendering or visual credit.')
    (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')


if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--boot-image',type=Path,required=True)
    p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.boot_image,a.output)
