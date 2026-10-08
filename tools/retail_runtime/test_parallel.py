#!/usr/bin/env python3
"""Verify concurrent retail sessions, private builds, rollback and deadlines."""
import argparse
import copy
import hashlib
import json
from pathlib import Path
import shutil
import tempfile
import time
import unittest
from parallel import Pool, WorkerFailure, _Worker, digest,load_manifest
from variants import Workspace


ROOT = Path(__file__).resolve().parents[2]
BASELINE = ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe'
SCENARIO = json.loads((Path(__file__).parent/'scenarios/render_state.json').read_text())
EVIDENCE = {}


class ParallelTests(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory()
        self.path = Path(self.temporary.name)

    def tearDown(self):
        self.temporary.cleanup()

    def test_two_persistent_workers_execute_original_retail_and_recover(self):
        with Pool(BASELINE, workers=2, setup=SCENARIO['setup'], artifact_root=self.path) as pool:
            pids = [w.process.pid for w in pool.workers]
            first = pool.execute([dict(scenario=SCENARIO) for _ in range(20)])
            bad = dict(operations=[dict(op='write_u32', address='0x675f10', value=999), dict(op='invalid')])
            mixed = pool.execute([dict(scenario=bad), dict(scenario=SCENARIO), dict(scenario=SCENARIO)])
            self.assertEqual([r['status'] for r in mixed], ['error', 'pass', 'pass'])
            self.assertEqual([w.process.pid for w in pool.workers], pids)
            self.assertEqual(len(set(pids)), 2)
            self.assertTrue(all(r['status']=='pass' and r['worker_starts']==1 for r in first))
            self.assertTrue(all(r['results']==first[0]['results'] for r in first))
            self.assertEqual([r['id'] for r in first[:3]], ['job-000000', 'job-000001', 'job-000002'])
            self.assertTrue(all(r['executable_sha256']==digest(BASELINE) for r in first))
            EVIDENCE['parallel_original_render_state'] = dict(worker_pids=pids, scenarios=len(first),
                first_values=[first[0]['results'][2]['value'],first[0]['results'][5]['value']],
                error_recovery=[r['status'] for r in mixed], warm_worker_starts=[w.starts for w in pool.workers])

    def test_per_job_capture_paths_and_traversal_rejection(self):
        setup = [dict(op='surface', name='screen', width=3, height=2),
                 dict(op='capture',name='screen',output='initial.raw')]
        scenario = dict(operations=[dict(op='capture', name='screen', output='images/frame.rgb565')])
        with Pool(BASELINE, setup=setup, artifact_root=self.path) as pool:
            results = pool.execute([dict(scenario=scenario),dict(scenario=scenario)])
            paths = [Path(r['artifact_dir'])/'images/frame.rgb565' for r in results]
            self.assertNotEqual(paths[0], paths[1])
            self.assertEqual(paths[0].read_bytes(), paths[1].read_bytes())
            scenario['operations'][0]['output']='../escape.raw'
            result = pool.execute([dict(scenario=scenario)])[0]
            self.assertEqual(result['status'],'error')
            self.assertIn('relative path',result['message'])
            worker=pool.workers[0]
            old_metadata=worker.build_file;old_data=old_metadata.read_bytes()
            old_initial=worker.build_file.parent/'setup-artifacts/initial.raw'
            pool.reload(worker.name,worker.record)
            new_initial=worker.build_file.parent/'setup-artifacts/initial.raw'
            self.assertNotEqual(old_initial,new_initial)
            self.assertEqual(old_initial.read_bytes(),new_initial.read_bytes())
            self.assertEqual(old_metadata.read_bytes(),old_data)

    def test_worker_crash_restarts_only_that_worker(self):
        with Pool(BASELINE, setup=SCENARIO['setup'], artifact_root=self.path) as pool:
            old = pool.workers[0].process
            sibling = pool.workers[1].process.pid
            old.kill(); old.wait(timeout=2)
            result = pool.execute([dict(scenario=SCENARIO)])[0]
            self.assertEqual(result['status'],'pass')
            self.assertNotEqual(result['worker_pid'],old.pid)
            self.assertEqual(result['worker_starts'],2)
            self.assertEqual(pool.workers[1].process.pid,sibling)

    def test_worker_wall_deadline_kills_stalled_process(self):
        runner = self.path/'stalled.py'
        runner.write_text('import sys,json,time,hashlib\n'
            'for line in sys.stdin:\n'
            ' r=json.loads(line)\n'
            ' if r["command"]=="status":\n'
            '  print(json.dumps(dict(id=r["id"],status="ready",executable_sha256=hashlib.sha256(open(sys.argv[1],"rb").read()).hexdigest())),flush=True)\n'
            ' else: time.sleep(10)\n')
        setup = self.path/'setup.json'; setup.write_text('[]')
        worker = _Worker(BASELINE,0,setup,digest(BASELINE),0.5,runner)
        try:
            worker.start(); process=worker.process
            started=time.perf_counter()
            with self.assertRaisesRegex(WorkerFailure,'deadline'):
                worker.request(dict(id='hung',command='execute',scenario=dict(operations=[])))
            self.assertLess(time.perf_counter()-started,3)
            self.assertIsNotNone(process.poll())
            self.assertIsNone(worker.process)
        finally:
            worker.close()

    def test_named_private_variants_and_explicit_reload(self):
        before=digest(BASELINE)
        workspace=Workspace.create(self.path/'agent',BASELINE)
        versions=[]
        for name,value in [('trace-flame',6),('no-collision',2),('trace-flame',4)]:
            candidate=self.path/f'candidate-{value}.exe'
            image=bytearray(BASELINE.read_bytes())
            # Test-only instrumentation replaces SWSetRenderState with a small
            # stdcall function forcing DESTBLEND. This is not a retail fix.
            patch=bytes.fromhex('c705105f6700')+value.to_bytes(4,'little')+bytes.fromhex('31c0c20800')
            image[0x16d400:0x16d400+len(patch)]=patch
            candidate.write_bytes(image)
            versions.append(workspace.import_variant(name,candidate,dict(kind='test-only',address='0x56d400',forced_value=value)))
        self.assertNotEqual(versions[0]['build_id'],versions[2]['build_id'])
        self.assertEqual(Path(versions[0]['executable']).name,'Revenant.trace-flame.exe')
        self.assertEqual(Path(versions[1]['executable']).name,'Revenant.no-collision.exe')
        def scenario(record,value):
            return dict(executable_sha256=record['executable_sha256'],operations=[
                dict(op='call',address='0x56d400',args=[20,99]),
                dict(op='read_u32',address='0x675f10',expect=value)])
        sessions=[dict(name='flame',build=versions[0]),dict(name='collision',build=versions[1])]
        with Pool(sessions=sessions,artifact_root=self.path) as pool:
            jobs=[dict(session='flame',scenario=scenario(versions[0],6)),dict(session='collision',scenario=scenario(versions[1],2))]
            results=pool.execute(jobs)
            self.assertEqual([r['status'] for r in results],['pass','pass'])
            self.assertNotEqual(results[0]['executable_sha256'],results[1]['executable_sha256'])
            sibling=pool.workers[1].process.pid
            pool.reload('flame',versions[2])
            jobs[0]['scenario']=scenario(versions[2],4)
            after=pool.execute(jobs)
            self.assertEqual([r['status'] for r in after],['pass','pass'])
            self.assertEqual(pool.workers[1].process.pid,sibling)
            mismatch=pool.execute([dict(session='flame',scenario=SCENARIO)])[0]
            self.assertEqual(mismatch['status'],'error')
            self.assertIn('different executable',mismatch['message'])
            EVIDENCE['private_variants'] = dict(builds=versions,
                observed_values=[r['results'][-1]['value'] for r in results],
                values_after_reload=[r['results'][-1]['value'] for r in after],
                baseline_unchanged=digest(BASELINE)==before,
                note='Temporary test-only variants force software render state; no game-rendering claim.')
        self.assertEqual(digest(BASELINE),before)
        self.assertEqual(digest(versions[0]['executable']),versions[0]['executable_sha256'])
        self.assertEqual(digest(versions[1]['executable']),versions[1]['executable_sha256'])

    def test_private_source_copies_and_named_assembly_build(self):
        source=self.path/'original';source.mkdir()
        (source/'retail.asm').write_text('BITS 32\n%include "symbols.inc"\n')
        (source/'symbols.inc').write_text('; copied symbol comment\n')
        original_digest=digest(source/'retail.asm')
        workspace=Workspace.create(self.path/'assembly',BASELINE,source)
        private=workspace.path/'source/retail.asm'
        self.assertNotEqual(private.stat().st_ino,(source/'retail.asm').stat().st_ino)
        # Assemble a private baseline payload to exercise named publication.
        shutil.copyfile(BASELINE,workspace.path/'source/payload.exe')
        private.write_text('BITS 32\nincbin "payload.exe"\n')
        record=workspace.build_variant('private-baseline',dict(kind='test-only-assembly'),nasm=shutil.which('nasm'))
        self.assertEqual(record['executable_sha256'],digest(BASELINE))
        self.assertEqual(digest(source/'retail.asm'),original_digest)
        private.write_text('this is invalid assembly\n')
        with self.assertRaises(RuntimeError):workspace.build_variant('bad-build',{})
        self.assertEqual(digest(record['executable']),record['executable_sha256'])
        self.assertEqual(len(workspace.manifest['builds']),2)

    def test_manifest_build_paths_are_resolved_from_the_build_file(self):
        builds=self.path/'private-builds';builds.mkdir()
        shutil.copyfile(BASELINE,builds/'retail.exe')
        (builds/'build.json').write_text(json.dumps(dict(name='private-baseline',build_id='private-baseline-test',
            executable='retail.exe',executable_sha256=digest(BASELINE),baseline_sha256=digest(BASELINE),
            patch_manifest=dict(kind='unchanged-baseline'))))
        batch=self.path/'batch';batch.mkdir()
        (batch/'case.json').write_text(json.dumps(SCENARIO))
        (batch/'jobs.json').write_text(json.dumps(dict(sessions=[dict(name='private',build='../private-builds/build.json',
            runner=str(Path(__file__).with_name('run.py')),setup=SCENARIO['setup'])],
            jobs=[dict(session='private',scenario='case.json')])))
        manifest=load_manifest(batch/'jobs.json')
        self.assertEqual(manifest['sessions'][0]['build']['executable'],str((builds/'retail.exe').resolve()))
        with Pool(sessions=manifest['sessions'],artifact_root=self.path) as pool:
            self.assertEqual(pool.execute(manifest['jobs'])[0]['status'],'pass')


if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--output',type=Path);args=parser.parse_args()
    started=time.perf_counter()
    result=unittest.TextTestRunner(verbosity=2).run(unittest.defaultTestLoader.loadTestsFromTestCase(ParallelTests))
    report=dict(status='pass' if result.wasSuccessful() else 'fail',tests=result.testsRun,
        failures=len(result.failures),errors=len(result.errors),elapsed_seconds=time.perf_counter()-started,
        baseline_sha256=digest(BASELINE),evidence=EVIDENCE,
        scope='Independent warm retail processes, named private binary variants, explicit reload, source copy isolation, request recovery, deadlines and capture routing. No full retail startup or VFX pixels yet.')
    if args.output:
        args.output.parent.mkdir(parents=True,exist_ok=True)
        args.output.write_text(json.dumps(report,indent=2)+'\n')
    raise SystemExit(0 if result.wasSuccessful() else 1)
