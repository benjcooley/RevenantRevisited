"""Explicit modified-retail Sandswirl child diagnostic, never a retail visual pass.

Default: original sand lookup fails in a genuine active map-sector window.
--substitute-sparks: change only the Init push-name immediate to licensed sparks.
No resource, map return, particle allocator/update, parameter or raster replacement.
This diagnoses surviving parent/particle ABI, not the missing intended sand imagery.
"""
import argparse
import hashlib
import json
import struct
import zipfile
from pathlib import Path
from character_native_preflight import AuthenticCharacters, ROOT, sha
from lit_software_probe import LitSoftwareFixture
from software_probe import save_rgb565_png
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_ESP

SAMPLES = (1, 2, 5, 10, 20, 35, 50, 65, 74)


def run(executable, boot, output, substitute=False):
    output.mkdir(parents=True, exist_ok=False)
    f = AuthenticCharacters(executable, boot)
    f.select_screen_prefix()
    v, call = f.vm, f.world.call
    sw = LitSoftwareFixture(executable, 1280, 960, camera=(3185, 3185, 0), vm=v)
    sw.lighting((38, 38, 38), (1, 1, 1))
    camera = v.allocate(12)
    v.write(camera, struct.pack('<3i', 3185, 3185, 16))
    call(0x450d20, (camera,), this=0x6668d8)
    call(0x459220, this=0x6668d8, instruction_limit=100000000)
    sectors = []
    for i in range(v.u32(0x668578)):
        p = v.u32(v.u32(0x668588)+4*i)
        sectors.append(dict(position=list(struct.unpack('<3i', v.uc.mem_read(p+4,12))),
                            active=v.u32(p+0x98), objects=v.u32(p+0xb8)))
    assert sectors and all(x['objects'] == 0 for x in sectors)
    lookups = []
    for name in ('sand', 'sparks', 'Sandswirl'):
        p = v.allocate(len(name)+1)
        v.write(p, name.encode()+b'\0')
        typ = call(0x475210, (p,0), this=0x66cc68)
        builder = call(0x40dca0, (p,))
        lookups.append(dict(name=name, type=typ, builder=hex(builder),
                            factory=hex(v.u32(v.u32(builder)))))
        if name == 'sparks':
            replacement = p
    assert [x['type'] for x in lookups] == [0xffffffff,34,61]
    assert [x['factory'] for x in lookups] == ['0x40dc00','0x4f5eb0','0x4f8380']
    before = bytes(v.uc.mem_read(0x4ede49,5))
    assert before == bytes.fromhex('6804105e00')
    if substitute:
        v.write_code(0x4ede49,b'\x68'+struct.pack('<I',replacement))
    patch = dict(address='0x4ede49', before=before.hex(),
                 after=bytes(v.uc.mem_read(0x4ede49,5)).hex(),
                 original_name=v.string(0x5e1004), diagnostic_name='sparks' if substitute else 'sand')
    material = [0]
    original_device = f.world.ddraw._device_method
    def device(interface, method, args):
        if method in ('GetLightState','SetLightState') and args[1] == 1:
            if method == 'GetLightState':
                v.put_u32(args[2], material[0])
            else:
                material[0] = args[2]
            return 0
        return original_device(interface,method,args)
    f.world.ddraw._device_method = device
    events, draws, rng = [], [], [0]
    def observe(uc,address,*_):
        sp=uc.reg_read(UC_X86_REG_ESP)
        if address == 0x58c582:
            rng[0] += 1
        elif address == 0x56eb30:
            a=[v.u32(sp+4+4*i) for i in range(7)]
            assert a[1] in (0x112,0x1e2)
            draws.append(dict(arguments=a, vertices_hex=bytes(uc.mem_read(a[2],a[3]*32)).hex(),
                              indices_hex=bytes(uc.mem_read(a[4],a[5]*2)).hex(),
                              world_hex=bytes(uc.mem_read(0x675ed0,64)).hex()))
        elif address == 0x450e40:
            events.append(dict(address=hex(address),definition_hex=bytes(uc.mem_read(v.u32(sp+4),52)).hex()))
        else:
            events.append(dict(address=hex(address),result=hex(uc.reg_read(UC_X86_REG_EAX))))
    for address in (0x58c582,0x56eb30,0x450e40,0x4ede62,0x4edeb1,0x4edebc):
        v.uc.hook_add(UC_HOOK_CODE,observe,begin=address,end=address)
    call(0x58c575,(1,))
    tls=call(0x58fbf1)
    owner=f.new_object('Sandswirl',(3185,3185,16),25,0x5ab72c)
    assert call(0x46e8b0,this=owner)==1
    parent=v.u32(owner+0x58)
    childanim=v.u32(parent+0x104)
    rows=[]
    initial=None
    if substitute:
        assert childanim and v.u32(childanim)==0x5a9ec0
        child=v.u32(childanim+4)
        count=v.u32(childanim+0xfc)
        assert 50 <= count <= 100
        initial=dict(particles=count,child_animator_vtable=hex(v.u32(childanim)),
                     parameters_hex=bytes(v.uc.mem_read(childanim+0xfc,176)).hex(),
                     seekz_raw=v.u32(childanim+0xfc+92),rng_calls=rng[0],seed=v.u32(tls+0x14))
        def state():
            pools={}
            for name, offset, stride in [('position',0x1b0,12),('velocity',0x1ac,12),
                                        ('life',0x1b4,4),('start',0x1b8,4),('variant',0x1bc,4),
                                        ('target',0x1c0,4)]:
                p=v.u32(childanim+offset)
                pools[name]=bytes(v.uc.mem_read(p,count*stride)).hex()
            return dict(parent_frame=v.u32(parent+0xfc),pool=pools,
                        parameters_hex=bytes(v.uc.mem_read(childanim+0xfc,176)).hex(),
                        seed=v.u32(tls+0x14),rng_calls=rng[0],owner_flags=v.u32(owner+8),
                        child_flags=v.u32(child+8))
        for tick in range(1,75):
            v.put_u32(0x65cb38,tick)
            for obj in (owner,child):
                call(0x470cc0,this=obj)
                call(0x4de800,this=obj)
            assert v.u32(parent+0xfc)==tick
            draws.clear()
            sw.clear()
            call(0x416f70,this=0x65a57c)
            split=[]
            for animator in (parent,childanim):
                start=len(draws)
                call(0x40e460,(0,),this=animator)
                call(0x40e740,this=animator)
                call(0x54de20,(sw.screen.address,sw.depth.address),this=sw.camera)
                call(0x54dd70,(0,1279,0,959,1280),this=sw.camera)
                v.put_u32(0x675fc0,640)
                v.put_u32(0x675fc4,480)
                call(v.u32(v.u32(animator)+52),this=animator,instruction_limit=100000000)
                call(0x40eb00,this=animator)
                split.append(len(draws)-start)
            call(0x416fb0,this=0x65a57c)
            assert split[0]==0
            raw=v.surface_bytes('screen')
            row=dict(tick=tick,state=state(),draw_counts=split,
                     draws_sha256=hashlib.sha256(json.dumps(draws,sort_keys=True).encode()).hexdigest(),
                     rgb565_sha256=hashlib.sha256(raw).hexdigest(),
                     nonzero_pixels=sum(x[0]!=0 for x in struct.iter_unpack('<H',raw)))
            if tick in SAMPLES:
                name=f'diagnostic-{tick:03}.png'
                save_rgb565_png(output/name,raw,1280,960)
                row.update(image=name,image_sha256=sha(output/name),draws=list(draws))
            rows.append(row)
            print(tick,split,row['nonzero_pixels'],flush=True)
        assert any(row['nonzero_pixels'] for row in rows)
    else:
        assert childanim==0
    tick=75 if substitute else 1
    try:
        v.put_u32(0x65cb38,tick)
        call(0x470cc0,this=owner)
        call(0x4edd80,this=owner)
    except RuntimeError as error:
        fault=dict(tick=tick,error=str(error),details=f.world.last_fault)
        assert f.world.last_fault[:2] == ((0x4ee13d,4) if substitute else (0x4ee0bc,0))
    else:
        raise AssertionError('Expected missing-spell/child boundary was not reached')
    with zipfile.ZipFile(ROOT/'data/imagery.rvi') as z:
        resources={name:hashlib.sha256(z.read(name)).hexdigest() for name in
                   ('class.def','Imagery/Magic/sandswirl.i3d','Imagery/Misc/sparks.i3d')}
        exact_sand=[name for name in z.namelist() if Path(name).stem.lower()=='sand']
    assert not exact_sand
    result=dict(status='qualified_substitute_child_diagnostic' if substitute else 'original_missing_child',
                accepted=False,visual_pass=False,original_identical=not substitute,
                patch=patch,lookups=lookups,sectors=sectors,events=events,initial=initial,
                records=rows,fault=fault,resource_sha256=resources,
                viewport=[1280,960],center=[640,480],source=[3185,3185,16],camera=[3185,3185,0],
                executable_sha256=sha(executable),boot_image_sha256=sha(boot),probe_sha256=sha(__file__),
                scope=__doc__,limits='Explicit isolated parent-before-child tick order; empty active map, '
                'null spell stops at75. Original stack-derived seekz is retained and recorded, not normalized. '
                'Sparks imagery is a declared substitute, not inferred sand identity. No production admission.')
    (output/'manifest.json').write_text(json.dumps(result,indent=2)+'\n')
    return result


if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--executable',type=Path,default=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe')
    p.add_argument('--boot-image',type=Path,required=True)
    p.add_argument('--output',type=Path,required=True)
    p.add_argument('--substitute-sparks',action='store_true')
    a=p.parse_args()
    run(a.executable,a.boot_image,a.output,a.substitute_sparks)
