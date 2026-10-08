#!/usr/bin/env python3
"""Structured fixture control through original retail code, without editor input."""
import argparse
import copy
from dataclasses import dataclass
import hashlib
import json
from pathlib import Path
import sys
import time
import struct
import math
import zipfile


class UnsupportedControl(RuntimeError):
    pass


@dataclass(frozen=True)
class Profile:
    factory: object
    description: str
    actions: tuple
    required_sha256: str | None = None
    kind: str = 'stationary'
    requires_destination: bool = False
    input_fields: tuple = ()
    named_builds: bool = False
    catalog: tuple = ()


class PartsysAdapter:
    """Synthetic one-emitter/owner fixture; Pulse and pose execute original x86."""
    def __init__(self, executable, params):
        from effect_probe import PartsysProbe
        unknown=set(params)-{'shape','face','render','seed','owner'}
        if unknown:
            raise ValueError('Unsupported authoredpartsys parameters: '+', '.join(sorted(unknown)))
        self.probe=PartsysProbe(executable)
        self.probe.configure(**params)
        self.probe.checkpoint()

    def advance_ticks(self,ticks):
        self.probe.step(ticks)
        return self.inspect_state()

    def inspect_state(self):
        state=self.probe.inspect()
        # Restored-page counts are instrumentation, not effect state.
        state.pop('restored_pages',None)
        state.update(ticks=self.probe.tick,fixture_parameters=copy.deepcopy(self.probe.config),
            original_pulse='0x403760',render_scope='pose/color only; device submission not executed',
            independent_fixture=True,pixel_capture=False)
        return state

    def checkpoint(self):
        self.probe.checkpoint()

    def reset(self):
        self.probe.reset()

    def capture(self,kind):
        if kind!='state':
            raise UnsupportedControl('authoredpartsys supports state capture; retail pixel rendering is not connected')
        return self.inspect_state()

    def set_owner(self,owner):
        position=owner['position'];face=owner['facing']
        vm=self.probe.vm
        for offset,value in zip((16,20,24),position):
            vm.put_u32(self.probe.owner+offset,value & 0xffffffff)
        vm.write(self.probe.owner+54,bytes([face]))
        self.probe.config.update(owner=list(position),face=face)
        return dict(position=list(position),facing=face,
            scope='Synthetic owner fields at +16/+20/+24 and facing +54; no map actor creation',
            unmapped_fields=['stats','attachments'])


class MissileAdapter:
    """Original shared missile launch/Move/Pulse, not the complete Fireball leaf."""
    def __init__(self,executable,params,build=None):
        from projectile_probe import MissileProbe
        unknown=set(params)-{'speed','wall_x','ground_height','launch_ready','animator_present'}
        if unknown:raise ValueError('Unsupported shared_missile_base launch parameters: '+', '.join(sorted(unknown)))
        if 'speed' in params and (type(params['speed']) is not int or not 1<=params['speed']<=120):
            raise ValueError('Missile speed must be an integer from 1 to 120')
        for name in ('ground_height','wall_x'):
            value=params.get(name)
            if value is not None and (type(value) is not int or not -0x80000000<=value<=0x7fffffff):
                raise ValueError(name+' must fit signed int32')
        for name in ('launch_ready','animator_present'):
            if name in params and type(params[name]) is not bool:raise ValueError(name+' must be boolean')
        self.probe=MissileProbe(executable,build=build)
        self.trajectory=[]

    @staticmethod
    def _target_options(entity):
        stats=entity.get('stats',{})
        hp=stats.get('hp',100);enemy=stats.get('enemy',True)
        if type(hp) is not int or not -0x80000000<=hp<=0x7fffffff or type(enemy) is not bool:
            raise ValueError('Missile target hp must fit int32 and enemy must be boolean')
        return dict(target_hp=hp,enemy=enemy)

    def launch(self,source,destination,params):
        if destination is None:raise ValueError('shared_missile_base requires a supplied destination')
        initial=self.probe.configure(source['position'],destination['position'],
            **self._target_options(destination),**params)
        self.trajectory=[copy.deepcopy(initial)]
        self.source=copy.deepcopy(source);self.destination=copy.deepcopy(destination)

    def advance_ticks(self,ticks):
        self.trajectory.extend(self.probe.step(ticks))
        return self.inspect_state()

    def inspect_state(self):
        state=self.probe.inspect()
        positions={tuple(row['position']) for row in self.trajectory}
        state.update(ticks=state['tick'],trajectory=copy.deepcopy(self.trajectory),
            events=copy.deepcopy(self.probe.events),original_function_calls=dict(self.probe.calls),
            motion_observed=len(positions)>1 and self.probe.calls.get('0x470920',0)>0,
            source_fixture=copy.deepcopy(self.source),destination_fixture=copy.deepcopy(self.destination),
            scope='Original shared missile base: launch/Move/Pulse, collision/impact and kill request. No full Fireball animator/damage or map reaper.',
            pixel_capture=False,map_reaper_executed=False,fireball_leaf_acceptance=False)
        return state

    def checkpoint(self):
        self.probe.checkpoint()
        self.saved=copy.deepcopy(dict(trajectory=self.trajectory,source=self.source,destination=self.destination))

    def reset(self):
        self.probe.reset()
        for key,value in copy.deepcopy(self.saved).items():setattr(self,key,value)

    def set_source(self,entity):
        self.probe.set_source(entity['position'])
        self.source=copy.deepcopy(entity)
        return dict(source=copy.deepcopy(entity),missile_position_unchanged=True)

    def set_destination(self,entity):
        self.probe.set_target(entity['position'],**self._target_options(entity))
        self.destination=copy.deepcopy(entity)
        return dict(destination=copy.deepcopy(entity),missile_position_unchanged=True)

    def capture(self,kind):
        if kind!='state':raise UnsupportedControl('Shared missile base does not capture rendered pixels')
        return self.inspect_state()


