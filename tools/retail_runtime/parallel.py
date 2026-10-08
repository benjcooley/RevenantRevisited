#!/usr/bin/env python3
"""Persistent, isolated retail JSONL processes for concurrent scenario batches."""
import argparse
from collections import deque
from concurrent.futures import ThreadPoolExecutor
import copy
import hashlib
import json
import math
from pathlib import Path
import queue
import subprocess
import sys
import threading
import time
import uuid


def digest(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def load_manifest(path):
    """Resolve a CLI or Python batch into explicit executable/runner/scenario inputs."""
    path=Path(path).resolve();base=path.parent
    manifest=json.loads(path.read_text())
    for job in manifest['jobs']:
        if isinstance(job['scenario'],str):
            job['scenario']=json.loads((base/job['scenario']).read_text())
    for session in manifest.get('sessions',[]):
        if 'runner' in session:session['runner']=str((base/session['runner']).resolve())
        build_base=base
        if isinstance(session.get('build'),str):
            build_path=(base/session['build']).resolve();build_base=build_path.parent
            session['build']=json.loads(build_path.read_text())
        record=session.get('build',session)
        executable=Path(record['executable'])
        if not executable.is_absolute():record['executable']=str((build_base/executable).resolve())
    return manifest


class WorkerFailure(RuntimeError):
    pass


class _Worker:
    def __init__(self, executable, index, setup_file, expected_sha, timeout, runner=None, extra_args=()):
        self.index = index
        self.executable = executable
        self.setup_file = setup_file
        self.expected_sha = expected_sha
        self.timeout = timeout
        self.runner = runner or Path(__file__).with_name('run.py')
        self.extra_args = list(extra_args)
        self.process = None
        self.responses = queue.Queue()
        self.stderr = deque(maxlen=40)
        self.starts = 0

    def start(self):
        self.close()
        self.responses = queue.Queue()
        self.stderr.clear()
        self.process = subprocess.Popen(
            [sys.executable, str(self.runner), str(self.executable), '--setup', str(self.setup_file), *self.extra_args],
            stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
            text=True, bufsize=1)
        process = self.process
        responses = self.responses
        def read_stdout():
            try:
                for line in process.stdout:
                    responses.put(line)
            finally:
                responses.put(None)
        def read_stderr():
            for line in process.stderr:
                self.stderr.append(line.rstrip())
        self.readers = [threading.Thread(target=read_stdout, daemon=True), threading.Thread(target=read_stderr, daemon=True)]
        for reader in self.readers:
            reader.start()
        self.starts += 1
        ready = self.request({'id': f'startup-{self.index:03d}-{self.starts}', 'command': 'status'})
        if ready.get('status') != 'ready' or ready.get('executable_sha256') != self.expected_sha:
            self.close()
            raise WorkerFailure('Worker did not load the pinned executable SHA256')
        self.ready = ready

    def request(self, request):
        try:
            # Include pipe transmission in the deadline. A stuck worker must
            # not block the controller even if a scenario exceeds pipe capacity.
            process = self.process
            responses = self.responses
            payload = json.dumps(request) + '\n'
            def send():
                try:
                    process.stdin.write(payload)
                    process.stdin.flush()
                except (BrokenPipeError, OSError, ValueError) as error:
                    responses.put(error)
            threading.Thread(target=send, daemon=True).start()
            line = self.responses.get(timeout=self.timeout)
            if isinstance(line, Exception):
                raise WorkerFailure(f'Worker request transmission failed: {line}')
            if line is None:
                raise WorkerFailure('Worker exited: ' + '\n'.join(self.stderr))
            result = json.loads(line)
            if not isinstance(result, dict) or result.get('id') != request['id']:
                raise WorkerFailure('Worker response ID or JSON object is invalid')
            return result
        except queue.Empty as error:
            self.close()
            raise WorkerFailure(f'Worker deadline exceeded ({self.timeout:g} seconds)') from error
        except (BrokenPipeError, OSError, ValueError, AttributeError) as error:
            self.close()
            raise WorkerFailure(f'Worker protocol failed: {error}') from error

    def close(self):
        process = self.process
        self.process = None
        if process is None:
            return
        if process.poll() is None:
            process.terminate()
            try:
                process.wait(timeout=1)
            except subprocess.TimeoutExpired:
                process.kill()
                process.wait(timeout=1)
        for reader in getattr(self, 'readers', []):
            reader.join(timeout=1)
        for stream in (process.stdin, process.stdout, process.stderr):
            if stream:
                stream.close()


class Pool:
    """Independent named sessions, each pinned to one explicit executable build."""
    def __init__(self, executable=None, workers=2, setup=(), artifact_root=None, request_timeout=30, sessions=None):
        if not isinstance(workers, int) or workers < 1:
            raise ValueError('workers must be a positive integer')
        if not math.isfinite(request_timeout) or request_timeout <= 0:
            raise ValueError('request_timeout must be finite and positive')
        if sessions is None:
            if executable is None:
                raise ValueError('Provide an executable or named sessions')
            sessions = [dict(name=f'worker-{i:03d}', executable=str(executable), setup=list(setup)) for i in range(workers)]
        if not sessions or len({s['name'] for s in sessions}) != len(sessions):
            raise ValueError('Session names must be unique and nonempty')
        self.next_id = 0
        self.lock = threading.Lock()
        base = Path(artifact_root or 'recon/retail_asm/runtime/parallel').resolve()
        self.artifact_dir = base / ('session-' + uuid.uuid4().hex)
        self.artifact_dir.mkdir(parents=True)
        self.workers = []
        try:
            for index, config in enumerate(sessions):
                load_dir = self.artifact_dir / f'worker-{index:03d}-load-001'
                load_dir.mkdir()
                setup_file = load_dir / 'setup.json'
                initial = self._capture_paths(config.get('setup', []), load_dir / 'setup-artifacts')
                setup_file.write_text(json.dumps(initial))
                record = self._build_record(config.get('build', config))
                build_file = load_dir / 'build.json'
                build_file.write_text(json.dumps(record))
                extra_args = ['--build', str(build_file)] if config.get('pass_build_record',False) else []
                worker = _Worker(Path(record['executable']), index, setup_file, record['executable_sha256'],
                    request_timeout, config.get('runner'), extra_args)
                worker.build_file = build_file
                worker.pass_build_record = config.get('pass_build_record',False)
                worker.name = config['name']
                worker.record = record
                worker.setup = copy.deepcopy(config.get('setup', []))
                self.workers.append(worker)
                worker.start()
        except Exception:
            self.close()
            raise

    @staticmethod
    def _build_record(record):
        record = copy.deepcopy(record)
        record['executable'] = str(Path(record['executable']).resolve())
        actual = digest(record['executable'])
        if record.get('executable_sha256', actual) != actual:
            raise ValueError('Build record SHA256 does not match its executable')
        record['executable_sha256'] = actual
        record.setdefault('build_id', 'unregistered-'+actual)
        record.setdefault('baseline_sha256', None)
        record.setdefault('patch_manifest', None)
        return record

    def reload(self, session, build):
        """Explicitly replace one session with a published build and fresh checkpoint."""
        with self.lock:
            worker = next(w for w in self.workers if w.name == session)
            record = self._build_record(build)
            worker.close()
            worker.record = record
            worker.executable = Path(record['executable'])
            worker.expected_sha = record['executable_sha256']
            load_dir = self.artifact_dir / f'worker-{worker.index:03d}-load-{worker.starts+1:03d}'
            load_dir.mkdir()
            worker.build_file = load_dir / 'build.json'
            worker.build_file.write_text(json.dumps(record))
            worker.setup_file = load_dir / 'setup.json'
            worker.setup_file.write_text(json.dumps(self._capture_paths(worker.setup,load_dir/'setup-artifacts')))
            if worker.pass_build_record:
                worker.extra_args = ['--build',str(worker.build_file)]
            worker.start()
            return dict(session=session, build=copy.deepcopy(record), worker_pid=worker.process.pid)

    @staticmethod
    def _capture_paths(operations, artifact_dir):
        operations = copy.deepcopy(operations)
        for operation in operations:
            if operation.get('op') != 'capture':continue
            for key in ('output','depth_output'):
                if key not in operation:continue
                path = Path(operation[key])
                if path.is_absolute() or '..' in path.parts or not path.parts:
                    raise ValueError('Capture output must be a relative path without ..')
                if path.parts[0] == 'result.json':
                    raise ValueError('Capture output result.json is reserved for job metadata')
                operation[key] = str(artifact_dir / path)
        return operations

    def _job(self, worker, request_id, job):
        started = time.perf_counter()
        artifact_dir = self.artifact_dir / request_id
        artifact_dir.mkdir()
        result = {}
        try:
            if not isinstance(job, dict):
                raise ValueError('Job must be a JSON object')
            if job.get('session', worker.name) != worker.name:
                raise ValueError('Unknown session: '+str(job['session']))
            scenario = copy.deepcopy(job['scenario'])
            if scenario.get('executable_sha256', worker.expected_sha) != worker.expected_sha:
                raise ValueError('Job pins a different executable SHA256')
            if scenario.get('setup', worker.setup) != worker.setup:
                raise ValueError('Job setup differs from the pool checkpoint; create a separate pool')
            scenario['executable_sha256'] = worker.expected_sha
            scenario['operations'] = self._capture_paths(scenario['operations'], artifact_dir)
            if worker.process is None or worker.process.poll() is not None:
                worker.start()
            result = worker.request({'id': request_id, 'command': 'execute', 'scenario': scenario})
        except Exception as error:
            result = dict(status='error', type=type(error).__name__, message=str(error))
            # Scenario errors returned by run.py already rolled back. A protocol
            # failure discards the whole process rather than retaining bad state.
            if isinstance(error, WorkerFailure):
                worker.close()
        result.update(id=request_id, worker=worker.index, worker_pid=worker.process.pid if worker.process else None,
            worker_starts=worker.starts, session=worker.name, job_name=job.get('name') if isinstance(job,dict) else None,
            executable_sha256=worker.expected_sha, build_id=worker.record['build_id'],
            baseline_sha256=worker.record['baseline_sha256'], patch_manifest=worker.record['patch_manifest'],
            artifact_dir=str(artifact_dir), wall_ms=(time.perf_counter()-started)*1000)
        (artifact_dir / 'result.json').write_text(json.dumps(result, indent=2) + '\n')
        return result

    def execute(self, jobs):
        """Return input-ordered results; one failed job never aborts its siblings."""
        with self.lock:
            if not self.workers:
                raise RuntimeError('Pool is closed')
            for worker in self.workers:
                if digest(worker.executable) != worker.expected_sha:
                    raise ValueError('Executable changed after pool creation: '+worker.name)
            jobs = list(jobs)
            groups = [[] for _ in self.workers]
            for offset, job in enumerate(jobs):
                request_id = f'job-{self.next_id:06d}'
                self.next_id += 1
                if isinstance(job,dict) and 'session' in job:
                    indices = [w.index for w in self.workers if w.name == job['session']]
                    index = indices[0] if indices else offset % len(self.workers)
                else:
                    index = offset % len(self.workers)
                groups[index].append((offset, request_id, job))
            results = [None] * len(jobs)
            def run_group(worker, group):
                for offset, request_id, job in group:
                    results[offset] = self._job(worker, request_id, job)
            with ThreadPoolExecutor(max_workers=len(self.workers)) as executor:
                futures = [executor.submit(run_group, worker, group) for worker, group in zip(self.workers, groups)]
                for future in futures:
                    future.result()
            for worker in self.workers:
                if digest(worker.executable) != worker.expected_sha:
                    raise ValueError('Executable changed while scenarios ran: '+worker.name)
            return results

    def close(self):
        for worker in self.workers:
            worker.close()
        self.workers = []

    def __enter__(self):
        return self

    def __exit__(self, *args):
        self.close()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('executable', type=Path, nargs='?', help='Omit when manifest declares named sessions/builds')
    parser.add_argument('--jobs', type=Path, required=True, help='JSON manifest: setup array and jobs array')
    parser.add_argument('--workers', type=int, default=2)
    parser.add_argument('--timeout', type=float, default=30, help='Wall deadline per worker request in seconds')
    parser.add_argument('--repeat',type=int,default=1,help='Replay the batch without reloading any worker')
    parser.add_argument('--artifacts', type=Path)
    parser.add_argument('--output', type=Path)
    args = parser.parse_args()
    if args.repeat<1:parser.error('--repeat must be positive')
    manifest=load_manifest(args.jobs)
    jobs=manifest['jobs'];sessions=manifest.get('sessions')
    started = time.perf_counter()
    with Pool(args.executable, args.workers, manifest.get('setup', []), args.artifacts, args.timeout, sessions) as pool:
        results=[];batches=[]
        for index in range(args.repeat):
            batch_started=time.perf_counter()
            current=pool.execute(jobs)
            for result in current:result['batch']=index
            results.extend(current)
            batches.append(dict(index=index,elapsed_ms=(time.perf_counter()-batch_started)*1000,
                jobs=len(current),status='pass' if all(r['status']=='pass' for r in current) else 'fail'))
        report = dict(status='pass' if all(r['status'] == 'pass' for r in results) else 'fail',
            builds={w.name:w.record for w in pool.workers}, workers=len(pool.workers), artifact_dir=str(pool.artifact_dir),
            elapsed_seconds=time.perf_counter()-started,repeats=args.repeat,batches=batches, results=results)
    if args.output:
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(json.dumps(report, indent=2)+'\n')
    print(json.dumps(report))
    raise SystemExit(0 if report['status']=='pass' else 1)


if __name__ == '__main__':
    main()
