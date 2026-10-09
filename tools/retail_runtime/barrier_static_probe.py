"""Shipped LabGate barriers: authored zero-texture geometry and material inputs.

Original full illumination and actual map ownership remain outside this fixture.
No texture substitution, camera fit, engine patch or graphics-device claim.
"""
import argparse
import json
import struct
import subprocess
import time
import zipfile
from pathlib import Path
from unicorn import UC_HOOK_CODE
from default_static_probe import original_registry
from static_mesh_probe import ROOT,StaticFixture,build_port,production_spans,OWNERS
from software_probe import RETAIL_SHA,save_rgb565_png
from fizzle_probe import sha,span

PROFILES = (
    dict(name='LabGateBarrierS',id='0xad92bc37',asset='Misc/IBarrier1.I3D',
         asset_sha256='aee3ba4debae7894ae093a8047e03491b7e2b2eb22275a85ad0048fca354fb91'),
    dict(name='LabGateBarrierE',id='0xad92bc38',asset='Misc/IBarrier2.I3D',
         asset_sha256='782a2fc0a2f0ffe3fe0811df78dcd39752603ced7df6213ec5bdceb1b8439718'))
WIDTH=HEIGHT=512


def parse_asset(data,profile):
    if sha(data)!=profile['asset_sha256']:raise ValueError('Exact barrier asset changed')
    u=lambda a:struct.unpack_from('<I',data,a)[0]
    r=lambda a:a+u(a)
    body=20+u(16)
    if tuple(u(body+i)for i in (0,4,12,20,28,36,44,52))!=(0xdc,3,4,2,1,0,1,0):
        raise ValueError('Barrier topology/material/zero-texture/no-tag header changed')
    if u(24)!=1 or data[28:60].split(b'\0')[0]!=b'STILL' or struct.unpack_from('<2h',data,68)!=(0x2001,1):
        raise ValueError('Barrier constant STILL state changed')
    obj=r(body+48);state=r(obj+44);keys=r(state+8)
    if struct.unpack_from('<4H',data,obj+32)!=(0,0,4,0) or struct.unpack_from('<2i',data,state)!=(-1,6):
        raise ValueError('Expected complete independent root mesh')
    words=list(struct.unpack_from('<6I',data,keys))
    if [w&255 for w in words]!=[8,12,16,20,24,28]:raise ValueError('Constant scalar key stream changed')
    if struct.unpack_from('<2H',data,r(obj+40))!=(0,2):raise ValueError('Untextured bin must retain both faces')
    vertices=r(r(r(body+16)));faces=r(body+24);material=r(body+32)
    vb=data[vertices:vertices+128];fb=data[faces:faces+12];mb=data[material:material+80]
    indices=list(struct.unpack('<6H',fb))
    if max(indices)>=4 or len(set(indices))!=4:raise ValueError('Invalid complete local faces')
    if struct.unpack_from('<I',mb)[0]!=80:raise ValueError('Original material layout changed')
    diffuse=list(struct.unpack_from('<4f',mb,4));emissive=list(struct.unpack_from('<4f',mb,52))
    if diffuse!=[1.,0.,0.,1.] or emissive!=[0.,0.,0.,1.]:raise ValueError('Actual red diffuse/zero emissive changed')
    return dict(frames=1,keys=words,vertices=[list(v)for v in struct.iter_unpack('<8f',vb)],
        indices=indices,vertex_hex=vb.hex(),face_hex=fb.hex(),material=mb,
        material_hex=mb.hex(),diffuse=diffuse,emissive=emissive,
        vertices_offset=vertices,faces_offset=faces,keys_offset=keys,material_offset=material,
        object_name=data[obj:obj+32].split(b'\0')[0].decode())


