"""Direct bounded effect/renderer adapters; all simulation and raster stays x86."""
import copy
from dataclasses import dataclass
import hashlib
import json
from pathlib import Path
import struct
import zipfile

ROOT=Path(__file__).resolve().parents[2]


@dataclass
class PixelFrame:
    color: bytes
    depth: bytes
    width: int
    height: int
    metadata: dict


def xyz(value):
    if not isinstance(value,(list,tuple)) or len(value)!=3 or any(
            type(v) is not int or not -0x80000000<=v<=0x7fffffff for v in value):
        raise ValueError('Fixture coordinates must be three signed int32 integers')
    return list(value)


class SoftwareEffectAdapter:
    """Explicit projection/lighting inputs, independent guest renderer surfaces."""
    supports_position=True

    def inputs(self,params,extra=()):
        supported={'archive','camera','rgba5'}|set(extra)
        if self.supports_position:supported.add('position')
        unknown=set(params)-supported
        if unknown:raise ValueError('Unsupported renderer fixture inputs: '+', '.join(sorted(unknown)))
        self.ticks=0;self.position=xyz(params.get('position',[0,0,0]))
        self.camera=[0,0,0];self.zdist=1925;self.rgba5=[31,31,31,31]
        self.set_camera(params.get('camera',dict(position=[0,0,0],zdist=1925)))
        self.set_light(dict(rgba5=params.get('rgba5',[31,31,31,31])))
        self.archive=Path(params.get('archive',ROOT/'data/imagery.rvi')).resolve()

    def asset(self,member):
        with zipfile.ZipFile(self.archive) as archive:data=archive.read(member)
        self.asset_info=dict(member=member,sha256=hashlib.sha256(data).hexdigest(),bytes=len(data))
        return data

    def set_position(self,position):
        if not self.supports_position:raise ValueError('This local-coordinate effect does not apply an owner position')
        self.position=xyz(position)
        return dict(position=list(self.position),scope='Explicit upstream world-matrix fixture input; no retail map object spawned')

    def set_owner(self,owner):
        result=self.set_position(owner['position'])
        result['unmapped_fields']=['facing','stats','attachments']
        return result

    def set_camera(self,camera):
        if not isinstance(camera,dict) or set(camera)-{'position','zdist'}:
            raise ValueError('Camera supports position and zdist only; viewport is fixed by the profile')
        position=xyz(camera.get('position',self.camera))
        zdist=camera.get('zdist',self.zdist)
        if type(zdist) is not int or not 1<=zdist<=0x7fffffff:raise ValueError('Camera zdist must be positive int32')
        self.camera=position;self.zdist=zdist
        return dict(position=list(position),zdist=zdist,scope='Original software world/view/projection inputs')

    def set_light(self,light):
        if not isinstance(light,dict) or set(light)!={'rgba5'}:
            raise ValueError('Light fixture requires rgba5; map ambient/light collection is not connected')
        rgba=light['rgba5']
        if not isinstance(rgba,(list,tuple)) or len(rgba)!=4 or any(type(v) is not int or not 0<=v<=31 for v in rgba):
            raise ValueError('rgba5 must be four integers from 0 to 31')
        self.rgba5=list(rgba)
        return dict(rgba5=list(rgba),scope='Vertex RGBA5 modulation supplied to original raster; not map lighting')

    def project(self,positions):
        matrix=[float(i%5==0) for i in range(16)]
        matrix[12:15]=self.position
        return self.fixture.software.project(positions,world_matrix=matrix,camera=self.camera,zdist=self.zdist)

    def draw_quad(self,positions,uvs,indices):
        projected=self.project(positions)
        vertices=[(*point,*self.rgba5,*uv) for point,uv in zip(projected,uvs)]
        return (*self.fixture.software.draw(vertices,indices,z_enabled=True,z_write=False),projected)

    def checkpoint(self):
        self.fixture.software.checkpoint()
        self.saved=copy.deepcopy(dict(ticks=self.ticks,position=self.position,camera=self.camera,
            zdist=self.zdist,rgba5=self.rgba5,calls=self.fixture.calls))
        self.checkpoint_extra()

    def checkpoint_extra(self):pass

    def reset(self):
        self.fixture.software.restore()
        for name,value in copy.deepcopy(self.saved).items():
            if name=='calls':self.fixture.calls=value
            else:setattr(self,name,value)
        self.reset_extra()

    def reset_extra(self):pass

    def state_metadata(self):
        return dict(ticks=self.ticks,asset=copy.deepcopy(self.asset_info),
            camera=dict(position=list(self.camera),zdist=self.zdist),fixture_position=list(self.position),
            vertex_rgba5=list(self.rgba5),external_calls=dict(self.fixture.calls),
            original_raster='0x56d960',pixel_capture=True,
            scope='Bounded original effect/projection/software raster; independent surfaces, no map or modern Metal renderer')

    def capture(self,kind):
        if kind=='state':return self.inspect_state()
        if kind!='pixels':raise ValueError('Capture kind must be state or pixels')
        color,depth,packet=self.render()
        software=self.fixture.software
        return PixelFrame(color,depth,software.width,software.height,
            dict(packet=packet,state=self.inspect_state(),depth_format='U16'))