class FireballHeadAdapter(MissileAdapter):
    """Actual moving missile and native head/glow raster in the same guest VM."""
    def __init__(self,executable,params):
        from fireball_head_probe import HeadFixture,ASSET_SHA,ROOT,INDEX_OFFSET,INDICES
        self.physics_keys={'speed','wall_x','ground_height','launch_ready','animator_present'}
        if set(params)-self.physics_keys-{'archive','render_inputs'}:
            raise ValueError('Fireball head supports missile launch parameters, archive and explicit render_inputs only')
        self.render_inputs=dict(frame=0,rotation=0,scale=.3,glow=1.5,face=0)
        self.set_render_inputs(params.get('render_inputs',{}))
        archive_path=Path(params.get('archive',ROOT/'data/imagery.rvi')).resolve()
        with zipfile.ZipFile(archive_path) as archive:asset=archive.read('Imagery/Magic/newfireball.i3d')
        if hashlib.sha256(asset).hexdigest()!=ASSET_SHA or struct.unpack_from('<6H',asset,INDEX_OFFSET)!=INDICES:
            raise ValueError('Unsupported shipped Fireball head asset')
        self.asset=dict(member='Imagery/Magic/newfireball.i3d',sha256=ASSET_SHA,bytes=len(asset))
        self.fixture=HeadFixture(executable,asset);self.probe=self.fixture.missile;self.trajectory=[]

    def launch(self,source,destination,params):
        super().launch(source,destination,{key:value for key,value in params.items() if key in self.physics_keys})

    def set_render_inputs(self,values):
        if not isinstance(values,dict) or set(values)-{'frame','rotation','scale','glow','face'}:
            raise ValueError('Head render inputs are frame, rotation, scale, glow and face')
        result={**self.render_inputs,**values}
        if type(result['frame']) is not int or not 0<=result['frame']<=15:
            raise ValueError('Head render frame must be an integer0..15')
        if type(result['rotation']) is not int or not -0x80000000<=result['rotation']<=0x7fffffff:
            raise ValueError('Head rotation must fit signed int32')
        if type(result['face']) is not int or not 0<=result['face']<=255:
            raise ValueError('Head face must fit a byte')
        for key in ('scale','glow'):
            if type(result[key]) not in (int,float) or not math.isfinite(result[key]) or not 0<result[key]<=10:
                raise ValueError('Head '+key+' must be finite and positive <=10')
        self.render_inputs=result
        return dict(inputs=copy.deepcopy(result),scope='Selected renderer inputs; no original full animator waveform is claimed')

    def inspect_state(self):
        state=super().inspect_state()
        state.update(pixel_capture=True,asset=copy.deepcopy(self.asset),render_inputs=copy.deepcopy(self.render_inputs),
            original_head_kernels=['0x511dd0','0x511fa0'],original_raster='0x56d960',
            head_scope='Moving original missile plus selected head/glow pose inputs; no trail/burst/sparks/damage/full animator/map removal or modern GPU acceptance')
        return state

    def checkpoint(self):
        super().checkpoint();self.fixture.surface.checkpoint()
        self.saved_render_inputs=copy.deepcopy(self.render_inputs)

    def reset(self):
        super().reset();self.fixture.surface.restore()
        self.render_inputs=copy.deepcopy(self.saved_render_inputs)

    def capture(self,kind):
        if kind=='state':return self.inspect_state()
        if kind!='pixels':raise ValueError('Capture kind must be state or pixels')
        from pixel_controls import PixelFrame
        p=self.render_inputs
        cards=[self.fixture.packet(glow,p['frame'],p['rotation'],p['scale'],p['glow'],p['face'])
            for glow in (True,False)]
        color=self.fixture.pixels(cards);depth=self.probe.vm.surface_bytes('depth')
        return PixelFrame(color,depth,self.fixture.surface.width,self.fixture.surface.height,
            dict(cards=cards,state=self.inspect_state(),same_vm_motion_and_raster=True,pose_input_policy='explicit selected inputs'))


