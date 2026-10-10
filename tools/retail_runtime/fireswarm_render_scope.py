"""Summarize visible FireSwarm reference samples without crediting empty matches.

Reads only the filter-zero RGB PNGs emitted by software_probe.py. Rendering,
state generation and original instructions remain in fireswarm_probe.py.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct
import zlib


def visible_pixels(path):
    data=Path(path).read_bytes()
    if data[:8]!=b'\x89PNG\r\n\x1a\n':raise ValueError('Not a generated PNG')
    offset=8;compressed=[];width=height=None
    while offset<len(data):
        size=struct.unpack_from('>I',data,offset)[0];kind=data[offset+4:offset+8];chunk=data[offset+8:offset+8+size]
        if kind==b'IHDR':
            width,height,bits,color,compression,filtering,interlace=struct.unpack('>2I5B',chunk)
            if(bits,color,compression,filtering,interlace)!=(8,2,0,0,0):raise ValueError('Only generated RGB8 PNGs supported')
        elif kind==b'IDAT':compressed.append(chunk)
        elif kind==b'IEND':break
        offset+=size+12
    rows=zlib.decompress(b''.join(compressed));stride=width*3+1
    if len(rows)!=stride*height:raise ValueError('PNG row extent mismatch')
    count=0
    for y in range(height):
        row=rows[y*stride:(y+1)*stride]
        if row[0]:raise ValueError('Only generated filter-zero rows supported')
        count+=sum(row[x:x+3]!=b'\0\0\0'for x in range(1,stride,3))
    return count


def classify(report,counts):
    visible=[];empty=[];failed=[]
    for case in report['cases']:
        if case.get('image_hashes')is None:continue
        tick=case['tick'];hashes=case['image_hashes'];count=counts[tick]
        matched=not case['state_errors']and case['max_corner_error']<=5e-5 and case['differing_rgb565_pixels']==0 and hashes[0]==hashes[1]and hashes[2]==hashes[3]
        if not matched:failed.append(tick)
        elif count>0:visible.append(tick)
        else:empty.append(tick)
    return dict(visible_frontend_ticks=visible,empty_matching_ticks=empty,failed_sample_ticks=failed,
        early_nonempty_appearance_verified=False,complete_effect_accepted=False,independent_device_or_metal_parity=False,
        classification='stationary expanding/shrinking tube mesh, not a missile or point-to-point effect')


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('report',type=Path);p.add_argument('--output',type=Path,required=True);args=p.parse_args()
    report=json.loads(args.report.read_text());counts={t:visible_pixels(args.report.parent/f'retail-{t:03d}.png')for t in report['sampled_pixel_ticks']}
    result=classify(report,counts)
    result.update(report=str(args.report.resolve()),report_sha256=hashlib.sha256(args.report.read_bytes()).hexdigest(),visible_pixel_counts=counts,
        original_functions=['Initialize0x4f0130','Animate0x4f0160','Render0x4f0270'],native_scale_yaw_float32_checks=3*report['case_count'],
        state_ticks=report['case_count'],pixel_depth_pairs=len(report['sampled_pixel_ticks']),complete_warm_replays=report['replays_per_case'],
        original_software_limits=report['original_software_limits'],provider_scope='Identity owner/null spell, white vertices, explicit current-frontend culling provider; original software projection/raster. Device culling, normals/lighting, early visible appearance and modernGPU remain open.')
    args.output.write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result,indent=2))
    if report['status']!='pass'or result['failed_sample_ticks']or not result['visible_frontend_ticks']:raise SystemExit(1)


if __name__=='__main__':main()
