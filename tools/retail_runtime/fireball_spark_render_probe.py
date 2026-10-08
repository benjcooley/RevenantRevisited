"""Authored FireBall spark raw ABS packets and bridge-relative software images.

Native50b2b0 executes its real blend Save/Set/Restore on the software path.
Raw native matrices/vertices are retained. Common-world Zx1.46 is the existing
REV_FIX_Z_VALUE adapter boundary, not an independent native device pixel proof.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct
import subprocess
import time
import zipfile
from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EIP
from fireball_spark_probe import SparkFixture,compile_prefix
from fireball_trail_probe import position_tolerance
from fireball_head_probe import ROOT,ASSET_SHA,port_driver,particle_card
from software_probe import save_rgb565_png

SPARK_VERTICES=0x2f0;SPARK_INDICES=0xf7c;SPARK_TEXTURE=0x21518

def float32(x):return struct.unpack('<f',struct.pack('<f',x))[0]


class SparkRenderFixture(SparkFixture):
    def __init__(self,executable,asset,spark_vertices=SPARK_VERTICES,spark_texture=SPARK_TEXTURE,**asset_offsets):
        super().__init__(executable,asset,**asset_offsets);self.spark_raw=[]
        self.spark_texture=spark_texture
        self.spark_object=self.vm.allocate(0x200)
        self.vm.put_u32(self.animator+0x2d4,self.spark_object)
        self.spark_vertices=[struct.unpack_from('<8f',asset,spark_vertices+32*i)for i in range(4)]
        self.vm.put_u32(0x5d7a28,1) # Actual software Scene3D branch, no COM blend proxy.
        self.surface.checkpoint()

    def boundary(self,uc,address,size,user):
        if address==0x40a8f0:
            sp=uc.reg_read(UC_X86_REG_ESP)
            if self.vm.u32(sp+4)==getattr(self,'spark_object',None):
                self.spark_raw.append(dict(matrix=bytes(self.vm.uc.mem_read(self.spark_object+0x58,64)).hex(),
                    flags=self.vm.u32(self.spark_object),scale=list(struct.unpack('<3f',self.vm.uc.mem_read(self.spark_object+0x40,12)))))
                uc.reg_write(UC_X86_REG_EIP,self.vm.u32(sp));uc.reg_write(UC_X86_REG_ESP,sp+28);return
        super().boundary(uc,address,size,user)

    def spark_packets(self):
        self.spark_raw=[];self.vm.call(0x50b2b0,this=self.animator+0x248);cards=[]
        for raw in self.spark_raw:
            if raw['flags']!=0x400100:raise AssertionError('Expected native MATRIX|ABSPOS')
            self.vm.write(self.combined_matrix,bytes.fromhex(raw['matrix']));native=[]
            for vertex in self.spark_vertices:
                self.vm.write(self.point,struct.pack('<3f',*vertex[:3]));self.vm.call(0x43ad80,(self.combined_matrix,self.point,self.result))
                native.append(list(struct.unpack('<3f',self.vm.uc.mem_read(self.result,12))))
            cards.append(dict(positions=[[x,y,float32(z*float32(1.46))]for x,y,z in native],
                native_d3d_positions=native,native_matrix=raw['matrix'],flags=raw['flags'],scale=raw['scale'],
                uvs=[list(v[6:8])for v in self.spark_vertices],count=4,texture=2))
        return cards

    def spark_pixels(self,cards,asset):
        self.surface.set_texture(64,64,asset[self.spark_texture:self.spark_texture+8192],format='ARGB4444')
        return self.pixels(cards,instruction_limit=5000000)


def compile_raw(output):
    source=output/'port-native-spark.cpp'
    source.write_text(r'''#include <cstdio>
#include "fireballquad.h"
#include "render3d_types.h"
int main(int argc,char**argv){if(argc!=3)return 2;S3DVertex vertices[8];FILE*v=std::fopen(argv[1],"rb");if(!v)return 3;
 if(std::fread(vertices,1,256,v)!=256)return 4;std::fclose(v);
 std::array<hmm_vec3,4> authored{};std::array<std::array<float,2>,4> uv{};
 for(int i=0;i<4;++i){authored[i]=vertices[i+4].pos;uv[i]={vertices[i+4].tu,vertices[i+4].tv};}
 FILE*f=std::fopen(argv[2],"rb");if(!f)return 5;int count=0;std::printf("[");
 for(int slot=0;slot<40;++slot){float r[8];unsigned used,flicker;if(std::fread(r,1,32,f)!=32||std::fread(&used,1,4,f)!=4||std::fread(&flicker,1,4,f)!=4)return 6;
  if(!used)continue;const hmm_vec3 position{r[0],r[1],r[2]};const auto q=fireball_quad::BuildSparkNativeD3D(authored,uv,r[6]*(flicker?1.75f:1.f),position);
  if(count++)std::printf(",");std::printf("[ ");for(int i=0;i<4;++i){if(i)std::printf(",");std::printf("[%.9g,%.9g,%.9g]",q.position[i].X,q.position[i].Y,q.position[i].Z);}std::printf("]");
 }std::fclose(f);std::puts("]");}
''')
    binary=output/'port-native-spark'
    command=['clang++','-std=c++17','-I',str(ROOT/'thirdparty/handmademath'),'-iquote',str(ROOT/'src'),str(source),str(ROOT/'src/fireballquad.cpp'),str(ROOT/'src/math3d.cpp'),'-o',str(binary)]
    result=subprocess.run(command,text=True,capture_output=True);(output/'raw-compile.log').write_text(result.stdout+result.stderr)
    if result.returncode:raise RuntimeError('Raw native geometry compilation failed')
    return binary,dict(command=command,source_sha256=hashlib.sha256(source.read_bytes()).hexdigest(),binary_sha256=hashlib.sha256(binary.read_bytes()).hexdigest())


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path);p.add_argument('--output',type=Path,required=True)
    p.add_argument('--producer',type=Path);p.add_argument('--producer-manifest',type=Path)
    args=p.parse_args();args.output.mkdir(parents=True,exist_ok=True)
    with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:asset=z.read('Imagery/Magic/newfireball.i3d')
    if hashlib.sha256(asset).hexdigest()!=ASSET_SHA:raise ValueError('Wrong authored asset')
    if struct.unpack_from('<6H',asset,SPARK_INDICES)!=(2,0,3,1,3,0):raise ValueError('Unexpected spark topology')
    if args.producer:
        if not args.producer_manifest:p.error('--producer requires manifest')
        old=json.loads(args.producer_manifest.read_text());compiled=dict(old['compiled_render']);binary=args.producer.resolve()
        if hashlib.sha256(binary.read_bytes()).hexdigest()!=compiled['binary_sha256']:raise ValueError('Retained geometry producer changed')
        compiled['retained_manifest']=str(args.producer_manifest.resolve())
        vertices=args.output/'authored-quads.bin'
    else:binary,vertices,compiled=port_driver(args.output,asset)
    vertices.write_bytes(asset[0x270:0x370])
    prefix,core_compiled=compile_prefix(args.output)
    raw_binary,raw_compiled=compile_raw(args.output)
    sourcefiles=['src/effect.cpp','src/effect.h','src/fireballquad.h','src/fireballquad.cpp','src/fireballspark.h']
    hashes={f:hashlib.sha256((ROOT/f).read_bytes()).hexdigest()for f in sourcefiles}
    fixture=SparkRenderFixture(args.executable,asset);fixture.missile.configure([0,0,128],[1000,0,128],target_hp=0,animator_present=True)
    states=[];motion=[]
    for tick in range(1,97):
        motion.append(fixture.missile.step(1)[-1]);fixture.advance_original_prefix();states.append(fixture.state())
    inputs=args.output/'motion.txt';inputs.write_text(''.join(' '.join(map(str,[m['state'],*m['position']]))+'\n'for m in motion))
    generated=json.loads(subprocess.check_output([str(prefix),str(inputs),'1'],text=True))
    if any(a['seed']!=b['seed']or a['rng_calls']!=b['rng_calls']for a,b in zip(states,generated)):raise AssertionError('Verified production state adapter regressed')
    fixture.surface.restore();fixture.missile.configure([0,0,128],[1000,0,128],target_hp=0,animator_present=True)
    selected={1,2,5,10,20,37,50,60,61,62,65,69,70,72,80,81,96};cases=[];errors=0;pixels=0;checks=0;raw_checks=0;raw_errors=0;started=time.perf_counter()
    for tick in range(1,97):
        row=fixture.missile.step(1)[-1];fixture.advance_original_prefix()
        if row!=motion[tick-1]or fixture.state()!=states[tick-1]:raise AssertionError('Original moving spark state replay differs')
        if tick not in selected:continue
        native=fixture.spark_packets();state=generated[tick-1];h=state['head']
        trail=args.output/'trail.bin';trail.write_bytes(b''.join(struct.pack('<4fiff',*s['position'],s['scale'],s['rotation'],s['frame'],s['glow'])for s in state['trail']))
        spark=args.output/'sparks.bin';spark.write_bytes(b''.join(struct.pack('<7fiII',*s['position'],*s['velocity'],s['scale'],s['life'],s['used'],s['flicker_status'])for s in state['sparks']))
        packet=json.loads(subprocess.check_output([str(binary),str(vertices),*map(str,row['position']),'0',str(row['angle']),
            str(h['frame']),str(h['rotation']),str(h['scale']),str(h['glow']),str(trail),str(spark)],text=True))
        port_raw=json.loads(subprocess.check_output([str(raw_binary),str(vertices),str(spark)],text=True))
        if len(native)!=len(port_raw):raw_errors+=1
        for a,b in zip(native,port_raw):
            for x,y in zip(a['native_d3d_positions'],b):
                for u,v in zip(x,y):
                    raw_checks+=1
                    if abs(u-v)>position_tolerance(u):raw_errors+=1
        port=[dict(particle_card(v),texture=v['texture'])for v in packet['particles']if v['texture']==2]+[v for v in packet['quads']if v['texture']==2]
        if len(native)!=len(port):errors+=1
        max_delta=0
        for a,b in zip(native,port):
            for field in ('positions','uvs'):
                for x,y in zip(a[field],b[field]):
                    for u,v in zip(x,y):
                        checks+=1;max_delta=max(max_delta,abs(u-v))
                        if abs(u-v)>(position_tolerance(u)if field=='positions'else 1e-7):errors+=1
        original=fixture.spark_pixels(native,asset);candidate=fixture.spark_pixels(port,asset);count=960*540
        different=sum(x!=y for x,y in zip(struct.unpack('<'+'H'*count,original),struct.unpack('<'+'H'*count,candidate)));pixels+=different
        save_rgb565_png(args.output/f'retail-{tick:03d}.png',original,960,540);save_rgb565_png(args.output/f'port-{tick:03d}.png',candidate,960,540)
        cases.append(dict(tick=tick,motion=row,native_cards=native,port_cards=port,port_native_d3d_positions=port_raw,max_packet_error=max_delta,
            original_live_count=sum(v['used']for v in states[tick-1]['sparks']),different_pixels=different,
            original_visible_pixels=sum(v!=0 for v in struct.unpack('<'+'H'*count,original)),
            retail_sha256=hashlib.sha256(original).hexdigest(),port_sha256=hashlib.sha256(candidate).hexdigest()))
    # Repeat original matrices and bridge-relative pixels from the same warm
    # baseline. No game boot or process launch is involved in this reset.
    fixture.surface.restore();fixture.missile.configure([0,0,128],[1000,0,128],target_hp=0,animator_present=True)
    retained={c['tick']:c for c in cases};warm_images=0
    for tick in range(1,97):
        row=fixture.missile.step(1)[-1];fixture.advance_original_prefix()
        if row!=motion[tick-1]or fixture.state()!=states[tick-1]:raise AssertionError('Warm original state changed')
        if tick not in retained:continue
        cards=fixture.spark_packets();prior=retained[tick]
        if cards!=prior['native_cards']:raise AssertionError('Warm raw native spark packet changed')
        if hashlib.sha256(fixture.spark_pixels(cards,asset)).hexdigest()!=prior['retail_sha256']:raise AssertionError('Warm bridge-relative spark image changed')
        warm_images+=1
    if hashes!={f:hashlib.sha256((ROOT/f).read_bytes()).hexdigest()for f in sourcefiles}:raise AssertionError('Source changed during rendering proof')
    report=dict(status='pass'if not errors and not pixels and not raw_errors else'differences_found',geometry_uv_checks=checks,geometry_errors=errors,pixel_differences=pixels,
        native_d3d_checks=raw_checks,native_d3d_errors=raw_errors,exact_original_packet_pixel_warm_replays=warm_images,cases=cases,compiled_render=compiled,compiled_state=core_compiled,compiled_native_geometry=raw_compiled,source_sha256=hashes,elapsed_seconds=time.perf_counter()-started,
        asset_sha256=ASSET_SHA,retail_binary_sha256=hashlib.sha256(args.executable.read_bytes()).hexdigest(),
        authored_offsets=dict(vertices=hex(SPARK_VERTICES),indices=hex(SPARK_INDICES),texture=hex(SPARK_TEXTURE)),texture=dict(width=64,height=64,format='ARGB4444',diffuse=[1,1,1,1],emissive=[0,0,0,1]),
        original_functions=['0x50b2b0','0x4de1f0','0x4de210','0x4de200','0x43ad80','0x56d960'],viewport=[960,540],camera=[240,0,128],
        float_position_bound='max(3e-5 absolute,8binary32ULPs), established authored-transform bound; raw D3D stage checked independently before bridge',
        bridge_sources=['src/3dimage.cpp:RenderObject OBJ3D_ABSPOS bypasses owner matrix','src/object.h:REV_FIX_Z_VALUE maps D3D Z back to map coordinates (x1.46)','src/3dimage.cpp:GetObjectMapPos applies REV_FIX_Z_VALUE to complete transformed D3D point','src/renderer.cpp:SubmitFxQuad forwards each world_pos corner to common-world FX projection','src/shaders/fx.glsl.h:world-space isometric/perspective projection'],
        coordinate_bridge='Original MATRIX|ABSPOS bypasses owner. Retained raw native D3D points convert to common-world Z via existing object.h REV_FIX_Z_VALUE (x1.46), matching existing absolute effect adapters and common-world quad renderer. Bridge-relative software images are not an independent original device parity proof.',
        scope='Actual original shared spark render transforms and shipped box02 topology/UV/64x64ARGB texture against compiled production Submit. Candidate state comes from compiled verified prefix, original owner motion through impact and pool drain. Explicit unlit/white material environment and common-world bridge; no growth/burst/ring/damage/complete FireBall/Metal acceptance.')
    (args.output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items()if k!='cases'},indent=2))
    if errors or pixels or raw_errors:raise SystemExit(1)


if __name__=='__main__':main()
