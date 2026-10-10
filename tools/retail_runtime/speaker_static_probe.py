#!/usr/bin/env python3
"""Exact shipped speaker generic mesh: native keys versus compiled port submission."""
import argparse
import json
import re
import statistics
import struct
import subprocess
import time
import zipfile
from pathlib import Path
from unicorn import UC_HOOK_CODE
from barrier_static_probe import BarrierFixture
from default_static_probe import original_registry
from static_mesh_probe import ROOT, build_port, production_spans, OWNERS
from software_probe import RETAIL_SHA, save_rgb565_png
from fizzle_probe import sha, span

PROFILE = dict(name='speaker', id='0xad92bc10', asset='Misc/speaker.i3d',
    asset_sha256='a0255572172177816cdaf3f81185ef7d398df7063faf0c5d5db33547e8f42949')
FRAMES = (0, 50, 99)
WIDTH = HEIGHT = 512


def parse_asset(data):
    if sha(data) != PROFILE['asset_sha256']:
        raise ValueError('Exact shipped speaker asset changed')
    u = lambda p: struct.unpack_from('<I', data, p)[0]
    r = lambda p: p + u(p)
    body = 20 + u(16)
    if tuple(u(body+i) for i in (0,4,12,20,28,36,44,52)) != (0xfc,2,55,30,1,0,1,0):
        raise ValueError('Exact speaker version2/nonmorph/no-texture/no-tag topology changed')
    if u(24) != 1 or data[28:60].split(b'\0')[0] != b'STILL' or struct.unpack_from('<2h',data,68) != (0x2001,100):
        raise ValueError('Exact speaker STILL100 header changed')
    obj = r(body+48); state = r(obj+44); keys = r(state+8)
    if struct.unpack_from('<4H',data,obj+32) != (0,0,55,0) or struct.unpack_from('<2i',data,state) != (-1,9):
        raise ValueError('Speaker independent full root mesh changed')
    words = list(struct.unpack_from('<9I',data,keys))
    if words != [8,12,275728,20,24,28,112672,112676,112680]:
        raise ValueError('Speaker constant authored TRS stream changed')
    if struct.unpack_from('<2H',data,r(obj+40)) != (0,30):
        raise ValueError('Speaker untextured bin lost faces')
    vertices = r(r(r(body+16))); faces = r(body+24); material = r(body+32)
    vb = data[vertices:vertices+55*32]; fb = data[faces:faces+30*6]; mb = data[material:material+80]
    indices = list(struct.unpack('<90H',fb))
    if max(indices) >= 55 or struct.unpack_from('<I',mb)[0] != 80:
        raise ValueError('Malformed complete speaker geometry/material')
    return dict(frames=100,keys=words,vertices=[list(v) for v in struct.iter_unpack('<8f',vb)],
        indices=indices,vertex_hex=vb.hex(),face_hex=fb.hex(),material=mb,material_hex=mb.hex(),
        diffuse=list(struct.unpack_from('<4f',mb,4)),emissive=list(struct.unpack_from('<4f',mb,52)),
        vertices_offset=vertices,faces_offset=faces,keys_offset=keys,material_offset=material,
        object_name=data[obj:obj+32].split(b'\0')[0].decode(),imagery_flags=0xfc)


def compile_material(output,m):
    method=span((ROOT/'src/d3dport.cpp').read_text(),'void LoadMaterial(',
                '// --------------------------------------------------------------------------\n// RenderObject')
    prelude=r'''#include <cstdio>
#include <cstring>
#include <fstream>
struct Color{float r,g,b,a;};
struct Material{unsigned size;Color diffuse,ambient,specular,emissive;float power;unsigned texture,ramp;};
struct S3DMat{Material matdesc;};struct S3DObj{int material=0;};
struct T3DImagery{S3DMat mat;int NumMaterials(){return 1;}
 void GetObject(int,S3DObj*o){o->material=0;}void GetMaterial(int,S3DMat*out){*out=mat;}};
'''
    tail=r'''
int main(int argc,char**argv){T3DImagery image;std::ifstream f(argv[1],std::ios::binary);
 f.read((char*)&image.mat.matdesc,80);if(!f)return 2;float diffuse[4],emissive[4];LoadMaterial(&image,0,diffuse,emissive);
 for(auto x:diffuse)printf("%a ",double(x));for(auto x:emissive)printf("%a ",double(x));puts("");}
'''
    generated=output/'speaker-material.cpp';generated.write_text(prelude+method+tail)
    binary=output/'speaker-material';cmd=['clang++','-std=c++17',str(generated),'-o',str(binary)]
    subprocess.run(cmd,check=True,capture_output=True,text=True)
    path=output/'speaker.material-input';path.write_bytes(m['material'])
    values=[float.fromhex(x) for x in subprocess.check_output([str(binary),str(path)],text=True).split()]
    if values != m['diffuse']+m['emissive'][:3]+[1.]:
        raise AssertionError('Compiled LoadMaterial changed authored binary32 channels')
    return values,dict(command=cmd,source_span_sha256=sha(method.encode()),
        generated_source_sha256=sha(generated.read_bytes()),binary_sha256=sha(binary.read_bytes()))