class Controller:
    """Explicit profile registry and per-effect fixtures, not a simulated game world."""
    def __init__(self,executable,build=None,profiles=None):
        self.executable=Path(executable).resolve()
        self.executable_sha256=hashlib.sha256(self.executable.read_bytes()).hexdigest()
        if isinstance(build,(str,Path)):build=json.loads(Path(build).read_text())
        self.build=copy.deepcopy(build or {})
        if self.build.get('executable_sha256',self.executable_sha256)!=self.executable_sha256:
            raise ValueError('Build SHA256 differs from the selected executable')
        self.build.update(executable=str(self.executable),executable_sha256=self.executable_sha256)
        self.build.setdefault('build_id','unregistered-'+self.executable_sha256)
        self.profiles={}
        self.effects={}
        self.saved_effects={}
        self.entities={}
        self.bindings={}
        self.saved_entities={}
        self.saved_bindings={}
        self.timeline_tick=0
        self.actor_motion={}
        self.actor_input_trace=[]
        self.saved_timeline={}
        if profiles is None:
            from effect_probe import RETAIL_SHA
            self.register_profile('authoredpartsys',Profile(PartsysAdapter,
                'Synthetic bounded one-emitter/owner partsys fixture. Original Pulse/spawn/update and optional pose/color instructions; identity emitter transforms and ground zero.',
                ('advance_ticks','inspect_state','checkpoint','reset','capture_state','set_owner'),RETAIL_SHA,
                input_fields=('source.position','source.facing')))
            self.register_profile('shared_missile_base',Profile(
                lambda executable,params:MissileAdapter(executable,params,build=self.profile_build()),
                'Original shared missile launch/GetAngle/Move/Pulse with supplied source+destination, explicit target health/enmity and ground/wall inputs. No full Fireball leaf, damage, animator, map reaper or pixels.',
                ('launch','advance_ticks','inspect_state','checkpoint','reset','capture_state','set_source','set_destination'),
                RETAIL_SHA,kind='projectile',requires_destination=True,
                input_fields=('source.position','destination.position','destination.stats.hp','destination.stats.enemy'),named_builds=True))
            self.register_profile('fireball_head',Profile(FireballHeadAdapter,
                'Original moving shared missile plus original FireBall head/glow kernels and software raster in the same VM. Pose frame/spin/scale/glow selected explicitly; no full animator/trail/burst/sparks/damage/map/GPU acceptance.',
                ('launch','advance_ticks','inspect_state','checkpoint','reset','capture_state','capture_pixels','set_source','set_destination','set_render_inputs'),
                RETAIL_SHA,kind='projectile',requires_destination=True,
                input_fields=('source.position','destination.position','destination.stats.hp','destination.stats.enemy','selected_head_pose'),
                catalog=(('FireBall','0x63fd382a'),)))
            from pixel_controls import FlameAdapter,RippleAdapter,FizzleAdapter,MistFogAdapter,StaticMeshAdapter,static_profile_catalog,FountainAdapter,fountain_profile_catalog,ColoredRibbonAdapter,ribbon_profile_catalog,StreamerAdapter,MistAdapter,PixieAdapter,SymGlowAdapter,WaterfallAdapter,SparksAdapter
            actions=('advance_ticks','inspect_state','checkpoint','reset','capture_state','capture_pixels','set_camera','set_light')
            for name,adapter,description,position,named in (
                ('flame',FlameAdapter,'Original Flame Animate/Render, shipped atlas and original software pixels; bounded world/camera inputs, no map/Metal parity.',True,True),
                ('ripple_no_splash',RippleAdapter,'Original Ripple Initialize/Animate/Render and software pixels; lengths4/20/24 only, no random splash children.',True,False),
                ('fizzle',FizzleAdapter,'Original Fizzle Initialize/Animate/Render, native RNG and software pixels; local-Z abs_pos=0, no translated owner/map/Metal parity.',False,False)):
                factory=(lambda executable,params,adapter=adapter:adapter(executable,params,build=self.profile_build()))
                exact_ids={'flame':(('Flame','0x50ba373b'),('Flame','0x50ba373c'),('Flame','0x50ba373d')),
                    'ripple_no_splash':(('Ripple','0x12309867'),),'fizzle':(('Fizzle','0xab8800dd'),)}
                self.register_profile(name,Profile(factory,description,
                    actions+(('set_position','set_owner') if position else ()),RETAIL_SHA,
                    input_fields=('camera.position','vertex_rgba5')+(('source.position',) if position else ()),named_builds=named,
                    catalog=exact_ids[name]))
            self.register_profile('mistfog',Profile(MistFogAdapter,
                'Original 25-puff MistFog Initialize/Animate/Render/native RNG/software pixels. Identity owner, fixed camera and full-white vertex color; no map illumination/hardware blend/modern renderer acceptance.',
                ('advance_ticks','inspect_state','checkpoint','reset','capture_state','capture_pixels'),RETAIL_SHA,
                catalog=(('MistFog','0x180674ba'),)))
            self.register_profile('static_mesh',Profile(StaticMeshAdapter,
                'Verified thirteen constant single-object asset profiles: original key/matrix/mesh/software raster, explicit sampled frames0/50/99 and two owner translations. No full Globe/sign effect behavior, map lighting/culling or modern GPU.',
                ('advance_ticks','inspect_state','checkpoint','reset','capture_state','capture_pixels','set_position','set_owner','set_sample_frame'),
                RETAIL_SHA,input_fields=('profile.name','type_id','source.position','sample_frame'),
                catalog=tuple((p['name'],p['id']) for p in static_profile_catalog())))
            self.register_profile('fountain',Profile(FountainAdapter,
                'Four verified color leaves: original Fountain Initialize/Animate/Render, delay/rise/shrink/respawn/native RNG and software pixels. Identity owner/fixed camera/full-white inputs; no natural map ownership/lighting or modern GPU acceptance.',
                ('advance_ticks','inspect_state','checkpoint','reset','capture_state','capture_pixels'),RETAIL_SHA,
                input_fields=('profile.name','color','type_id'),catalog=tuple(fountain_profile_catalog())))
            self.register_profile('colored_ribbon',Profile(ColoredRibbonAdapter,
                'Seven verified generic static colored Ribbon assets, both parts/textures composited in authored order through original key/matrix/software raster. Constantframe0/two verified owner poses; base Ribbon revive/combat, map and modern GPU excluded.',
                ('advance_ticks','inspect_state','checkpoint','reset','capture_state','capture_pixels','set_position','set_owner','set_sample_frame'),
                RETAIL_SHA,input_fields=('profile.name','type_id','source.position'),
                catalog=tuple((p['name'],p['id']) for p in ribbon_profile_catalog())))
            self.register_profile('streamer',Profile(StreamerAdapter,
                'Original four-stream one-shot Init/Animate/Render/software pixels; fixed face0/identity owner and explicit provider culling. No spell/audio/map illumination/device culling or modern GPU acceptance.',
                ('advance_ticks','inspect_state','checkpoint','reset','capture_state','capture_pixels'),RETAIL_SHA,
                catalog=(('Streamer','0x482dfe82'),)))
            self.register_profile('mist',Profile(
                lambda executable,params:MistAdapter(executable,params,build=self.profile_build()),
                'Original continuous50-drop Mist Init/Animate/Render/native RNG/software pixels. Face0/identity/fixed white inputs; map/component binding/lighting/culling and modern GPU separate.',
                ('advance_ticks','inspect_state','checkpoint','reset','capture_state','capture_pixels'),RETAIL_SHA,
                named_builds=True,catalog=(('Mist','0x2093487a'),)))
            self.register_profile('pixie',Profile(
                lambda executable,params:PixieAdapter(executable,params,build=self.profile_build()),
                'Original persistent25-particle Pixie swarm/native RNG/mesh selection/software pixels. Only empty nearby-character context/identity owner/face0 and provider culling verified; natural target interactions/map/GPU remain separate.',
                ('advance_ticks','inspect_state','checkpoint','reset','capture_state','capture_pixels'),RETAIL_SHA,
                named_builds=True,catalog=(('Pixie','0x89abcde1'),)))
            self.register_profile('symglow',Profile(SymGlowAdapter,
                'Original SymGlow Setup/Animate/UV-mutating Render/software pixels. Capture cadence explicit and cached per timer; fixed owner/lighting/provider culling, no map/GPU acceptance.',
                ('advance_ticks','inspect_state','checkpoint','reset','capture_state','capture_pixels'),RETAIL_SHA,
                catalog=(('SymGlow','0x27df45be'),)))
            self.register_profile('waterfall',Profile(
                lambda executable,params:WaterfallAdapter(executable,params,build=self.profile_build()),
                'Literal Waterfall original100-tick warmup/100-drop state/RNG/render/software pixels. Fixed identity/white inputs, copied-light boundary and original indices explicit; no WCap/WFall family or map/device/GPU acceptance.',
                ('advance_ticks','inspect_state','checkpoint','reset','capture_state','capture_pixels'),RETAIL_SHA,
                named_builds=True,catalog=(('Waterfall','0xa907dabf'),)))
            self.register_profile('sparks',Profile(
                lambda executable,params:SparksAdapter(executable,params,build=self.profile_build()),
                'Original Sparks InitParticles/Animate/Render/native RNG/software pixels with exact editor and controlled bounce/trails parameter blocks. Native POS1 geometry, targetless identity/white fixture; seeking/combat/map/device/GPU separate.',
                ('advance_ticks','inspect_state','checkpoint','reset','capture_state','capture_pixels'),RETAIL_SHA,
                named_builds=True,input_fields=('profile.name',),catalog=(('Sparks','0x14db0f2e'),)))
        else:
            for name,profile in profiles.items():
                self.register_profile(name,profile)

    def register_profile(self,name,profile):
        if name in self.profiles or not isinstance(profile,Profile):
            raise ValueError('Profile must be a new name with an explicit Profile descriptor')
        if profile.kind not in ('stationary','projectile','point_to_point'):
            raise ValueError('Profile kind must be stationary, projectile or point_to_point')
        self.profiles[name]=profile

    def profile_build(self):
        """Validate explicit named experimental images, retaining fixed-address ABI scope."""
        from build_contract import verify_build,RETAIL_SHA
        record=copy.deepcopy(self.build)
        if self.executable_sha256==RETAIL_SHA:
            record.setdefault('name','retail-original')
            if record.get('baseline_sha256') is None:record['baseline_sha256']=RETAIL_SHA
            if record.get('patch_manifest') is None:record['patch_manifest']=dict(kind='unchanged-baseline')
        verified=verify_build(self.executable,record)
        self.build=copy.deepcopy(verified)
        return verified

    def capabilities(self):
        optional={'set_owner','set_position','set_camera','set_light'}
        supported={action for p in self.profiles.values() for action in p.actions}
        return dict(build=copy.deepcopy(self.build),world_objects=False,shared_world=False,
            verified_retail_types=[dict(profile=name,name=retail_name,type_id=type_id)
                for name,p in self.profiles.items() for retail_name,type_id in p.catalog],
            verified_retail_types_scope='Audited identities with bounded adapter/frontend proof; not full game acceptance',
            editor_input=False,guest_os_boots=0,
            actions=['create_effect','remove_effect','advance_ticks','inspect_state','checkpoint','reset','capture','validate_execution_contract'],
            entity_inputs=dict(actions=['create_entity','remove_entity','set_entity_position','inspect_entities','spawn_effect'],
                scope='Fixture input handles, not original map actors',stats='recorded; authoredpartsys does not apply stats',
                attachments='recorded; authoredpartsys does not apply attachments',
                targeted_missiles=any(p.kind=='projectile' and 'launch' in p.actions for p in self.profiles.values()),
                scripted_actor_motion=True,
                motion_actions=['set_entity_motion','inspect_actor_inputs'],
                motion_scope='Recorded SOURCE/TARGET fixture inputs applied before original effect ticks; never missile teleportation'),
            unsupported=sorted(optional-supported)+([] if 'capture_pixels' in supported else ['pixel_capture']),
            profiles={name:dict(description=p.description,actions=list(p.actions),
                required_sha256=p.required_sha256,compatible=self.profile_compatible(p),
                kind=p.kind,motion_required=p.kind=='projectile',
                input_fields=list(p.input_fields),
                named_build_contract_supported=p.named_builds,
                catalog=[dict(name=name,type_id=type_id) for name,type_id in p.catalog],
                source_required=p.kind in ('projectile','point_to_point'),
                destination_required=p.requires_destination or p.kind=='point_to_point',
                distinct_primary_endpoints=p.requires_destination or p.kind=='point_to_point',
                acceptance='Original projectile motion over ticks; distinct point-to-point endpoints. Stationary previews cannot satisfy either contract.')
                for name,p in self.profiles.items()})

    def profile_compatible(self,profile):
        if profile.named_builds:
            try:self.profile_build()
            except ValueError:return False
            return True
        return profile.required_sha256 in (None,self.executable_sha256)

    def _effect(self,effect_id):
        if effect_id not in self.effects:
            raise ValueError('Unknown effect ID: '+str(effect_id))
        return self.effects[effect_id]

    def create_effect(self,effect_id,effect_type,params=None):
        profile=self._validate_new_effect(effect_id,effect_type,params)
        if profile.kind!='stationary':
            raise UnsupportedControl(profile.kind+' requires a validated original launch adapter; stationary fixture creation cannot test this effect')
        adapter=profile.factory(self.executable,copy.deepcopy(params or {}))
        self.effects[effect_id]=dict(type=effect_type,adapter=adapter)
        return dict(id=effect_id,type=effect_type,state=adapter.inspect_state(),build=copy.deepcopy(self.build))

    def _validate_new_effect(self,effect_id,effect_type,params):
        if params is not None and not isinstance(params,dict):
            raise ValueError('Effect parameters must be a JSON object')
        if not isinstance(effect_id,str) or not effect_id or effect_id in self.effects:
            raise ValueError('Effect ID must be a new nonempty string')
        if effect_type not in self.profiles:
            raise UnsupportedControl('No validated direct-code adapter for effect type: '+str(effect_type))
        profile=self.profiles[effect_type]
        if profile.required_sha256 not in (None,self.executable_sha256):
            if not profile.named_builds:
                raise UnsupportedControl('Profile addresses/ABI are not validated for this executable build')
            self.profile_build()
        if hashlib.sha256(self.executable.read_bytes()).hexdigest()!=self.executable_sha256:
            raise ValueError('Executable changed after controller creation; explicitly load a new build')
        return profile

    def remove_effect(self,effect_id):
        self._effect(effect_id)
        del self.effects[effect_id]
        self.bindings.pop(effect_id,None)
        return dict(id=effect_id,removed=True,scope='Fixture lifecycle; no retail map object destructor is claimed')

    def _supported(self,effect_id,action):
        effect=self._effect(effect_id)
        if action not in self.profiles[effect['type']].actions:
            raise UnsupportedControl(f"{effect['type']} does not support {action}")
        return effect['adapter']

    def advance_ticks(self,ticks=1,effect_id=None):
        if type(ticks) is not int or not 0 <= ticks <= 10000:
            raise ValueError('ticks must be an integer from 0 to 10000')
        selected=[effect_id] if effect_id is not None else sorted(self.effects)
        # Validate all selected profiles before running any tick.
        adapters=[self._supported(name,'advance_ticks') for name in selected]
        if self.actor_motion and set(selected)!=set(self.effects):
            raise UnsupportedControl('Scripted actor motion requires advancing every active fixture on the same control timeline')
        for _ in range(ticks):
            for entity_id,frames in sorted(self.actor_motion.items()):
                frame=frames.get(self.timeline_tick)
                if frame is not None:
                    self.set_entity_position(entity_id,frame['position'],frame.get('facing'))
                    self.actor_input_trace.append(dict(tick=self.timeline_tick,entity_id=entity_id,
                        position=list(frame['position']),facing=self.entities[entity_id]['facing']))
            for adapter in adapters:adapter.advance_ticks(1)
            self.timeline_tick+=1
        return self.inspect_state(effect_id)

    def inspect_state(self,effect_id=None):
        selected=[effect_id] if effect_id is not None else sorted(self.effects)
        return {name:self._supported(name,'inspect_state').inspect_state() for name in selected}

    def checkpoint(self):
        for effect_id in self.effects:
            self._supported(effect_id,'checkpoint').checkpoint()
        # Retain adapter objects so removing a fixture can be undone by reset.
        self.saved_effects={name:dict(value) for name,value in self.effects.items()}
        self.saved_entities=copy.deepcopy(self.entities)
        self.saved_bindings=copy.deepcopy(self.bindings)
        self.saved_timeline=copy.deepcopy(dict(tick=self.timeline_tick,motion=self.actor_motion,inputs=self.actor_input_trace))
        return dict(checkpointed=sorted(self.effects))

    def reset(self):
        self.effects={name:dict(value) for name,value in self.saved_effects.items()}
        self.entities=copy.deepcopy(self.saved_entities)
        self.bindings=copy.deepcopy(self.saved_bindings)
        timeline=copy.deepcopy(self.saved_timeline)
        self.timeline_tick=timeline.get('tick',0)
        self.actor_motion=timeline.get('motion',{})
        self.actor_input_trace=timeline.get('inputs',[])
        for effect_id in self.effects:
            self._supported(effect_id,'reset').reset()
        return dict(restored=sorted(self.effects))

    def capture(self,effect_id,kind='state',output=None,depth_output=None):
        if output is not None and depth_output is not None and Path(output).resolve()==Path(depth_output).resolve():
            raise ValueError('Color and depth outputs must have different paths')
        action='capture_state' if kind=='state' else 'capture_pixels'
        state=self._supported(effect_id,action).capture(kind)
        from pixel_controls import PixelFrame
        if isinstance(state,PixelFrame):
            color=state.color;depth=state.depth
            if len(color)!=state.width*state.height*2 or len(depth)!=len(color):
                raise AssertionError('Original pixel/depth buffers do not match the declared fixture dimensions')
            result=dict(id=effect_id,kind='pixels',format='RGB565',width=state.width,height=state.height,
                stride=state.width*2,bytes=len(color),sha256=hashlib.sha256(color).hexdigest(),
                depth_format='U16',depth_sha256=hashlib.sha256(depth).hexdigest(),
                nonzero_pixels=sum(value!=0 for value in struct.unpack('<'+'H'*(len(color)//2),color)),
                data=state.metadata,build=copy.deepcopy(self.build),actor_inputs=self.inspect_actor_inputs(),
                binding=copy.deepcopy(self.bindings.get(effect_id)))
            if output is not None:
                path=Path(output);path.parent.mkdir(parents=True,exist_ok=True)
                if path.suffix.lower()=='.png':
                    from software_probe import save_rgb565_png
                    save_rgb565_png(path,color,state.width,state.height)
                else:path.write_bytes(color)
                result.update(output=str(path),output_sha256=hashlib.sha256(path.read_bytes()).hexdigest())
            if depth_output is not None:
                path=Path(depth_output);path.parent.mkdir(parents=True,exist_ok=True);path.write_bytes(depth)
                result['depth_output']=str(path)
            return result
        if depth_output is not None:raise ValueError('Depth output requires a pixel capture')
        data=json.dumps(state,sort_keys=True,separators=(',',':')).encode()
        if output is not None:
            path=Path(output);path.parent.mkdir(parents=True,exist_ok=True);path.write_bytes(data)
        return dict(id=effect_id,kind=kind,sha256=hashlib.sha256(data).hexdigest(),bytes=len(data),
            data=state,build=copy.deepcopy(self.build),actor_inputs=self.inspect_actor_inputs(),
            binding=copy.deepcopy(self.bindings.get(effect_id)))

    def validate_execution_contract(self,effect_id):
        effect=self._effect(effect_id);profile=self.profiles[effect['type']]
        state=effect['adapter'].inspect_state();binding=self.bindings.get(effect_id,{})
        if profile.kind=='projectile' and not state.get('motion_observed',False):
            raise AssertionError('Projectile contract requires movement computed by original simulation ticks')
        if profile.requires_destination or profile.kind=='point_to_point':
            source=binding.get('source_at_launch');destination=binding.get('destination_at_launch')
            if source is None or destination is None:
                raise AssertionError('Point-to-point contract requires two supplied endpoints')
            if binding.get('case_kind')!='degenerate' and source['position']==destination['position']:
                raise AssertionError('Primary source/destination endpoints must be distinct')
        return dict(status='pass',profile=effect['type'],kind=profile.kind,
            motion_observed=state.get('motion_observed'),case_kind=binding.get('case_kind','primary'),
            counts_as_primary=binding.get('case_kind')!='degenerate',
            scope='Execution contract only; not full effect visual/map/damage acceptance')

    def operation(self,request):
        op=request['op']
        if op=='create_entity':
            return self.create_entity(request['id'],request['position'],request.get('facing',0),
                request.get('stats'),request.get('attachments'))
        if op=='remove_entity':
            return self.remove_entity(request['id'])
        if op=='set_entity_position':
            return self.set_entity_position(request['id'],request['position'],request.get('facing'))
        if op=='set_entity_motion':
            return self.set_entity_motion(request['id'],request['frames'])
        if op=='inspect_actor_inputs':
            return self.inspect_actor_inputs()
        if op=='inspect_entities':
            return self.inspect_entities(request.get('id'))
        if op=='spawn_effect':
            return self.spawn_effect(request['id'],request['type'],request.get('source_id'),
                request.get('destination_id'),request.get('launch_params'),
                request.get('case_kind','primary'),request.get('case_label'))
        if op=='create_effect':
            return self.create_effect(request['id'],request['type'],request.get('params'))
        if op=='remove_effect':
            return self.remove_effect(request['id'])
        if op=='advance_ticks':
            return self.advance_ticks(request.get('ticks',1),request.get('id'))
        if op=='inspect_state':
            return self.inspect_state(request.get('id'))
        if op=='checkpoint':
            return self.checkpoint()
        if op=='reset':
            return self.reset()
        if op=='capture':
            return self.capture(request['id'],request.get('kind','state'),request.get('output'),request.get('depth_output'))
        if op=='validate_execution_contract':
            return self.validate_execution_contract(request['id'])
        if op=='set_sample_frame':
            return self.set_sample_frame(request['id'],request['frame'])
        if op=='set_render_inputs':
            return self.set_render_inputs(request['id'],request['inputs'])
        if op in ('set_owner','set_position','set_camera','set_light'):
            return getattr(self,op)(request['id'],request[op[4:]])
        raise UnsupportedControl('Unknown structured control: '+str(op))

    def _setting(self,action,effect_id,value):
        adapter=self._supported(effect_id,action)
        return getattr(adapter,action)(copy.deepcopy(value))

    def set_owner(self,effect_id,owner):
        if owner not in self.entities:
            raise ValueError('Unknown source entity handle: '+str(owner))
        profile=self.profiles[self._effect(effect_id)['type']]
        action='set_source' if profile.kind!='stationary' else 'set_owner'
        result=self._setting(action,effect_id,self.entities[owner])
        self.bindings.setdefault(effect_id,dict(destination_id=None))['source_id']=owner
        return result

    def set_position(self,effect_id,position):
        return self._setting('set_position',effect_id,position)

    def set_camera(self,effect_id,camera):
        return self._setting('set_camera',effect_id,camera)

    def set_light(self,effect_id,light):
        return self._setting('set_light',effect_id,light)

    def set_sample_frame(self,effect_id,frame):
        return self._setting('set_sample_frame',effect_id,frame)

    def set_render_inputs(self,effect_id,inputs):
        return self._setting('set_render_inputs',effect_id,inputs)

    @staticmethod
    def _position(position):
        if not isinstance(position,(list,tuple)) or len(position)!=3 or any(
                type(v) is not int or not -0x80000000 <= v <= 0x7fffffff for v in position):
            raise ValueError('Fixture position must be three signed 32-bit integers')
        return list(position)

    def create_entity(self,entity_id,position,facing=0,stats=None,attachments=None):
        if not isinstance(entity_id,str) or not entity_id or entity_id in self.entities:
            raise ValueError('Entity handle must be a new nonempty string')
        if type(facing) is not int or not 0 <= facing <= 255:
            raise ValueError('facing must be an integer from 0 to 255')
        if stats is not None and not isinstance(stats,dict) or attachments is not None and not isinstance(attachments,dict):
            raise ValueError('stats and attachments must be JSON objects')
        entity=dict(id=entity_id,position=self._position(position),facing=facing,
            stats=copy.deepcopy(stats or {}),attachments=copy.deepcopy(attachments or {}),
            scope='Fixture input handle; no retail actor has been spawned')
        json.dumps(entity,allow_nan=False)
        self.entities[entity_id]=entity
        return copy.deepcopy(entity)

    def inspect_entities(self,entity_id=None):
        if entity_id is not None:
            if entity_id not in self.entities:raise ValueError('Unknown entity handle: '+str(entity_id))
            return copy.deepcopy(self.entities[entity_id])
        return copy.deepcopy(self.entities)

    def remove_entity(self,entity_id):
        self.inspect_entities(entity_id)
        if any(binding['source_id']==entity_id or binding['destination_id']==entity_id for binding in self.bindings.values()):
            raise ValueError('Entity is bound to an effect; remove or rebind the effect first')
        del self.entities[entity_id]
        self.actor_motion.pop(entity_id,None)
        return dict(id=entity_id,removed=True)

    def set_entity_position(self,entity_id,position,facing=None):
        entity=self.inspect_entities(entity_id);entity['position']=self._position(position)
        if facing is not None:
            if type(facing) is not int or not 0 <= facing <= 255:
                raise ValueError('facing must be an integer from 0 to 255')
            entity['facing']=facing
        updates=[]
        for name,binding in self.bindings.items():
            if binding['source_id']==entity_id:
                action='set_source' if self.profiles[self.effects[name]['type']].kind!='stationary' else 'set_owner'
                updates.append((name,action))
            if binding['destination_id']==entity_id:updates.append((name,'set_destination'))
        for name,action in updates:self._supported(name,action)
        for name,action in updates:self._setting(action,name,entity)
        self.entities[entity_id]=entity
        return dict(entity=copy.deepcopy(entity),updated_fixture_owners=[name for name,action in updates])

    def spawn_effect(self,effect_id,effect_type,source_id=None,destination_id=None,launch_params=None,
                     case_kind='primary',case_label=None):
        profile=self.profiles.get(effect_type)
        if profile is None:
            raise UnsupportedControl('No validated direct-code adapter for effect type: '+str(effect_type))
        if profile.kind!='stationary':
            if 'launch' not in profile.actions:
                raise UnsupportedControl(profile.kind+' launch is not connected: original motion/two-endpoint acceptance must not use a stationary fixture')
            self._validate_new_effect(effect_id,effect_type,launch_params)
            if source_id is None:
                raise ValueError('A moving/two-point fixture requires a supplied source entity handle')
            source=self.inspect_entities(source_id)
            requires_destination=profile.requires_destination or profile.kind=='point_to_point'
            if requires_destination and destination_id is None:
                raise ValueError('This profile requires a supplied destination entity handle')
            destination=self.inspect_entities(destination_id) if destination_id is not None else None
            if case_kind not in ('primary','degenerate'):
                raise ValueError('case_kind must be primary or degenerate')
            if case_kind=='degenerate' and (not isinstance(case_label,str) or not case_label):
                raise ValueError('Degenerate endpoint cases require an explicit case_label')
            if destination is not None and source['position']==destination['position'] and case_kind!='degenerate':
                raise ValueError('Primary source/destination endpoints must be distinct; label zero-length cases separately')
            adapter=profile.factory(self.executable,copy.deepcopy(launch_params or {}))
            adapter.launch(source,destination,copy.deepcopy(launch_params or {}))
            self.effects[effect_id]=dict(type=effect_type,adapter=adapter)
            self.bindings[effect_id]=dict(source_id=source_id,destination_id=destination_id,
                case_kind=case_kind,case_label=case_label,source_at_launch=source,destination_at_launch=destination)
            return dict(id=effect_id,type=effect_type,state=adapter.inspect_state(),
                binding=copy.deepcopy(self.bindings[effect_id]),build=copy.deepcopy(self.build))
        if destination_id is not None:
            raise UnsupportedControl('No validated original targeted/moving effect adapter; destination launches are unsupported')
        if source_id is not None:
            self.inspect_entities(source_id)
            if profile is None or 'set_owner' not in profile.actions:
                raise UnsupportedControl('Profile cannot bind a source entity owner')
        result=self.create_effect(effect_id,effect_type,launch_params)
        if source_id is not None:
            result['owner_binding']=self.set_owner(effect_id,source_id)
            result['state']=self._effect(effect_id)['adapter'].inspect_state()
        return result

    def set_entity_motion(self,entity_id,frames):
        self.inspect_entities(entity_id)
        if not isinstance(frames,list):raise ValueError('Actor motion frames must be a JSON array')
        parsed={}
        for frame in frames:
            tick=frame['tick']
            if type(tick) is not int or tick<self.timeline_tick or tick in parsed:
                raise ValueError('Actor motion ticks must be unique integers at or after the current control tick')
            value=dict(tick=tick,position=self._position(frame['position']))
            if 'facing' in frame:
                facing=frame['facing']
                if type(facing) is not int or not 0<=facing<=255:
                    raise ValueError('Motion facing must fit a byte')
                value['facing']=facing
            parsed[tick]=value
        self.actor_motion[entity_id]=parsed
        return dict(id=entity_id,frames=copy.deepcopy([parsed[tick] for tick in sorted(parsed)]),
            scope='Explicit actor fixture inputs; original Pulse still computes effect/particle motion')

    def inspect_actor_inputs(self):
        return dict(timeline_tick=self.timeline_tick,inputs=copy.deepcopy(self.actor_input_trace),
            schedules={entity_id:copy.deepcopy([frames[t] for t in sorted(frames)])
                for entity_id,frames in sorted(self.actor_motion.items())})

    def execute(self,scenario):
        if scenario.get('executable_sha256',self.executable_sha256)!=self.executable_sha256:
            raise ValueError('Scenario pins a different executable build')
        self.reset();start=time.perf_counter()
        try:
            results=[self.operation(op) for op in scenario['operations']]
        except Exception:
            self.reset()
            raise
        return dict(status='pass',name=scenario.get('name'),results=results,
            executable_sha256=self.executable_sha256,build=copy.deepcopy(self.build),
            elapsed_ms=(time.perf_counter()-start)*1000)


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('executable',type=Path)
    parser.add_argument('--setup',type=Path,help='One-time structured control operations')
    parser.add_argument('--build',type=Path,help='Exact named build.json provenance')
    parser.add_argument('--scenario',type=Path)
    parser.add_argument('--output',type=Path)
    args=parser.parse_args()
    build=json.loads(args.build.read_text()) if args.build else None
    controller=Controller(args.executable,build)
    if args.setup:
        for op in json.loads(args.setup.read_text()):controller.operation(op)
    controller.checkpoint()
    if args.scenario:
        result=controller.execute(json.loads(args.scenario.read_text()))
        if args.output:
            args.output.parent.mkdir(parents=True,exist_ok=True)
            args.output.write_text(json.dumps(result,indent=2)+'\n')
        print(json.dumps(result));return
    for line in sys.stdin:
        request={}
        try:
            request=json.loads(line)
            if not isinstance(request,dict):raise ValueError('Request must be an object')
            command=request.get('command','execute')
            if command=='status':
                result=dict(status='ready',executable_sha256=controller.executable_sha256,
                    capabilities=controller.capabilities(),guest_os_boots=0)
            elif command=='execute':result=controller.execute(request['scenario'])
            elif command=='action':result=dict(status='pass',result=controller.operation(request['action']))
            elif command=='checkpoint':result=dict(status='checkpointed',**controller.checkpoint())
            elif command=='restore':result=dict(status='restored',**controller.reset())
            else:raise UnsupportedControl('Unknown command: '+str(command))
        except Exception as error:
            result=dict(status='error',type=type(error).__name__,message=str(error))
        if isinstance(request,dict) and 'id' in request:result['id']=request['id']
        print(json.dumps(result),flush=True)


if __name__=='__main__':main()
