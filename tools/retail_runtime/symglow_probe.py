#!/usr/bin/env python3
"""Original SymGlow breathing/vertex UV stores versus its production producer."""
import argparse
import json
import statistics
import struct
import subprocess
import time
import zipfile
from pathlib import Path

from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_EIP, UC_X86_REG_ESP
from software_probe import SoftwareFixture, save_rgb565_png, RETAIL_SHA
from fountain_probe import FountainFixture
from fizzle_probe import span, sha, f32

ROOT = Path(__file__).resolve().parents[2]
ASSET_SHA = '3265c339d6e44f4be2d1c319c7e3a7d10cf7be3a22243c78b457d3925dbe6ea7'
WIDTH, HEIGHT = 256, 384


def parse_asset(data):
    if sha(data) != ASSET_SHA:
        raise ValueError('Unexpected shipped SymGlow asset')
    u = lambda offset: struct.unpack_from('<I', data, offset)[0]
    relative = lambda offset: offset + u(offset)
    if (u(104), u(108), u(368), u(0x3fc), u(0x7c0)) != (0, 66, 92, 1, 1):
        raise ValueError('Unexpected old SymGlow body/counts')
    vertices, faces, texture = relative(relative(112)), relative(372), relative(relative(0x760))
    if struct.unpack_from('<8I', data, 0x448) != (32, 0x41, 0, 16, 0xf00, 0xf0, 0xf, 0xf000):
        raise ValueError('SymGlow texture format changed')
    return dict(vertices=list(struct.iter_unpack('<8f', data[vertices:vertices+66*32])),
                indices=list(struct.unpack_from('<276H', data, faces)),
                texture=data[texture:texture+8192], offsets=[vertices, faces, texture])


def production_spans():
    source = (ROOT/'src/effect.cpp').read_text()
    header = (ROOT/'src/effect.h').read_text()
    return dict(types=span(header, 'inline constexpr double  kSymGlowBespokeSimTickMs', '// ----- M07 TPhotonEffect (Bespoke)'),
                initialize=span(source, '    // SetupObjects changes V once;', '    if (attach_runtime_component) {'),
                advance_submit=span(source, 'void TSymGlowEffect_Bespoke::Advance(', 'void TSymGlowEffect_Bespoke::TickAndSubmitForTest_BESPOKE('),
                math=(ROOT/'src/math3d.cpp').read_text(), math_header=(ROOT/'src/math3d.h').read_text())