class FlameAdapter(SoftwareEffectAdapter):
    def __init__(self,executable,params,build=None):
        from flame_probe import FlameFixture,PROFILES
        self.inputs(params,('variant','type_id'))
        selected=params.get('variant')
        type_id=params.get('type_id')
        if type_id is not None:type_id=int(type_id,0) if isinstance(type_id,str) else int(type_id)
        matches=[name for name,(value,member) in PROFILES.items() if (selected is None or name==selected)
            and (type_id is None or int(value,0)==type_id)]
        if selected is None and type_id is None:matches=['base']
        if len(matches)!=1:raise ValueError('Flame variant/type_id is unknown or inconsistent; no selector fallback')
        self.variant=matches[0];self.type_id,self.member=PROFILES[self.variant]
        asset=self.asset(self.member)
        expected={'base':'c04203f693567490cc1754d2bd0c10333c8db776e74e2d2b6d5fdfac81bf9ed7',
            'blue':'1e94317594421341b8b3d9c9b41be01ce724b63c9702deb0fcd6bd2773241718',
            'green':'a60243384bcfb5700e543d15c543cfcc25cca3d34baf4d37811cef386254a778'}
        if hashlib.sha256(asset).hexdigest()!=expected[self.variant]:raise ValueError('Unsupported selected Flame asset revision')
        self.fixture=FlameFixture(executable,asset,build=build)

    def advance_ticks(self,ticks):
        for _ in range(ticks):
            self.fixture.vm.call(0x4e4ef0,this=self.fixture.animator)
            self.fixture.vm.advance();self.ticks+=1
        return self.inspect_state()

    def inspect_state(self):
        return dict(**self.state_metadata(),frame=self.fixture.vm.u32(self.fixture.animator+0xfc),
            variant=self.variant,type_id=self.type_id,
            frame_count=18,original_animate='0x4e4ef0',original_render='0x4e4f20',
            owner_world_policy='Explicit upstream world-matrix fixture; base hierarchy/imagery interfaces excluded')

    def render(self):
        from flame_probe import INDICES
        frame=self.fixture.vm.u32(self.fixture.animator+0xfc)
        matrix,positions,uvs=self.fixture.original_packet(frame)
        self.fixture.software.clear()
        color,depth,projected=self.draw_quad(positions,uvs,INDICES)
        return color,depth,dict(frame=frame,matrix=list(matrix),positions=positions,uvs=uvs,projected=projected)


class RippleAdapter(SoftwareEffectAdapter):
    def __init__(self,executable,params,build=None):
        from ripple_probe import RippleFixture,LENGTHS
        self.inputs(params,('length',))
        self.length=params.get('length',20)
        if type(self.length) is not int or self.length not in LENGTHS:
            raise ValueError('Ripple supports verified no-splash lengths 4, 20 or 24')
        # Ripple's current producer has no named-variant constructor. Keep its
        # baseline-only profile guard until that verified interface exists.
        self.fixture=RippleFixture(executable,self.asset('Imagery/Magic/ripples.i3d'))
        self.fixture.packet(self.length,0)

    def advance_ticks(self,ticks):
        for _ in range(ticks):
            if not self.fixture.vm.u32(self.fixture.owner+8)&0x1000:
                self.fixture.vm.call(0x4f08b0,this=self.fixture.animator)
            self.fixture.vm.advance();self.ticks+=1
        return self.inspect_state()

    def inspect_state(self):
        vm=self.fixture.vm;a=self.fixture.animator
        return dict(**self.state_metadata(),length=self.length,frameon=vm.u32(a+0x104),
            ripframe=vm.u32(a+0x100),scale=struct.unpack('<f',vm.uc.mem_read(a+0x10c,4))[0],
            alive=not bool(vm.u32(self.fixture.owner+8)&0x1000),splash_count=vm.u32(a+0x108),
            original_initialize='0x4f0780',original_animate='0x4f08b0',original_render='0x4f0c10',
            scope_limit='No-splash lengths only; random splashes and recursive child effects unsupported')

    def render(self):
        from ripple_probe import INDICES
        fixture=self.fixture;vm=fixture.vm
        fixture.software.clear()
        if not self.inspect_state()['alive']:
            return vm.surface_bytes('screen'),vm.surface_bytes('depth'),dict(alive=False,positions=[],uvs=[])
        vm.call(0x4f0c10,this=fixture.animator)
        positions=[]
        for point in fixture.points:
            vm.write(fixture.point,struct.pack('<3f',*point))
            vm.call(0x43ad80,(fixture.obj+0x58,fixture.point,fixture.result))
            positions.append(struct.unpack('<3f',vm.uc.mem_read(fixture.result,12)))
        uvs=[struct.unpack('<2f',vm.uc.mem_read(fixture.lverts+i*32+24,8)) for i in range(4)]
        color,depth,projected=self.draw_quad(positions,uvs,INDICES)
        return color,depth,dict(alive=True,positions=positions,uvs=uvs,projected=projected)


class FizzleAdapter(SoftwareEffectAdapter):
    supports_position=False

    def __init__(self,executable,params,build=None):
        from fizzle_probe import FizzleFixture
        self.inputs(params)
        self.fixture=FizzleFixture(executable,self.asset('Imagery/Magic/fizzle.i3d'))

    def advance_ticks(self,ticks):
        for _ in range(ticks):
            self.fixture.animate();self.fixture.vm.advance();self.ticks+=1
        return self.inspect_state()

    def inspect_state(self):
        return dict(**self.state_metadata(),**self.fixture.state(),
            original_initialize='0x4f3f90',original_animate='0x4f4050',original_render='0x4f4440',
            particle_render='0x50c220',native_random_inputs=copy.deepcopy(self.fixture.random),
            owner_world_policy='Local particle Z, native flicker=1/abs_pos=0; owner translation unsupported')

    def checkpoint_extra(self):
        self.saved_fizzle=copy.deepcopy({name:getattr(self.fixture,name)
            for name in ('packets','random','pending_random')})
        self.saved_texture=self.fixture.software.texture

    def reset_extra(self):
        for name,value in copy.deepcopy(self.saved_fizzle).items():setattr(self.fixture,name,value)
        self.fixture.software.texture=self.saved_texture

    def render(self):
        from fizzle_probe import SYSTEM_OBJECTS,INDICES
        fixture=self.fixture;draws=fixture.original_draws();fixture.software.clear()
        projected=[]
        for draw in draws:
            fixture.software.texture=fixture.textures[SYSTEM_OBJECTS[draw['system']]]
            fixture.vm.call(0x56d3a0,(draw['system']+1,fixture.software.texture_object))
            _,_,projection=self.draw_quad(draw['positions'],draw['uvs'],INDICES)
            projected.append(projection)
        return fixture.vm.surface_bytes('screen'),fixture.vm.surface_bytes('depth'),dict(draws=draws,projected=projected)


