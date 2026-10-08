#!/usr/bin/env python3
"""Exercise concrete direct-code fixture controls, reset and parallel routing."""
import argparse
import json
from pathlib import Path
import subprocess
import sys
import tempfile
import time
import unittest
from controls import Controller,Profile,UnsupportedControl
from parallel import Pool,digest
from variants import Workspace

ROOT=Path(__file__).resolve().parents[2]
BASELINE=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe'
EVIDENCE={}


class ControlTests(unittest.TestCase):
    def setUp(self):
        self.temporary=tempfile.TemporaryDirectory();self.path=Path(self.temporary.name)
    def tearDown(self):
        self.temporary.cleanup()

    def test_create_advance_inspect_checkpoint_reset_remove_original_fixture(self):
        service=Controller(BASELINE)
        initial=service.create_effect('water','authoredpartsys',dict(shape=2,face=64,render=True,seed=7))
        self.assertEqual(initial['state']['ticks'],0)
        service.advance_ticks(3,'water');saved=service.inspect_state('water')['water']
        self.assertEqual(saved['ticks'],3)
        self.assertEqual(saved['original_function_calls']['0x403760'],3)
        service.checkpoint();service.advance_ticks(5,'water')
        self.assertEqual(service.inspect_state('water')['water']['ticks'],8)
        service.reset();self.assertEqual(service.inspect_state('water')['water'],saved)
        first=service.advance_ticks(2,'water')['water']
        service.reset();second=service.advance_ticks(2,'water')['water']
        self.assertEqual(first,second)
        capture=service.capture('water',output=self.path/'water-state.json')
        self.assertEqual(capture['kind'],'state')
        self.assertTrue((self.path/'water-state.json').is_file())
        service.remove_effect('water');self.assertEqual(service.inspect_state(),{})
        service.reset();self.assertEqual(service.inspect_state('water')['water'],saved)
        EVIDENCE['authoredpartsys']=dict(initial_ticks=0,checkpoint_tick=3,replayed_tick=5,
            original_pulse_calls=first['original_function_calls']['0x403760'],
            complete_pool_sha256=first['state_sha256'],capture_kind='state',pixel_comparison=False,
            note='Synthetic authoredpartsys fixture; ID water is a test label, not a shipped water effect.')

    def test_supported_features_and_unsupported_world_operations_fail(self):
        service=Controller(BASELINE)
        service.create_effect('fixture','authoredpartsys')
        self.assertFalse(service.capabilities()['shared_world'])
        self.assertFalse(service.capabilities()['editor_input'])
        for method,value in [('set_position',[1,2,3]),
                             ('set_camera',dict(x=0,y=0)),('set_light',dict(ambient=16))]:
            with self.assertRaises(UnsupportedControl):getattr(service,method)('fixture',value)
        with self.assertRaises(UnsupportedControl):service.capture('fixture','pixels')
        with self.assertRaises(UnsupportedControl):service.create_effect('flame','unregistered-flame')
        with self.assertRaises(ValueError):service.create_effect('fixture','authoredpartsys')
        with self.assertRaises(ValueError):service.create_effect('bad','authoredpartsys',dict(fake_parameter=1))
        def forbidden_factory(*args):
            raise AssertionError('A stationary fixture must never substitute for a launch adapter')
        for kind in ('projectile','point_to_point'):
            service.register_profile(kind,Profile(forbidden_factory,'Acceptance-contract test only',(),kind=kind))
            descriptor=service.capabilities()['profiles'][kind]
            self.assertEqual(descriptor['motion_required'],kind=='projectile')
            self.assertEqual(descriptor['distinct_primary_endpoints'],kind=='point_to_point')
            with self.assertRaises(UnsupportedControl):service.create_effect('forbidden-'+kind,kind)
            with self.assertRaises(UnsupportedControl):service.spawn_effect('forbidden-'+kind,kind)

    def test_source_entity_owner_binding_and_targeted_missile_rejection(self):
        service=Controller(BASELINE)
        service.create_entity('source',[10,20,30],64,stats=dict(hp=50),attachments=dict(hand='metadata-only'))
        result=service.spawn_effect('particles','authoredpartsys',source_id='source',launch_params=dict(render=True))
        self.assertEqual(result['state']['fixture_parameters']['owner'],[10,20,30])
        service.checkpoint()
        service.set_entity_position('source',[-3,40,7],128)
        state=service.inspect_state('particles')['particles']
        self.assertEqual(state['fixture_parameters']['owner'],[-3,40,7])
        self.assertEqual(state['fixture_parameters']['face'],128)
        adapter=service.effects['particles']['adapter']
        self.assertEqual(adapter.probe.vm.u32(adapter.probe.owner+16),0xfffffffd)
        self.assertEqual(adapter.probe.vm.uc.mem_read(adapter.probe.owner+54,1),bytes([128]))
        self.assertEqual(service.advance_ticks(2,'particles')['particles']['original_function_calls']['0x403760'],2)
        with self.assertRaises(UnsupportedControl):service.spawn_effect('missile','authoredpartsys',
            source_id='source',destination_id='target')
        with self.assertRaises(ValueError):service.remove_entity('source')
        service.reset()
        self.assertEqual(service.inspect_entities('source')['position'],[10,20,30])
        self.assertEqual(service.inspect_state('particles')['particles']['fixture_parameters']['owner'],[10,20,30])
        EVIDENCE['entity_owner_binding']=dict(owner_offsets=[16,20,24],facing_offset=54,
            position_before_reset=[-3,40,7],position_after_reset=[10,20,30],
            stats_and_attachments_applied=False,targeted_missiles_supported=False)

    def test_failed_scenario_rolls_back_lifecycle_and_ticks(self):
        service=Controller(BASELINE);service.create_effect('fixture','authoredpartsys',dict(seed=123))
        service.checkpoint();initial=service.inspect_state()
        with self.assertRaises(UnsupportedControl):service.execute(dict(operations=[
            dict(op='advance_ticks',id='fixture',ticks=3),
            dict(op='create_effect',id='temporary',type='authoredpartsys'),
            dict(op='invalid')]))
        self.assertEqual(service.inspect_state(),initial)
        scenario=dict(operations=[dict(op='advance_ticks',id='fixture',ticks=4),
                                  dict(op='capture',id='fixture',kind='state')])
        first=service.execute(scenario);second=service.execute(scenario)
        self.assertEqual(first['results'],second['results'])

    def test_jsonl_direct_actions_keep_state_and_pin_build(self):
        build=dict(build_id='named-baseline-control',executable_sha256=digest(BASELINE),
            baseline_sha256=digest(BASELINE),patch_manifest=dict(kind='unchanged-baseline'))
        build_file=self.path/'build.json';build_file.write_text(json.dumps(build))
        requests=[dict(id='status',command='status'),
            dict(id='create',command='action',action=dict(op='create_effect',id='p',type='authoredpartsys')),
            dict(id='save',command='checkpoint'),
            dict(id='step',command='action',action=dict(op='advance_ticks',id='p',ticks=2)),
            dict(id='reset',command='restore'),
            dict(id='inspect',command='action',action=dict(op='inspect_state',id='p'))]
        result=subprocess.run([sys.executable,str(Path(__file__).with_name('controls.py')),str(BASELINE),
            '--build',str(build_file)],input='\n'.join(json.dumps(r) for r in requests)+'\n',
            capture_output=True,text=True,check=True)
        responses=[json.loads(line) for line in result.stdout.splitlines()]
        self.assertEqual([r['id'] for r in responses],[r['id'] for r in requests])
        self.assertEqual(responses[0]['capabilities']['build']['build_id'],build['build_id'])
        self.assertEqual(responses[3]['result']['p']['ticks'],2)
        self.assertEqual(responses[-1]['result']['p']['ticks'],0)
        with self.assertRaises(ValueError):Controller(BASELINE,dict(executable_sha256='0'*64))

    def test_two_named_parallel_control_sessions(self):
        setup=[dict(op='create_effect',id='fixture',type='authoredpartsys',params=dict(shape=1,seed=42))]
        build=dict(build_id='baseline-direct-controls',executable=str(BASELINE),
            executable_sha256=digest(BASELINE),baseline_sha256=digest(BASELINE),
            patch_manifest=dict(kind='unchanged-baseline'))
        sessions=[dict(name=name,build=build,setup=setup,pass_build_record=True,
            runner=str(Path(__file__).with_name('controls.py'))) for name in ('agent-a','agent-b')]
        scenario=dict(executable_sha256=digest(BASELINE),operations=[dict(op='advance_ticks',id='fixture',ticks=3),
            dict(op='capture',id='fixture',kind='state',output='state.json')])
        with Pool(sessions=sessions,artifact_root=self.path) as pool:
            results=pool.execute([dict(session=name,scenario=scenario) for name in ('agent-a','agent-b')])
            self.assertEqual([r['status'] for r in results],['pass','pass'])
            self.assertEqual(results[0]['results'],results[1]['results'])
            self.assertNotEqual(results[0]['worker_pid'],results[1]['worker_pid'])
            self.assertEqual(results[0]['results'][-1]['build']['build_id'],build['build_id'])
            self.assertTrue(all((Path(r['artifact_dir'])/'state.json').is_file() for r in results))
            EVIDENCE['parallel_controls']=dict(sessions=[r['session'] for r in results],
                worker_pids=[r['worker_pid'] for r in results],
                capture_sha256=results[0]['results'][-1]['sha256'],independent=True,editor_input=False)

    def test_multiple_effects_scripted_actor_inputs_checkpoint_and_remove(self):
        service=Controller(BASELINE)
        service.create_entity('source',[10,20,30],32)
        service.create_entity('target',[100,200,30],64)
        service.spawn_effect('a','authoredpartsys','source',launch_params=dict(shape=1,seed=7))
        service.spawn_effect('b','authoredpartsys','source',launch_params=dict(shape=2,seed=9))
        service.set_entity_motion('source',[dict(tick=0,position=[11,20,30]),
            dict(tick=2,position=[15,20,30],facing=128)])
        service.set_entity_motion('target',[dict(tick=1,position=[100,210,30])])
        service.checkpoint()
        with self.assertRaises(UnsupportedControl):service.advance_ticks(3,'a')
        self.assertEqual(service.inspect_actor_inputs()['timeline_tick'],0)
        first=service.advance_ticks(4)
        inputs=service.inspect_actor_inputs()
        self.assertEqual([row['tick'] for row in inputs['inputs']],[0,1,2])
        self.assertEqual(first['a']['ticks'],4);self.assertEqual(first['b']['ticks'],4)
        self.assertEqual(first['a']['fixture_parameters']['owner'],[15,20,30])
        self.assertEqual(first['b']['fixture_parameters']['owner'],[15,20,30])
        self.assertEqual(first['a']['original_function_calls']['0x403760'],4)
        self.assertEqual(first['b']['original_function_calls']['0x403760'],4)
        service.remove_effect('a');service.create_effect('new','authoredpartsys',dict(seed=99))
        service.reset()
        self.assertEqual(sorted(service.effects),['a','b'])
        self.assertEqual(service.inspect_entities('source')['position'],[10,20,30])
        self.assertEqual(service.inspect_entities('target')['position'],[100,200,30])
        second=service.advance_ticks(4)
        self.assertEqual(first,second)
        self.assertEqual(inputs,service.inspect_actor_inputs())
        EVIDENCE['scripted_actor_inputs']=dict(effect_ids=['a','b'],ticks=4,
            recorded_inputs=inputs['inputs'],repeat_equal=True,
            effect_motion='Original Pulse; scripted positions apply only to SOURCE/TARGET fixture actors')

    def test_original_missile_launch_motion_impact_replay_and_actor_setters(self):
        service=Controller(BASELINE)
        service.create_entity('source',[0,0,128])
        service.create_entity('target',[320,0,128],stats=dict(hp=100,enemy=True))
        initial=service.spawn_effect('missile','shared_missile_base','source','target',dict(speed=8))
        self.assertFalse(initial['state']['motion_observed'])
        with self.assertRaises(AssertionError):service.validate_execution_contract('missile')
        service.checkpoint()
        first=service.advance_ticks(40,'missile')['missile']
        self.assertTrue(first['motion_observed'])
        self.assertGreater(first['original_function_calls']['0x470920'],0)
        self.assertEqual(first['position'],[288,0,110])
        self.assertEqual([(event['tick'],event['event']) for event in first['events']],
                         [(1,'launch'),(37,'impact'),(38,'kill_requested')])
        self.assertEqual(service.validate_execution_contract('missile')['status'],'pass')
        self.assertFalse(first['fireball_leaf_acceptance'])
        service.reset();self.assertEqual(service.advance_ticks(40,'missile')['missile'],first)
        service.reset()
        before=service.inspect_state('missile')['missile']['position']
        service.set_entity_position('source',[10,20,128])
        service.set_entity_position('target',[600,80,128])
        after=service.inspect_state('missile')['missile']
        self.assertEqual(after['position'],before) # Actor writes never teleport the missile.
        self.assertEqual(after['source_fixture']['position'],[10,20,128])
        self.assertEqual(after['destination_fixture']['position'],[600,80,128])
        service.reset()
        service.set_entity_motion('target',[dict(tick=10,position=[600,80,128])])
        service.checkpoint()
        scripted=service.advance_ticks(64,'missile')['missile']
        self.assertTrue(scripted['motion_observed'])
        self.assertEqual(service.inspect_actor_inputs()['inputs'][0]['tick'],10)
        service.reset();self.assertEqual(service.advance_ticks(64,'missile')['missile'],scripted)
        EVIDENCE['shared_missile_base']=dict(source=[0,0,128],destination=[320,0,128],
            impact_position=first['position'],events=first['events'],
            original_function_calls=first['original_function_calls'],motion_observed=True,
            repeat_equal=True,source_target_setters_teleport_missile=False,
            full_fireball_leaf=False,map_reaper=False,pixel_comparison=False)

    def test_missile_requires_distinct_supplied_primary_endpoints(self):
        service=Controller(BASELINE)
        service.create_entity('source',[0,0,128]);service.create_entity('same',[0,0,128])
        with self.assertRaises(ValueError):service.spawn_effect('missing','shared_missile_base','source')
        with self.assertRaises(ValueError):service.spawn_effect('equal','shared_missile_base','source','same')
        with self.assertRaises(ValueError):service.spawn_effect('unlabeled','shared_missile_base','source','same',
            case_kind='degenerate')
        with self.assertRaises(UnsupportedControl):service.create_effect('stationary','shared_missile_base')
        self.assertEqual(service.effects,{})

    def test_parallel_original_missile_fixtures_keep_distinct_actor_state(self):
        build=dict(build_id='retail-original-shared-missile',executable=str(BASELINE),
            executable_sha256=digest(BASELINE),baseline_sha256=digest(BASELINE),patch_manifest=dict(kind='unchanged-baseline'))
        sessions=[]
        for name,hp in [('impact-agent',100),('range-agent',0)]:
            setup=[dict(op='create_entity',id='source',position=[0,0,128]),
                dict(op='create_entity',id='target',position=[320,0,128],stats=dict(hp=hp,enemy=True)),
                dict(op='spawn_effect',id='missile',type='shared_missile_base',source_id='source',destination_id='target')]
            sessions.append(dict(name=name,build=build,runner=str(Path(__file__).with_name('controls.py')),
                pass_build_record=True,setup=setup))
        scenario=dict(operations=[dict(op='advance_ticks',id='missile',ticks=40),
            dict(op='validate_execution_contract',id='missile'),dict(op='capture',id='missile',output='missile-state.json')])
        jobs=[dict(session=session['name'],scenario=scenario) for session in sessions]
        with Pool(sessions=sessions,artifact_root=self.path) as pool:
            pids=[worker.process.pid for worker in pool.workers]
            first=pool.execute(jobs);second=pool.execute(jobs)
            self.assertEqual([result['status'] for result in first],['pass','pass'])
            self.assertEqual([result['results'] for result in first],[result['results'] for result in second])
            self.assertEqual([worker.process.pid for worker in pool.workers],pids)
            self.assertNotEqual(first[0]['results'][-1]['sha256'],first[1]['results'][-1]['sha256'])
            self.assertEqual(first[0]['results'][0]['missile']['state'],2)
            self.assertEqual(first[1]['results'][0]['missile']['state'],1)
            self.assertTrue(all(result['results'][0]['missile']['motion_observed'] for result in first))
            EVIDENCE['parallel_shared_missile']=dict(sessions=[session['name'] for session in sessions],
                worker_pids=pids,warm_replay_equal=True,impact_state=2,dead_target_flight_state=1,
                distinct_capture_sha256=[result['results'][-1]['sha256'] for result in first],
                original_motion_executed=True,full_fireball_leaf=False)

    def test_flame_original_animation_pixels_checkpoint_and_named_variant(self):
        service=Controller(BASELINE)
        service.create_effect('torch','flame')
        service.checkpoint()
        initial=service.capture('torch','pixels',self.path/'initial.png',self.path/'initial.depth-u16')
        self.assertGreater(initial['nonzero_pixels'],0)
        self.assertEqual((initial['width'],initial['height']),(96,96))
        self.assertTrue((self.path/'initial.png').read_bytes().startswith(b'\x89PNG'))
        with self.assertRaises(ValueError):service.capture('torch','pixels',self.path/'same',self.path/'same')
        service.advance_ticks(1,'torch')
        one=service.capture('torch','pixels')
        self.assertEqual(one['data']['state']['frame'],1)
        # Native UV selection holds an atlas cell for multiple Animate ticks.
        service.advance_ticks(2,'torch');three=service.capture('torch','pixels')
        self.assertEqual(three['data']['state']['frame'],3)
        self.assertNotEqual(initial['sha256'],three['sha256'])
        service.advance_ticks(15,'torch')
        self.assertEqual(service.inspect_state('torch')['torch']['frame'],0)
        service.reset()
        restored=service.capture('torch','pixels')
        self.assertEqual(initial['sha256'],restored['sha256'])
        self.assertEqual(initial['depth_sha256'],restored['depth_sha256'])
        service.remove_effect('torch');service.reset()
        self.assertEqual(service.inspect_state('torch')['torch']['frame'],0)
        # A distinct named build changes only the PE checksum field. The full
        # fixed-layout/ancestry/declared-span contract, not a hash bypass, permits it.
        workspace=Workspace.create(self.path/'private',BASELINE)
        data=bytearray(BASELINE.read_bytes());pe=int.from_bytes(data[60:64],'little')
        offset=pe+24+64;data[offset]^=1
        candidate=self.path/'checksum-variant.exe';candidate.write_bytes(data)
        build=workspace.import_variant('torch-checksum',candidate,
            dict(kind='test-only-checksum',allowed_modified_spans=[[offset,offset+1]]))
        private=Controller(build['executable'],build)
        private.create_effect('torch','flame');capture=private.capture('torch','pixels')
        self.assertEqual(capture['sha256'],initial['sha256'])
        self.assertEqual(capture['build']['executable_sha256'],build['executable_sha256'])
        self.assertTrue(capture['build']['experimental'])
        self.assertEqual(digest(BASELINE),service.executable_sha256)
        unchanged=workspace.manifest['builds'][0]
        private_missile=Controller(unchanged['executable'],unchanged)
        private_missile.create_entity('source',[0,0,128])
        private_missile.create_entity('target',[320,0,128],stats=dict(hp=100,enemy=True))
        private_missile.spawn_effect('m','shared_missile_base','source','target')
        private_missile.advance_ticks(40,'m')
        self.assertEqual(private_missile.inspect_state('m')['m']['position'],[288,0,110])
        self.assertEqual(private_missile.capture('m')['build']['build_id'],unchanged['build_id'])
        EVIDENCE['flame_pixels']=dict(original_animate='0x4e4ef0',original_render='0x4e4f20',
            original_raster='0x56d960',initial_rgb565_sha256=initial['sha256'],
            tick1_rgb565_sha256=one['sha256'],tick3_rgb565_sha256=three['sha256'],nonzero_pixels=initial['nonzero_pixels'],
            exact_restore=True,frame_wrap=18,named_private_build=build['build_id'],
            variant_sha256=build['executable_sha256'],named_variant_pixels_unchanged=True,
            modern_renderer_compared=False)

    def test_ripple_and_fizzle_original_ticks_pixels_and_reset(self):
        cases=[]
        for effect_type,params,ticks in [('ripple_no_splash',dict(length=4),2),('fizzle',{},4)]:
            service=Controller(BASELINE);service.create_effect('fx',effect_type,params);service.checkpoint()
            initial=service.inspect_state('fx')['fx']
            service.advance_ticks(ticks,'fx')
            capture=service.capture('fx','pixels')
            self.assertGreater(capture['nonzero_pixels'],0)
            state=capture['data']['state']
            self.assertEqual(state['ticks'],ticks)
            self.assertNotEqual(initial,state)
            service.reset();service.advance_ticks(ticks,'fx')
            repeated=service.capture('fx','pixels')
            self.assertEqual(capture['sha256'],repeated['sha256'])
            self.assertEqual(capture['depth_sha256'],repeated['depth_sha256'])
            self.assertEqual(capture['data'],repeated['data'])
            if effect_type=='ripple_no_splash':
                service.advance_ticks(8,'fx')
                self.assertFalse(service.inspect_state('fx')['fx']['alive'])
                self.assertEqual(service.capture('fx','pixels')['nonzero_pixels'],0)
            else:
                self.assertGreater(state['random_count'],0)
                self.assertEqual(state['owner_world_policy'],'Local particle Z, native flicker=1/abs_pos=0; owner translation unsupported')
                with self.assertRaises(UnsupportedControl):service.set_position('fx',[1,2,3])
            cases.append(dict(profile=effect_type,ticks=ticks,sha256=capture['sha256'],
                nonzero_pixels=capture['nonzero_pixels'],exact_restore=True,original_animate=state['original_animate']))
        EVIDENCE['ripple_fizzle_pixels']=cases

    def test_parallel_pixel_profiles_write_independent_png_and_depth_artifacts(self):
        build=dict(name='retail-original',build_id='retail-original-pixel-controls',executable=str(BASELINE),
            executable_sha256=digest(BASELINE),baseline_sha256=digest(BASELINE),patch_manifest=dict(kind='unchanged-baseline'))
        sessions=[]
        for name,effect_type,params in [('flame-agent','flame',{}),('ripple-agent','ripple_no_splash',dict(length=4))]:
            sessions.append(dict(name=name,build=build,runner=str(Path(__file__).with_name('controls.py')),
                pass_build_record=True,setup=[dict(op='create_effect',id='fx',type=effect_type,params=params)]))
        scenario=dict(operations=[dict(op='advance_ticks',id='fx',ticks=3),
            dict(op='capture',id='fx',kind='pixels',output='frame.png',depth_output='frame.depth-u16')])
        jobs=[dict(session=session['name'],scenario=scenario) for session in sessions]
        with Pool(sessions=sessions,artifact_root=self.path) as pool:
            first=pool.execute(jobs);second=pool.execute(jobs)
            self.assertEqual([r['status'] for r in first],['pass','pass'])
            for a,b in zip(first,second):
                captured=a['results'][-1]
                self.assertGreater(captured['nonzero_pixels'],0)
                self.assertEqual(captured['sha256'],b['results'][-1]['sha256'])
                self.assertEqual(captured['depth_sha256'],b['results'][-1]['depth_sha256'])
                self.assertTrue((Path(a['artifact_dir'])/'frame.png').read_bytes().startswith(b'\x89PNG'))
                self.assertEqual((Path(a['artifact_dir'])/'frame.depth-u16').stat().st_size,captured['bytes'])
                self.assertEqual(a['worker_starts'],1)
            self.assertNotEqual(first[0]['results'][-1]['sha256'],first[1]['results'][-1]['sha256'])
            EVIDENCE['parallel_pixels']=dict(sessions=[r['session'] for r in first],
                worker_pids=[r['worker_pid'] for r in first],
                rgb565_sha256=[r['results'][-1]['sha256'] for r in first],
                depth_sha256=[r['results'][-1]['depth_sha256'] for r in first],
                warm_replay_equal=True,editor_input=False,modern_renderer_compared=False)

    def test_mistfog_original_loop_respawns_rng_pixels_and_midcycle_reset(self):
        service=Controller(BASELINE);service.create_effect('fog','mistfog')
        initial=service.inspect_state('fog')['fog']
        self.assertEqual(initial['puff_count'],25)
        self.assertEqual(initial['original_function_calls']['0x4f2840'],1)
        service.checkpoint();service.advance_ticks(120,'fog')
        midpoint=service.inspect_state('fog')['fog']
        service.checkpoint();service.advance_ticks(120,'fog')
        state=service.inspect_state('fog')['fog']
        self.assertEqual(state['ticks'],240)
        self.assertGreater(state['random_count'],initial['random_count'])
        self.assertEqual(state['original_function_calls']['0x4f2950'],240)
        capture=service.capture('fog','pixels',self.path/'fog.png')
        self.assertGreater(capture['nonzero_pixels'],0)
        self.assertEqual((capture['width'],capture['height']),(384,384))
        self.assertEqual(len(capture['data']['packet']['draws']),25)
        service.reset();self.assertEqual(service.inspect_state('fog')['fog'],midpoint)
        service.advance_ticks(120,'fog');again=service.capture('fog','pixels')
        self.assertEqual(capture['sha256'],again['sha256'])
        self.assertEqual(capture['depth_sha256'],again['depth_sha256'])
        self.assertEqual(capture['data'],again['data'])
        for method,value in [('set_position',[1,2,3]),('set_camera',dict(position=[1,0,0])),
                             ('set_light',dict(rgba5=[16,16,16,31]))]:
            with self.assertRaises(UnsupportedControl):getattr(service,method)('fog',value)
        service.remove_effect('fog');service.reset()
        self.assertEqual(service.inspect_state('fog')['fog'],midpoint)
        EVIDENCE['mistfog_pixels']=dict(puffs=25,ticks=240,midcycle_checkpoint_tick=120,
            native_random_count=state['random_count'],native_random_count_at_init=initial['random_count'],
            rgb565_sha256=capture['sha256'],depth_sha256=capture['depth_sha256'],
            nonzero_pixels=capture['nonzero_pixels'],exact_midcycle_replay=True,
            original_initialize='0x4f2840',original_animate='0x4f2950',original_render='0x4f2ae0',
            lighting='declared full white',owner_world='identity',modern_renderer_compared=False)

    def test_static_sign_and_globe_profiles_native_pose_samples_and_owner_translation(self):
        checks=[]
        for selector in [dict(profile='CampSign'),dict(type_id='0xad92bd24')]:
            service=Controller(BASELINE);service.create_effect('mesh','static_mesh',selector);service.checkpoint()
            captures=[]
            for frame in (0,50,99):
                service.set_sample_frame('mesh',frame)
                captures.append(service.capture('mesh','pixels'))
            self.assertTrue(all(capture['sha256']==captures[0]['sha256'] for capture in captures))
            self.assertTrue(all(capture['depth_sha256']==captures[0]['depth_sha256'] for capture in captures))
            self.assertGreater(captures[0]['nonzero_pixels'],0)
            service.set_position('mesh',[16,-8,4]);moved=service.capture('mesh','pixels')
            self.assertNotEqual(moved['sha256'],captures[0]['sha256'])
            service.advance_ticks(25,'mesh')
            self.assertEqual(service.capture('mesh','pixels')['sha256'],moved['sha256'])
            service.reset()
            self.assertEqual(service.inspect_state('mesh')['mesh']['frame'],0)
            self.assertEqual(service.capture('mesh','pixels')['sha256'],captures[0]['sha256'])
            service.remove_effect('mesh');service.reset()
            self.assertEqual(service.inspect_state('mesh')['mesh']['frame'],0)
            with self.assertRaises(ValueError):service.set_sample_frame('mesh',1)
            with self.assertRaises(ValueError):service.set_position('mesh',[10000,0,0])
            checks.append(dict(profile=captures[0]['data']['state']['profile'],
                type_id=captures[0]['data']['state']['type_id'],frames=[0,50,99],static_pixel_sha256=captures[0]['sha256'],
                moved_pixel_sha256=moved['sha256'],nonzero_pixels=captures[0]['nonzero_pixels'],exact_reset=True))
        service=Controller(BASELINE)
        self.assertEqual(len(service.capabilities()['profiles']['static_mesh']['catalog']),13)
        for params in [dict(profile='Unknown'),dict(profile='CampSign',type_id='0xad92bd24')]:
            with self.assertRaises(ValueError):service.create_effect('bad','static_mesh',params)
        EVIDENCE['static_mesh_pixels']=dict(cases=checks,catalog_entries=13,
            original_matrix='0x40a420',owner_translation=[16,-8,4],
            scope='Constant-track asset frontend only; no full Globe/sign effect, map lighting/culling or modern GPU acceptance')

    def test_four_fountain_leaves_native_lifecycle_rng_and_exact_pixel_reset(self):
        cases=[]
        for selector,color in [(dict(profile='CyanFont'),0),(dict(color='red'),1),
                               (dict(type_id='0x446b3627'),2),(dict(color=3),3)]:
            service=Controller(BASELINE);service.create_effect('bubbles','fountain',selector);service.checkpoint()
            initial=service.inspect_state('bubbles')['bubbles']
            self.assertEqual(initial['color'],color)
            self.assertEqual(len(initial['particles']),10)
            self.assertEqual(initial['original_function_calls']['0x4e4080'],1)
            service.advance_ticks(48,'bubbles');midpoint=service.inspect_state('bubbles')['bubbles']
            service.checkpoint();service.advance_ticks(48,'bubbles')
            state=service.inspect_state('bubbles')['bubbles']
            self.assertEqual(state['ticks'],96)
            self.assertGreater(state['random_count'],initial['random_count'])
            self.assertEqual(state['original_function_calls'][state['original_animate']],96)
            self.assertTrue(all(count>0 for count in state['lifecycle_observations'].values()))
            capture=service.capture('bubbles','pixels')
            self.assertGreater(capture['nonzero_pixels'],0)
            self.assertEqual((capture['width'],capture['height']),(192,192))
            service.reset();self.assertEqual(service.inspect_state('bubbles')['bubbles'],midpoint)
            service.advance_ticks(48,'bubbles');again=service.capture('bubbles','pixels')
            self.assertEqual(capture['sha256'],again['sha256'])
            self.assertEqual(capture['depth_sha256'],again['depth_sha256'])
            self.assertEqual(capture['data'],again['data'])
            service.remove_effect('bubbles');service.reset()
            self.assertEqual(service.inspect_state('bubbles')['bubbles'],midpoint)
            with self.assertRaises(UnsupportedControl):service.set_position('bubbles',[0,0,0])
            cases.append(dict(name=state['profile'],type_id=state['type_id'],color=color,ticks=96,
                random_count=state['random_count'],sha256=capture['sha256'],depth_sha256=capture['depth_sha256'],
                observed_native_lifecycle=state['lifecycle_observations'],
                nonzero_pixels=capture['nonzero_pixels'],exact_midcycle_reset=True))
        self.assertEqual(len({case['sha256'] for case in cases}),4)
        service=Controller(BASELINE)
        catalog=service.capabilities()['verified_retail_types']
        self.assertEqual(len(catalog),37);self.assertEqual(len({entry['type_id'] for entry in catalog}),37)
        for params in [{},dict(color='purple'),dict(profile='RedFont',type_id='0x557c4738')]:
            with self.assertRaises(ValueError):service.create_effect('bad','fountain',params)
        EVIDENCE['fountain_pixels']=dict(cases=cases,verified_type_catalog_entries=37,
            original_initialize='0x4e4080',original_render='0x4e41f0',
            scope='Native bounded delay/rise/shrink/respawn/color leaves; identity owner, no natural map lighting/ownership or modern GPU acceptance')

    def test_colored_ribbon_composites_all_parts_and_restores_texture_bindings(self):
        from pixel_controls import ribbon_profile_catalog
        cases=[]
        for entry in ribbon_profile_catalog():
            service=Controller(BASELINE)
            service.create_effect('ribbon','colored_ribbon',dict(type_id=entry['id']))
            service.checkpoint();initial=service.capture('ribbon','pixels')
            self.assertGreater(initial['nonzero_pixels'],0)
            state=initial['data']['state']
            self.assertEqual(state['vertices'],186);self.assertEqual(state['faces'],204)
            self.assertEqual([part['keys'] for part in state['parts']],[9,6])
            self.assertEqual([len(part['positions']) for part in initial['data']['packet']['parts']],[12,174])
            self.assertTrue(initial['data']['packet']['composited_in_asset_order'])
            service.advance_ticks(25,'ribbon')
            self.assertEqual(service.capture('ribbon','pixels')['sha256'],initial['sha256'])
            # Checkpoint after texture1/metadata1 are live, then change owner and
            # restore both host provider bindings alongside the guest pages.
            service.checkpoint();saved=service.inspect_state('ribbon')['ribbon']
            service.set_position('ribbon',[16,-8,4]);moved=service.capture('ribbon','pixels')
            self.assertNotEqual(initial['sha256'],moved['sha256'])
            service.reset();self.assertEqual(service.inspect_state('ribbon')['ribbon'],saved)
            self.assertEqual(service.capture('ribbon','pixels')['sha256'],initial['sha256'])
            service.remove_effect('ribbon');service.reset()
            self.assertEqual(service.capture('ribbon','pixels')['sha256'],initial['sha256'])
            with self.assertRaises(ValueError):service.set_sample_frame('ribbon',50)
            cases.append(dict(name=entry['name'],type_id=entry['id'],sha256=initial['sha256'],
                depth_sha256=initial['depth_sha256'],moved_sha256=moved['sha256'],host_texture_binding_reset=True))
        self.assertEqual(len({case['sha256'] for case in cases}),7)
        service=Controller(BASELINE)
        with self.assertRaises(ValueError):service.create_effect('base','colored_ribbon',dict(type_id='0x562c7439'))
        EVIDENCE['colored_ribbon_pixels']=dict(cases=cases,distinct_colors=7,
            parts=2,base_revive_ribbon_included=False,natural_map_or_gpu_acceptance=False)

    def test_fireball_head_pixels_move_with_original_simulation_in_same_vm(self):
        service=Controller(BASELINE)
        service.create_entity('source',[0,0,128]);service.create_entity('target',[1000,0,128],stats=dict(hp=0,enemy=True))
        service.spawn_effect('head','fireball_head','source','target',
            dict(render_inputs=dict(frame=3,rotation=12,scale=.3,glow=1.5,face=32)))
        service.checkpoint();service.advance_ticks(2,'head')
        first=service.capture('head','pixels')
        self.assertGreater(first['nonzero_pixels'],0)
        self.assertTrue(first['data']['same_vm_motion_and_raster'])
        self.assertTrue(first['data']['state']['motion_observed'])
        service.advance_ticks(4,'head');moved=service.capture('head','pixels')
        self.assertNotEqual(first['sha256'],moved['sha256'])
        self.assertNotEqual(first['data']['state']['position'],moved['data']['state']['position'])
        self.assertEqual(service.validate_execution_contract('head')['status'],'pass')
        service.reset();service.advance_ticks(6,'head');again=service.capture('head','pixels')
        self.assertEqual(moved['sha256'],again['sha256'])
        self.assertEqual(moved['depth_sha256'],again['depth_sha256'])
        self.assertEqual(moved['data'],again['data'])
        position=service.inspect_state('head')['head']['position']
        service.set_render_inputs('head',dict(frame=6,rotation=24))
        self.assertEqual(service.inspect_state('head')['head']['position'],position)
        self.assertFalse(again['data']['state']['fireball_leaf_acceptance'])
        EVIDENCE['fireball_head_pixels']=dict(first_position=first['data']['state']['position'],
            moved_position=moved['data']['state']['position'],first_sha256=first['sha256'],moved_sha256=moved['sha256'],
            same_vm=True,exact_replay=True,original_move=True,full_animator_or_damage=False,
            render_pose_policy='Explicit selected inputs; no waveform transcription')

    def test_streamer_native_one_shot_frees_arrays_and_midcycle_reset_recovers(self):
        service=Controller(BASELINE);service.create_effect('stream','streamer');service.checkpoint()
        service.advance_ticks(24,'stream');midpoint=service.inspect_state('stream')['stream']
        visible=service.capture('stream','pixels')
        self.assertGreater(visible['nonzero_pixels'],0)
        self.assertEqual(midpoint['frameon'],24);self.assertEqual(len(midpoint['particles']),200)
        midpoint=service.inspect_state('stream')['stream'] # Include the render observer call in the saved checkpoint.
        service.checkpoint();service.advance_ticks(80,'stream')
        dead=service.inspect_state('stream')['stream']
        self.assertEqual(dead['ticks'],104);self.assertEqual(dead['frameon'],101)
        self.assertFalse(dead['alive']);self.assertEqual(dead['particles'],[])
        self.assertEqual(service.capture('stream','pixels')['nonzero_pixels'],0)
        service.reset();self.assertEqual(service.inspect_state('stream')['stream'],midpoint)
        replay=service.capture('stream','pixels')
        self.assertEqual(visible['sha256'],replay['sha256'])
        self.assertEqual(visible['depth_sha256'],replay['depth_sha256'])
        service.remove_effect('stream');service.reset()
        self.assertEqual(service.capture('stream','pixels')['sha256'],visible['sha256'])
        EVIDENCE['streamer_pixels']=dict(type_id='0x482dfe82',native_kill_tick=101,arrays_freed=True,
            midcycle_checkpoint_tick=24,rgb565_sha256=visible['sha256'],exact_restore_after_free=True,
            culling_policy='Existing explicit frontend boundary',natural_map_device_gpu_acceptance=False)

    def test_three_exact_flame_variants_select_their_own_shipped_assets(self):
        captures=[]
        for variant,type_id in [('base','0x50ba373b'),('blue','0x50ba373c'),('green','0x50ba373d')]:
            service=Controller(BASELINE);service.create_effect('torch','flame',dict(type_id=type_id))
            service.checkpoint();service.advance_ticks(3,'torch');capture=service.capture('torch','pixels')
            state=capture['data']['state']
            self.assertEqual(state['variant'],variant);self.assertEqual(state['type_id'],type_id)
            self.assertGreater(capture['nonzero_pixels'],0)
            service.reset();service.advance_ticks(3,'torch')
            self.assertEqual(service.capture('torch','pixels')['sha256'],capture['sha256'])
            captures.append(dict(variant=variant,type_id=type_id,asset_sha256=state['asset']['sha256'],sha256=capture['sha256']))
        self.assertEqual(len({capture['sha256'] for capture in captures}),3)
        service=Controller(BASELINE)
        for params in [dict(variant='unknown'),dict(type_id='0xffffffff'),dict(variant='blue',type_id='0x50ba373d')]:
            with self.assertRaises(ValueError):service.create_effect('bad','flame',params)
        EVIDENCE['flame_variants']=dict(cases=captures,selector_fallback=False,exact_restore=True)

    def test_mist_fifty_native_drops_respawn_rng_and_midcycle_pixels_repeat(self):
        service=Controller(BASELINE);service.create_effect('drops','mist');service.checkpoint()
        service.advance_ticks(48,'drops');midpoint=service.inspect_state('drops')['drops'];service.checkpoint()
        service.advance_ticks(48,'drops');state=service.inspect_state('drops')['drops']
        self.assertEqual(len(state['particles']),50);self.assertEqual(state['ticks'],96)
        self.assertEqual(state['random_count'],1380)
        self.assertEqual(state['lifecycle_observations'],dict(deaths=227,respawns=226))
        capture=service.capture('drops','pixels')
        self.assertGreater(capture['nonzero_pixels'],0)
        service.reset();self.assertEqual(service.inspect_state('drops')['drops'],midpoint)
        service.advance_ticks(48,'drops');again=service.capture('drops','pixels')
        self.assertEqual(capture['sha256'],again['sha256']);self.assertEqual(capture['data'],again['data'])
        service.remove_effect('drops');service.reset()
        self.assertEqual(service.inspect_state('drops')['drops'],midpoint)
        EVIDENCE['mist_pixels']=dict(type_id='0x2093487a',ticks=96,random_inputs=1380,
            native_lifecycle=state['lifecycle_observations'],sha256=capture['sha256'],exact_midcycle_replay=True,
            natural_map_or_gpu_acceptance=False)

    def test_pixie_empty_character_context_native_queries_rng_and_reset(self):
        service=Controller(BASELINE);service.create_effect('swarm','pixie');service.checkpoint()
        service.advance_ticks(48,'swarm');midpoint=service.inspect_state('swarm')['swarm'];service.checkpoint()
        service.advance_ticks(48,'swarm');state=service.inspect_state('swarm')['swarm']
        self.assertEqual(len(state['particles']),25);self.assertEqual(state['random_count'],5000)
        self.assertEqual(state['map_queries'],192);self.assertEqual(state['owner_position'],[0,0,0])
        self.assertEqual(state['original_function_calls']['0x4f39a0'],96)
        capture=service.capture('swarm','pixels')
        self.assertGreater(capture['nonzero_pixels'],0)
        self.assertEqual(state['nearby_character_context'],'explicitly empty')
        service.reset();self.assertEqual(service.inspect_state('swarm')['swarm'],midpoint)
        service.advance_ticks(48,'swarm');again=service.capture('swarm','pixels')
        self.assertEqual(capture['sha256'],again['sha256']);self.assertEqual(capture['data'],again['data'])
        service.remove_effect('swarm');service.reset()
        self.assertEqual(service.inspect_state('swarm')['swarm'],midpoint)
        with self.assertRaises(ValueError):service.create_effect('targeted','pixie',dict(characters=['enemy']))
        with self.assertRaises(UnsupportedControl):service.set_position('swarm',[0,0,0])
        EVIDENCE['pixie_pixels']=dict(type_id='0x89abcde1',ticks=96,native_random_inputs=5000,map_queries=192,
            nearby_characters='empty',sha256=capture['sha256'],exact_midcycle_replay=True,natural_map_or_character_acceptance=False)

    def test_symglow_render_mutates_uv_once_per_timer_and_checkpoint_preserves_cache(self):
        service=Controller(BASELINE);service.create_effect('glow','symglow')
        service.advance_ticks(3,'glow') # Setup has zero UV step; exercise an actual nonzero native render phase.
        before=service.inspect_state('glow')['glow'];service.checkpoint()
        first=service.capture('glow','pixels');after=service.inspect_state('glow')['glow']
        self.assertNotEqual(before['uvs'],after['uvs'])
        self.assertEqual(after['original_function_calls']['0x4e5260'],1)
        repeated=service.capture('glow','pixels')
        self.assertEqual(first,repeated)
        self.assertEqual(service.inspect_state('glow')['glow'],after)
        service.checkpoint();service.advance_ticks(3,'glow')
        read_only=service.inspect_state('glow')['glow']
        self.assertEqual(read_only['uvs'],after['uvs'])
        changed=service.capture('glow','pixels')
        self.assertNotEqual(changed['data']['state']['uvs'],after['uvs'])
        self.assertEqual(changed['data']['state']['original_function_calls']['0x4e5260'],2)
        service.reset();self.assertEqual(service.inspect_state('glow')['glow'],after)
        self.assertEqual(service.capture('glow','pixels'),first)
        service.advance_ticks(3,'glow');self.assertEqual(service.capture('glow','pixels'),changed)
        service.remove_effect('glow');service.reset();self.assertEqual(service.capture('glow','pixels'),first)
        EVIDENCE['symglow_pixels']=dict(type_id='0x27df45be',sha256=first['sha256'],
            captures_per_timer=1,duplicate_capture_mutates_uv=False,checkpoint_restores_cached_render=True,
            explicit_render_stride=3,real_map_device_gpu_acceptance=False)

    def test_literal_waterfall_full_hundred_pool_warmup_rng_and_pixels_reset(self):
        service=Controller(BASELINE);service.create_effect('fall','waterfall')
        initial=service.inspect_state('fall')['fall']
        self.assertEqual(len(initial['particles']),100)
        self.assertEqual(initial['native_warmup_ticks'],100)
        self.assertEqual(initial['original_function_calls']['0x4f2e60'],1)
        self.assertEqual(initial['random_count'],602) # Warmup runs inside native Init, not through the Animate entry.
        service.checkpoint();service.advance_ticks(48,'fall')
        midpoint=service.inspect_state('fall')['fall'];service.checkpoint();service.advance_ticks(48,'fall')
        state=service.inspect_state('fall')['fall']
        self.assertEqual(state['random_count'],1566)
        self.assertEqual(state['original_function_calls']['0x4f2f00'],96)
        self.assertTrue(all(value>0 for value in state['lifecycle_observations'].values()))
        capture=service.capture('fall','pixels');self.assertGreater(capture['nonzero_pixels'],0)
        self.assertEqual(capture['data']['packet']['omitted_copied_light_calls'],len(capture['data']['packet']['draws']))
        service.reset();self.assertEqual(service.inspect_state('fall')['fall'],midpoint)
        service.advance_ticks(48,'fall');again=service.capture('fall','pixels')
        self.assertEqual(capture['sha256'],again['sha256']);self.assertEqual(capture['data'],again['data'])
        EVIDENCE['waterfall_pixels']=dict(type_id='0xa907dabf',warmup_ticks=100,pool_records=100,
            native_random_count=1566,sha256=capture['sha256'],exact_full_pool_midcycle_replay=True,
            aliases_included=False,natural_map_device_gpu_acceptance=False)

    def test_sparks_explicit_birth_full_pools_native_ballistics_pixels_and_checkpoint(self):
        evidence=[]
        for profile,count,range_calls,raw_draws in [('editor',10,90,90),('controlled_bounce_trail',20,180,160)]:
            service=Controller(BASELINE);service.create_effect('spark','sparks',dict(profile=profile))
            birth=service.inspect_state('spark')['spark']
            self.assertEqual(birth['ticks'],0);self.assertEqual(len(birth['particles']),count)
            self.assertEqual(birth['particles'],birth['native_initial_state']['particles'])
            self.assertEqual(birth['original_function_calls']['0x4e55b0'],1)
            self.assertNotIn('0x4e5830',birth['original_function_calls'])
            self.assertEqual(birth['random_count'],range_calls);self.assertEqual(birth['native_raw_crt_draws'],raw_draws)
            service.advance_ticks(12,'spark');midpoint=service.inspect_state('spark')['spark'];service.checkpoint()
            first=service.capture('spark','pixels');self.assertGreater(first['nonzero_pixels'],0)
            self.assertFalse(first['data']['packet']['stored_scale_applied_to_geometry'])
            for draw in first['data']['packet']['draws']:
                self.assertEqual([draw['matrix'][i] for i in (0,5,10)],[1.,1.,1.])
                self.assertEqual(draw['matrix'][12:15],draw['position'])
            repeated=service.capture('spark','pixels')
            self.assertEqual(first['sha256'],repeated['sha256'])
            self.assertEqual(first['data']['packet'],repeated['data']['packet'])
            service.advance_ticks(40,'spark');final=service.inspect_state('spark')['spark']
            self.assertTrue(final['done']);self.assertEqual(len(final['particles']),count)
            self.assertEqual(final['original_function_calls']['0x4e5830'],52)
            self.assertEqual(final['random_count'],range_calls);self.assertEqual(final['native_raw_crt_draws'],raw_draws)
            service.reset();self.assertEqual(service.inspect_state('spark')['spark'],midpoint)
            replay=service.capture('spark','pixels');self.assertEqual(first,replay)
            service.remove_effect('spark');service.reset();self.assertEqual(service.inspect_state('spark')['spark'],midpoint)
            with self.assertRaises(UnsupportedControl):service.set_position('spark',[1,0,0])
            with self.assertRaises(ValueError):service.create_effect('unknown','sparks',dict(profile='seeking'))
            with self.assertRaises(ValueError):service.create_effect('target','sparks',dict(numtargets=1))
            evidence.append(dict(profile=profile,pool_records=count,native_range_calls=range_calls,
                native_raw_crt_draws=raw_draws,birth_tick=0,pixel_sha256=first['sha256'],
                exact_replay=True,native_pos1_only=True,natural_map_or_gpu_acceptance=False))
        EVIDENCE['sparks_pixels']=evidence


if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--output',type=Path);args=parser.parse_args()
    started=time.perf_counter()
    result=unittest.TextTestRunner(verbosity=2).run(unittest.defaultTestLoader.loadTestsFromTestCase(ControlTests))
    report=dict(status='pass' if result.wasSuccessful() else 'fail',tests=result.testsRun,
        failures=len(result.failures),errors=len(result.errors),elapsed_seconds=time.perf_counter()-started,
        retail_sha256=digest(BASELINE),evidence=EVIDENCE,
        scope='Direct authoredpartsys and shared missile trajectories; Flame/Ripple/Fizzle/MistFog original animation/software captures; verified static asset key/matrix/frame sampling and owner translation; actor inputs, lifecycle/reset and independent agents. Named builds remain guarded. No generic map world, full Fireball/Globe leaf, map reaper, editor input or modern renderer/full VFX acceptance.')
    if args.output:
        args.output.parent.mkdir(parents=True,exist_ok=True)
        args.output.write_text(json.dumps(report,indent=2)+'\n')
    raise SystemExit(0 if result.wasSuccessful() else 1)
