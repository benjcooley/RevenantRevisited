#!/usr/bin/env python3
"""Compare real port frames with explicitly mapped thin-reference PNGs.

No search, phase adjustment, translation, rescale, masking or brightness fitting.
Non-identical output is a diagnostic, not an automatic VFX acceptance.
"""
import argparse
import hashlib
import json
from pathlib import Path
from PIL import Image,ImageChops,ImageStat


def sha(path):return hashlib.sha256(path.read_bytes()).hexdigest()


def compare(port_manifest,mapping_path,output):
    port_manifest=port_manifest.resolve();mapping_path=mapping_path.resolve()
    capture=json.loads(port_manifest.read_text());mapping=json.loads(mapping_path.read_text())
    if capture['status']!='pass':raise ValueError('Modern capture did not pass its own completion/timing checks')
    cases=[]
    for pair in mapping['frames']:
        frame=capture['frames'][int(pair['port_index'])]
        port=Path(frame['path']);reference=(mapping_path.parent/pair['reference']).resolve()
        if sha(port)!=frame['sha256']:raise ValueError('Modern capture frame changed: '+str(port))
        if 'reference_sha256' not in pair or sha(reference)!=pair['reference_sha256']:
            raise ValueError('Reference frame hash missing or changed: '+str(reference))
        a=Image.open(reference).convert('RGB');b=Image.open(port).convert('RGB')
        if a.size!=b.size:raise ValueError('Different dimensions require a corrected fixture, not automatic resizing')
        ap=list(a.getdata());bp=list(b.getdata());diff=ImageChops.difference(a,b)
        threshold=int(mapping.get('active_threshold',16))
        selected=[i for i,(x,y) in enumerate(zip(ap,bp)) if max(x)>threshold or max(y)>threshold]
        def bbox(pixels):
            locations=[(i%a.width,i//a.width) for i,p in enumerate(pixels) if max(p)>threshold]
            if not locations:return None
            return [min(x for x,y in locations),min(y for x,y in locations),
                    max(x for x,y in locations)+1,max(y for x,y in locations)+1]
        cases.append(dict(port_index=pair['port_index'],reference=str(reference),reference_sha256=pair['reference_sha256'],
            port=str(port),port_sha256=frame['sha256'],size=list(a.size),
            mean_whole_roi_rgb_error=sum(ImageStat.Stat(diff).mean)/3,
            active_union_pixels=len(selected),active_threshold=threshold,
            mean_active_union_rgb_error=sum(abs(ap[i][c]-bp[i][c]) for i in selected for c in range(3))/(max(1,len(selected))*3),
            differing_pixels=sum(x!=y for x,y in zip(ap,bp)),
            reference_active_bbox=bbox(ap),port_active_bbox=bbox(bp)))
    if not cases:raise ValueError('At least one explicit frame pair is required')
    report=dict(status='pixels_identical' if all(c['differing_pixels']==0 for c in cases) else 'diagnostic_pixels_differ',
        comparison_scope='Real modern GPU output versus selected thin original software reference.',
        port_capture_manifest=str(port_manifest),port_capture_manifest_sha256=sha(port_manifest),
        modern_binary_sha256=capture['binary_sha256'],mapping=mapping,mapping_sha256=sha(mapping_path),
        fitted_position=False,fitted_scale=False,phase_search=False,brightness_fit=False,
        automatic_full_acceptance=False,cases=cases)
    output.parent.mkdir(parents=True,exist_ok=True);output.write_text(json.dumps(report,indent=2)+'\n')
    return report


if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--port',type=Path,required=True)
    p.add_argument('--mapping',type=Path,required=True);p.add_argument('--output',type=Path,required=True);args=p.parse_args()
    result=compare(args.port,args.mapping,args.output)
    print(json.dumps(dict(status=result['status'],frames=len(result['cases']),automatic_full_acceptance=False),indent=2))
