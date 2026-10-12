"""Opt-in original CRT/file-manager startup for filename-based effect presets."""
import argparse
import importlib.util
import json
import zipfile
from pathlib import Path

from unicorn.x86_const import UC_X86_REG_ESI
from software_probe import SoftwareFixture
from speed_profile_contract import ROOT, sha


def filesystem_type():
    path=ROOT/'tools/retail_runtime/slots/hud/winfs.py'
    spec=importlib.util.spec_from_file_location('vfx_preset_winfs',path)
    module=importlib.util.module_from_spec(spec);spec.loader.exec_module(module)
    return module.WinFileSystem


def install(vm, data_root):
    """Run only the original low-level I/O initializers; leave game state alone.

    SoftwareFixture already ran the CRT heap/thread prefix. _ioinit creates
    descriptors; the separate C initializer59151e creates FILE streams.
    Merely mounting files cannot make fopen work without both.
    """
    fs=filesystem_type()(vm)
    mounted=fs.mount_tree(data_root)
    if not vm.u32(0x6774e0):
        vm.call(0x59299f,instruction_limit=2_000_000)
    if not vm.u32(0x678600):
        vm.call(0x59151e,instruction_limit=2_000_000)
    assert vm.u32(0x678600)==512
    # The exact WinMain fragment creates the file-manager mutex. It does
    # not execute the rest of WinMain or replace any parser/file function.
    if not vm.u32(0x65b8ac):
        vm.uc.reg_write(UC_X86_REG_ESI,vm.api_address('kernel32.dll','CreateMutexA'))
        vm.call(0x4865c5,stop_address=0x4865d7)
    return fs,mounted


def cstring(vm, text):
    data=text.encode('cp1252')+b'\0';p=vm.allocate(len(data));vm.write(p,data);return p


def read(vm, name):
    handle=vm.call(0x4a1240,(cstring(vm,name),cstring(vm,'rb'),0),instruction_limit=2_000_000)
    if not handle:return None
    length=vm.call(0x4a17b0,(handle,),instruction_limit=2_000_000)
    assert length<16*1024*1024
    buffer=vm.allocate(max(1,length))
    count=vm.call(0x4a15a0,(buffer,1,length,handle),instruction_limit=2_000_000)
    assert count==length,(name,count,length)
    data=bytes(vm.uc.mem_read(buffer,length))
    vm.call(0x4a1540,(handle,),instruction_limit=2_000_000)
    return data


def run(data_root, output):
    output.mkdir(parents=True,exist_ok=False)
    f=SoftwareFixture(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe');v=f.vm
    # Negative control: descriptors alone do not initialize FILE streams.
    fs=filesystem_type()(v);fs.mount_tree(data_root);v.call(0x59299f)
    before=len(v.api_trace)
    handle=v.call(0x58b5db,(cstring(v,'revenant.ini'),cstring(v,'rb')))
    assert handle==0 and not any(x['function']=='CreateFileA'for x in v.api_trace[before:])
    fs,mounted=install(v,data_root)
    known=read(v,'revenant.ini');assert known==(data_root/'revenant.ini').read_bytes()
    missing={name:read(v,name)for name in ['cfire.txt','csparks.txt','vapor.txt']}
    assert all(value is None for value in missing.values())
    archives=[]
    for path in sorted(data_root.iterdir()):
        if not path.is_file() or not zipfile.is_zipfile(path):continue
        with zipfile.ZipFile(path) as archive:
            matches=[name for name in archive.namelist()
                     if name.replace('\\','/').lower().rsplit('/',1)[-1] in missing]
            archives.append(dict(name=path.name,sha256=sha(path.read_bytes()),
                                 entries=len(archive.namelist()),matching_presets=matches))
    assert archives and all(not a['matching_presets'] for a in archives)
    trace=[x for x in v.api_trace if x['function'] in ['CreateFileA','ReadFile','CloseHandle']]
    assert len([x for x in trace if x['function']=='CreateFileA'])>=4
    report=dict(status='pass',files_mounted=mounted,known_file_sha256=sha(known),
                missing_presets=list(missing),archive_audit=archives,api_trace=trace,
                negative='Original _ioinit alone leaves FILE stream count zero: existing file fopen fails before CreateFileA.',
                scope='Original CRT descriptor/stream initializers, actual WinMain mutex creation, original custom fopen/fread/length/close and exact mounted retail bytes. Core Windows file-service boundary reused from HUD slot. No game parser, preset definition or effect controller substituted. Pack search roots are not initialized; every installed top-level ZIP-compatible pack is independently audited for the three exact preset basenames. No broader installation/caller absence claim.')
    (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
    print('PASS original known-file read and three missing loose-file controls')


if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--data-root',type=Path,required=True)
    p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.data_root,a.output)
