#!/usr/bin/env python3
"""Load retail once; execute JSON scenarios or serve JSONL requests on stdin."""
import argparse
import hashlib
import json
from pathlib import Path
import sys
import time
from runtime import Runtime


def number(value):
    return int(value,0) if isinstance(value,str) else int(value)


class Session:
    def __init__(self,executable,setup=()):
        start=time.perf_counter();self.vm=Runtime(executable);self.names={}
        self.executable_sha256=hashlib.sha256(self.vm.image).hexdigest()
        for op in setup:self.operation(op)
        self.checkpoint()
        self.load_seconds=time.perf_counter()-start

    def address(self,value):
        if isinstance(value,str) and value.startswith('$'):
            return self.names[value[1:]]
        return number(value)

    def operation(self,op):
        vm=self.vm;kind=op['op']
        if kind=='allocate':
            address=vm.allocate(number(op['size']));self.names[op['name']]=address
            return dict(address=address)
        if kind=='surface':
            s=vm.surface(op['name'],number(op['width']),number(op['height']))
            self.names[op['name']]=s.address
            return dict(address=s.address,stride=s.stride)
        if kind in ('write','write_code'):
            address=self.address(op['address']);data=bytes.fromhex(op['hex'])
            (vm.write_code if kind=='write_code' else vm.write)(address,data)
            return dict(bytes_written=len(data))
        if kind=='write_u32':
            vm.put_u32(self.address(op['address']),number(op['value']));return dict(bytes_written=4)
        if kind=='mount_file':
            name=op['guest_path'].replace('\\','/').lower()
            vm.files[name]=bytearray(Path(op['host_path']).read_bytes());return dict(bytes_mounted=len(vm.files[name]))
        if kind=='post_message':
            vm.post_message(number(op.get('hwnd',0)),number(op['message']),
                number(op.get('wparam',0)),number(op.get('lparam',0)),number(op.get('x',0)),number(op.get('y',0)))
            return dict(queued=len(vm.messages))
        if kind=='register_window':
            vm.windows[number(op['hwnd'])]=self.address(op['wndproc']);return dict(registered=True)
        if kind=='call':
            result=vm.call(self.address(op['address']),tuple(self.address(a) for a in op.get('args',())),
                this=self.address(op['this']) if 'this' in op else None,
                instruction_limit=number(op.get('instruction_limit',1000000)),
                stop_address=self.address(op['stop_address']) if 'stop_address' in op else None)
            if 'expect_eax' in op and result!=number(op['expect_eax']):
                raise AssertionError(f'EAX {result:#x} != expected {number(op["expect_eax"]):#x}')
            return dict(eax=result)
        if kind=='read_u32':
            value=vm.u32(self.address(op['address']))
            if 'expect' in op and value!=number(op['expect']):
                raise AssertionError(f'Value {value:#x} != expected {number(op["expect"]):#x}')
            return dict(value=value)
        if kind=='advance':
            vm.advance(number(op.get('ticks',1)));return dict(ticks=vm.ticks,milliseconds=vm.milliseconds)
        if kind=='capture':
            s=vm.surfaces[op['name']];data=vm.surface_bytes(op['name'])
            # Output is explicitly requested. Virtual guest file calls never
            # write host files; captures are host-side artifacts.
            if 'output' in op:
                path=Path(op['output']);path.parent.mkdir(parents=True,exist_ok=True);path.write_bytes(data)
            result=dict(sha256=hashlib.sha256(data).hexdigest(),width=s.width,height=s.height,
                stride=s.stride,format=s.format,bytes=len(data))
            if 'expect_sha256' in op and result['sha256']!=op['expect_sha256']:
                raise AssertionError('Surface hash mismatch')
            return result
        raise ValueError(f'Unknown scenario operation: {kind}')

    def execute(self,scenario):
        if 'executable_sha256' in scenario and scenario['executable_sha256']!=self.executable_sha256:
            raise ValueError('Scenario addresses belong to a different executable')
        start=time.perf_counter();restored=self.vm.restore();self.names=dict(self.baseline_names)
        try:results=[self.operation(op) for op in scenario['operations']]
        except Exception:
            self.vm.restore();self.names=dict(self.baseline_names);raise
        files={name:dict(size=len(data),sha256=hashlib.sha256(data).hexdigest()) for name,data in self.vm.files.items()}
        return dict(status='pass',name=scenario.get('name'),results=results,files=files,
            api_trace=list(self.vm.api_trace),restored_pages=restored,elapsed_ms=(time.perf_counter()-start)*1000)

    def checkpoint(self):
        self.vm.checkpoint();self.baseline_names=dict(self.names)


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('executable',type=Path)
    parser.add_argument('--scenario',type=Path,help='Run one scenario; otherwise serve JSONL requests')
    parser.add_argument('--setup',type=Path,help='JSON file containing one-time setup operations')
    parser.add_argument('--repeat',type=int,default=1)
    args=parser.parse_args()
    setup=json.loads(args.setup.read_text()) if args.setup else []
    scenario=json.loads(args.scenario.read_text()) if args.scenario else None
    session=Session(args.executable,setup if scenario is None else scenario.get('setup',setup))
    if scenario is not None:
        if args.repeat<1:parser.error('--repeat must be positive')
        for _ in range(args.repeat):print(json.dumps(session.execute(scenario)),flush=True)
        return
    for line in sys.stdin:
        request={}
        try:
            parsed=json.loads(line)
            if not isinstance(parsed,dict):raise ValueError('Request must be a JSON object')
            request=parsed;command=request.get('command','execute')
            if command=='status':
                result=dict(status='ready',executable_sha256=session.executable_sha256,
                    pe_load_seconds=session.load_seconds,guest_os_boots=0)
            elif command=='execute':result=session.execute(request['scenario'])
            elif command=='checkpoint':session.checkpoint();result=dict(status='checkpointed')
            elif command=='restore':
                count=session.vm.restore();session.names=dict(session.baseline_names);result=dict(status='restored',pages=count)
            else:raise ValueError(f'Unknown command: {command}')
        except Exception as error:result=dict(status='error',type=type(error).__name__,message=str(error))
        if 'id' in request:result['id']=request['id']
        print(json.dumps(result),flush=True)


if __name__=='__main__':main()