class MistFogAdapter(SoftwareEffectAdapter):
    """Original 25-puff looping machine with strictly declared identity inputs."""
    supports_position=False

    def __init__(self,executable,params,build=None):
        from mistfog_probe import MistFogFixture
        from unicorn import UC_HOOK_CODE
        if set(params)-{'archive'}:
            raise ValueError('MistFog accepts archive only; identity owner, fixed camera and full-white lighting are the verified fixture')
        self.inputs(params)
        self.fixture=MistFogFixture(executable,self.asset('Imagery/Misc/mistfog.i3d'))
        self.fixture.calls={};self.original_calls={}
        def observe(uc,address,size,user):
            key=hex(address);self.original_calls[key]=self.original_calls.get(key,0)+1
        for address in (0x4f2840,0x4f2950,0x4f2ae0,0x40a420):
            self.fixture.vm.uc.hook_add(UC_HOOK_CODE,observe,begin=address,end=address)
        self.fixture.reset()

    def set_camera(self,camera):
        if camera.get('position',[0,0,0])!=[0,0,0] or camera.get('zdist',1925)!=1925:
            raise ValueError('MistFog camera changes are not validated; use the fixed declared camera')
        return super().set_camera(camera)

    def set_light(self,light):
        if light.get('rgba5')!=[31,31,31,31]:
            raise ValueError('MistFog supports the declared full-intensity white vertex lighting only')
        return super().set_light(light)

    def advance_ticks(self,ticks):
        for _ in range(ticks):
            self.fixture.animate();self.fixture.vm.advance();self.ticks+=1
        return self.inspect_state()

    def inspect_state(self):
        state=self.state_metadata();state.update(self.fixture.state())
        state.update(original_initialize='0x4f2840',original_animate='0x4f2950',original_render='0x4f2ae0',
            original_matrix='0x40a420',original_function_calls=dict(self.original_calls),
            native_random_inputs=copy.deepcopy(self.fixture.random),puff_count=25,looping=True,
            owner_world_policy='Identity owner and fixed camera; no position/source bindings',
            lighting_policy='Full-white RGBA5 [31,31,31,31]; map illumination and hardware blend remain separate gates')
        return state

    def checkpoint_extra(self):
        self.saved_mist=copy.deepcopy({name:getattr(self.fixture,name)
            for name in ('random','random_pending','range_pending','packets')})
        self.saved_original_calls=dict(self.original_calls)

    def reset_extra(self):
        for name,value in copy.deepcopy(self.saved_mist).items():setattr(self.fixture,name,value)
        self.original_calls=dict(self.saved_original_calls)

    def render(self):
        draws=self.fixture.original_draws()
        color,depth=self.fixture.pixels(draws)
        return color,depth,dict(draws=draws,lighting='full-white',owner_world='identity')


def static_profile_catalog():
    return json.loads(Path(__file__).with_name('static_profiles.json').read_text())['profiles']


class StaticMeshAdapter(SoftwareEffectAdapter):
    """Verified asset key/matrix/mesh sampling; no effect Animate is invented."""
    def __init__(self,executable,params,build=None):
        from static_mesh_probe import StaticFixture,parse_asset,FRAMES
        unknown=set(params)-{'profile','type_id','archive','position','frame'}
        if unknown:raise ValueError('Static mesh supports profile/type_id, archive, position and frame only')
        profiles=static_profile_catalog();name=params.get('profile');type_id=params.get('type_id')
        if name is None and type_id is None:raise ValueError('Choose a verified static profile name or type_id')
        if type_id is not None:type_id=int(type_id,0) if isinstance(type_id,str) else int(type_id)
        matching=[p for p in profiles if (name is None or p['name'].lower()==str(name).lower())
            and (type_id is None or int(p['id'],0)==type_id)]
        if len(matching)!=1:raise ValueError('Static profile name/type_id is unknown or inconsistent')
        self.profile=copy.deepcopy(matching[0]);self.frame=params.get('frame',0)
        if type(self.frame) is not int or self.frame not in FRAMES:
            raise ValueError('Static sampling supports verified frames 0, 50 and 99 only')
        self.inputs({key:value for key,value in params.items() if key in ('archive','position')})
        self.set_position(self.position)
        member='Imagery/'+self.profile['asset'].replace('\\','/')
        with zipfile.ZipFile(self.archive) as archive:
            names={n.lower():n for n in archive.namelist()}
            member=names[member.lower()];asset=archive.read(member)
        self.asset_info=dict(member=member,sha256=hashlib.sha256(asset).hexdigest(),bytes=len(asset))
        self.metadata=parse_asset(asset,self.profile)
        self.fixture=StaticFixture(executable);self.fixture.setup(self.metadata)
        self.fixture.calls={}

    def set_position(self,position):
        from static_mesh_probe import OWNERS
        position=xyz(position)
        if tuple(position) not in OWNERS:
            raise ValueError('Static owner translation supports the verified positions [0,0,0] and [16,-8,4] only')
        self.position=position
        return dict(position=list(position),scope='Verified owner matrix translation through original matrix/point functions')

    def set_camera(self,camera):
        if camera.get('position',[0,0,0])!=[0,0,0] or camera.get('zdist',1925)!=1925:
            raise ValueError('Static mesh uses the fixed verified camera')
        return super().set_camera(camera)

    def set_light(self,light):
        if light.get('rgba5')!=[31,31,31,31]:raise ValueError('Static mesh uses fixed full-white vertex modulation')
        return super().set_light(light)

    def set_sample_frame(self,frame):
        from static_mesh_probe import FRAMES
        if type(frame) is not int or frame not in FRAMES:raise ValueError('Static sampling supports frames 0, 50 and 99 only')
        self.frame=frame
        return dict(frame=frame,scope='Explicit constant-track frame sample; no native effect Animate is claimed')

    def advance_ticks(self,ticks):
        # Time may advance while a selected constant pose remains unchanged.
        # Frame sampling is explicit and never posed as recovered effect motion.
        for _ in range(ticks):self.fixture.vm.advance();self.ticks+=1
        return self.inspect_state()

    def inspect_state(self):
        return dict(**self.state_metadata(),profile=self.profile['name'],type_id=self.profile['id'],frame=self.frame,
            sampled_frames=[0,50,99],constant_track=True,vertices=self.profile['vertices'],faces=self.profile['faces'],
            original_key_decoder='0x409430',original_key_getter='0x409950',original_matrix='0x40a420',
            lighting='full-white31',culling='NONE',depth_policy='test/write',
            scope_limit='Constant asset pose/mesh frontend only; no native effect Pulse/Animate, real map or modern GPU acceptance')

    def checkpoint_extra(self):self.saved_frame=self.frame
    def reset_extra(self):self.frame=self.saved_frame

    def render(self):
        from static_mesh_probe import OWNERS
        matrix,points=self.fixture.original(self.frame,OWNERS.index(tuple(self.position)))
        uvs=[v[6:8] for v in self.metadata['vertices']]
        color,depth=self.fixture.pixels(points,uvs,self.metadata['indices'])
        return color,depth,dict(frame=self.frame,profile=self.profile['name'],type_id=self.profile['id'],
            matrix=matrix,positions=points,uvs=uvs,indices=self.metadata['indices'],owner_position=list(self.position))