def material_port(output,metadata):
    source=(ROOT/'src/d3dport.cpp').read_text()
    method=span(source,'void LoadMaterial(','// --------------------------------------------------------------------------\n// RenderObject')
    prelude=r'''
#include <cstdio>
#include <cstring>
#include <fstream>
struct Color{float r,g,b,a;};
struct Material{unsigned size;Color diffuse,ambient,specular,emissive;float power;unsigned texture,ramp;};
struct S3DMat{Material matdesc;};struct S3DObj{int material=0;};
struct T3DImagery{S3DMat mat;int NumMaterials(){return 1;}
 void GetObject(int,S3DObj*o){o->material=0;}void GetMaterial(int,S3DMat*m){*m=mat;}};
'''
    tail=r'''
int main(int argc,char**argv){T3DImagery image;std::ifstream f(argv[1],std::ios::binary);
 f.read((char*)&image.mat.matdesc,80);if(!f)return 2;float diffuse[4],emissive[4];LoadMaterial(&image,0,diffuse,emissive);
 for(auto x:diffuse)printf("%.9g ",x);for(auto x:emissive)printf("%.9g ",x);puts("");}
'''
    generated=output/'barrier-material.cpp';generated.write_text(prelude+method+tail)
    binary=output/'barrier-material';cmd=['clang++','-std=c++17',str(generated),'-o',str(binary)]
    p=subprocess.run(cmd,text=True,capture_output=True);(output/'material-compile.log').write_text(p.stdout+p.stderr)
    if p.returncode:raise RuntimeError('Material compilation failed')
    decoded={}
    for name,m in metadata.items():
        path=output/(name+'.material-input');path.write_bytes(m['material'])
        values=list(map(float,subprocess.check_output([str(binary),str(path)],text=True).split()))
        if values!=m['diffuse']+m['emissive'][:3]+[1.]:raise AssertionError('Actual LoadMaterial changed authored color')
        decoded[name]=values
    return decoded,dict(command=cmd,source_span_sha256=sha(method.encode()),
        generated_source_sha256=sha(generated.read_bytes()),binary_sha256=sha(binary.read_bytes()))


