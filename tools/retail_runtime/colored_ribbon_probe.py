#!/usr/bin/env python3
"""Seven generic static Ribbon colors: both parts, poses and original SW pixels."""
import argparse
import json
from pathlib import Path
import statistics
import struct
import time
import zipfile
from static_mesh_probe import StaticFixture,build_port,production_spans,ROOT,WIDTH,HEIGHT,PIXELS,OWNERS
from software_probe import save_rgb565_png,RETAIL_SHA
from fizzle_probe import sha


def parse_parts(data,profile):
    if sha(data)!=profile['asset_sha256']:raise ValueError('Wrong colored Ribbon asset SHA')
    u=lambda a:struct.unpack_from('<I',data,a)[0];relative=lambda a:a+u(a);body=20+u(16)
    if(u(body),u(body+4),u(body+44),u(body+36),u(body+52))!=(0xdc,3,2,2,0):raise ValueError('Ribbon body topology/controller tags changed')
    if u(body+12)!=186 or u(body+20)!=204:raise ValueError('Wrong Ribbon vertex/face counts')
    vertices=relative(relative(relative(body+16)));faces=relative(body+24);objects=relative(body+48);textures=relative(body+40)
    parts=[]
    for object_index in range(2):
        obj=objects+object_index*48;material,start,count,_=struct.unpack_from('<4H',data,obj+32)
        state=relative(obj+44);parent,nkeys=struct.unpack_from('<2i',data,state);keys_offset=relative(state+8)
        if parent!=-1 or nkeys not in(6,9):raise ValueError('Ribbon static hierarchy/key pattern changed')
        keys=list(struct.unpack_from('<'+'I'*nkeys,data,keys_offset))
        if [word&255 for word in keys]!=([8,12,16,20,24,28] if nkeys==6 else [8,12,16,20,24,28,32,36,40]):raise ValueError('Unexpected Ribbon scalar keys')
        runs=struct.unpack_from('<6H',data,relative(obj+40))
        texture_index=object_index;face_start,face_count=runs[(texture_index+1)*2:(texture_index+2)*2]
        if face_count!=(12 if object_index==0 else 192):raise ValueError('Ribbon texture-face run changed')
        desc=textures+texture_index*120;frames=relative(desc+108);bits=relative(frames)
        height,width=struct.unpack_from('<2I',data,desc+8);pf=struct.unpack_from('<8I',data,desc+72)
        if(width,height,pf[1],pf[3],pf[4],pf[5],pf[6],pf[7])!=(128,128,0x41,16,0xf00,0xf0,0xf,0xf000):raise ValueError('Unexpected Ribbon texture format')
        vertex_bytes=data[vertices+start*32:vertices+(start+count)*32]
        face_bytes=data[faces+face_start*6:faces+(face_start+face_count)*6]
        indices=list(struct.unpack('<'+'H'*(face_count*3),face_bytes))
        if max(indices)>=count:raise ValueError('Ribbon object-local face index out of bounds')
        parts.append(dict(object_index=object_index,object_name=data[obj:obj+32].split(b'\0')[0].decode(),material=material,
            vertices=[list(x)for x in struct.iter_unpack('<8f',vertex_bytes)],indices=indices,keys=keys,
            vertex_hex=vertex_bytes.hex(),face_hex=face_bytes.hex(),texture=data[bits:bits+32768],width=width,height=height,
            format='ARGB4444',frames=1,vertices_offset=vertices+start*32,faces_offset=faces+face_start*6,keys_offset=keys_offset,texture_offset=bits))
    return parts


def pixels(fixture,draws):
    fixture.software.clear()
    for draw in draws:
        m=draw['metadata'];fixture.software.set_texture(m['width'],m['height'],m['texture'],format=m['format'])
        projected=fixture.software.project(draw['positions'],camera=(0,0,0),zdist=1925)
        if any(not(0<=p[0]<WIDTH and 0<=p[1]<HEIGHT)for p in projected):raise AssertionError('Ribbon viewport clips geometry')
        fixture.software.draw([(*p,31,31,31,31,*uv)for p,uv in zip(projected,draw['uvs'])],draw['indices'],z_enabled=True,z_write=True)
    return fixture.vm.surface_bytes('screen'),fixture.vm.surface_bytes('depth')


