"""Explicit executable identity/layout contract for fixed-address VFX fixtures."""
import copy
import hashlib
import json
from pathlib import Path
from runtime import pe_layout

RETAIL_SHA='28bec27387bf53a553da320dd4883d00ff5bcc5a5f3d2658cca8e8ed586372b5'
BASELINE=Path(__file__).resolve().parents[2]/'recon/retail_asm/baseline/Revenant.rebuilt.exe'


def verify_build(executable,build=None):
    path=Path(executable).resolve();data=path.read_bytes();actual=hashlib.sha256(data).hexdigest()
    if build is None:
        if actual!=RETAIL_SHA:raise ValueError('Changed executable requires an explicit named build.json')
        return dict(name='retail-original',build_id='retail-original-'+actual,
            executable=str(path),executable_sha256=actual,baseline_sha256=RETAIL_SHA,
            experimental=False,patch_manifest=dict(kind='unchanged-baseline'))
    record=json.loads(Path(build).read_text()) if isinstance(build,(str,Path)) else copy.deepcopy(build)
    if not record.get('name') or not record.get('build_id'):raise ValueError('Build needs a name and immutable build ID')
    if record.get('executable_sha256')!=actual:raise ValueError('Named build SHA does not match the loaded image')
    if Path(record.get('executable','')).resolve()!=path:raise ValueError('Named build selects a different executable path')
    if record.get('baseline_sha256')!=RETAIL_SHA:raise ValueError('Build does not identify the verified retail baseline')
    original=BASELINE.read_bytes()
    if hashlib.sha256(original).hexdigest()!=RETAIL_SHA:raise ValueError('Shared fixture baseline changed')
    expected=pe_layout(original);layout=pe_layout(data)
    def addresses(value):
        return value['image_base'],value['entry_rva'],[(s['rva'],s['offset'],s['size']) for s in value['sections']]
    if addresses(expected)!=addresses(layout) or len(data)!=len(original):
        raise ValueError('Fixed-address fixture requires unchanged PE base, entry, section layout and file length')
    patch=record.get('patch_manifest')
    if not isinstance(patch,dict):raise ValueError('Named experimental builds require a patch manifest')
    spans=patch.get('allowed_modified_spans')
    if spans is not None:
        if not all(isinstance(s,list) and len(s)==2 and 0<=s[0]<=s[1]<=len(data) for s in spans):
            raise ValueError('Invalid declared patch spans')
        for offset,(old,new) in enumerate(zip(original,data)):
            if old!=new and not any(start<=offset<end for start,end in spans):
                raise ValueError(f'Undeclared binary change at file offset {offset:#x}')
    record['experimental']=actual!=RETAIL_SHA
    record['fixture_contract']='fixed original addresses; explicit variant SHA/layout; behavioral equivalence is tested, not assumed'
    return record
