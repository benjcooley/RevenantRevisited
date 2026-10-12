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


def run(executable, boot, output, substitute=False, spell_control=False):
    assert not spell_control or substitute
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
    spell = None
    spell_inventory = []
    actors = []
    actors_before = []
    if spell_control:
        # Public map allocation makes the characters visible to the original
        # tick75 FindObjectsInRange. No query or actor result is replaced.
        def map_character(position):
            record = next(v.u32(v.u32(0x659d40)+4*i) for i in range(v.u32(0x659d30))
                          if v.string(v.u32(v.u32(v.u32(0x659d40)+4*i))) == 'Acolyte'
                          and v.uc.mem_read(v.u32(v.u32(0x659d40)+4*i)+0x22,1)[0] == 12)
            definition = v.allocate(52)
            typ = struct.unpack('<H',v.uc.mem_read(record+0x20,2))[0]
            v.write(definition,struct.pack('<HH',12,typ))
            v.write(definition+12,struct.pack('<3i',*position))
            index = call(0x450e40,(definition,0xffffffff),this=0x6668d8)
            assert index != 0xffffffff
            actor = call(0x452690,(index,0),this=0x6668d8)
            assert actor and v.u32(actor)==0x5a7848
            return actor
        actors = [map_character((3185,3185,16)),map_character((3281,3185,16))]
        actors_before = [f.dump(a) for a in actors]
        call(0x4d3b90,(actors[1],3),this=actors[0])
        assert call(0x53ead0,this=0x667c38,instruction_limit=100000000)==1
        selected=[]
        for i in range(v.u32(0x667c3c)):
            sd=v.u32(v.u32(0x667c4c)+4*i)
            for j in range(v.u32(sd)):
                variant=v.u32(v.u32(sd+0x10)+4*j)
                name=v.string(variant+4)
                effect=v.string(variant+0x2a)
                spell_inventory.append(dict(name=name,effect=effect))
                if name=='Quicksand':selected.append((sd,variant))
        assert not any('sandswirl' in (x['name']+' '+x['effect']).lower() for x in spell_inventory)
        assert len(selected)==1
        targets,source=v.allocate(4),v.allocate(12)
        v.put_u32(targets,actors[1]);v.write(source,struct.pack('<3i',3185,3185,16))
        spell=call(0x542200,(actors[0],targets,1,source,*selected[0],0),this=0x670630)
        assert spell and v.u32(spell+4)==actors[0]
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
    quicksands = []
    queries = []
    particle_calls = []
    particle_packets = []
    child_render = [False]
    particle_object = [None]
    def observe(uc,address,*_):
        sp=uc.reg_read(UC_X86_REG_ESP)
        if address == 0x40eef0:
            if child_render[0]: particle_object[0] = v.u32(sp+4)
        elif address == 0x40a8f0:
            if child_render[0]:
                obj=v.u32(sp+4)
                particle_packets.append(dict(variant=particle_object[0],position_hex=bytes(uc.mem_read(obj+0x10,12)).hex()))
        elif address in (0x4e55b0,0x4e54b0):
            parameters=v.u32(sp+4) if address==0x4e55b0 else sp+4
            particle_calls.append(dict(address=hex(address),tick=v.u32(0x65cb38),
                                       seed=v.u32(call_tls[0]+0x14),rng_calls=rng[0],
                                       parameters_hex=bytes(uc.mem_read(parameters,176)).hex()))
        elif address == 0x58c582:
            rng[0] += 1
        elif address == 0x4ee248:
            quicksands.append(uc.reg_read(UC_X86_REG_EAX))
            assert v.u32(quicksands[-1]) == 0x5ab4c4
        elif address == 0x4ee169:
            found=uc.reg_read(UC_X86_REG_EAX)
            queries.append(dict(found=found,indices=[v.u32(sp+0x118+4*i) for i in range(found)]))
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
    call_tls=[call(0x58fbf1)]
    for address in (0x40eef0,0x40a8f0,0x4e55b0,0x4e54b0,0x58c582,0x56eb30,0x450e40,0x4ede62,0x4edeb1,0x4edebc,0x4ee248,0x4ee169):
        v.uc.hook_add(UC_HOOK_CODE,observe,begin=address,end=address)
    call(0x58c575,(1,))
    tls=call(0x58fbf1)
    owner=f.new_object('Sandswirl',(3185,3185,16),25,0x5ab72c)
    if spell_control:
        v.put_u32(owner+0xd8,spell) # Original inline public TEffect::SetSpell.
    assert call(0x46e8b0,this=owner)==1
    parent=v.u32(owner+0x58)
    childanim=v.u32(parent+0x104)
    rows=[]
    initial=None
    if substitute:
        assert childanim and v.u32(childanim)==0x5a9ec0
        child=v.u32(childanim+4)
        assert v.u32(v.u32(owner)+0x110)==0x4edd80 and v.u32(v.u32(child)+0x110)==0x4de800
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
        for tick in range(1,311 if spell_control else 75):
            v.put_u32(0x65cb38,tick)
            for obj in (owner,child,*quicksands):
                if not v.u32(obj+8)&4096:
                    call(0x470cc0,this=obj)
                    call(v.u32(v.u32(obj)+0x110),this=obj)
            assert v.u32(parent+0xfc)==min(tick,300)
            draws.clear()
            particle_packets.clear()
            sw.clear()
            call(0x416f70,this=0x65a57c)
            split=[]
            for animator in (parent,childanim,*[v.u32(q+0x58) for q in quicksands]):
                start=len(draws)
                if v.u32(v.u32(animator+4)+8)&4096:
                    split.append(0)
                    continue
                call(0x40e460,(0,),this=animator)
                call(0x40e740,this=animator)
                call(0x54de20,(sw.screen.address,sw.depth.address),this=sw.camera)
                call(0x54dd70,(0,1279,0,959,1280),this=sw.camera)
                v.put_u32(0x675fc0,640)
                v.put_u32(0x675fc4,480)
                child_render[0] = animator == childanim
                call(v.u32(v.u32(animator)+52),this=animator,instruction_limit=100000000)
                child_render[0] = False
                call(0x40eb00,this=animator)
                split.append(len(draws)-start)
            call(0x416fb0,this=0x65a57c)
            assert split[0]==0
            raw=v.surface_bytes('screen')
            row=dict(tick=tick,state=state(),draw_counts=split,particle_packets=list(particle_packets),
                     draws_sha256=hashlib.sha256(json.dumps(draws,sort_keys=True).encode()).hexdigest(),
                     rgb565_sha256=hashlib.sha256(raw).hexdigest(),
                     nonzero_pixels=sum(x[0]!=0 for x in struct.iter_unpack('<H',raw)))
            row.update(actors=[f.dump(a) for a in actors],quicksands=[dict(pointer=hex(q),flags=v.u32(q+8),spell=v.u32(q+0xd8),frame=v.u32(v.u32(q+0x58)+0xfc)) for q in quicksands])
            if tick in SAMPLES or spell_control and tick in (75,76,90,120,150,200,299,300,310):
                name=f'diagnostic-{tick:03}.png'
                save_rgb565_png(output/name,raw,1280,960)
                row.update(image=name,image_sha256=sha(output/name),draws=list(draws))
            rows.append(row)
            print(tick,split,row['nonzero_pixels'],flush=True)
        assert any(row['nonzero_pixels'] for row in rows)
    else:
        assert childanim==0
    if spell_control:
        assert len(quicksands)==1 and queries[0]['found']==2
        assert set(queries[0]['indices'])=={v.u32(a+0x40) for a in actors}
        assert v.u32(parent+0xfc)==300 and v.u32(owner+8)&4096
        assert all(v.u32(q+8)&4096 for q in quicksands)
        for before,actor in zip(actors_before,actors):
            after=f.dump(actor)
            assert all(before[key]==after[key] for key in ('position','health','flags'))
    fault=None
    if not spell_control:
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
                guest_code_modified=substitute,executable_file_modified=False,
                patch=patch,lookups=lookups,sectors=sectors,events=events,initial=initial,
                records=rows,fault=fault,resource_sha256=resources,
                named_spell_control="Quicksand" if spell_control else None,
                loaded_spell_variants=spell_inventory,queries=queries,actors_before=actors_before,
                particle_calls=particle_calls,
                actors_after=[f.dump(a) for a in actors],
                actor_map_indices=[v.u32(a+0x40) for a in actors],
                viewport=[1280,960],center=[640,480],source=[3185,3185,16],camera=[3185,3185,0],
                executable_sha256=sha(executable),boot_image_sha256=sha(boot),probe_sha256=sha(__file__),
                scope=__doc__,limits='Explicit isolated parent-before-child tick order; newly spawned Quicksand first pulses next tick. Original stack-derived seekz is retained and recorded, not normalized. Sparks imagery is a declared substitute, not inferred sand identity. No production admission. '+('Real Quicksand named spell is only a public SetSpell control, not an authentic Sandswirl variant; genuine actors are map-resident but character/world scheduler and effect destruction remain outside this tick fixture.' if spell_control else 'Null spell stops at75.'))
    (output/'manifest.json').write_text(json.dumps(result,indent=2)+'\n')
    return result


if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--executable',type=Path,default=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe')
    p.add_argument('--boot-image',type=Path,required=True)
    p.add_argument('--output',type=Path,required=True)
    p.add_argument('--substitute-sparks',action='store_true')
    p.add_argument('--quicksand-spell-control',action='store_true')
    a=p.parse_args()
    run(a.executable,a.boot_image,a.output,a.substitute_sparks,a.quicksand_spell_control)