def fountain_profile_catalog():
    from fountain_probe import NAMES,IDS
    return list(zip(NAMES,IDS))


class FountainAdapter(SoftwareEffectAdapter):
    """Four actual color leaves with native bubble/RNG lifecycle and raster."""
    supports_position=False

    def __init__(self,executable,params,build=None):
        from fountain_probe import FountainFixture,NAMES,IDS,VTABLES
        from unicorn import UC_HOOK_CODE
        if set(params)-{'archive','profile','color','type_id'}:
            raise ValueError('Fountain accepts profile/color/type_id and archive only; owner/camera/lighting are fixed')
        candidates=set(range(4))
        if 'profile' in params:
            candidates&={i for i,name in enumerate(NAMES) if name.lower()==str(params['profile']).lower()}
        if 'color' in params:
            value=params['color']
            if type(value) is int:matching={value} if value in range(4) else set()
            else:matching={i for i,name in enumerate(NAMES) if str(value).lower() in (name.lower(),name[:-4].lower())}
            candidates&=matching
        if 'type_id' in params:
            value=params['type_id'];value=int(value,0) if isinstance(value,str) else int(value)
            candidates&={i for i,type_id in enumerate(IDS) if int(type_id,0)==value}
        if not any(key in params for key in ('profile','color','type_id')) or len(candidates)!=1:
            raise ValueError('Choose one matching registered Fountain profile, color or type_id')
        self.color=next(iter(candidates));self.name=NAMES[self.color];self.type_id=IDS[self.color]
        self.inputs({key:value for key,value in params.items() if key=='archive'})
        self.fixture=FountainFixture(executable,self.asset('Imagery/Misc/sparkle.i3d'))
        self.fixture.calls={};self.original_calls={}
        self.lifecycle_observations=dict(delay=0,rise=0,shrink=0,respawn=0)
        self.animate_address=self.fixture.vm.u32(VTABLES[self.color]+11*4)
        def observe(uc,address,size,user):
            key=hex(address);self.original_calls[key]=self.original_calls.get(key,0)+1
        for address in {0x4e4080,0x4e41f0,0x40a420,self.animate_address}:
            self.fixture.vm.uc.hook_add(UC_HOOK_CODE,observe,begin=address,end=address)
        self.fixture.reset(self.color)

    def set_camera(self,camera):
        if camera.get('position',[0,0,0])!=[0,0,0] or camera.get('zdist',1925)!=1925:
            raise ValueError('Fountain uses the fixed verified camera')
        return super().set_camera(camera)

    def set_light(self,light):
        if light.get('rgba5')!=[31,31,31,31]:raise ValueError('Fountain uses fixed full-white vertex modulation')
        return super().set_light(light)

    def advance_ticks(self,ticks):
        for _ in range(ticks):
            before=self.fixture.state()['particles']
            self.fixture.animate()
            after=self.fixture.state()['particles']
            # Observe native state transitions only. No host value is written
            # to particle frame, position, scale, rise or random state.
            for old,new in zip(before,after):
                if old['frame']<0 and new['frame']<=0:self.lifecycle_observations['delay']+=1
                if new['values'][2]>old['values'][2]:self.lifecycle_observations['rise']+=1
                if new['values'][3]<old['values'][3]:self.lifecycle_observations['shrink']+=1
                if new['frame']<old['frame']:self.lifecycle_observations['respawn']+=1
            self.fixture.vm.advance();self.ticks+=1
        return self.inspect_state()

    def inspect_state(self):
        state=self.state_metadata();state.update(self.fixture.state())
        state.update(profile=self.name,type_id=self.type_id,bubble_count=10,looping=True,
            original_initialize='0x4e4080',original_animate=hex(self.animate_address),original_render='0x4e41f0',
            original_function_calls=dict(self.original_calls),native_random_inputs=copy.deepcopy(self.fixture.random),
            lifecycle_observations=dict(self.lifecycle_observations),
            owner_world_policy='Identity owner/fixed camera/full-white inputs; no natural map ownership or lighting',
            lifecycle='Original delayed emission, rise, shrink and respawn; external removal ends the fixture')
        return state

    def checkpoint_extra(self):
        self.saved_fountain=copy.deepcopy({name:getattr(self.fixture,name)
            for name in ('random','random_pending','packets','color')})
        self.saved_original_calls=dict(self.original_calls)
        self.saved_lifecycle_observations=dict(self.lifecycle_observations)

    def reset_extra(self):
        for name,value in copy.deepcopy(self.saved_fountain).items():setattr(self.fixture,name,value)
        self.original_calls=dict(self.saved_original_calls)
        self.lifecycle_observations=dict(self.saved_lifecycle_observations)

    def render(self):
        draws=self.fixture.original_draws()
        color,depth=self.fixture.pixels(draws)
        return color,depth,dict(profile=self.name,type_id=self.type_id,draws=draws,owner_world='identity')