def main():
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('executable',type=Path)
    parser.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--profiles',type=Path,default=Path(__file__).with_name('ribbon_profiles.json'));parser.add_argument('--repeat',type=int,default=2);args=parser.parse_args()
    if args.repeat<2:parser.error('At least two replays required')
    args.output.mkdir(parents=True,exist_ok=True);profiles=json.loads(args.profiles.read_text())['profiles'];metadata={};assets={}
    before={k:sha(v.encode())for k,v in production_spans().items()}
    with zipfile.ZipFile(args.archive)as archive:
        names={n.lower():n for n in archive.namelist()}
        for profile in profiles:
            member='Imagery/'+profile['asset'].replace('\\','/');data=archive.read(names[member.lower()]);parts=parse_parts(data,profile)
            assets[profile['name']]=parts
            for part in parts:metadata[profile['name']+'-part'+str(part['object_index'])]=part
    port,compiled=build_port(args.output,metadata,frames=(0,));fixture=StaticFixture(args.executable)
    # Capture one immutable baseline; per-part input rebinds do not checkpoint
    # or clear the output between the two original object submissions.
    fixture.setup(next(iter(metadata.values())))
    cases=[];errors=[];times=[];color_hashes={}
    for profile in profiles:
        name=profile['name'];parts=assets[name];position_hashes=[]
        for moved in (0,1):
            first=None
            for repeat in range(args.repeat):
                start=time.perf_counter();fixture.software.restore();native_draws=[];port_draws=[];part_errors=[]
                for part in parts:
                    key=name+'-part'+str(part['object_index']);fixture.setup(part,checkpoint=False)
                    matrix,points=fixture.original(0,moved);packet=port[key]['cases'][(0,moved)]
                    if port[key]['indices']!=part['indices']:raise AssertionError('Production Ribbon extraction changed indices')
                    uvs=[v[6:8]for v in part['vertices']];modern_points=[v[:3]for v in packet['vertices']];modern_uvs=[v[6:8]for v in packet['vertices']]
                    position_error=max(abs(a-b)for p,q in zip(points,modern_points)for a,b in zip(p,q))
                    uv_error=max(abs(a-b)for p,q in zip(uvs,modern_uvs)for a,b in zip(p,q))
                    native_draws.append(dict(metadata=part,positions=points,uvs=uvs,indices=part['indices']))
                    port_draws.append(dict(metadata=part,positions=modern_points,uvs=modern_uvs,indices=port[key]['indices']))
                    part_errors.append(dict(object_index=part['object_index'],name=part['object_name'],original_matrix=matrix,
                        production_submit_matrix=packet['matrix'],max_position_error=position_error,max_uv_error=uv_error,
                        vertex_count=len(points),triangle_count=len(part['indices'])//3,vertex_offset=hex(part['vertices_offset']),
                        faces_offset=hex(part['faces_offset']),keys_offset=hex(part['keys_offset']),texture_offset=hex(part['texture_offset']),
                        original_vertices=points,port_vertices=modern_points,original_uvs=uvs,port_uvs=modern_uvs,indices=part['indices']))
                native,z=pixels(fixture,native_draws);modern,mz=pixels(fixture,port_draws);times.append((time.perf_counter()-start)*1000)
                pair=(sha(native),sha(modern),sha(z),sha(mz))
                if first is None:first=pair
                elif first!=pair:raise AssertionError('Colored Ribbon warm pixels/depth changed')
                if repeat:continue
                diff=sum(a!=b for a,b in zip(struct.unpack('<'+str(PIXELS)+'H',native),struct.unpack('<'+str(PIXELS)+'H',modern)))
                visible=sum(p!=0 for p in struct.unpack('<'+str(PIXELS)+'H',native))
                max_position=max(p['max_position_error']for p in part_errors);max_uv=max(p['max_uv_error']for p in part_errors)
                if diff or not visible or max_position>3e-5 or max_uv>1e-7:errors.append(dict(name=name,moved=moved,differing_rgb565_pixels=diff,max_position_error=max_position,max_uv_error=max_uv,visible_pixels=visible))
                save_rgb565_png(args.output/f'{name}-retail-P{moved}.png',native,WIDTH,HEIGHT);save_rgb565_png(args.output/f'{name}-port-P{moved}.png',modern,WIDTH,HEIGHT)
                cases.append(dict(name=name,type_id=profile['id'],asset=profile['asset'],asset_sha256=profile['asset_sha256'],frame=0,
                    owner_position=list(OWNERS[moved]),parts=part_errors,image_hashes=pair,differing_rgb565_pixels=diff,nonzero_retail_pixels=visible))
            position_hashes.append(first[0])
            if not moved:color_hashes[name]=first[0]
        if position_hashes[0]==position_hashes[1]:raise AssertionError('Ribbon owner translation did not move visible pixels')
    if len(set(color_hashes.values()))!=7:raise AssertionError('Seven colored Ribbon assets did not render seven distinct outputs')
    after={k:sha(v.encode())for k,v in production_spans().items()}
    if before!=after:raise AssertionError('Production generic mesh spans changed during Ribbon comparison')
    report=dict(status='pass'if not errors else'fail',failures=errors,retail_sha256=RETAIL_SHA,source_span_sha256=after,
        probe_sha256=sha(Path(__file__).read_bytes()),profiles_sha256=sha(args.profiles.read_bytes()),port_component=compiled,
        profile_count=7,case_count=len(cases),replays_per_case=args.repeat,median_warm_pair_ms=statistics.median(times),
        p95_warm_pair_ms=sorted(times)[min(len(times)-1,int(len(times)*.95))],distinct_color_outputs=len(set(color_hashes.values())),
        original_functions=['GetAniKey409950','decoder409430','CalcObjectMatrix40a420','matrix/point43aa90/43ad80','software raster56d960'],
        scope='Seven distinct generic static Ribbon types, not base Ribbon revive/combat controller. Exact two root subobjects, texture slots, 186vertices/204faces, constant1frame TRS tracks; each part remapped to object0 at the explicit decoded-asset provider boundary because both parents are -1. Originalkey/matrix/projection/raster vscompiledproductiongenerickey/matrix/extraction/SubmitMesh, composited in original asset part order. Naturalmap owner creation/light/cull/floor andmodernGPU remainseparate.',
        camera=dict(position=[0,0,0],zdist=1925,viewport=[WIDTH,HEIGHT],lighting='explicitwhite31pervertex',cull_mode='NONE',depth='test/write'),
        guest_os_boots=0,dosbox_used=False,pixel_rendering_intercepted=False,full_game_integration=False,metal_backend_compared=False,cases=cases)
    (args.output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items()if k!='cases'},indent=2))
    if errors:raise SystemExit(1)


if __name__=='__main__':main()