def build_port(output, mesh, rng_path, ticks, render_stride=1):
    pieces = production_spans()
    prelude = r'''
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <fstream>
#include <stdexcept>
#include "particlefx.h"
#include "math3d.h"
#undef min
#undef max
#define _CLASSDEF(name)
enum class EFxBlend:uint8_t{Alpha};enum class EFxDepthMode:uint8_t{TestNoWrite};enum class EFxDebugMode:uint8_t{Normal};
struct SMeshVertex{float pos[3]{},normal[3]{},uv[2]{};};
struct SQuadDrawItem{bool retail_argb4444=false;int corner_count=4,retail_texture=0;float world_pos[4][3]{},uv[4][2]{};
 struct{TTextureHandle texture{};uint8_t blend{},depth_mode{};}key;EFxDebugMode debug_mode{};};
struct TRenderer{bool UsesRetailSoftwareMeshLighting()const{return true;}std::vector<SQuadDrawItem>draws;void SubmitFxQuad(const SQuadDrawItem&i){draws.push_back(i);}};
TRenderer renderer;TRenderer*Renderer=&renderer;
struct TObjectImagery{};struct SObjectDef{};
struct World{hmm_mat4 m;World(){MtxClear(&m);}const hmm_mat4&Matrix()const{return m;}};
struct TEffect{World world;TEffect(TObjectImagery*){};TEffect(SObjectDef*,TObjectImagery*){};virtual~TEffect()=default;
 virtual void OffScreen(){};void KillThisEffect(){};void SetCommandDone(bool){};const World&Transform()const{return world;}};
std::ifstream inputs;int rng_count=0;
int random(int low,int high){int a,b,v;if(!(inputs>>a>>b>>v)||a!=low||b!=high)throw std::runtime_error("Native RNG order changed");++rng_count;return v;}
'''
    def lit(v):
        text = format(v, '.9g')
        return text + ('f' if '.' in text or 'e' in text else '.0f')
    vertices = ','.join('{{'+','.join(lit(x) for x in v[:3])+'},{'+','.join(lit(x) for x in v[3:6])+'},{'+','.join(lit(x) for x in v[6:])+'}}' for v in mesh['vertices'])
    initialize = 'void TSymGlowEffect_Bespoke::Initialize(bool) {struct{TTextureHandle htexture=1;}tex;\n'+pieces['initialize']+'}\n'
    trailer = r'''
int main(int argc,char**argv){inputs.open(argv[1]);TSymGlowEffect_Bespoke effect(nullptr);
 SMeshVertex vertices[]={VERTICES};uint16_t indices[]={INDICES};
 effect.retail_argb4444_=true;effect.vertices_.assign(vertices,vertices+66);effect.indices_.assign(indices,indices+276);effect.Initialize(false);
 for(int tick=0;tick<=TICKS;++tick){if(tick)effect.Advance(1.0/24.0);renderer.draws.clear();if(!(tick%STRIDE))effect.Submit(EFxDebugMode::Normal);
 printf("S %d %d %.9g %.9g %.9g %d\n",tick,effect.timer_,effect.zscale_,effect.dz_,effect.u_step_,rng_count);
 for(const auto&v:effect.vertices_)printf("V %d %.9g %.9g\n",tick,v.uv[0],v.uv[1]);
 for(const auto&d:renderer.draws){printf("D %d",tick);for(int k=0;k<3;++k)printf(" %.9g %.9g %.9g %.9g %.9g",d.world_pos[k][0],d.world_pos[k][1],d.world_pos[k][2],d.uv[k][0],d.uv[k][1]);puts("");}
 }int a,b,c;if(inputs>>a>>b>>c)throw std::runtime_error("Unconsumed RNG");
}
'''.replace('VERTICES', vertices).replace('INDICES', ','.join(map(str, mesh['indices']))).replace('TICKS', str(ticks)).replace('STRIDE', str(render_stride))
    source = output/'symglow-port-components.cpp'
    source.write_text(prelude+pieces['types'].replace('  private:', '  public:')+initialize+pieces['advance_submit']+trailer)
    binary = output/'symglow-port-components'
    command = ['clang++', '-std=c++17', '-I', str(ROOT/'thirdparty/handmademath'), '-iquote', str(ROOT/'src'), str(source), str(ROOT/'src/math3d.cpp'), '-o', str(binary)]
    result = subprocess.run(command, capture_output=True, text=True)
    (output/'port-compile.log').write_text(result.stdout+result.stderr)
    if result.returncode:
        raise RuntimeError('SymGlow compile failed; see port-compile.log')
    result = subprocess.run([str(binary), str(rng_path)], capture_output=True, text=True)
    (output/'port-trace.txt').write_text(result.stdout)
    if result.returncode:
        raise RuntimeError(result.stderr)
    frames = {}
    for line in result.stdout.splitlines():
        fields = line.split(); tick = int(fields[1])
        if fields[0] == 'S':
            frames[tick] = dict(timer=int(fields[2]), zscale=float(fields[3]), dz=float(fields[4]), step=float(fields[5]), rng_count=int(fields[6]), uvs=[], draws=[])
        elif fields[0] == 'V':
            frames[tick]['uvs'].append(list(map(float, fields[2:])))
        else:
            values = list(map(float, fields[2:]))
            frames[tick]['draws'].append(dict(positions=[values[i*5:i*5+3] for i in range(3)], uvs=[values[i*5+3:i*5+5] for i in range(3)]))
    return frames, dict(command=command, source_sha256=sha(source.read_bytes()), binary_sha256=sha(binary.read_bytes()))