def ribbon_profile_catalog():
    return json.loads(Path(__file__).with_name('ribbon_profiles.json').read_text())['profiles']


class ColoredRibbonAdapter(StaticMeshAdapter):
    """Two original static parts composited in authored order; base Ribbon excluded."""
    def __init__(self,executable,params,build=None):
        from static_mesh_probe import StaticFixture
        from colored_ribbon_probe import parse_parts
        if set(params)-{'profile','type_id','archive','position','frame'}:
            raise ValueError('Colored Ribbon supports profile/type_id, archive, position and frame0 only')
        name=params.get('profile');type_id=params.get('type_id')
        if name is None and type_id is None:raise ValueError('Choose a verified colored Ribbon profile or type_id')
        if type_id is not None:type_id=int(type_id,0) if isinstance(type_id,str) else int(type_id)
        matching=[p for p in ribbon_profile_catalog() if (name is None or p['name'].lower()==str(name).lower())
            and (type_id is None or int(p['id'],0)==type_id)]
        if len(matching)!=1:raise ValueError('Colored Ribbon profile/type_id is unknown or inconsistent; base Ribbon is excluded')
        self.profile=copy.deepcopy(matching[0]);self.frame=params.get('frame',0)
        if type(self.frame) is not int or self.frame!=0:raise ValueError('Colored Ribbon tracks have one verified frame0')
        self.inputs({key:value for key,value in params.items() if key in ('archive','position')})
        self.set_position(self.position)
        member='Imagery/'+self.profile['asset'].replace('\\','/')
        with zipfile.ZipFile(self.archive) as archive:
            names={n.lower():n for n in archive.namelist()}
            member=names[member.lower()];asset=archive.read(member)
        self.asset_info=dict(member=member,sha256=hashlib.sha256(asset).hexdigest(),bytes=len(asset))
        self.parts=parse_parts(asset,self.profile)
        self.fixture=StaticFixture(executable);self.fixture.setup(self.parts[0]);self.fixture.calls={}

    def set_sample_frame(self,frame):
        if type(frame) is not int or frame!=0:raise ValueError('Colored Ribbon supports constant frame0 only')
        self.frame=0
        return dict(frame=0,scope='Constant composite asset sample; base Ribbon revive/combat is excluded')

    def inspect_state(self):
        return dict(**self.state_metadata(),profile=self.profile['name'],type_id=self.profile['id'],frame=0,
            constant_track=True,parts=[dict(index=p['object_index'],name=p['object_name'],vertices=len(p['vertices']),
                faces=len(p['indices'])//3,keys=len(p['keys']),texture_sha256=hashlib.sha256(p['texture']).hexdigest()) for p in self.parts],
            vertices=self.profile['vertices'],faces=self.profile['faces'],depth_policy='test/write',culling='NONE',
            scope_limit='Two independent constant root subobjects remapped to original object0 provider; no base Ribbon revive/combat, natural map or modern GPU acceptance')

    def checkpoint_extra(self):
        self.saved_frame=self.frame
        self.saved_metadata=self.fixture.metadata
        self.saved_texture=self.fixture.software.texture

    def reset_extra(self):
        self.frame=self.saved_frame
        self.fixture.metadata=self.saved_metadata
        self.fixture.software.texture=self.saved_texture

    def render(self):
        from static_mesh_probe import OWNERS
        from colored_ribbon_probe import pixels
        draws=[];packets=[]
        for part in self.parts:
            self.fixture.setup(part,checkpoint=False)
            matrix,points=self.fixture.original(0,OWNERS.index(tuple(self.position)))
            uvs=[v[6:8] for v in part['vertices']]
            draws.append(dict(metadata=part,positions=points,uvs=uvs,indices=part['indices']))
            packets.append(dict(index=part['object_index'],name=part['object_name'],matrix=matrix,
                positions=points,uvs=uvs,indices=part['indices']))
        color,depth=pixels(self.fixture,draws)
        return color,depth,dict(profile=self.profile['name'],type_id=self.profile['id'],frame=0,
            owner_position=list(self.position),parts=packets,composited_in_asset_order=True)


class StreamerAdapter(SoftwareEffectAdapter):
    """Native four-stream one-shot, fixed face/identity inputs and provider culling."""
    supports_position=False

    def __init__(self,executable,params,build=None):
        from streamer_probe import StreamerFixture,parse_asset
        from unicorn import UC_HOOK_CODE
        if set(params)-{'archive'}:
            raise ValueError('Streamer accepts archive only; face0, owner/camera/light and provider culling are fixed')
        self.inputs(params)
        self.fixture=StreamerFixture(executable,parse_asset(self.asset('Imagery/Magic/streamer.i3d')))
        self.fixture.calls={};self.original_calls={}
        def observe(uc,address,size,user):
            key=hex(address);self.original_calls[key]=self.original_calls.get(key,0)+1
        for address in (0x4efbd0,0x4efd50,0x4efe50,0x4efa90,0x4efcc0):
            self.fixture.vm.uc.hook_add(UC_HOOK_CODE,observe,begin=address,end=address)
        self.fixture.reset()

    def set_camera(self,camera):
        if camera.get('position',[0,0,0])!=[0,0,0] or camera.get('zdist',1925)!=1925:
            raise ValueError('Streamer uses the fixed verified camera')
        return super().set_camera(camera)

    def set_light(self,light):
        if light.get('rgba5')!=[31,31,31,31]:raise ValueError('Streamer uses fixed full-white vertex modulation')
        return super().set_light(light)

    def advance_ticks(self,ticks):
        for _ in range(ticks):
            self.fixture.animate();self.fixture.vm.advance();self.ticks+=1
        return self.inspect_state()

    def inspect_state(self):
        state=self.state_metadata();state.update(self.fixture.state())
        state.update(stream_count=4,slots_per_stream=50,original_initialize='0x4efbd0',original_animate='0x4efd50',
            original_render='0x4efe50',original_function_calls=dict(self.original_calls),rng='none',
            scope_limit='Face0/identity owner, no spell, optional sound unavailable; existing frontend culling boundary retained, real map/device/GPU culling/lighting unverified')
        return state

    def checkpoint_extra(self):
        self.saved_packets=copy.deepcopy(self.fixture.packets)
        self.saved_original_calls=dict(self.original_calls)
        self.saved_texture=self.fixture.software.texture

    def reset_extra(self):
        self.fixture.packets=copy.deepcopy(self.saved_packets)
        self.original_calls=dict(self.saved_original_calls)
        self.fixture.software.texture=self.saved_texture

    def render(self):
        draws=self.fixture.original_draws();color,depth=self.fixture.pixels(draws)
        return color,depth,dict(draws=draws,face=0,owner_world='identity',
            culling='Existing explicit frontend/provider boundary; not original device-culling acceptance')


class MistAdapter(SoftwareEffectAdapter):
    """Native continuous fifty-drop machine; no guessed movement or owner pose."""
    supports_position=False

    def __init__(self,executable,params,build=None):
        from mist_probe import MistFixture
        from unicorn import UC_HOOK_CODE
        if set(params)-{'archive'}:raise ValueError('Mist accepts archive only; face0/identity/fixed lighting-camera inputs are verified')
        self.inputs(params)
        self.fixture=MistFixture(executable,self.asset('Imagery/Magic/mist.i3d'),build=build)
        self.fixture.calls={};self.original_calls={};self.lifecycle_observations=dict(deaths=0,respawns=0)
        def observe(uc,address,size,user):
            key=hex(address);self.original_calls[key]=self.original_calls.get(key,0)+1
        for address in (0x4f2260,0x4f2400,0x4f2560):
            self.fixture.vm.uc.hook_add(UC_HOOK_CODE,observe,begin=address,end=address)
        self.fixture.reset()

    def set_camera(self,camera):
        if camera.get('position',[0,0,0])!=[0,0,0] or camera.get('zdist',1925)!=1925:raise ValueError('Mist uses its fixed verified camera')
        return super().set_camera(camera)

    def set_light(self,light):
        if light.get('rgba5')!=[31,31,31,31]:raise ValueError('Mist uses fixed full-white vertex modulation')
        return super().set_light(light)

    def advance_ticks(self,ticks):
        for _ in range(ticks):
            before=self.fixture.state()['particles'];self.fixture.animate();after=self.fixture.state()['particles']
            for old,new in zip(before,after):
                if not old['dead'] and new['dead']:self.lifecycle_observations['deaths']+=1
                if old['dead'] and not new['dead']:self.lifecycle_observations['respawns']+=1
            self.fixture.vm.advance();self.ticks+=1
        return self.inspect_state()

    def inspect_state(self):
        state=self.state_metadata();state.update(self.fixture.state())
        state.update(drop_count=50,persistent=True,lifecycle_observations=dict(self.lifecycle_observations),
            original_initialize='0x4f2260',original_animate='0x4f2400',original_render='0x4f2560',
            original_function_calls=dict(self.original_calls),native_random_inputs=copy.deepcopy(self.fixture.random),
            scope_limit='Face0/identity owner/fixed white inputs; natural map/component binding/lighting/culling and modern GPU remain separate')
        return state

    def checkpoint_extra(self):
        self.saved_mist=copy.deepcopy({name:getattr(self.fixture,name) for name in ('random','random_pending','packets')})
        self.saved_original_calls=dict(self.original_calls);self.saved_lifecycle=dict(self.lifecycle_observations)

    def reset_extra(self):
        for name,value in copy.deepcopy(self.saved_mist).items():setattr(self.fixture,name,value)
        self.original_calls=dict(self.saved_original_calls);self.lifecycle_observations=dict(self.saved_lifecycle)

    def render(self):
        draws=self.fixture.original_draws();color,depth=self.fixture.pixels(draws)
        return color,depth,dict(draws=draws,face=0,owner_world='identity')


class PixieAdapter(MistAdapter):
    """Native persistent swarm in the specifically verified empty-character context."""
    def __init__(self,executable,params,build=None):
        from pixie_probe import PixieFixture
        from unicorn import UC_HOOK_CODE
        if set(params)-{'archive'}:
            raise ValueError('Pixie accepts archive only; translation/face/nearby character inputs are unverified')
        self.inputs(params)
        self.fixture=PixieFixture(executable,self.asset('Imagery/Misc/pixies.i3d'),build=build)
        self.fixture.calls={};self.original_calls={}
        def observe(uc,address,size,user):
            key=hex(address);self.original_calls[key]=self.original_calls.get(key,0)+1
        for address in (0x4f3820,0x4f39a0,0x4f3d10):
            self.fixture.vm.uc.hook_add(UC_HOOK_CODE,observe,begin=address,end=address)
        self.fixture.reset()

    def advance_ticks(self,ticks):
        for _ in range(ticks):
            self.fixture.animate();self.fixture.vm.advance();self.ticks+=1
        return self.inspect_state()

    def inspect_state(self):
        state=self.state_metadata();state.update(self.fixture.state())
        state.update(particle_count=25,persistent=True,nearby_character_context='explicitly empty',
            original_initialize='0x4f3820',original_animate='0x4f39a0',original_render='0x4f3d10',
            original_function_calls=dict(self.original_calls),native_random_inputs=copy.deepcopy(self.fixture.random),
            owner_position=list(struct.unpack('<3i',self.fixture.vm.uc.mem_read(self.fixture.owner+16,12))),
            scope_limit='Identity owner/face0, empty player/character queries, declared provider culling; natural interactions/map/lighting/device culling/modern GPU remain separate')
        return state

    def checkpoint_extra(self):
        self.saved_pixie=copy.deepcopy({name:getattr(self.fixture,name)
            for name in ('random','random_pending','packets','map_queries')})
        self.saved_original_calls=dict(self.original_calls);self.saved_texture=self.fixture.software.texture

    def reset_extra(self):
        for name,value in copy.deepcopy(self.saved_pixie).items():setattr(self.fixture,name,value)
        self.original_calls=dict(self.saved_original_calls);self.fixture.software.texture=self.saved_texture

    def render(self):
        draws=self.fixture.original_draws();color,depth=self.fixture.pixels(draws)
        return color,depth,dict(draws=draws,face=0,owner_world='identity',nearby_characters='empty',
            culling='Existing frontend/provider formula; not native device parity')


class SymGlowAdapter(MistAdapter):
    """Native render-mutated UVs: one Render per captured simulation timer."""
    def __init__(self,executable,params,build=None):
        from symglow_probe import SymGlowFixture,parse_asset
        from unicorn import UC_HOOK_CODE
        if set(params)-{'archive'}:raise ValueError('SymGlow accepts archive only; verified owner/camera/light/provider boundaries are fixed')
        self.inputs(params)
        self.fixture=SymGlowFixture(executable,parse_asset(self.asset('Imagery/Misc/symglow.i3d')))
        self.fixture.calls={};self.original_calls={};self.cached_key=None;self.cached_render=None
        def observe(uc,address,size,user):
            key=hex(address);self.original_calls[key]=self.original_calls.get(key,0)+1
        for address in (0x4e5130,0x4e51c0,0x4e5260):
            self.fixture.vm.uc.hook_add(UC_HOOK_CODE,observe,begin=address,end=address)
        self.fixture.reset()

    def advance_ticks(self,ticks):
        for _ in range(ticks):
            self.fixture.advance();self.fixture.vm.advance();self.ticks+=1
        return self.inspect_state()

    def inspect_state(self):
        state=self.state_metadata();state.update(self.fixture.render_state(False))
        state.update(original_setup='0x4e5130',original_animate='0x4e51c0',original_render='0x4e5260',
            original_function_calls=dict(self.original_calls),native_random_inputs=copy.deepcopy(self.fixture.random),
            rendered_current_timer=self.cached_key==(self.ticks,state['timer']),
            render_cadence='Native Render scrolls UV; captures render once per simulation tick/timer; state inspection is read-only',
            scope_limit='Selected capture cadence/fixed owner/white/provider culling only; real map/render rate/light/GPU separate')
        return state

    def checkpoint_extra(self):
        self.saved_sym=copy.deepcopy(dict(random=self.fixture.random,random_pending=self.fixture.random_pending,
            original_calls=self.original_calls,cached_key=self.cached_key,cached_render=self.cached_render))

    def reset_extra(self):
        saved=copy.deepcopy(self.saved_sym)
        self.fixture.random=saved['random'];self.fixture.random_pending=saved['random_pending']
        for key in ('original_calls','cached_key','cached_render'):setattr(self,key,saved[key])

    def render(self):
        timer=self.fixture.render_state(False)['timer'];key=(self.ticks,timer)
        if key!=self.cached_key:
            self.fixture.render_state(True)
            draws=self.fixture.original_draws();color,depth=self.fixture.pixels(draws)
            self.cached_render=(color,depth,dict(draws=draws,capture_tick=self.ticks,native_timer=timer,
                policy='One native UV-mutating Render per captured timer; repeated reads use exact cached buffers'))
            self.cached_key=key
        return copy.deepcopy(self.cached_render)


class WaterfallAdapter(MistAdapter):
    """Literal Waterfall with unchanged native warmup and all hundred records."""
    def __init__(self,executable,params,build=None):
        from waterfall_probe import WaterfallFixture
        from unicorn import UC_HOOK_CODE
        if set(params)-{'archive'}:raise ValueError('Literal Waterfall accepts archive only; owner/face/light/camera remain fixed')
        self.inputs(params)
        self.fixture=WaterfallFixture(executable,self.asset('Imagery/Misc/water.i3d'),build=build)
        self.fixture.calls={};self.original_calls={};self.lifecycle_observations=dict(landings=0,respawns=0)
        def observe(uc,address,size,user):
            key=hex(address);self.original_calls[key]=self.original_calls.get(key,0)+1
        for address in (0x4f2e60,0x4f2f00,0x4f3060):
            self.fixture.vm.uc.hook_add(UC_HOOK_CODE,observe,begin=address,end=address)
        self.fixture.reset()

    def pool(self):
        vm=self.fixture.vm;count=vm.u32(self.fixture.animator+0x100)
        if count!=100:raise AssertionError('Literal Waterfall native count changed')
        pointer=vm.u32(self.fixture.animator+0xfc)
        records=[]
        for index in range(count):
            values=struct.unpack('<9fi',vm.uc.mem_read(pointer+index*40,40))
            records.append(dict(index=index,time=values[9],values=list(values[:9])))
        return records

    def advance_ticks(self,ticks):
        for _ in range(ticks):
            before=self.pool();self.fixture.animate();after=self.pool()
            for old,new in zip(before,after):
                if old['time']!=-1 and new['time']==-1:self.lifecycle_observations['landings']+=1
                if old['time']==-1 and new['time']!=-1:self.lifecycle_observations['respawns']+=1
            self.fixture.vm.advance();self.ticks+=1
        return self.inspect_state()

    def inspect_state(self):
        return dict(**self.state_metadata(),particles=self.pool(),drop_count=100,random_count=len(self.fixture.random),
            native_warmup_ticks=100,native_random_inputs=copy.deepcopy(self.fixture.random),
            original_initialize='0x4f2e60',original_animate='0x4f2f00',original_render='0x4f3060',
            original_function_calls=dict(self.original_calls),lifecycle_observations=dict(self.lifecycle_observations),
            omitted_copied_light_calls=self.fixture.ignored_light_calls,
            scope_limit='Literal type only; controlled-white original indices, unused copied lighting boundary explicit. No WCap/WFall aliases, natural map or modern triangulation/culling/light/GPU acceptance')

    def checkpoint_extra(self):
        self.saved_waterfall=copy.deepcopy({name:getattr(self.fixture,name)
            for name in ('random','random_pending','packets','ignored_light_calls')})
        self.saved_original_calls=dict(self.original_calls);self.saved_lifecycle=dict(self.lifecycle_observations)

    def reset_extra(self):
        for name,value in copy.deepcopy(self.saved_waterfall).items():setattr(self.fixture,name,value)
        self.original_calls=dict(self.saved_original_calls);self.lifecycle_observations=dict(self.saved_lifecycle)

    def render(self):
        draws=self.fixture.original_draws();color,depth=self.fixture.pixels(draws)
        return color,depth,dict(draws=draws,controlled_white=True,original_indices=True,
            omitted_copied_light_calls=self.fixture.ignored_light_calls)


class SparksAdapter(MistAdapter):
    """Two proven targetless parameter blocks; InitParticles defines tick zero."""
    PARAMETERS={
        'editor':dict(particles=10,pos=[0,0,70],pspread=[3,3,3],dir=[1,-1,.5],
            spread=[.5,.5,.5],gravity=.2,trails=1,minstart=0,maxstart=8,
            minlife=10,maxlife=30,bounce=False,killobj=False,objflags=15,
            seektargets=False,seekz=False,numtargets=0),
        'controlled_bounce_trail':dict(particles=20,pos=[0,0,45],pspread=[3,3,3],dir=[1,0,0],
            spread=[.5,.5,.5],gravity=.25,trails=2,minstart=0,maxstart=8,
            minlife=20,maxlife=40,bounce=True,killobj=True,objflags=2,
            seektargets=False,seekz=False,numtargets=0)}

    def __init__(self,executable,params,build=None):
        from sparks_probe import SparksFixture,parse_asset
        from unicorn import UC_HOOK_CODE
        if set(params)-{'archive','profile'}:
            raise ValueError('Sparks accepts only archive/profile; seeking, owner pose and arbitrary parameters are unverified')
        self.inputs(params,('profile',));self.profile=params.get('profile','editor')
        if self.profile not in self.PARAMETERS:raise ValueError('Unknown verified Sparks parameter profile')
        self.fixture=SparksFixture(executable,parse_asset(self.asset('Imagery/Misc/sparks.i3d')),build=build)
        self.fixture.calls={};self.original_calls={}
        def observe(uc,address,size,user):
            key=hex(address);self.original_calls[key]=self.original_calls.get(key,0)+1
        for address in (0x4e53c0,0x4e55b0,0x4e5830,0x4e5d30):
            self.fixture.vm.uc.hook_add(UC_HOOK_CODE,observe,begin=address,end=address)
        self.fixture.reset();self.fixture.initialize(copy.deepcopy(self.PARAMETERS[self.profile]))

    def advance_ticks(self,ticks):
        for _ in range(ticks):
            self.fixture.advance();self.fixture.vm.advance();self.ticks+=1
        return self.inspect_state()

    def inspect_state(self):
        state=self.state_metadata();state.update(self.fixture.state())
        state.update(profile=self.profile,type_id='0x14db0f2e',native_params=copy.deepcopy(self.fixture.params),
            native_initial_state=copy.deepcopy(self.fixture.initial_state),
            native_random_inputs=copy.deepcopy(self.fixture.random),native_raw_crt_draws=self.fixture.raw_random_draws,
            original_initialize='0x4e53c0',original_init_particles='0x4e55b0',
            original_animate='0x4e5830',original_render='0x4e5d30',
            original_function_calls=dict(self.original_calls),birth_tick_policy='Explicit native InitParticles; no implicit Animate offset',
            scope_limit='Two proven targetless profiles, identity owner/full-white original indices. POS1-only matrix ignores stored .01 scale. Seeking/combat/natural map/hardware blend/modern GPU remain separate')
        return state

    def checkpoint_extra(self):
        self.saved_sparks=copy.deepcopy({name:getattr(self.fixture,name) for name in
            ('random','random_pending','packets','done','initial_state','params','raw_random_draws')})
        self.saved_original_calls=dict(self.original_calls)

    def reset_extra(self):
        for name,value in copy.deepcopy(self.saved_sparks).items():setattr(self.fixture,name,value)
        self.original_calls=dict(self.saved_original_calls)

    def render(self):
        draws=self.fixture.original_draws();color,depth=self.fixture.pixels(draws)
        return color,depth,dict(draws=draws,profile=self.profile,controlled_white=True,
            original_indices=True,object_flags=4,stored_scale_applied_to_geometry=False)
