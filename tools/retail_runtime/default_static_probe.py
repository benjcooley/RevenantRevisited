#!/usr/bin/env python3
"""Pinned default animator root meshes: actual retail vs compiled current port."""
import argparse
import hashlib
import json
from pathlib import Path
import statistics
import struct
import time
import zipfile
from static_mesh_probe import ROOT,StaticFixture,build_port,production_spans,OWNERS
from software_probe import RETAIL_SHA,save_rgb565_png
from function_hook import file_offset

WIDTH=HEIGHT=512


def sha(data):return hashlib.sha256(data).hexdigest()


def parse_parts(data,profile):
    if sha(data)!=profile['asset_sha256']:raise ValueError('Selected shipped asset SHA changed')
    u=lambda a:struct.unpack_from('<I',data,a)[0];r=lambda a:a+u(a);body=20+u(16)
    if (u(body),u(body+4),u(body+12),u(body+20),u(body+36),u(body+44),u(body+52))!=(
            0xdc,3,profile['vertices'],profile['faces'],profile['textures'],profile['objects'],0):
        raise ValueError('Selected body topology or controller tags changed')
    if u(24)!=1 or data[28:60].split(b'\0')[0]!=b'STILL' or struct.unpack_from('<2h',data,68)!=(0x2001,profile['frames']):
        raise ValueError('Selected constant STILL state changed')
    vertices=r(r(r(body+16)));faces=r(body+24);objects=r(body+48);textures=r(body+40);parts=[]
    for index in range(profile['objects']):
        obj=objects+48*index;material,start,count,_=struct.unpack_from('<4H',data,obj+32)
        state=r(obj+44);parent,nkeys=struct.unpack_from('<2i',data,state);keys_offset=r(state+8)
        if parent!=-1 or nkeys!=9:raise ValueError('Only selected independent scalar root parts are supported')
        keys=list(struct.unpack_from('<9I',data,keys_offset))
        if [word&255 for word in keys]!=[8,12,16,20,24,28,32,36,40]:raise ValueError('Selected scalar key stream changed')
        runs=struct.unpack_from('<'+'H'*(2*(profile['textures']+1)),data,r(obj+40))
        active=[i for i in range(profile['textures']) if runs[2*(i+1)+1]]
        if active!=[material] or material!=index:raise ValueError('Selected one-material root-part mapping changed')
        face_start,face_count=runs[2*(material+1):2*(material+2)]
        vertex_bytes=data[vertices+32*start:vertices+32*(start+count)]
        face_bytes=data[faces+6*face_start:faces+6*(face_start+face_count)]
        indices=list(struct.unpack('<'+'H'*(3*face_count),face_bytes))
        if len(vertex_bytes)!=count*32 or not indices or max(indices)>=count:raise ValueError('Invalid complete local mesh')
        texture=textures+120*material;pixel=r(r(texture+108));height,width=struct.unpack_from('<2I',data,texture+8)
        pf=struct.unpack_from('<8I',data,texture+72)
        if u(texture+116)!=1 or pf[3]!=16:raise ValueError('Only single-frame original16bit textures are supported')
        if pf[4:8]==(0xf00,0xf0,0xf,0xf000):fmt='ARGB4444'
        elif pf[4:8]==(0xf800,0x7e0,0x1f,0):fmt='RGB565'
        else:raise ValueError('Original software texture format is unsupported')
        raw=data[pixel:pixel+width*height*2]
        if len(raw)!=width*height*2:raise ValueError('Truncated texture')
        parts.append(dict(object_index=index,object_name=data[obj:obj+32].split(b'\0')[0].decode(),
            frames=profile['frames'],vertices=[list(v)for v in struct.iter_unpack('<8f',vertex_bytes)],
            indices=indices,keys=keys,vertex_hex=vertex_bytes.hex(),face_hex=face_bytes.hex(),
            texture=raw,width=width,height=height,format=fmt,texture_sha256=sha(raw),
            vertices_offset=vertices+32*start,faces_offset=faces+6*face_start,keys_offset=keys_offset,texture_offset=pixel))
    if sum(len(p['vertices'])for p in parts)!=profile['vertices'] or sum(len(p['indices'])//3 for p in parts)!=profile['faces']:
        raise AssertionError('Entire selected asset must be inspected')
    return parts


def original_registry(vm,names):
    registrations=[]
    for section in vm.layout['sections']:
        if not section['executable']:continue
        code=vm.image[section['offset']:section['offset']+section['size']]
        for i in range(10,len(code)-5):
            if code[i]!=0xe8 or code[i-10]!=0x68 or code[i-5]!=0xb9:continue
            va=vm.base+section['rva']+i
            if va+5+struct.unpack_from('<i',code,i+1)[0]!=0x40db90:continue
            name=struct.unpack_from('<I',code,i-9)[0];builder=struct.unpack_from('<I',code,i-4)[0]
            offset=file_offset(vm.layout,name);text=vm.image[offset:vm.image.index(0,offset)].decode('ascii')
            if not text or len(text)>80:raise AssertionError('Recovered static registration name changed')
            vm.call(0x40db90,(name,),this=builder)
            registrations.append(dict(call=hex(va),name=text,name_address=hex(name),builder_address=hex(builder)))
    if len(registrations)!=92:raise AssertionError('Complete pinned retail registration set changed')
    results={}
    for name in (*names,'EFFECT','FLAME'):
        pointer=vm.allocate(len(name)+1);vm.write(pointer,name.encode()+b'\0')
        results[name]=hex(vm.call(0x40dca0,(pointer,)))
    if any(results[name]!='0x5e8508'for name in (*names,'EFFECT')) or results['FLAME']=='0x5e8508':
        raise AssertionError('Default animator dispatch or positive control changed')
    return dict(registrations=registrations,lookup=results,constructor='0x40db90',lookup_function='0x40dca0')


def pixels(fixture,draws):
    fixture.software.clear()
    for draw in draws:
        m=draw['metadata'];fixture.software.set_texture(m['width'],m['height'],m['texture'],format=m['format'])
        projected=fixture.software.project(draw['positions'],camera=(0,0,0),zdist=1925)
        if any(not(0<=p[0]<WIDTH and 0<=p[1]<HEIGHT)for p in projected):raise AssertionError('Unmodified geometry exceeds fixed viewport; defer rather than fit')
        fixture.software.draw([(*p,31,31,31,31,*uv)for p,uv in zip(projected,draw['uvs'])],
            draw['indices'],z_enabled=True,z_write=True,instruction_limit=5000000)
    return fixture.vm.surface_bytes('screen'),fixture.vm.surface_bytes('depth')


def main():
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('executable',type=Path)
    parser.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi')
    parser.add_argument('--profiles',type=Path,default=Path(__file__).with_name('default_static_profiles.json'))
    parser.add_argument('--output',type=Path,required=True);args=parser.parse_args();args.output.mkdir(parents=True,exist_ok=True)
    before={k:sha(v.encode())for k,v in production_spans().items()};profiles=json.loads(args.profiles.read_text())['profiles']
    metadata={};assets={}
    with zipfile.ZipFile(args.archive)as archive:
        members={n.lower():n for n in archive.namelist()}
        for profile in profiles:
            data=archive.read(members[('Imagery/'+profile['asset']).lower()]);parts=parse_parts(data,profile)
            assets[profile['name']]=parts
            for part in parts:metadata[profile['name']+'-'+str(part['object_index'])]=part
    port,compiled=build_port(args.output,metadata,frames=(0,50,99))
    fixture=StaticFixture(args.executable,WIDTH,HEIGHT,5000000)
    dispatch=original_registry(fixture.vm,[p['name']for p in profiles]);fixture.setup(next(iter(metadata.values())))
    cases=[];failures=[];times=[]
    for profile in profiles:
        name=profile['name'];frames=(0,50,99) if profile['frames']>1 else(0,)
        for frame in frames:
            for moved in (0,1):
                first=None
                for repeat in range(2):
                    started=time.perf_counter();fixture.software.restore();native_draws=[];modern_draws=[];pose=[]
                    for m in assets[name]:
                        fixture.setup(m,checkpoint=False);matrix,points=fixture.original(frame,moved)
                        packet=port[name+'-'+str(m['object_index'])]['cases'][(frame,moved)]
                        if port[name+'-'+str(m['object_index'])]['indices']!=m['indices']:raise AssertionError('Production local indices changed')
                        uvs=[v[6:]for v in m['vertices']];modern_points=[v[:3]for v in packet['vertices']];modern_uvs=[v[6:]for v in packet['vertices']]
                        position_error=max(abs(a-b)for p,q in zip(points,modern_points)for a,b in zip(p,q))
                        uv_error=max(abs(a-b)for p,q in zip(uvs,modern_uvs)for a,b in zip(p,q))
                        pose.append(dict(object=m['object_name'],object_index=m['object_index'],original_matrix=matrix,
                            port_matrix=packet['matrix'],original_positions=points,port_positions=modern_points,
                            max_position_error=position_error,max_uv_error=uv_error,texture_sha256=m['texture_sha256']))
                        native_draws.append(dict(metadata=m,positions=points,uvs=uvs,indices=m['indices']))
                        modern_draws.append(dict(metadata=m,positions=modern_points,uvs=modern_uvs,indices=m['indices']))
                    native,z=pixels(fixture,native_draws);modern,mz=pixels(fixture,modern_draws)
                    pair=[sha(native),sha(modern),sha(z),sha(mz)];times.append((time.perf_counter()-started)*1000)
                    if first is None:first=pair
                    elif pair!=first:raise AssertionError('Complete original/port image/depth replay changed')
                diff=sum(a!=b for a,b in zip(struct.unpack('<'+str(WIDTH*HEIGHT)+'H',native),struct.unpack('<'+str(WIDTH*HEIGHT)+'H',modern)))
                visible=sum(v!=0 for v in struct.unpack('<'+str(WIDTH*HEIGHT)+'H',native))
                errors=[]
                if max(p['max_position_error']for p in pose)>5e-5:errors.append('geometry')
                if max(p['max_uv_error']for p in pose)>1e-7:errors.append('uv')
                if diff or z!=mz:errors.append('pixels_or_depth')
                if not visible:errors.append('invisible')
                if errors:failures.append(dict(name=name,frame=frame,moved=moved,errors=errors))
                for label,data in [('retail',native),('port',modern)]:save_rgb565_png(args.output/f'{name}-{label}-{frame:03d}-{moved}.png',data,WIDTH,HEIGHT)
                cases.append(dict(name=name,type_id=profile['id'],asset=profile['asset'],asset_sha256=profile['asset_sha256'],
                    frame=frame,owner_position=list(OWNERS[moved]),poses=pose,differing_rgb565_pixels=diff,
                    depth_equal=z==mz,nonzero_retail_pixels=visible,image_hashes=pair,errors=errors))
        selected=[c for c in cases if c['name']==name]
        for moved in (0,1):
            if len({c['image_hashes'][0]for c in selected if c['owner_position']==list(OWNERS[moved])})!=1:
                raise AssertionError('Selected constant track changed across sampled frames')
        if selected[0]['image_hashes'][0]==selected[1]['image_hashes'][0]:
            raise AssertionError('Selected owner translation did not move visible pixels')
    after={k:sha(v.encode())for k,v in production_spans().items()}
    if after!=before:raise AssertionError('Production source changed during comparison')
    report=dict(status='fail'if failures else'pass',failures=failures,retail_sha256=RETAIL_SHA,
        profile_count=len(profiles),case_count=len(cases),replays_per_case=2,source_span_sha256=after,
        probe_sha256=sha(Path(__file__).read_bytes()),profiles_sha256=sha(args.profiles.read_bytes()),port_component=compiled,
        native_builder_dispatch=dispatch,median_warm_pair_ms=statistics.median(times),
        original_functions=['registry constructor0x40db90','builder lookup0x40dca0','GetAniKey0x409950',
            'key decoder0x409430','CalcObjectMatrix0x40a420','matrix multiply0x43aa90',
            'point transform0x43ad80','camera/view/projection0x56cdc0','software raster0x56d960'],
        scope='Complete selected default/no-tag constant asset root parts vs compiled production keys/matrix/extraction/static submission. Original software projection/raster shared, identity/translated owner, fixed512viewport/white/cullNONE/depthtestwrite. Part0 remapping/decodedasset/provider interfaces explicit; character utility selection, natural owner/map/illumination/device/Metal separate.',
        dosbox_used=False,full_game_integration=False,metal_backend_compared=False,cases=cases)
    (args.output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({k:v for k,v in report.items()if k not in('cases','native_builder_dispatch','port_component','source_span_sha256')},indent=2))
    if failures:raise SystemExit(1)


if __name__=='__main__':main()