class SymGlowFixture:
    observe_random = FountainFixture.observe_random

    def __init__(self, executable, mesh):
        self.software = SoftwareFixture(executable, WIDTH, HEIGHT)
        self.vm = self.software.vm; self.mesh = mesh
        self.software.set_texture(64, 64, mesh['texture'], format='ARGB4444')
        self.animator = self.vm.allocate(0x200); self.obj = self.vm.allocate(0x200); self.imagery = self.vm.allocate(0x200)
        self.vertices = self.vm.allocate(66*32); self.point = self.vm.allocate(12); self.result = self.vm.allocate(12)
        self.vm.put_u32(self.animator, 0x5a9e50); self.vm.put_u32(self.animator+8, self.imagery)
        self.vm.put_u32(self.obj+0xa0, 66); self.vm.put_u32(self.obj+0xa4, self.vertices)
        for i, v in enumerate(mesh['vertices']):
            self.vm.write(self.vertices+i*32, struct.pack('<3f3I2f', *v[:3], 0, 0xffffffff, 0, *v[6:]))
        self.boundaries = {0x408f70:0, 0x409ca0:8, 0x40a0c0:8, 0x40ed40:4,
                           0x40e2e0:0, 0x40eef0:4, 0x4178e0:0, 0x417d60:8, 0x417b00:0,
                           0x40c960:0, 0x40a8f0:24, 0x40c9c0:4}
        for address in self.boundaries:
            self.vm.uc.hook_add(UC_HOOK_CODE, self.external, begin=address, end=address)
        for address in (0x483300, 0x48332c):
            self.vm.uc.hook_add(UC_HOOK_CODE, self.observe_random, begin=address, end=address)
        self.random = []; self.random_pending = []
        for address,value in ((0x5d7a28,1),(0x66818c,0),(0x5e91c0,0),
            (0x5e8740,0),(0x5c61ac,1),(0x5e8790,0)):
            self.vm.put_u32(address,value)
        self.software.clear(); self.software.checkpoint()

    def external(self, uc, address, size, user):
        sp = uc.reg_read(UC_X86_REG_ESP)
        if address == 0x408f70:
            uc.reg_write(UC_X86_REG_EAX, 1)
        elif address in (0x409ca0, 0x40eef0):
            if self.vm.u32(sp+4) != 0:
                raise AssertionError('SymGlow requested a different object')
            uc.reg_write(UC_X86_REG_EAX, self.obj)
        elif address == 0x417d60:
            if (self.vm.u32(sp+4), self.vm.u32(sp+8)) != (2, 1):
                raise AssertionError('SymGlow blend request changed')
            return  # Execute actual Scene.SetBlendMode and native software states.
        elif address == 0x40a8f0:
            if self.vm.u32(sp+4) != self.obj or self.vm.u32(self.obj) != 0x2040:
                raise AssertionError('SymGlow pose flags changed')
        uc.reg_write(UC_X86_REG_EIP, self.vm.u32(sp))
        uc.reg_write(UC_X86_REG_ESP, sp+4+self.boundaries[address])

    def reset(self):
        self.software.restore(); self.random = []; self.random_pending = []
        self.vm.call(self.vm.u32(0x5a9e50+92), this=self.animator)

    def advance(self):
        self.vm.call(self.vm.u32(0x5a9e50+44), this=self.animator)

    def render_state(self, render=True):
        if render:
            self.vm.call(self.vm.u32(0x5a9e50+52), this=self.animator)
        return dict(timer=self.vm.u32(self.animator+0x108), zscale=struct.unpack('<f', self.vm.uc.mem_read(self.animator+0x100, 4))[0],
                    dz=struct.unpack('<f', self.vm.uc.mem_read(self.animator+0x104, 4))[0],
                    step=struct.unpack('<f', self.vm.uc.mem_read(self.animator+0xfc, 4))[0], rng_count=len(self.random),
                    uvs=[list(struct.unpack('<2f', self.vm.uc.mem_read(self.vertices+i*32+24, 8))) for i in range(66)])

    def original_draws(self):
        """Transform the native RenderObject payload; do not scroll a second time."""
        self.vm.call(0x40a420, (self.obj, 0, 0, 0, 0), this=self.imagery)
        positions = []
        for vertex in self.mesh['vertices']:
            self.vm.write(self.point, struct.pack('<3f', *vertex[:3]))
            self.vm.call(0x43ad80, (self.obj+0x58, self.point, self.result))
            positions.append(list(struct.unpack('<3f', self.vm.uc.mem_read(self.result, 12))))
        uvs = self.render_state(False)['uvs']
        draws = []
        # Render selected original mode2 above: native CULL_NONE. Submit every
        # authored triangle; the software device independently decides coverage.
        for i in range(0, len(self.mesh['indices']), 3):
            indices = self.mesh['indices'][i:i+3]
            draws.append(dict(positions=[positions[j] for j in indices],
                              uvs=[uvs[j] for j in indices]))
        return draws

    def pixels(self, draws):
        self.software.clear()
        for draw in draws:
            projected = self.software.project(draw['positions'], camera=(0,0,0), zdist=1925)
            if any(not (0<=p[0]<WIDTH and 0<=p[1]<HEIGHT) for p in projected):
                raise AssertionError('SymGlow viewport clips geometry')
            self.software.draw([(*p,31,31,31,31,*uv) for p,uv in zip(projected,draw['uvs'])], (0,1,2), z_enabled=True, z_write=False)
        return self.vm.surface_bytes('screen'), self.vm.surface_bytes('depth')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('executable', type=Path); parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--ticks', type=int, default=161)
    parser.add_argument('--render-stride', type=int, default=1)
    parser.add_argument('--state-only', action='store_true')
    args = parser.parse_args(); args.output.mkdir(parents=True, exist_ok=True)
    if args.render_stride < 1: parser.error('--render-stride must be positive')
    probe_sha = sha(Path(__file__).read_bytes())
    before = {k:sha(v.encode()) for k,v in production_spans().items()}
    with zipfile.ZipFile(ROOT/'data/imagery.rvi') as archive:
        asset = archive.read('Imagery/Misc/symglow.i3d')
    mesh = parse_asset(asset); fixture = SymGlowFixture(args.executable, mesh); fixture.reset()
    samples = [] if args.state_only else [tick for tick in (0,1,2,24,30,39,40,41,79,80,119,120,160,161) if tick<=args.ticks and not tick%args.render_stride]
    states = []; native_draws = {}
    for tick in range(args.ticks+1):
        if tick: fixture.advance()
        states.append(fixture.render_state(not tick%args.render_stride))
        if tick in samples: native_draws[tick] = fixture.original_draws()
    rng_path = args.output/'native-rng.txt'; rng_path.write_text(''.join('%d %d %d\n'%record for record in fixture.random))
    port, compiled = build_port(args.output, mesh, rng_path, args.ticks, args.render_stride)
    failures = []; cases = []; pixel_hashes = {}; pair_times = []
    for tick, native in enumerate(states):
        modern = port[tick]; errors = []
        for field in ('timer', 'rng_count'):
            if native[field] != modern[field]: errors.append(field)
        for field in ('zscale', 'dz', 'step'):
            if f32(native[field]) != f32(modern[field]): errors.append(field)
        uv_errors = sum(f32(a)!=f32(b) for x,y in zip(native['uvs'], modern['uvs']) for a,b in zip(x,y))
        if uv_errors: errors.append('vertex_uv_stores')
        diff = None; max_position = 0; hashes = None
        if tick in samples:
            start = time.perf_counter(); draws = native_draws[tick]
            if len(draws) != len(modern['draws']): errors.append('triangle_count')
            max_position = max([abs(a-b) for p,q in zip(draws,modern['draws']) for x,y in zip(p['positions'],q['positions']) for a,b in zip(x,y)] or [0])
            if max_position > 5e-5: errors.append('geometry')
            original, z = fixture.pixels(draws); revised, rz = fixture.pixels(modern['draws'])
            hashes = [sha(original), sha(revised), sha(z), sha(rz)]
            pixel_hashes[tick] = hashes
            diff = sum(a!=b for a,b in zip(struct.unpack('<'+str(WIDTH*HEIGHT)+'H',original),struct.unpack('<'+str(WIDTH*HEIGHT)+'H',revised)))
            if diff or z != rz: errors.append('pixels_or_depth')
            pair_times.append((time.perf_counter()-start)*1000)
            save_rgb565_png(args.output/f'retail-{tick:03d}.png',original,WIDTH,HEIGHT)
            save_rgb565_png(args.output/f'port-{tick:03d}.png',revised,WIDTH,HEIGHT)
        if errors: failures.append(dict(tick=tick, fields=errors, differing_uv_fields=uv_errors))
        cases.append(dict(tick=tick, original_state=native, port_state=modern, state_errors=errors, differing_uv_fields=uv_errors,
                          max_position_error=max_position, differing_rgb565_pixels=diff, image_hashes=hashes))
    # Exact warm replay verifies native state/RNG and sampled color/depth.
    fixture.reset()
    for tick, native in enumerate(states):
        if tick: fixture.advance()
        if fixture.render_state(not tick%args.render_stride) != native:
            raise AssertionError('SymGlow warm state replay changed')
        if tick in samples:
            original,z=fixture.pixels(fixture.original_draws()); revised,rz=fixture.pixels(port[tick]['draws'])
            if [sha(original),sha(revised),sha(z),sha(rz)] != pixel_hashes[tick]:
                raise AssertionError('SymGlow warm pixel/depth replay changed')
    if rng_path.read_text() != ''.join('%d %d %d\n'%record for record in fixture.random):
        raise AssertionError('SymGlow warm RNG replay changed')
    after = {k:sha(v.encode()) for k,v in production_spans().items()}
    if before != after: raise AssertionError('SymGlow production spans changed')
    if probe_sha != sha(Path(__file__).read_bytes()): raise AssertionError('SymGlow probe changed during comparison')
    report = dict(status='fail' if failures else 'pass', failures=failures, case_count=len(cases),
                  native_rng_calls=len(fixture.random), retail_sha256=RETAIL_SHA, asset_sha256=sha(asset),
                  asset_member='Imagery/Misc/symglow.i3d', type_id='0x27df45be', source_span_sha256=after,
                  probe_sha256=probe_sha, viewport=[WIDTH,HEIGHT], original_raster='0x56d960',
                  port_component=compiled, original_setup='0x4e5130', original_animate='0x4e51c0', original_render='0x4e5260',
                  sampled_pixel_ticks=samples, replays_per_case=2, render_stride=args.render_stride,
                  median_warm_pixel_pair_ms=statistics.median(pair_times) if pair_times else None,
                  scope='Actual native setup/animation/render vertex stores, object matrix and SW pixels versus production initialization/Advance/Submit. Authored66vertex/92triangleARGB4444cylinder, identityowner, selected render stride and explicitwhitevertex light. OriginalSceneMode2/CULL_NONE executes and all authored triangles reach the native raster; realmapbinding/ownerposes/light/Metal remain open.',
                  dosbox_used=False, pixel_rendering_intercepted=False, cases=cases)
    (args.output/'manifest.json').write_text(json.dumps(report, indent=2)+'\n')
    print(json.dumps({k:v for k,v in report.items() if k not in ('cases','failures','port_component','source_span_sha256')}, indent=2))
    if failures: raise SystemExit(1)


if __name__ == '__main__':
    main()