class BarrierFixture(StaticFixture):
    def setup(self,metadata,checkpoint=True):
        self.metadata=metadata
        self.vm.put_u32(self.nkeys,len(metadata['keys']))
        self.vm.write(self.keys,struct.pack('<6I',*metadata['keys']))
        self.vm.put_u32(self.frame_count_value,1)
        if self.software.texture is not None or self.vm.u32(0x676050)!=0:
            raise AssertionError('A barrier must not inherit any bound texture')
        if checkpoint:self.software.clear();self.software.checkpoint()

    def raster(self,points,uvs,indices,diffuse):
        self.software.clear()
        projected=self.software.project(points,camera=(0,0,0),zdist=1925)
        if any(not(0<=p[0]<WIDTH and 0<=p[1]<HEIGHT)for p in projected):
            raise AssertionError('Geometry outside declared viewport: do not fit camera')
        rgba=tuple(int(c*31)for c in diffuse)
        self.software.draw([(*p,*rgba,*uv)for p,uv in zip(projected,uvs)],indices,
            z_enabled=True,z_write=True,instruction_limit=5000000)
        if self.software.com_trace or self.vm.u32(0x676050)!=0:
            raise AssertionError('Original untextured branch used a surface')
        return self.vm.surface_bytes('screen'),self.vm.surface_bytes('depth')


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('executable',type=Path);parser.add_argument('--output',type=Path,required=True)
    args=parser.parse_args();args.output.mkdir(parents=True,exist_ok=True)
    before={k:sha(v.encode())for k,v in production_spans().items()};metadata={}
    with zipfile.ZipFile(ROOT/'data/imagery.rvi')as archive:
        names={n.lower():n for n in archive.namelist()}
        for p in PROFILES:metadata[p['name']]=parse_asset(archive.read(names[('Imagery/'+p['asset']).lower()]),p)
    port,compiled=build_port(args.output,metadata,frames=(0,),untextured=True)
    decoded,material_compiled=material_port(args.output,metadata)
    fixture=BarrierFixture(args.executable,WIDTH,HEIGHT,5000000)
    dispatch=original_registry(fixture.vm,[p['name']for p in PROFILES])
    branch={'no_texture':0,'texture':0}
    def observe(uc,address,size,user):branch['no_texture'if address==0x56d992 else'texture']+=1
    for address in(0x56d992,0x56dbce):fixture.vm.uc.hook_add(UC_HOOK_CODE,observe,begin=address,end=address)
    cases=[];errors=[];times=[]
    for profile in PROFILES:
        name=profile['name'];m=metadata[name];fixture.setup(m)
        for moved in(0,1):
            prior=None
            for repeat in range(2):
                started=time.perf_counter();fixture.software.restore();matrix,points=fixture.original(0,moved)
                candidate=port[name]['cases'][(0,moved)];cp=[v[:3]for v in candidate['vertices']]
                uv=[v[6:]for v in m['vertices']];cu=[v[6:]for v in candidate['vertices']]
                if port[name]['indices']!=m['indices']:raise AssertionError('Untextured fallback lost faces')
                color,depth=fixture.raster(points,uv,m['indices'],m['diffuse'])
                modern,mdepth=fixture.raster(cp,cu,port[name]['indices'],decoded[name][:4])
                hashes=[sha(color),sha(modern),sha(depth),sha(mdepth)]
                if prior is not None and prior!=hashes:raise AssertionError('Warm image/depth replay changed')
                prior=hashes;times.append((time.perf_counter()-started)*1000)
            pixels=struct.unpack('<262144H',color);different=sum(a!=b for a,b in zip(color,modern));visible=sum(p!=0 for p in pixels)
            position_error=max(abs(a-b)for p,q in zip(points,cp)for a,b in zip(p,q))
            uv_error=max(abs(a-b)for p,q in zip(uv,cu)for a,b in zip(p,q))
            if different or depth!=mdepth or not visible or position_error>5e-5 or uv_error>1e-7:
                errors.append(dict(name=name,moved=moved,pixels=different,visible=visible,position=position_error,uv=uv_error))
            blue,bdepth=fixture.raster(points,uv,m['indices'],[0.,0.,1.,1.])
            mutation_differences=sum(a!=b for a,b in zip(struct.unpack('<262144H',color),struct.unpack('<262144H',blue)))
            if mutation_differences!=visible or bdepth!=depth:raise AssertionError('Material control was ineffective')
            for label,data in(('retail',color),('port',modern),('blue-control',blue)):
                save_rgb565_png(args.output/f'{name}-{label}-{moved}.png',data,WIDTH,HEIGHT)
            cases.append(dict(name=name,id=profile['id'],asset=profile['asset'],asset_sha256=profile['asset_sha256'],
                owner_position=list(OWNERS[moved]),original_matrix=matrix,port_matrix=candidate['matrix'],
                max_position_error=position_error,max_uv_error=uv_error,indices=m['indices'],
                authored_diffuse=m['diffuse'],compiled_diffuse=decoded[name][:4],image_hashes=hashes,
                nonzero_pixels=visible,color_different_bytes=different,depth_equal=depth==mdepth,
                material_control_changed_pixels=mutation_differences,material_control_depth_equal=bdepth==depth))
        if cases[-1]['image_hashes'][0]==cases[-2]['image_hashes'][0]:raise AssertionError('Owner did not visibly move mesh')
    after={k:sha(v.encode())for k,v in production_spans().items()}
    if after!=before:raise AssertionError('Source changed during comparison')
    report=dict(status='fail'if errors else'pass',errors=errors,retail_sha256=RETAIL_SHA,
        case_count=len(cases),replays_per_case=2,native_builder_dispatch=dispatch,
        original_raster_branch_calls=branch,compiled_generic_submission=compiled,compiled_material=material_compiled,
        source_span_sha256=after,probe_sha256=sha(Path(__file__).read_bytes()),
        original_functions=['lookup40dca0','CalcObjectMatrix40a420','GetAniKey409950','decode409430',
            'point43ad80','projection56cdc0','raster56d960/untextured56d992'],
        median_warm_pair_ms=sorted(times)[len(times)//2],cases=cases,
        scope='Exact stationary no-tag/STILL1 zero-texture roots; compiled production extraction/static preview SubmitMesh and LoadMaterial; '
            'fixed authored red diffuse submitted as5bit vertex input to original no-texture Gouraud raster. '
            'Full original material/illumination vertex producer, natural visibility/caller, map runtime and GPU remain open.',
        dummy_texture_used=False,native_illumination_producer=False,actual_map_runtime=False,
        full_effect_accepted=False,dosbox_used=False)
    if branch['texture']or not branch['no_texture']:raise AssertionError('Wrong original raster branch')
    (args.output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({k:v for k,v in report.items()if k not in('cases','native_builder_dispatch','compiled_generic_submission','source_span_sha256')},indent=2))
    if errors:raise SystemExit(1)


if __name__=='__main__':main()
