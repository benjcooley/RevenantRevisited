#!/usr/bin/env python3
"""Original Flame x86 versus compiled port component, using original SW pixels.

The two effect frontends share a deliberately selected camera and original
software raster. This isolates VFX geometry/UV behavior; it is not Metal parity
or a complete map/game integration test. No DOSBox or Windows boot occurs.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import statistics
import struct
import subprocess
import time
import zipfile

from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_EIP, UC_X86_REG_ESP
from software_probe import SoftwareFixture, save_rgb565_png

ROOT=Path(__file__).resolve().parents[2]
RETAIL_SHA='28bec27387bf53a553da320dd4883d00ff5bcc5a5f3d2658cca8e8ed586372b5'
VERTEX_OFFSET=0x1dd4
INDEX_OFFSET=0x1e54
TEXTURE_OFFSET=0x2184
INDICES=(2,0,3,1,3,0)
PROFILES={
    'base':('0x50ba373b','Imagery/Magic/flame.i3d'),
    'blue':('0x50ba373c','Imagery/Magic/flameb.i3d'),
    'green':('0x50ba373d','Imagery/Magic/flameg.i3d'),
}


def digest(data):return hashlib.sha256(data).hexdigest()


def parse_asset(asset):
    """Read the old Flame body or the shipped relative-offset colored bodies."""
    u=lambda offset:struct.unpack_from('<I',asset,offset)[0]
    relative=lambda offset:offset+u(offset)
    body=20+u(16)
    if len(asset)==41348 and u(body)==0:
        vertices,indices,texture=VERTEX_OFFSET,INDEX_OFFSET,TEXTURE_OFFSET
        width,height=struct.unpack_from('<2I',asset,0x408)
    else:
        if tuple(u(body+i) for i in (0,4,12,20,28,36,44,52))!=(0xdc,3,4,2,1,1,1,0):
            raise ValueError('Unsupported colored Flame body/counts')
        vertices=relative(relative(relative(body+16)))
        indices=relative(body+24)
        desc=relative(body+40)
        height,width=struct.unpack_from('<2I',asset,desc+8)
        if struct.unpack_from('<8I',asset,desc+72)!=(32,0x41,0,16,0xf00,0xf0,0xf,0xf000):
            raise ValueError('Colored Flame texture is not authored ARGB4444')
        texture=relative(relative(desc+108))
    if (width,height)!=(128,128) or struct.unpack_from('<6H',asset,indices)!=INDICES:
        raise ValueError('Unsupported Flame atlas/topology')
    if vertices+4*32>len(asset) or texture+width*height*2>len(asset):
        raise ValueError('Flame asset offsets outside the body')
    return dict(vertices_offset=vertices,index_offset=indices,texture_offset=texture,
                width=width,height=height)


def extract_class(path,name,terminator):
    source=path.read_text();start=source.index('class '+name)
    end=source.index(terminator,start)
    return source[start:end].rstrip()


def production_spans(definitions=None):
    text=(definitions or (ROOT/'data/Resources/effects.def').read_bytes()).decode()
    start=text.index('effect TorchFlame')
    return dict(base=extract_class(ROOT/'src/effect.h','TFlipbookBillboardComponent','\nstruct SParticleBucketEffectDef'),
        flame=extract_class(ROOT/'src/effect.cpp','TFlameQuadComponent','\nDEFINE_BUILDER("FLAME"'),
        definition=text[start:text.index('\n    emitter billboard',start)],
        **{name:(ROOT/'src'/name).read_text() for name in ('math3d.cpp','math3d.h','particlefx.cpp','particlefx.h')})


def build_port(output,asset,definitions):
    """Capture exact current component classes with external engine stubs.

    The state clock, expression VM, authored transform and Submit implementation
    are production source. Asset provider, world matrix and renderer are explicit
    fixture boundaries. Generated source records the complete compiled bodies.
    """
    pieces=production_spans(definitions);base=pieces['base'];flame=pieces['flame']
    values={}
    block=pieces['definition']
    for name in ('frame_expr','uv_rect_expr'):
        values[name]=re.search(r'\b'+name+r'\s*=\s*"([^"]+)"',block)[1]
    scale=float(re.search(r'\bscale\s*=\s*([-0-9.]+)',block)[1])
    additive=re.search(r'\bblend\s*=\s*(\w+)',block)[1]!='alpha'
    records=struct.unpack_from('<32f',asset,parse_asset(asset)['vertices_offset'])
    initializer=','.join('{{'+','.join(format(v,'.9g')+'f' if '.' in format(v,'.9g') or 'e' in format(v,'.9g') else format(v,'.9g')+'.0f' for v in records[i:i+3])+'},{0,0,1},0,0}' for i in range(0,32,8))
    prelude=r'''
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <string>
#include <stdexcept>
#include "particlefx.h"
#include "math3d.h"
#undef min
#undef max
enum class EFxBlend:uint8_t {Alpha,Additive};
enum class EFxDepthMode:uint8_t {TestNoWrite};
enum class EFxDebugMode:uint8_t {Normal,SolidColor,FullTexture};
struct SQuadDrawItem {float world_pos[4][3]{};float uv[4][2]{};
    struct {TTextureHandle texture{};uint8_t blend{},depth_mode{};} key;
    EFxDebugMode debug_mode{};};
struct TRenderer {SQuadDrawItem last;void SubmitFxQuad(const SQuadDrawItem& i){last=i;}};
struct World {hmm_mat4 m;const hmm_mat4& Matrix()const{return m;}};
struct TObjectInstance {World world;const World& Transform()const{return world;}};
struct TObjectComponent {
    virtual const char* ComponentName()const{return "fixture";}
    virtual void OnAttach(){};virtual void OnDetach(){};
    void RegisterUpdate(void(TObjectComponent::*)()){};void UnregisterUpdate(void(TObjectComponent::*)()){};
    void Update(){OnUpdate();}protected:virtual void OnUpdate(){}
};
struct TTime {static float DeltaTime(){return 1.0f/24.0f;}};
void ParticleFatal(const std::string& error){throw std::runtime_error(error);}
struct T3DImagery {S3DVertex vertices[4];int NumObjVerts(int)const{return 4;}
    void GetObjVerts(int,S3DVertex* dest,int,int,ERender3DVertex){std::memcpy(dest,vertices,sizeof(vertices));}};
'''
    trailer='''
void TFlipbookBillboardComponent::Submit(TRenderer&,const TObjectInstance&)const{}
int main(){
    T3DImagery imagery{{%s}};
    TFlameQuadComponent component;component.Configure(1,128,128,4,2,8,25,62.5,%s,true);
    component.ConfigureAuthoredQuad(imagery,%.9g);
    std::string error;
    if(!component.SetFrameExpression(%s,&error)||!component.SetUvRectExpression(%s,&error))return 2;
    TObjectInstance owner;MtxClear(&owner.world.m);TRenderer renderer;
    for(int frame=0;frame<18;++frame){
        component.Submit(renderer,owner);
        std::printf("%%d",frame);
        for(int i=0;i<4;++i)std::printf(" %%.9g %%.9g %%.9g %%.9g %%.9g",
            renderer.last.world_pos[i][0],renderer.last.world_pos[i][1],renderer.last.world_pos[i][2],
            renderer.last.uv[i][0],renderer.last.uv[i][1]);
        std::puts("");component.Update();
    }
}
'''%(initializer,'true' if additive else 'false',scale,json.dumps(values['frame_expr']),json.dumps(values['uv_rect_expr']))
    generated=output/'flame-port-component.cpp'
    generated.write_text(prelude+'\n'+base+'\n'+flame+'\n'+trailer)
    binary=output/'flame-port-component'
    command=['clang++','-std=c++17','-I',str(ROOT/'thirdparty/handmademath'),
             '-iquote',str(ROOT/'src'),str(generated),str(ROOT/'src/math3d.cpp'),
             str(ROOT/'src/particlefx.cpp'),'-o',str(binary)]
    result=subprocess.run(command,capture_output=True,text=True)
    (output/'port-compile.log').write_text(result.stdout+result.stderr)
    if result.returncode:raise RuntimeError('Port component compile failed; see port-compile.log')
    rows=[[float(x) for x in line.split()] for line in subprocess.check_output([str(binary)],text=True).splitlines()]
    if len(rows)!=18 or any(len(row)!=21 for row in rows):raise AssertionError('Unexpected component trace')
    return rows,dict(command=command,generated_source_sha256=digest(generated.read_bytes()),
                     binary_sha256=digest(binary.read_bytes()),external_boundaries=
                     ['asset provider supplies shipped raw vertices','owner world matrix identity',
                      'renderer records actual SubmitFxQuad payload','time steps exactly one float 24Hz delta'])


class FlameFixture:
    def __init__(self,executable,asset,build=None):
        self.software=SoftwareFixture(executable,96,96,build=build)
        self.vm=self.software.vm
        self.asset=asset
        self.metadata=parse_asset(asset)
        self.points=[struct.unpack_from('<3f',asset,self.metadata['vertices_offset']+i*32) for i in range(4)]
        texture=self.metadata['texture_offset']
        self.software.set_texture(128,128,asset[texture:texture+32768],format='ARGB4444')
        self.animator=self.vm.allocate(0x200);self.obj=self.vm.allocate(0x200)
        self.lverts=self.vm.allocate(4*32);self.point=self.vm.allocate(12);self.result=self.vm.allocate(12)
        self.vm.put_u32(self.obj+0xa0,4);self.vm.put_u32(self.obj+0xa4,self.lverts)
        self.vm.put_u32(self.animator+8,self.vm.allocate(0x200))
        for i,xyz in enumerate(self.points):
            self.vm.write(self.lverts+i*32,struct.pack('<3f3I2f',*xyz,0,0xffffffff,0,0,0))
        self.calls={}
        self.boundaries={0x4178e0:0,0x417d60:8,0x417b00:0,0x40eef0:4,
                         0x40c960:0,0x40a8f0:24,0x40c9c0:4,0x40e2e0:0}
        for address in self.boundaries:
            self.vm.uc.hook_add(UC_HOOK_CODE,self.external,begin=address,end=address)
        self.software.clear();self.software.checkpoint()

    def external(self,uc,address,size,user):
        sp=uc.reg_read(UC_X86_REG_ESP)
        self.calls[hex(address)]=self.calls.get(hex(address),0)+1
        if address==0x40eef0:
            if self.vm.u32(sp+4)!=0:raise AssertionError('Flame requested different object')
            uc.reg_write(UC_X86_REG_EAX,self.obj)
        elif address==0x417d60:
            if (self.vm.u32(sp+4),self.vm.u32(sp+8))!=(2,1):
                raise AssertionError('Unexpected retail Flame blend request')
        elif address==0x40a8f0:
            if self.vm.u32(sp+4)!=self.obj:raise AssertionError('Wrong Flame submission')
        uc.reg_write(UC_X86_REG_EIP,self.vm.u32(sp))
        uc.reg_write(UC_X86_REG_ESP,sp+4+self.boundaries[address])

    def original_packet(self,frame):
        self.vm.put_u32(self.animator+0xfc,frame)
        self.vm.call(0x4e4f20,this=self.animator)
        matrix=struct.unpack('<16f',self.vm.uc.mem_read(self.obj+0x58,64))
        positions=[]
        for xyz in self.points:
            self.vm.write(self.point,struct.pack('<3f',*xyz))
            self.vm.call(0x43ad80,(self.obj+0x58,self.point,self.result))
            positions.append(struct.unpack('<3f',self.vm.uc.mem_read(self.result,12)))
        uvs=[struct.unpack('<2f',self.vm.uc.mem_read(self.lverts+i*32+24,8)) for i in range(4)]
        return matrix,positions,uvs

    def pixels(self,positions,uvs):
        self.software.clear()
        projected=self.software.project(positions,camera=(0,0,0),zdist=1925)
        vertices=[(*xyz,31,31,31,31,*uv) for xyz,uv in zip(projected,uvs)]
        color,depth=self.software.draw(vertices,INDICES,z_enabled=True,z_write=False)
        return color,depth,projected


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('executable',type=Path)
    parser.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi')
    parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--repeat',type=int,default=2)
    parser.add_argument('--build',type=Path,help='Explicit named build.json for a private fixed-layout variant')
    parser.add_argument('--profile',choices=PROFILES,default='base',help='Exact shipped Flame type and atlas')
    args=parser.parse_args()
    if args.repeat<2:parser.error('--repeat must be at least two')
    args.output.mkdir(parents=True,exist_ok=True)
    source_paths=[ROOT/p for p in ('src/effect.cpp','src/effect.h','src/math3d.cpp','src/math3d.h',
                   'src/particlefx.cpp','src/particlefx.h','data/Resources/effects.def')]
    before={str(p.relative_to(ROOT)):digest(p.read_bytes()) for p in source_paths}
    before_spans={name:digest(text.encode()) for name,text in production_spans().items()}
    type_id,asset_member=PROFILES[args.profile]
    with zipfile.ZipFile(args.archive) as archive:asset=archive.read(asset_member)
    metadata=parse_asset(asset)
    definitions=(ROOT/'data/Resources/effects.def').read_bytes()
    port_rows,compiled=build_port(args.output,asset,definitions)
    start=time.perf_counter();fixture=FlameFixture(args.executable,asset,build=args.build)
    setup_ms=(time.perf_counter()-start)*1000
    cases=[];timings=[];errors=[]
    for frame in range(18):
        first=None
        for repeat in range(args.repeat):
            start=time.perf_counter();fixture.software.restore();fixture.calls={}
            matrix,positions,uvs=fixture.original_packet(frame)
            retail,retail_depth,projected=fixture.pixels(positions,uvs)
            port_vertices=[port_rows[frame][1+i*5:4+i*5] for i in range(4)]
            port_uvs=[port_rows[frame][4+i*5:6+i*5] for i in range(4)]
            port,port_depth,port_projected=fixture.pixels(port_vertices,port_uvs)
            timings.append((time.perf_counter()-start)*1000)
            current=(digest(retail),digest(port),digest(retail_depth),digest(port_depth))
            if first is None:first=current
            elif first!=current:raise AssertionError('Flame pixels/depth failed exact replay')
            if repeat==0:
                save_rgb565_png(args.output/f'retail-{frame:02d}.png',retail,96,96)
                save_rgb565_png(args.output/f'port-{frame:02d}.png',port,96,96)
                raw_diff=sum(a!=b for a,b in zip(struct.unpack('<9216H',retail),struct.unpack('<9216H',port)))
                differences=[abs(a-b) for p,q in zip(positions,port_vertices) for a,b in zip(p,q)]
                uv_errors=[abs(a-b) for p,q in zip(uvs,port_uvs) for a,b in zip(p,q)]
                if max(differences)>2e-5 or max(uv_errors)>1e-7:
                    errors.append(dict(frame=frame,max_position_error=max(differences),max_uv_error=max(uv_errors)))
                cases.append(dict(frame=frame,original_matrix=list(matrix),original_vertices=positions,
                    port_vertices=port_vertices,original_uvs=uvs,port_uvs=port_uvs,
                    max_position_error=max(differences),max_uv_error=max(uv_errors),
                    original_projected=projected,port_projected=port_projected,
                    differing_rgb565_pixels=raw_diff,retail_rgb565_sha256=current[0],
                    port_rgb565_sha256=current[1],retail_depth_sha256=current[2],
                    port_depth_sha256=current[3],external_call_counts=dict(fixture.calls),
                    virtual_logs={name:dict(sha256=digest(bytes(data)),bytes=len(data),
                        text=bytes(data[:65536]).decode('cp1252'),truncated=len(data)>65536)
                        for name,data in fixture.vm.files.items() if name.endswith('.log')},
                    nonzero_retail_pixels=sum(p!=0 for p in struct.unpack('<9216H',retail))))
    after={str(p.relative_to(ROOT)):digest(p.read_bytes()) for p in source_paths}
    after_spans={name:digest(text.encode()) for name,text in production_spans().items()}
    if before_spans!=after_spans:raise AssertionError('Compiled Flame production bodies changed during A/B')
    # Verify the original local 18-frame Animate counter independently of the
    # chosen Render frames. No host transcription performs the increment/wrap.
    animation_sequences=[]
    for _ in range(args.repeat):
        fixture.software.restore();sequence=[]
        for tick in range(36):
            fixture.vm.call(0x4e4ef0,this=fixture.animator)
            frame=fixture.vm.u32(fixture.animator+0xfc)
            if frame!=(tick+1)%18:raise AssertionError('Original Flame Animate counter mismatch')
            sequence.append(frame)
        animation_sequences.append(sequence)
    if any(seq!=animation_sequences[0] for seq in animation_sequences):
        raise AssertionError('Original Flame Animate failed warm replay')
    report=dict(status='pass' if not errors else 'fail',errors=errors,
        frontend_state_status='pass' if not errors else 'fail',
        shared_original_raster_pixel_status='pass' if all(c['differing_rgb565_pixels']==0 for c in cases) else 'pixels_differ',
        matching_pixel_frames=sum(c['differing_rgb565_pixels']==0 for c in cases),
        distinct_original_pixel_frames=len({c['retail_rgb565_sha256'] for c in cases}),
        fixture_source_sha256=digest(Path(__file__).read_bytes()),
        original_software_adapter_sha256=digest(Path(__file__).with_name('software_probe.py').read_bytes()),
        retail_sha256=fixture.software.build['executable_sha256'],baseline_sha256=RETAIL_SHA,
        build=fixture.software.build,type_id=type_id,asset_member=asset_member,
        asset_sha256=digest(asset),asset_bytes=len(asset),vertex_offset=hex(metadata['vertices_offset']),
        index_offset=hex(metadata['index_offset']),
        texture_offset=hex(metadata['texture_offset']),texture_sha256=digest(asset[metadata['texture_offset']:metadata['texture_offset']+32768]),
        source_sha256=after,source_span_sha256=after_spans,
        source_contract='Exact compiled production bodies/helpers/TorchFlame definition; whole-file hashes are provenance, not unrelated-edit invalidation.',
        port_component=compiled,case_count=18,replays_per_case=args.repeat,
        setup_ms=setup_ms,median_warm_pair_ms=statistics.median(timings),
        p95_warm_pair_ms=sorted(timings)[min(len(timings)-1,int(len(timings)*.95))],
        original_render_function='0x4e4f20',original_raster_function='0x56d960',
        original_animate_function='0x4e4ef0',animate_sequence=animation_sequences[0],
        camera=dict(position=[0,0,0],zdist=1925,viewport=[96,96],owner_world='identity'),
        guest_os_boots=0,dosbox_used=False,pixel_rendering_intercepted=False,
        full_game_integration=False,metal_backend_compared=False,ledger_acceptance_changed=False,
        known_renderer_limits=['Original ARGB4444/RGB565 quantization, alpha and edge coverage are retained.',
            'Both frontends use the same original raster; Metal/new-renderer parity is a separate gate.'],
        external_boundaries=['base animator hierarchy refresh skipped in isolated identity-owner fixture',
            'imagery provider returns authored box01 prototype','extents tracking skipped',
            'Scene blend save/set/restore intercepted, expected original request2/1 asserted',
            'imagery submission captured; original projection and textured raster then execute'],
        cases=cases,scope='Actual retail Flame matrix/UV instructions versus exact compiled port component bodies. '
            'Shipped mesh/ARGB4444 atlas and original camera/projection/software raster produce both image sets. '
            'This is a bounded frontend/render fixture, not a comparison against the full Revisited Metal renderer '
            'or natural map context. Pixel mismatches from bounded float tolerances remain visible in report.')
    (args.output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({k:v for k,v in report.items() if k not in ('cases','errors')},indent=2))
    if errors:raise SystemExit(1)


if __name__=='__main__':main()