class SpeakerFixture(BarrierFixture):
    def setup(self,metadata,checkpoint=True):
        self.metadata = metadata
        self.vm.put_u32(self.context+0x1c,metadata['imagery_flags'])
        self.vm.put_u32(self.nkeys,len(metadata['keys']))
        self.vm.write(self.keys,struct.pack('<9I',*metadata['keys']))
        self.vm.put_u32(self.frame_count_value,metadata['frames'])
        if self.software.texture is not None or self.vm.u32(0x676050) != 0:
            raise AssertionError('Untextured speaker inherited a surface')
        if checkpoint:
            self.software.clear(); self.software.checkpoint()


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('executable',type=Path); p.add_argument('--output',type=Path,required=True)
    args = p.parse_args(); args.output.mkdir(parents=True,exist_ok=True)
    before = {k:sha(v.encode()) for k,v in production_spans().items()}
    with zipfile.ZipFile(ROOT/'data/imagery.rvi') as archive:
        names = {n.lower():n for n in archive.namelist()}
        registry = archive.read('class.def')
        binding = re.findall(r'"speaker"\s+"([^\"]+)"\s+(0x[0-9a-fA-F]+)',registry.decode('latin1'))
        if binding != [('Misc\\Speaker.I3D','0xad92bc10')]:
            raise ValueError('Exact shipped speaker CLASS binding changed')
        metadata = {'speaker':parse_asset(archive.read(names[('Imagery/'+PROFILE['asset']).lower()]))}
    port, compiled = build_port(args.output,metadata,frames=FRAMES,untextured=True)
    # The common fixture defaults to v3 flags0xdc. Speaker is v2 with HASPLAYSOUND;
    # supply its exact flags to the adapter, while retaining all production bodies.
    generated = args.output/'static-port-components.cpp'
    code = generated.read_text()
    if code.count('int flags=0xdc;') != 1:
        raise AssertionError('Common adapter changed; reassess exact speaker flags')
    generated.write_text(code.replace('int flags=0xdc;','int flags=0xfc;'))
    subprocess.run(compiled['command'],check=True,capture_output=True,text=True)
    # build_port decodes traces; rerun it with the exact generated flags binary.
    # Only HASPLAYSOUND differs: verify the entire textual trace remains identical.
    binary = args.output/'static-port-components'
    trace = subprocess.check_output([str(binary),str(args.output/'speaker.mesh-input')],text=True)
    if trace != (args.output/'speaker-port-trace.txt').read_text():
        raise AssertionError('Speaker HASPLAYSOUND unexpectedly changed mesh packets')
    compiled.update(generated_source_sha256=sha(generated.read_bytes()),binary_sha256=sha(binary.read_bytes()),
        imagery_flags=0xfc,adapter_flag_control='Full trace equals prior0xdc (HASPLAYSOUND only); exact0xfc binary used')
    colors, material_compiled = compile_material(args.output,metadata['speaker'])
    fixture = SpeakerFixture(args.executable,WIDTH,HEIGHT,5000000)
    dispatch = original_registry(fixture.vm,[])
    vm=fixture.vm; name=vm.allocate(8); vm.write(name,b'Speaker\0')
    builder=vm.call(0x40dca0,(name,))
    if builder!=0x66cdc0 or vm.u32(builder)!=0x5a359c or vm.u32(0x5a359c)!=0x40dc00:
        raise AssertionError('Explicit Speaker registration/generic factory changed')
    if not any(row['name']=='Speaker' and row['builder_address']=='0x66cdc0' for row in dispatch['registrations']):
        raise AssertionError('Speaker static constructor not in complete registration set')
    if bytes(vm.uc.mem_read(0x40dc64,6))!=bytes.fromhex('c7060c375a00'):
        raise AssertionError('Generic factory does not assign pinned T3DAnimator vtable')
    dispatch['lookup']['Speaker']=hex(builder)
    dispatch['speaker_factory']='0x40dc00'
    dispatch['speaker_animator_vtable']='0x5a370c'
    dispatch['admission']='Explicit Speaker registration creates generic T3DAnimator, not absent-name fallback'
    branch = {'no_texture':0,'texture':0}
    def observe(uc,address,size,user):
        branch['no_texture' if address==0x56d992 else 'texture'] += 1
    for address in (0x56d992,0x56dbce):
        fixture.vm.uc.hook_add(UC_HOOK_CODE,observe,begin=address,end=address)
    m = metadata['speaker']; fixture.setup(m); cases=[]; failures=[]; timings=[]
    for frame in FRAMES:
        for moved in (0,1):
            prior = None
            for repeat in range(2):
                start=time.perf_counter(); fixture.software.restore()
                matrix, points = fixture.original(frame,moved)
                packet = port['speaker']['cases'][(frame,moved)]
                cp=[v[:3] for v in packet['vertices']]; uv=[v[6:] for v in m['vertices']]; cu=[v[6:] for v in packet['vertices']]
                if port['speaker']['indices'] != m['indices']:
                    raise AssertionError('Compiled complete local indices changed')
                color,depth=fixture.raster(points,uv,m['indices'],m['diffuse'])
                modern,mdepth=fixture.raster(cp,cu,m['indices'],colors[:4])
                hashes=[sha(color),sha(modern),sha(depth),sha(mdepth)]
                if prior is not None and hashes != prior:
                    raise AssertionError('Full speaker image/depth warm replay changed')
                prior=hashes; timings.append((time.perf_counter()-start)*1000)
            visible=sum(v!=0 for v in struct.unpack('<262144H',color))
            diff=sum(a!=b for a,b in zip(struct.unpack('<262144H',color),struct.unpack('<262144H',modern)))
            position_error=max(abs(a-b) for p,q in zip(points,cp) for a,b in zip(p,q))
            uv_error=max(abs(a-b) for p,q in zip(uv,cu) for a,b in zip(p,q))
            if diff or depth != mdepth or not visible or position_error>5e-5 or uv_error>1e-7:
                failures.append(dict(frame=frame,moved=moved,pixels=diff,visible=visible,position=position_error,uv=uv_error))
            for label,image in (('retail',color),('port',modern)):
                save_rgb565_png(args.output/f'speaker-{label}-{frame}-{moved}.png',image,WIDTH,HEIGHT)
            cases.append(dict(name='speaker',id=PROFILE['id'],frame=frame,owner_position=list(OWNERS[moved]),
                original_matrix=matrix,port_matrix=packet['matrix'],native_positions=points,port_positions=cp,
                indices=m['indices'],authored_diffuse=m['diffuse'],compiled_diffuse=colors[:4],
                max_position_error=position_error,max_uv_error=uv_error,nonzero_pixels=visible,
                differing_rgb565_pixels=diff,depth_equal=depth==mdepth,image_hashes=hashes))
    for moved in (0,1):
        selected=[c for c in cases if c['owner_position']==list(OWNERS[moved])]
        if len({c['image_hashes'][0] for c in selected}) != 1:
            raise AssertionError('Constant speaker geometry changed at sampled frames')
    if cases[0]['image_hashes'][0] == cases[1]['image_hashes'][0]:
        raise AssertionError('Declared owner translation did not visibly move speaker')
    if not branch['no_texture'] or branch['texture']:
        raise AssertionError('Unexpected textured retail raster branch')
    after={k:sha(v.encode()) for k,v in production_spans().items()}
    if after != before: raise AssertionError('Production source changed during comparison')
    current_material=span((ROOT/'src/d3dport.cpp').read_text(),'void LoadMaterial(',
        '// --------------------------------------------------------------------------\n// RenderObject')
    if sha(current_material.encode()) != material_compiled['source_span_sha256']:
        raise AssertionError('Production material source changed during comparison')
    report=dict(status='fail' if failures else 'pass',failures=failures,profile=PROFILE,
        class_registry_sha256=sha(registry),class_binding=binding,retail_sha256=RETAIL_SHA,
        case_count=len(cases),replays_per_case=2,frames=list(FRAMES),native_builder_dispatch=dispatch,
        original_raster_branch_calls=branch,compiled_generic_submission=compiled,compiled_material=material_compiled,
        source_span_sha256=after,probe_sha256=sha(Path(__file__).read_bytes()),
        median_warm_pair_ms=statistics.median(timings),cases=cases,
        scope='Exact shipped speaker CLASS identity, version2 flags0xfc, entire constant STILL100 root mesh '
            '(55vertices/30faces), native explicit Speaker builder/generic animator/keys/matrix/point transform versus compiled '
            'production key decode/extraction/static submission/material. Original untextured Gouraud raster shared '
            'with fixed512viewport/camera0/zdist1925/cullNONE/depthtest-write. Authored material supplied directly '
            'as5bit vertex input; full original illumination, sound, natural caller/map runtime and GPU remain open.',
        native_illumination_producer=False,actual_map_runtime=False,full_effect_accepted=False,dosbox_used=False)
    (args.output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({k:v for k,v in report.items() if k not in ('cases','native_builder_dispatch','source_span_sha256')},indent=2))
    if failures: raise SystemExit(1)


if __name__=='__main__':main()
