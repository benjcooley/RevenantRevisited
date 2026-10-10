#!/usr/bin/env python3
"""Locate exact shipped EFFECT IDs in original module sectors without editing them.

This inventories serialized placement/flags, not visibility or A/B acceptance.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import struct
import sys
import zipfile

ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'tools/savefmt'))
from revsave import Decoder


def scan(module,type_ids):
    ledger=json.loads((ROOT/'docs/vfx/EFFECT_BURNDOWN.json').read_text())
    rows={int(row['retail_type_id'],16):row for row in ledger['retail_effects']}
    if any(value not in rows for value in type_ids):raise ValueError('Select exact shipped EFFECT IDs from the ledger')
    patterns=[struct.pack('<I',value)for value in type_ids]
    placements=[];errors=[];scanned=0;candidates=0
    with zipfile.ZipFile(module)as archive:
        for member in archive.namelist():
            match=re.fullmatch(r'Map/(\d+)_(\d+)_(\d+)\.dat',member,re.I)
            if not match:continue
            scanned+=1;data=archive.read(member)
            if not any(pattern in data for pattern in patterns):continue
            candidates+=1
            try:
                decoder=Decoder(data);version=decoder.sector()
                fields={field.path:field.value for field in decoder.fields}
                if 'TRAILING' in fields:raise ValueError('Sector did not parse to exact EOF')
                for slot,record in enumerate(decoder.records):
                    if not record or record.objclass!=25:continue
                    prefix=f'obj[{slot}]';type_id=int(fields[prefix+'.uniqueid'],16)
                    if type_id not in type_ids:continue
                    position=[fields.get(prefix+'.pos.'+axis)for axis in 'xyz']
                    if not all(isinstance(value,int)for value in position):
                        raise ValueError(f'Undecoded placement at slot{slot}, stream version{version}')
                    placements.append(dict(type=rows[type_id]['retail_name'],type_id=hex(type_id),
                        asset=rows[type_id]['asset'],member=member,sector=[int(value)for value in match.groups()],
                        version=version,slot=slot,name=fields.get(prefix+'.name'),position=position,
                        map_index=fields.get(prefix+'.mapindex'),flags=fields.get(prefix+'.flags'),
                        sector_sha256=hashlib.sha256(data).hexdigest()))
            except Exception as error:
                errors.append(dict(member=member,error=str(error)))
    with Path(module).open('rb')as source:digest=hashlib.file_digest(source,'sha256').hexdigest()
    return dict(status='placement_inventory_complete'if not errors else'placement_inventory_partial',
        module=str(Path(module).resolve()),module_sha256=digest,selected_ids=[hex(value)for value in sorted(type_ids)],
        scanned_sectors=scanned,candidate_sectors=candidates,errors=errors,placements=placements,
        scope='Read-only top-level serialized EFFECT positions/flags; no natural visibility, rendering, trigger or acceptance claim.')


if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--module',type=Path,default=ROOT/'data/Modules/Ahkuilon.rvm')
    parser.add_argument('--type-id',action='append',type=lambda value:int(value,0),required=True)
    parser.add_argument('--output',type=Path,required=True)
    args=parser.parse_args()
    if args.output.exists():raise FileExistsError('Use a new output path to preserve previous inventories')
    report=scan(args.module,set(args.type_id));args.output.parent.mkdir(parents=True,exist_ok=True)
    args.output.write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps(dict(status=report['status'],scanned_sectors=report['scanned_sectors'],
        placements=len(report['placements']),errors=report['errors']),indent=2))
