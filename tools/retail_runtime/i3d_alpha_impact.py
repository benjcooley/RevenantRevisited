#!/usr/bin/env python3
"""List shipped 16-bit alpha assets affected by removing RGB black-key inference."""
import argparse
import json
import struct
import zipfile
from pathlib import Path


def audit(archive, ledger):
    result=[]
    with zipfile.ZipFile(archive) as z:
        names={n.lower():n for n in z.namelist()}
        for row in json.loads(Path(ledger).read_text())['retail_effects']:
            name='imagery/'+row['asset'].replace('\\','/').lower()
            if name not in names: continue
            data=z.read(names[name])
            def u(address): return struct.unpack_from('<I',data,address)[0]
            def rel(address): return address+u(address)
            body=20+u(16); textures=rel(body+40); affected=[]
            for index in range(u(body+36)):
                desc=textures+index*120
                height,width=u(desc+8),u(desc+12)
                alpha=u(desc+100); rgb=u(desc+88)|u(desc+92)|u(desc+96)
                if not alpha or u(desc+84)!=16: continue
                offsets=rel(desc+108)
                for frame in range(u(desc+116)):
                    pixels=struct.unpack_from('<'+str(width*height)+'H',data,rel(offsets+frame*4))
                    black=sum((p&rgb)==0 for p in pixels)
                    erased=sum((p&rgb)==0 and (p&alpha)!=0 for p in pixels)
                    if black*5>len(pixels) and erased:
                        affected.append(dict(texture=index,frame=frame,authored_alpha_texels=erased,total=len(pixels)))
            if affected:
                result.append(dict(name=row['retail_name'],id=row['retail_type_id'],asset=row['asset'],affected=affected))
    return result


if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--archive',type=Path,required=True)
    p.add_argument('--ledger',type=Path,required=True)
    p.add_argument('--output',type=Path,required=True)
    args=p.parse_args()
    result=audit(args.archive,args.ledger)
    args.output.write_text(json.dumps(result,indent=2)+'\n')
    print(f'{len(result)} affected exact retail types')
